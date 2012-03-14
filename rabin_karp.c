#include <pthread.h>
#include <stdio.h>
#include <assert.h>

#define THREADS 2

const long long MUL = 31;
int N;

pthread_mutex_t mutex;

const char *str;
const char *patt;
int pos = -1;

void update_pos(int p) {
    pthread_mutex_lock(&mutex);
    if (p < pos && p != -1 || pos == -1) {
        pos = p;
    }
    pthread_mutex_unlock(&mutex);
}

void* rabin_karp_worker(void *thread_id) {

    int id;
    int start, end;
    int i, patt_len;
    long long patt_hash = 0, str_hash = 0, pow = 0;

    id = *((int *) thread_id);

    start = id * N / THREADS;
    end = (id + 1) * N / THREADS;

    for (i = 0; str[i + start] && patt[i]; i++) {
        patt_hash = patt_hash * MUL + patt[i];
        str_hash = str_hash * MUL + str[i + start];
        if (pow) {
            pow *= MUL;
        } else {
            pow = 1;
        }
    }

    if (patt[i]) {
        /* pattern is longer than string */
        return NULL;
    }

    patt_len = i;

    end += patt_len;

    i += start;

    while (str[i]) {
        if (patt_hash == str_hash) {
            if (strncmp(str + i - patt_len, patt, patt_len) == 0) {
                update_pos(i - patt_len);
                return NULL;
            }
        }
        str_hash -= str[i - patt_len] * pow;
        str_hash = str_hash * MUL + str[i];
        i++;
    }
    if (strncmp(str + i - patt_len, patt, patt_len) == 0) {
        update_pos(i - patt_len);
        return NULL;
    }
    return NULL;
}

int rabin_karp(const char *s, int n, const char *p) {
    int id, status;

    str = s;
    patt = p;
    N = n;

    pthread_t threads[THREADS];

    pthread_mutex_init(&mutex, NULL);

    pos = -1;

    for (id = 0; id < THREADS; id++) {
        status = pthread_create(&threads[id], NULL, rabin_karp_worker, 
                (void*) &id);
        if (status) {
            fprintf(stderr, "error %d in pthread_create\n", status);
        }
    }

    for (id = 0; id < THREADS; id++) {
        pthread_join(threads[id], NULL);
    }

    return pos;
}

int main() {
    
    assert(rabin_karp("hello world", 12, "world") == 6);
    assert(rabin_karp("hello world", 12, "galaxy") == -1);
    assert(rabin_karp("abcdefghijk", 12, "c") == 2);
    assert(rabin_karp("abcdefghijk", 12, "l") == -1);
    assert(rabin_karp("first second", 13, "first") == 0);
    assert(rabin_karp("long", 4, "longer") == -1);

    return 0;
}

