// Homework #4 - ACM/CS 114
#include <getopt.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <time.h>

using namespace std;


struct Task {
  // shared information
  size_t workers;
  double *S;
  long lenS;
  // mutex to control access to the convergence criterion
  pthread_mutex_t lock;

  // constructor
  Task(size_t workers, double *S, long lenS) :
    workers(workers), S(S), lenS(lenS) {
    pthread_mutex_init(&lock, 0);
  }
  // destructor
  ~Task() {
    pthread_mutex_destroy(&lock);
  }
};

struct Context {
  // thread info
  size_t id;
  pthread_t descriptor;
  Task * task;
};

void merge(double *S, long p, long q, long r) {
  double temp[r + 1 - p];
  long t = 0;
  long i = p;
  long j = q + 1;
  
  // Merge in sorted form the 2 arrays
  while ( i <= q && j <= r ) {
    if ( S[i] <= S[j] ) {
      temp[t] = S[i];
      i++;
    }
    else {
      temp[t] = S[j];
      j++;
    }
    t++;
  }

  // any left behind elements
  while (i <= q) {
    temp[t] = S[i];
    t++;
    i++;
  }
  while (j <= r) {
    temp[t] = S[j];
    t++;
    j++;
  }

  for (long i = p; i <= r; i++) {
    S[i] = temp[i - p];
  }
  
}

// sequential mergeSort
void mergeSort(double *S, long p, long r) {
  if (p < r) {
    long q = floor((p + r) / 2);
    mergeSort(S, p, q);
    mergeSort(S, q + 1, r);
    merge(S, p, q, r);
  }
}


// workers
void * sort_worker(void * arg) {
  Context * context = static_cast<Context *>(arg);

  size_t id = context->id;
  Task * task = context->task;

  size_t workers = task->workers;
  long lenS = task->lenS;
  double *S = task->S;
  pthread_mutex_t lock = task->lock;

  cout << "start: " << id << endl;

  long lenMyS;


  if (id == workers - 1) {
    lenMyS = lenS / workers + lenS % workers;
  }
  else {
    lenMyS = lenS / workers ;
  }

  double myS[lenMyS];
  long start = id * lenS / workers ;
  for (long i = 0; i < lenMyS; i++) {
    myS[i] = S[start + i];
  }

  mergeSort(myS, 0, lenMyS - 1);

  
  // now myS is sorted ...

  // grab the lock 
  pthread_mutex_lock(&lock);

  for (long i = 0; i < lenMyS; i++) {
    S[start + i] = myS[i];
  }
  pthread_mutex_unlock(&lock);

  return 0;
}

// master thread
void masterThread(double *S, long lenS, size_t workers) {

  Task task(workers, S, lenS);    
  // per-thread information
  Context context[workers];

  
  for (size_t tid=0; tid < workers; tid++) {
    context[tid].id = tid;
    context[tid].task = &task;

    int status = pthread_create(&context[tid].descriptor, 0, sort_worker, &context[tid]);      
  }
  // harvest the threads
  for (size_t tid = 0; tid < workers; tid++) {
    pthread_join(context[tid].descriptor, 0);
  }

  std::cout << " --- done." << std::endl;

  int fr = lenS / workers;

  for (size_t tid = 1; tid < workers - 1; tid++) {
    merge(S, 0, tid * fr - 1, (tid + 1) * fr - 1);
  }
  merge(S, 0, workers * fr - 1, lenS - 1);


  return;
} 




// main program
int main(int argc, char* argv[]) {
// default values for our user configurable settings
  size_t workers = 2;
  long lenS = 500;
  double S[lenS];
  for (long i = lenS; i > 0; i--) {
    S[i] = double(i);
  }
  
  cout << "first" << endl;
  clock_t begin1 = clock();
  masterThread(S, lenS, workers);
  clock_t end1 = clock();
  cout << "elapsed:  " << end1 - begin1 << endl;

  for (long i = lenS; i > 0; i--) {
    S[i] = double(i);
  }

  cout << "second" << endl;
  clock_t begin2 = clock();
  mergeSort(S, 0, lenS - 1);
  clock_t end2 = clock();
  cout << "elapsed:  " << end2 - begin2 << endl;


  // all done
  return 0;
}



