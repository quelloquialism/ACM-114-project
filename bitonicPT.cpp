#include <iostream>
#include <cstdlib>
#include <pthread.h>


#define THREADS 4

using namespace std;

long next;
int *a;
unsigned long size;
pthread_mutex_t mutex;

long get_next() {
    long res;
    pthread_mutex_lock(&mutex);
    res = next;
    next <<= 1;
    pthread_mutex_unlock(&mutex);
    return res;
}



void * sort_cpu(void *thread_id) {
  for(unsigned long i = get_next(); i <= size; i = get_next()) {
    for(unsigned long j = i >> 1; j > 0; j >>= 1) {
      for(unsigned long k = 0; k < size; k++) {
        if((k&j) != 0) {
          k += j;
          if(k >= size) {
            break;
          }
        }
        unsigned long tmp = j^k;

        if(tmp > k && (((i&k) == 0 && a[k] > a[tmp]) || ((i&k) != 0 && a[k] < a[tmp]))) {
          a[k] ^= a[tmp];
          a[tmp] ^= a[k];
          a[k] ^= a[tmp];
        }
      }
    }
  }

  return 0;
}

void sort_master() {
  long id;
  int status;
  pthread_t threads[THREADS];
  pthread_mutex_init(&mutex, NULL);
  next = 2;

  for (id = 0; id < THREADS; id++) {
    status = pthread_create(&threads[id], NULL, sort_cpu, (void*) id);
    if (status) {
      fprintf(stderr, "error %d in pthread_create\n", status);
      }
  }

  for (id = 0; id < THREADS; id++) {
    pthread_join(threads[id], NULL);
  }
  
  return;
}


int main() {
  size =  1048576;
  a = new int[size];
  for (unsigned long i = 0; i < size; i++) {
    a[i] = rand();
//    cout << a[i] << " ";
  }
  cout << endl << endl << "Sorting..." << endl << endl;
  sort_master();
  for (unsigned long i = 0; i < size; i++) {
//    cout << a[i] << " ";
  }
  cout << endl;

  delete[] a;
  return 0;
}

