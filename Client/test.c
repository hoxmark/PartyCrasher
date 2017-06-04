#include "counters/clique-count-pthread.c"
#include "counters/clique-count.c"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <uuid/uuid.h>

int total = 0;
int* g;

typedef struct {
    int id;
    int from;
    int to;
    int m;
    int* g;
} thread_args_t;

int random_int(int min, int max) { return min + rand() % (max - min); }

void flip_entry(int* matrix, int row, int column, int m) {
    if (matrix[row * m + column] == 0)
        matrix[row * m + column] = 1;
    else
        matrix[row * m + column] = 0;
}

void print_counterexample(int* g, int m) {
    int i, j;
    for (i = 0; i < (m - 11) / 2; i++) {
        printf("*-");
    }
    printf(" Counter example at %d ", m);
    for (i = 0; i < (m - 11) / 2; i++) {
        printf("-*");
    }
    printf("\n");
    for (i = 0; i < m; i++) {
        for (j = 0; j < m; j++) {
            printf("%d ", g[i * m + j]);
        }
        printf("\n");
    }
}

void* ftcs_compute(void* arg) {
    thread_args_t* thread_args = (thread_args_t*)arg;
    // int local = 5;

    // thread_args->from = 0;
    // thread_args->to = 10;

    // printf("Thread %d: %d -> %d \n", thread_args->id, thread_args->from, thread_args->to);

    // int local = CliqueCountPthread(g, thread_args->m, thread_args->from, thread_args->to);
    int local = CliqueCountPthread(thread_args->g, thread_args->m, thread_args->from, thread_args->to);

    total += local;
    // return (void *)local;
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    srand(time(NULL) + getpid());
    // struct timeval begin, end;
    //
    // int m = 300;
    // for (int i = 0; i < 100; i++) {
    //     int row = random_int(0, m);
    //     int column = random_int(row, m);
    //     printf("%d %d \n", row, column);
    // }

    // char *test = "Hei1";
    // int res = strcmp(test, "Hei2");
    // printf("%d\n", res);

    // const char* s = "(13, 149), (13, 4949499)";
    // int total_n = 0;
    // int n;
    // int i;
    // while (1 == sscanf(s + total_n, "%*[^0123456789]%d%n", &i, &n)) {
    //     total_n += n;
    //     printf("%d\n", i);
    // }


    // int n_threads = 2;
    // thread_args_t* args[n_threads];
    // pthread_t threads[n_threads];
    //
    // char* name = "./counterexamples/300.txt";
    // FILE* file = fopen(name, "r");
    //
    // int m;
    // int cliques;
    // fscanf(file, "%d", &m);
    // fscanf(file, "%d", &cliques);
    // printf("%d\n", m);
    // g = (int*)malloc(m * m * sizeof(int));
    //
    // int i, j;
    // for (i = 0; i < m * m; i++) {
    //     fscanf(file, "%d", &g[i]);
    // }
    //
    // int width = m / n_threads;
    //
    // // print_counterexample(g, m);
    //
    // for (j = 0; j < 100; j++) {
    //
    //     int row = random_int(0, m);
    //     int column = random_int(row, m);
    //     flip_entry(g, row, column, m);
    //
    //     // Calculate ptreads
    //     gettimeofday(&begin, NULL);
    //     for (i = 0; i < n_threads; i++) {
    //         args[i] = malloc(sizeof(thread_args_t));
    //         args[i]->g = malloc(m * m * sizeof(int));
    //         args[i]->id = i;
    //         args[i]->m = m;
    //         args[i]->from = width * i;
    //         // Make sure we do every iteration. last does more
    //         if (i == n_threads - 1) {
    //             args[i]->to = m - 10 + 1;
    //         } else {
    //             args[i]->to = width * (i + 1);
    //         }
    //
    //         int k;
    //         for(k = 0; k < m * m; k++){
    //             args[i]->g[k] = g[k];
    //         }
    //
    //         // printf("%d -> %d\n", args[i]->from, args[i]->to);
    //         pthread_create(&threads[i], NULL, ftcs_compute, (void*)args[i]);
    //     }
    //
    //     // void* ret;
    //     for (int i = 0; i < n_threads; i++) {
    //         pthread_join(threads[i], NULL);
    //         // total += (int)ret;
    //     }
    //     gettimeofday(&end, NULL);
    //     double elapsed = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec) / 1000000.0);
    //     printf("Pthread answer is \t%d in %f seconds\n", total, elapsed);
    //
    //     gettimeofday(&begin, NULL);
    //     int ans = CliqueCount(g, m);
    //     gettimeofday(&end, NULL);
    //     elapsed = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec) / 1000000.0);
    //
    //     printf("Serial answer is \t%d in %f seconds \n", ans, elapsed);
    //     printf("\n");
    //     total = 0;
    // }

    exit(1);
}
