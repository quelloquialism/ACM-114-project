#include <pthread.h>
#include <iostream>

using namespace std;


struct Task {
  // shared information
  size_t workers;
  double *mat1;
  double *mat2;
  double *res;
  int m1;
  int m2;
  int n1;
  int n2;
  // mutex to control access to the convergence criterion
  pthread_mutex_t lock;

  // constructor
  Task(size_t workers, double *mat1, double *mat2, double *res, int m1, int m2, int n1, int n2) :
    workers(workers), mat1(mat1), mat2(mat2), res(res), m1(m1), m2(m2), n1(n1), n2(n2) {
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

void * mult_worker(void * arg) {
  Context * context = static_cast<Context *>(arg);

  size_t id = context->id;
  Task * task = context->task;

  size_t workers = task->workers;
  double *mat1 = task->mat1;
  double *mat2 = task->mat2;
  double *res = task->res;
  int m1 = task->m1;
  int m2 = task->m2;
  int n1 = task->n1;
  int n2 = task->n2;  
  pthread_mutex_t lock = task->lock;

  cout << "start: " << id << endl;

  //cout << m1 << "   " << n1 << "  " << m2 << "  " << n2 << endl;
  //cout << workers << endl;
  for (int i = id; i < m1; i +=workers) {
    //cout << "id: " << id << endl;
    for (int j = 0; j < n2; j++) {
      double temp = 0;
      // m2 and n1 are equal
      //cout << "here" << endl;
      for (int k = 0; k < m2; k++) {
        pthread_mutex_lock(&lock);
 //       cout << "i,j,k" << i << " " << j << " " << k << endl;
 //       cout << "vals" << mat1[i*n1 + k] << "    " << mat2[k*n2 + j] << endl;
        pthread_mutex_unlock(&lock);
        temp += mat1[i*n1 + k] * mat2[k*n2 + j];
      }
      res[i*n2 + j] = temp;
    }
  }

  // destroy the thread
  pthread_exit(NULL);
}

void masterThread(double *mat1, double *mat2, double *res, int m1, int m2, int n1, int n2, size_t workers) {

  Task task(workers, mat1, mat2, res, m1, m2, n1, n2);
  // per-thread information
  Context context[workers];

  for (size_t tid=0; tid < workers; tid++) {
    context[tid].id = tid;
    context[tid].task = &task;

    int status = pthread_create(&context[tid].descriptor, 0, mult_worker, &context[tid]);      
  }

  // harvest the threads
  for (size_t tid = 0; tid < workers; tid++) {
    pthread_join(context[tid].descriptor, 0);
  }

  std::cout << " --- done." << std::endl;

}

int main(int argc, char *argv[]) {
  int m1 = 4;
  int n1 = 3;
  int m2 = 3;
  int n2 = 2;
  double mat1[12] = {14, 9, 3, 2, 11, 15, 0, 12, 17, 5, 2, 3};
  double mat2[6] = {12, 25, 9, 10, 8, 5};
  int workers = 4;
  double res[m1 * n2];

  
  masterThread(mat1, mat2, res, m1, m2, n1, n2, workers);

  for (int i = 0; i < m1 * n2; i++) {
    cout << res[i] << "   ";
  }


  return 0;
}
