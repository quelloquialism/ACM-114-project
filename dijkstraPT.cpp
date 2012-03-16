// Homework #4 - ACM/CS 114
#include <getopt.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <time.h>
#include <vector>

#define THREADS 2

using namespace std;

// the graph is going to be a list of nodes
// and their adjacency list
struct Edge {
  int vertex2;
  double weight;

  // constructor
  Edge(int v2, double w) :
   vertex2(v2), weight(w) {};
  // destructor
  ~Edge() {};
  
};

vector<vector<Edge> > Graph;
vector<int> Queue; // queue of nodes to be visited
vector<int> Visited; // vector of nodes already visited
vector<double> Distances; // vector of distances from 1 to other nodes
int totalVis;  // total nubmer of visited nodes so far
int graphSize; // the number of nodes
pthread_mutex_t mutex;


int minQueue() {
  int temp = Queue[0];
  int tempK = 0;
  pthread_mutex_lock(&mutex);
  for (unsigned int k = 1; k <= Queue.size(); k++) {
    if (Distances[temp] > Distances[Queue[k]]) {
      temp = Queue[k];
      tempK = k;
    }
  }
  Queue.erase(Queue.begin() + tempK);
  pthread_mutex_unlock(&mutex);
  return temp;
}

void* dijkstra_worker(void *thread_id) {

  long id;
  double temp;
  int node, node2;

  id = (long) thread_id;

  while (totalVis <= graphSize) {
    pthread_mutex_lock(&mutex);
    totalVis++;
    pthread_mutex_unlock(&mutex);
    node = minQueue();
    Visited.push_back(node);
    vector<Edge> nodeAdj = Graph[node];
    for (unsigned int j = 0; j < nodeAdj.size(); j++) {
      node2 = nodeAdj[j].vertex2;
      temp = Distances[node] + nodeAdj[j].weight;
      if (temp < Distances[node2]) {
        pthread_mutex_lock(&mutex);
        Distances[node2] = temp;
        pthread_mutex_unlock(&mutex);
      }
    }
  }
}


void dijkstra_master() {
  long id;
  int status;
  pthread_t threads[THREADS];
  pthread_mutex_init(&mutex, NULL);
    
  for (id = 0; id < THREADS; id++) {
    status = pthread_create(&threads[id], NULL, dijkstra_worker, (void*) id);
    if (status) {
      fprintf(stderr, "error %d in pthread_create\n", status);
      }
  }
  
  for (id = 0; id < THREADS; id++) {
    pthread_join(threads[id], NULL);
  }


}

// main program
int main(int argc, char* argv[]) {
// default values for our user configurable settings

  // node 1
  vector<Edge> adjList;
  Edge addE(6, 14);
  adjList.push_back(addE);
  addE.vertex2 = 3; addE.weight = 9;
  adjList.push_back(addE);  
  addE.vertex2 = 2; addE.weight = 7;
  adjList.push_back(addE);
  Graph.push_back(adjList);
  adjList.clear();

  // node 2
  addE.vertex2 = 3; addE.weight = 10;
  adjList.push_back(addE);
  addE.vertex2 = 4; addE.weight = 15;
  adjList.push_back(addE);
  addE.vertex2 = 1; addE.weight = 7;
  adjList.push_back(addE);

  Graph.push_back(adjList);
  adjList.clear();

  // node 3
  addE.vertex2 = 6; addE.weight = 2;
  adjList.push_back(addE);
  addE.vertex2 = 4; addE.weight = 11;
  adjList.push_back(addE);
  addE.vertex2 = 2; addE.weight = 10;
  adjList.push_back(addE);
  addE.vertex2 = 1; addE.weight = 9;
  adjList.push_back(addE);
  Graph.push_back(adjList);
  adjList.clear();

  // node 4
  addE.vertex2 = 5; addE.weight = 6;
  adjList.push_back(addE);
  addE.vertex2 = 3; addE.weight = 11;
  adjList.push_back(addE);
  addE.vertex2 = 2; addE.weight = 15;
  adjList.push_back(addE);
  Graph.push_back(adjList);
  adjList.clear();

  // node 5
  addE.vertex2 = 6; addE.weight = 9;
  adjList.push_back(addE);
  addE.vertex2 = 4; addE.weight = 6;
  adjList.push_back(addE);
  Graph.push_back(adjList);
  adjList.clear();

  // node 6
  addE.vertex2 = 5; addE.weight = 9;
  adjList.push_back(addE);
  addE.vertex2 = 3; addE.weight = 2;
  adjList.push_back(addE);
  addE.vertex2 = 1; addE.weight = 14;
  adjList.push_back(addE);
  Graph.push_back(adjList);
  adjList.clear();

  graphSize = 6;
  totalVis = 0;

  Distances[0] = 0;
  Queue.push_back(0);  
  for (int i = 1; i < graphSize; i++) {
    Distances[i] = 1000000;
    Queue.push_back(i);
  }

  Visited.clear();
  
  
  cout << "first" << endl;
  clock_t begin1 = clock();
  dijkstra_master();
  clock_t end1 = clock();
  cout << "elapsed:  " << end1 - begin1 << endl;


  // all done
  return 0;
}




