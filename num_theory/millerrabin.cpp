#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <stdexcept>

struct ThreadData {
  int n;
  int id;
  int threads;
  bool result;
};

long long modpow(long long b, int e, int m) {
  if (e < 0 || m == 0) {
    throw std::runtime_error("modpow: Invalid parameters");
  }

  if (e == 0) {
    return 1;
  } else if (e == 1) {
    return b % m;
  } else if (e % 2 == 0) {
    return modpow(b * b % m, e >> 1, m);
  } else {
    return b * modpow(b, e - 1, m) % m;
  }
}

void* mr_worker(void* ptr) {
  ThreadData* tdata = (ThreadData*)ptr;
  int n = tdata->n;
  int threads = tdata->threads;
  int id = tdata->id;

  int d = n - 1, s = 0;
  while (d % 2 == 0) {
    d >>= 1;
    s++;
  }

  int trials[] = {2, 7, 61};
  int num_trials = sizeof(trials) / sizeof(int);

  for (int i = id; i < num_trials; i += threads) {
    if (trials[i] != n) {
      long long test = modpow(trials[i], d, n);
      if (test != 1) {
        bool works = true;
        for (int r = 0; r < s; r++, test = test * test % n) {
          if (test == n - 1) {
            works = false;
            break;
          }
        }

        if (works) {
          tdata->result = false;
          return 0;
        }
      }
    }
  }

  tdata->result = true;
  return 0;
}

bool miller_rabin(int n, int threads) {
  if (n == 2) {
    return true;
  } else if (n % 2 == 0 || n < 2) {
    return false;
  }

  pthread_t th[threads];
  ThreadData tdata[threads];
  for (int i = 0; i < threads; i++) {
    tdata[i].id = i;
    tdata[i].n = n;
    tdata[i].threads = threads;
    int status = pthread_create(&th[i], NULL, mr_worker, (void*)&tdata[i]);
    if (status) {
      throw "Error in thread creation.";
    }
  }

  bool result = true;
  for (int i = 0; i < threads; i++) {
    pthread_join(th[i], NULL);
    result &= tdata[i].result;
  }
  
  return result;
}

int main() {
  int true_tests[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47,
      53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 3571, 77977, 139921, 643883, 
      1299709, 9000011};
  int false_tests[] = {-1, 0, 1, 4, 9, 15, 21, 25, 27, 49, 143, 1111, 111111,
      9000009};
  int true_length = sizeof(true_tests) / sizeof(int);
  int false_length = sizeof(false_tests) / sizeof(int);

  bool passed = true;
  for (int i = 0; i < true_length; i++) {
    if (!miller_rabin(true_tests[i], 3)) {
      std::cout << "FAILED: " << true_tests[i] << " is prime" << std::endl;
      passed = false;
    }
  }

  for (int i = 0; i < false_length; i++) {
    if (miller_rabin(false_tests[i], 3)) {
      std::cout << "FAILED: " << true_tests[i] << " is not prime" << std::endl;
      passed = false;
    }
  }
  
  if (passed) {
    std::cout << "All " << (true_length + false_length) << " tests passed." <<
        std::endl;
  } else {
    std::cout << "Some tests failed." << std::endl;
  }
  return 0;
}

