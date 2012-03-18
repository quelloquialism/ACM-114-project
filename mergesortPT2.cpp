#include <getopt.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <time.h>
#include <vector>

using namespace std;

#define MAXthreads 100

clock_t all;

pthread_mutex_t lock;
vector<double> S;
vector<double> S1;
long lenS;
int currT;

struct Context {
  // thread info
  size_t id;
  long start;
  long end;
  pthread_t descriptor;

  // constructor
  Context(size_t id, long s, long e) :
  id(id), start(s), end(e) {}
  // destructor
  ~Context() {}

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
//  cout << "wtf?";
  clock_t timeZ = clock();
  Context * context = static_cast<Context *>(arg);

//  cout << "start";

  size_t id = context->id;
  long p = context->start;
  long r = context->end;

//  cout << "yea";

  if (currT < MAXthreads && (r - p) > 10000) {
    if (p < r) {
      long q = floor((p + r) / 2);
      pthread_mutex_lock(&lock);  
      Context larray(currT, p, q);
      currT++;
      Context rarray(currT, q + 1, r);
      currT++;
      pthread_mutex_unlock(&lock);     
      int status = pthread_create(&larray.descriptor, 0, sort_worker, &larray);
      if (status) {
        throw ("error in pthread_create");
      }
      status = pthread_create(&rarray.descriptor, 0, sort_worker, &rarray);
      if (status) {
        throw ("error in pthread_create");
      }
      pthread_join(larray.descriptor, 0);
      pthread_mutex_lock(&lock);                 
      currT--;
      pthread_mutex_unlock(&lock);                 
      pthread_join(rarray.descriptor, 0);
      pthread_mutex_lock(&lock);                 
      currT--;
      pthread_mutex_unlock(&lock);           
      merge(S, p, q, r);
    }
  }
  else {
    mergeSort(S, p, r);
  }
  return 0;
}


// main program
int main(int argc, char* argv[]) {
// default values for our user configurable settings
  cin >> lenS;
  double temp;
  for (long i = 0; i < lenS; i++) {
    cin >> temp;
    S.push_back(temp);
    S1.push_back(temp);
  }

  all = clock();

  currT = 1;
  Context firstT(currT, 0, lenS-1);
  currT++;

  cout << "first" << endl;
  clock_t begin1 = clock();
  pthread_create(&firstT.descriptor, 0, sort_worker, &firstT);
  pthread_join(firstT.descriptor, 0);
  clock_t end1 = clock();
  cout << "elapsed:  " << end1 - begin1 << endl;


  cout << "second" << endl;
  clock_t begin2 = clock();
  mergeSort(S1, 0, lenS - 1);
  clock_t end2 = clock();
  cout << "elapsed:  " << end2 - begin2 << endl;

/*
  for (long i=0; i < lenS; i++) {
    cout << S[i] << "  ";
  }


  for (long i=0; i < lenS; i++) {
    cout << S1[i] << "  ";
  }
*/

  // all done
  return 0;
}




