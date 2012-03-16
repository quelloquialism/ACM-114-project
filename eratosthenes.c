#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define THREADS 2

char *sieve;
long sieve_size;
pthread_mutex_t mutex;
long next;

long get_next() {
    long res;
    pthread_mutex_lock(&mutex);
    while (sieve[next]) {
        next++;
    }
    res = next;
    next++;
    pthread_mutex_unlock(&mutex);
    return res;
}
void* sieve_worker(void *thread_id) {

    long id;
    long i, j;

    id = (long) thread_id;

    for (i = get_next(); i < sqrt(sieve_size); i = get_next()) {
        if (sieve[i]) {
            continue;
        }
        for (j = 2 * i; j < sieve_size; j += i) {
            sieve[j] = 1;
        }
    }
}

void sieve_init(long size) {
    long id;
    int status;
    pthread_t threads[THREADS];

    pthread_mutex_init(&mutex, NULL);
    next = 2;
    sieve_size = size + 1;
    sieve = (char*) malloc(sieve_size);
    memset(sieve, 0, sieve_size);
    
    for (id = 0; id < THREADS; id++) {
        status = pthread_create(&threads[id], NULL, sieve_worker, (void*) id);
        if (status) {
            fprintf(stderr, "error %d in pthread_create\n", status);
        }
    }

    for (id = 0; id < THREADS; id++) {
        pthread_join(threads[id], NULL);
    }
}

void sieve_free() {
    free(sieve);
}

int sieve_is_prime(long n) {
    return !sieve[n];
}

int main() {

    sieve_init(100);
    assert(sieve_is_prime(1));
    assert(sieve_is_prime(2));
    assert(sieve_is_prime(3));
    assert(sieve_is_prime(5));
    assert(sieve_is_prime(7));
    assert(sieve_is_prime(23));
    assert(sieve_is_prime(41));
    assert(sieve_is_prime(97));

    assert(!sieve_is_prime(4));
    assert(!sieve_is_prime(6));
    assert(!sieve_is_prime(8));
    assert(!sieve_is_prime(25));
    assert(!sieve_is_prime(100));
    
    return 0;
}

