#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 1000000

typedef struct {
    double step;
    double local_pi;
    int start;
    int end;
}Arg;

void *pthread_function(void *arg) {
    Arg *data = (Arg *)arg;
    double x;
    for (int i = data->start; i < data->end; i++) {
        x = (double)(i + 0.5) * data->step;
        data->local_pi += 4.0 / (1.0 + (x * x)) * data->step;
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    double sum = 0.0;
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
    int num_threads = atoi(argv[1]);
    pthread_t *tid = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    Arg *pthread_data = (Arg *)malloc(num_threads * sizeof(Arg));
    double step = 1.0 / (double)(N);
    int chunk = N / num_threads;

    for (int i = 0; i < num_threads; i++) {
        pthread_data[i].start = i * chunk;
        pthread_data[i].end = (i == num_threads - 1) ? N : (i + 1) * chunk;
        pthread_data[i].step = step;
        pthread_data[i].local_pi = 0.0;
        pthread_create(&tid[i], NULL, pthread_function, &pthread_data[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(tid[i], NULL);
        sum += pthread_data[i].local_pi;        
    }
    printf("Pi = %.7lf.\n", sum);

    pthread_mutex_destroy(&lock);
    free(tid);
    free(pthread_data);
    return 0;
}