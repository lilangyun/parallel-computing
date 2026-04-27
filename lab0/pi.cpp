#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

int n = 10000;
int m = 4;
double sum;
pthread_t mutex;

void *pthread_function(void *arg){
    long index = (long)(arg);
    int num = n/m;
    long start = index;
    long end = index + num;
    double factor = 1.0;
    if(start%2==0){
        factor = -1.0;
    }
    for(int i = start; i < end; i++, factor=-factor){
        pthread_mutex_lock(&mutex);
        sum += factor*(1.0 / (2*i + 1));
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char *argv[]){
    sum = 0.0;

    // data
    pthread_mutex_init(&mutex, NULL);

    // 创建m个线程
    pthread_t *tid = (pthread_t *)malloc(sizeof(pthread_t)*m);
    for(int i=0;i<m;i++){
        pthread_create(&tid[i],NULL,pthread_function,(void*)i);
    }
    for(int i=0;i<m;i++){
        pthread_join(tid[i],NULL);
    }
    printf("%lf\n", 4 * sum);
    pthread_mutex_destroy(&mutex);
    return 0;
}