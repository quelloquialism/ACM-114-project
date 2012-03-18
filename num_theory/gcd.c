#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define THREADS 2

long binary_gcd(long larger, long smaller) {
    if (larger < smaller) {
        // swap
        larger ^= smaller;
        smaller ^= larger;
        larger ^= smaller;
    }
    if (smaller == 0) {
        return larger;
    }
    return binary_gcd(smaller, larger % smaller);
}

long *a;
int n;

void *gcd_worker(void *thread_id) {
    long id;
    int i;
    int start;
    int end;

    id = (long) thread_id;
    start = id * n/THREADS;
    end = (id + 1) * n/THREADS;
    if (end > n) {
        end = n;
    }

    for (i = start; i < end; i++) {
        a[start] = binary_gcd(a[start], a[i]);
    }

    return NULL;
}

long gcd(long *arr, int size) {
    pthread_t threads[THREADS];
    long id;
    long res;
    int status;
    
    n = size;
    a = (long*) malloc(size * sizeof(long));
    memcpy(a, arr, size * sizeof(long));

    for (id = 0; id < THREADS; id++) {
        status = pthread_create(&threads[id], NULL, gcd_worker, (void*) id);
        if (status) {
            fprintf(stderr, "error %d in pthread_create\n", status);
        }
    }

    for (id = 0; id < THREADS; id++) {
        pthread_join(threads[id], NULL);
    }

    res = a[0];
    for (id = 1; id < THREADS; id++) {
        res = binary_gcd(res, a[id * n / THREADS]);
    }
    free(a);



    return res;
}

int main(void) {
    long arr1[] = {3, 5, 8};
    long arr2[] = {2, 0, 8};
    long arr3[] = {30, 5, 45, 1500};
    long arr4[] = {3, 6, 9, 15};
    assert(gcd(arr1, 3) == 1);
    assert(gcd(arr2, 3) == 2);
    assert(gcd(arr3, 4) == 5);
    assert(gcd(arr4, 3) == 3);
    return 0;
}

