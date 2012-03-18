#include <cmath>
#include <iostream>
#include <limits>
#include <pthread.h>
#include <queue>

using namespace std;

struct CommonData {
  int vertices;
  int vis_count;
  priority_queue<pair<double, int> > q;
  double* dist;
  double* adjmat;
  pthread_mutex_t* vis_count_mutex;
  pthread_mutex_t* q_mutex;
};

void* dijkstra_worker(void* ptr) {
  CommonData* cdata = (CommonData*)ptr;
  int vertices = cdata->vertices;
  double* dist = cdata->dist;
  double* adjmat = cdata->adjmat;

  while (!cdata->q.empty()) {
    // lock q
    pthread_mutex_lock(cdata->q_mutex);
    bool valid = !cdata->q.empty();
    pair<double, int> curr;
    int vert;
    if (valid) {
      curr = cdata->q.top();
      vert = curr.second;
      cdata->q.pop();
    }
    // unlock q
    pthread_mutex_unlock(cdata->q_mutex);

    if (valid && -curr.first < dist[vert]) {
      dist[vert] = abs(curr.first);
      // lock vis_count
      pthread_mutex_lock(cdata->q_mutex);
      pthread_mutex_lock(cdata->vis_count_mutex);
      cdata->vis_count++;
      // unlock vis_count
      pthread_mutex_unlock(cdata->vis_count_mutex);

      for (int i = 0; i < vertices; i++) {
        // lock q
        if (dist[vert] + adjmat[vert * vertices + i] < dist[i]) {
          cdata->q.push(make_pair(
              -dist[vert] - adjmat[vert * vertices + i], i
          ));
        }
        // unlock q
      }
      pthread_mutex_unlock(cdata->q_mutex);
    }
  }

  return 0;
}

double* dijkstra(int vertices, double* adjmat, int source, int threads) {
  CommonData cdata;
  pthread_mutex_t vis_count_mutex;
  pthread_mutex_init(&vis_count_mutex, 0);
  pthread_mutex_t q_mutex;
  pthread_mutex_init(&q_mutex, 0);
  cdata.vertices = vertices;
  cdata.vis_count = 0;
  cdata.dist = new double[vertices];
  cdata.adjmat = adjmat;
  cdata.vis_count_mutex = &vis_count_mutex;
  cdata.q_mutex = &q_mutex;
  pthread_t th[threads];

  for (int i = 0; i < vertices; i++) {
    cdata.dist[i] = numeric_limits<double>::infinity();
  }
  cdata.q.push(make_pair(0, source));

  for (int i = 0; i < threads; i++) {
    pthread_create(&th[i], NULL, dijkstra_worker, (void*)&cdata);
  }
  for (int i = 0; i < threads; i++) {
    pthread_join(th[i], NULL);
  }

  return cdata.dist;
}

int main() {
  int vertices, source;
  cin >> vertices >> source;
  double adjmat[vertices * vertices];
  for (int i = 0; i < vertices * vertices; i++) {
    cin >> adjmat[i];
  }

  double* dist = dijkstra(vertices, adjmat, source, 4);
  for (int i = 0; i < vertices; i++) {
    cout << dist[i];
    if (i == vertices - 1) {
      cout << endl;
    } else {
      cout << ", ";
    }
  }

  return 0;
}

