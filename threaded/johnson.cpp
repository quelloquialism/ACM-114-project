#include <iostream>
#include <limits>
#include <pthread.h>
#include <queue>

using namespace std;

struct CommonData {
  int threads;
  int vertices;
  double* dist;
  double* adjmat;
};

struct ThreadData {
  int id;
  CommonData* cdata;
};

double* dijkstra(int vertices, double* adjmat, int source) {
  priority_queue<pair<double, int> > q;
  double* dist = new double[vertices];
  bool vis[vertices];
  
  for (int i = 0; i < vertices; i++) {
    dist[i] = numeric_limits<double>::infinity();
    vis[i] = false;
  }
  dist[source] = 0;
  q.push(make_pair(0, source));

  while (!q.empty()) {
    pair<double, int> curr = q.top();
    int vert = curr.second;
    q.pop();

    if (!vis[vert]) {
      vis[vert] = true;
      for (int i = 0; i < vertices; i++) {
        if (!vis[i] && dist[vert] + adjmat[vert * vertices + i] < dist[i]) {
          dist[i] = dist[vert] + adjmat[vert * vertices + i];
          q.push(make_pair(-dist[i], i));
        }
      }
    }
  }

  return dist;
}

void* johnson_worker(void * ptr) {
  ThreadData* tdata = (ThreadData*)ptr;
  CommonData* cdata = tdata->cdata;

  int id = tdata->id;
  int vertices = cdata->vertices;
  int threads = cdata->threads;
  double* adjmat = cdata->adjmat;
  double* dist = cdata->dist;

  for (int i = id; i < vertices; i += threads) {
    double* tmp = dijkstra(vertices, adjmat, i);
    for (int j = 0; j < vertices; j++) {
      dist[i * vertices + j] = tmp[j];
    }
  }
  return 0;
}

double* johnson(int vertices, double* adjmat, int threads) {
  double* dist = new double[vertices * vertices];
  pthread_t th[threads];
  CommonData cdata;
  cdata.threads = threads;
  cdata.dist = dist;
  cdata.vertices = vertices;
  cdata.adjmat = adjmat;
  ThreadData tdata[threads];
  for (int i = 0; i < threads; i++) {
    tdata[i].id = i;
    tdata[i].cdata = &cdata;
    int status = pthread_create(&th[i], NULL, johnson_worker, (void*)&tdata[i]);
    if (status) {
      throw "Error in thread creation.";
    }
  }
  for (int i = 0; i < threads; i++) {
    pthread_join(th[i], NULL);
  }

  return dist;
}

int main() {
  int vertices;
  cin >> vertices;
  double adjmat[vertices * vertices];
  for (int i = 0; i < vertices * vertices; i++) {
    cin >> adjmat[i];
  }

  double* dist = johnson(vertices, adjmat, 4);
  for (int i = 0; i < vertices; i++) {
    for (int j = 0; j < vertices; j++) {
      cout << dist[i * vertices + j];
      if (j == vertices - 1) {
        cout << endl;
      } else {
        cout << ", ";
      }
    }
  }

  return 0;
}

