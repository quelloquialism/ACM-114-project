#include <getopt.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <time.h>
#include <vector>

using namespace std;

clock_t all;

long lenS;
vector<double> S;
vector<double> S1;
pthread_mutex_t lock;
size_t workers;

struct Context {
  // thread info
  size_t id;
  pthread_t descriptor;
};

void merge(vector<double> &S, long p, long q, long r) {
  vector<double> temp;
  long t = 0;
  long i = p;
  long j = q + 1;
  
  // Merge in sorted form the 2 arrays
  while ( i <= q && j <= r ) {
    if ( S[i] <= S[j] ) {
      temp.push_back(S[i]);
      i++;
    }
    else {
      temp.push_back(S[j]);
      j++;
    }
    t++;
  }

  // any left behind elements
  while (i <= q) {
    temp.push_back(S[i]);
    t++;
    i++;
  }
  while (j <= r) {
    temp.push_back(S[j]);
    t++;
    j++;
  }

  for (long i = p; i <= r; i++) {
    S[i] = temp[i - p];
  }
  
}

// sequential mergeSort
void mergeSort(vector<double> &S, long p, long r) {
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

//  cout << "start: " << id << endl;

  long lenMyS;


  if (id == workers - 1) {
    lenMyS = lenS / workers + lenS % workers;
  }
  else {
    lenMyS = lenS / workers ;
  }

  long start = id * lenS / workers ;

  mergeSort(S, start, start + lenMyS - 1);

  
  // now myS is sorted ...

  return 0;
}

// master thread
void masterThread(vector<double> &S, long lenS, size_t workers) {

  // per-thread information
  Context context[workers];

  
  for (size_t tid=0; tid < workers; tid++) {
    context[tid].id = tid;

    int status = pthread_create(&context[tid].descriptor, 0, sort_worker, &context[tid]);
    if (status) {
      throw ("error in pthread_create");
    }
  }
  // harvest the threads
  for (size_t tid = 0; tid < workers; tid++) {
    pthread_join(context[tid].descriptor, 0);
  }

// std::cout << " --- done." << std::endl;

  int fr = lenS / workers; 


  for (size_t tid = 1; tid < workers - 1; tid++) {

    merge(S, 0, tid * fr - 1 , (tid + 1) * fr - 1);
  }

  merge(S, 0, (workers - 1) * fr - 1, lenS - 1);


  return;
} 




// main program
int main(int argc, char* argv[]) {
// default values for our user configurable settings
  workers = 8;
  cin >> lenS;
  double temp;
  for (long i = 0; i < lenS; i++) {
    cin >> temp;
    S1.push_back(temp);
    S.push_back(temp);
  }

  all = clock();

  cout << "first" << endl;
  clock_t begin1 = clock();
  masterThread(S, lenS, workers);
  clock_t end1 = clock();
  cout << "elapsed:  " << end1 - begin1 << endl;

  cout << "second" << endl;
  clock_t begin2 = clock();
  mergeSort(S1, 0, lenS - 1);
  clock_t end2 = clock();
  cout << "elapsed:  " << end2 - begin2 << endl;


  // all done
  return 0;
}



