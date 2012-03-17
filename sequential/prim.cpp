#include <iostream>
#include <limits>
#include <queue>

using namespace std;

vector<pair<int, int> > prim(int vertices, double* adjmat) {
  vector<pair<int, int> > mst;
  priority_queue<pair<double, int> > q;
  double* dist = new double[vertices];
  bool vis[vertices];
  int parent[vertices];
  
  for (int i = 0; i < vertices; i++) {
    dist[i] = numeric_limits<double>::infinity();
    vis[i] = false;
  }
  dist[0] = 0;
  parent[0] = -1;
  q.push(make_pair(0, 0));

  while (!q.empty()) {
    pair<double, int> curr = q.top();
    int vert = curr.second;
    q.pop();

    if (!vis[vert]) {
      vis[vert] = true;
      if (parent[vert] != -1) {
        mst.push_back(make_pair(parent[vert], vert));
      }
      for (int i = 0; i < vertices; i++) {
        if (!vis[i] && dist[vert] + adjmat[vert * vertices + i] < dist[i]) {
          parent[i] = vert;
          dist[i] = dist[vert] + adjmat[vert * vertices + i];
          q.push(make_pair(-dist[i], i));
        }
      }
    }
  }

  return mst;
}

int main() {
  int vertices;
  cin >> vertices;
  double adjmat[vertices * vertices];
  for (int i = 0; i < vertices * vertices; i++) {
    cin >> adjmat[i];
  }

  vector<pair<int, int> > mst = prim(vertices, adjmat);

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

