#include <iostream>
#include <cstdlib>

using namespace std;

void sort_cpu(int *a, unsigned int size) {
  for(int i = 2; i <= size; i <<= 1) {
    for(int j = i >> 1; j > 0; j >>= 1) {
      for(int k = 0; k < size; k++) {
        if((k&j) != 0) {
          k += j;
          if(k >= size) {
            break;
          }
        }
        int tmp = j^k;

        if(tmp > k && (((i&k) == 0 && a[k] > a[tmp]) || ((i&k) != 0 && a[k] < a[tmp]))) {
          a[k] ^= a[tmp];
          a[tmp] ^= a[k];
          a[k] ^= a[tmp];
        }
      }
    }
  }
}

int main() {
  int list[128];
  for (int i = 0; i < 128; i++) {
    list[i] = rand();
    cout << list[i] << " ";
  }
  cout << endl << endl << "Sorting..." << endl << endl;
  sort_cpu(list, 128);
  for (int i = 0; i < 128; i++) {
    cout << list[i] << " ";
  }
  cout << endl;
  return 0;
}
