#include <iostream>
#include <limits>
#include <queue>

using namespace std;

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

int main() {
  int vertices, source;
  cin >> vertices >> source;
  double adjmat[vertices * vertices];
  for (int i = 0; i < vertices * vertices; i++) {
    cin >> adjmat[i];
  }

  double* dist = dijkstra(vertices, adjmat, source);
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

