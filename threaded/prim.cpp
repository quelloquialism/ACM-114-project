#include <cmath>
#include <iostream>
#include <limits>
#include <pthread.h>
#include <queue>

using namespace std;

struct CommonData {
  int vertices;
  int vis_count;
  vector<pair<int, int> > mst;
  priority_queue<pair<double, int> > q;
  double* dist;
  double* adjmat;
  int* parent;
  pthread_mutex_t* vis_count_mutex;
  pthread_mutex_t* q_mutex;
};

void* prim_worker(void* ptr) {
  CommonData* cdata = (CommonData*)ptr;
  int vertices = cdata->vertices;
  double* dist = cdata->dist;
  double* adjmat = cdata->adjmat;
  int* parent = cdata->parent;

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
      if (parent[vert] != -1) {
        cdata->mst.push_back(make_pair(parent[vert], vert));
      }
      pthread_mutex_lock(cdata->q_mutex);
      // lock vis_count
      pthread_mutex_lock(cdata->vis_count_mutex);
      cdata->vis_count++;
      // unlock vis_count
      pthread_mutex_unlock(cdata->vis_count_mutex);

      for (int i = 0; i < vertices; i++) {
        if (dist[vert] + adjmat[vert * vertices + i] < dist[i]) {
          parent[i] = vert;
          cdata->q.push(make_pair(
              -dist[vert] - adjmat[vert * vertices + i], i
          ));
        }
      }
      pthread_mutex_unlock(cdata->q_mutex);
    }
  }

  return 0;
}

vector<pair<int, int> > prim(int vertices, double* adjmat, int threads) {
  vector<pair<int, int> > mst;
  CommonData cdata;
  pthread_mutex_t vis_count_mutex;
  pthread_mutex_init(&vis_count_mutex, 0);
  pthread_mutex_t q_mutex;
  pthread_mutex_init(&q_mutex, 0);
  cdata.mst = mst;
  cdata.vertices = vertices;
  cdata.vis_count = 0;
  cdata.dist = new double[vertices];
  cdata.parent = new int[vertices];
  cdata.parent[0] = -1;
  cdata.adjmat = adjmat;
  cdata.vis_count_mutex = &vis_count_mutex;
  cdata.q_mutex = &q_mutex;
  pthread_t th[threads];

  for (int i = 0; i < vertices; i++) {
    cdata.dist[i] = numeric_limits<double>::infinity();
  }
  cdata.q.push(make_pair(0, 0));

  for (int i = 0; i < threads; i++) {
    pthread_create(&th[i], NULL, prim_worker, (void*)&cdata);
  }
  for (int i = 0; i < threads; i++) {
    pthread_join(th[i], NULL);
  }

  return cdata.mst;
}

int main() {
  int vertices;
  cin >> vertices;
  double adjmat[vertices * vertices];
  for (int i = 0; i < vertices * vertices; i++) {
    cin >> adjmat[i];
  }

  vector<pair<int, int> > mst = prim(vertices, adjmat, 4);

  for (size_t i = 0; i < mst.size(); i++) {
    cout << mst[i].first << "-" << mst[i].second;
    if (i == mst.size() - 1) {
      cout << endl;
    } else {
      cout << ", ";
    }
  }

  return 0;
}

