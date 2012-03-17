#include <iostream>
#include <cstdlib>
#include <stdexcept>

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

bool miller_rabin(int n) {
  if (n == 2) {
    return true;
  } else if (n % 2 == 0 || n < 2) {
    return false;
  }

  int d = n - 1, s = 0;
  while (d % 2 == 0) {
    d >>= 1;
    s++;
  }

  int trials[] = {2, 7, 61};
  int num_trials = sizeof(trials) / sizeof(int);

  for (int i = 0; i < num_trials; i++) {
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
          return false;
        }
      }
    }
  }

  return true;
}

int main() {
  int true_tests[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47,
      53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 3571, 77977, 139921, 643883, 
      1299709, 9000011};
  int false_tests[] = {-1, 0, 1, 4, 9, 15, 21, 25, 27, 49, 143, 1111, 111111,
      9000009};
  int true_length = sizeof(true_tests) / sizeof(int);
  int false_length = sizeof(false_tests) / sizeof(int);

  for (int i = 0; i < true_length; i++) {
    if (!miller_rabin(true_tests[i])) {
      std::cout << "FAILED: " << true_tests[i] << " is prime" << std::endl;
    }
  }

  for (int i = 0; i < false_length; i++) {
    if (miller_rabin(false_tests[i])) {
      std::cout << "FAILED: " << true_tests[i] << " is not prime" << std::endl;
    }
  }
  
  return 0;
}

