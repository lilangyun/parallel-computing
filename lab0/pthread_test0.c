#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<pthread.h>

int n = 1000000;
int m = 4;
double sum;
pthread_mutex_t mutex;

void *pthread_function(void *arg){
    long rank = pthread_self();
    if(rank == 1){
        int ans = pthread_cancel(2);
        printf("ans: %d\n", ans);
    }
    else{
        _sleep(0.1);
    }
    
    pthread_detach(rank);

    long index = (long)(arg);
    printf("r: %d, index: %d\n", rank, index);
    assert(rank-1 == index);

    int num = n/m;
    long start = index * num;
    long end = start + num;
    double factor = 1.0;
    if(start%2 != 0){
        factor = -1.0;
    }
    for(int i = start + 1; i <= end; i++, factor = -factor){
        pthread_mutex_lock(&mutex);
        sum += 4.0 * factor / (2*i - 1);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;  // 也可以替换成pthread_exit(NULL)
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
    printf("%lf\n", sum);

    free(tid);
    pthread_mutex_destroy(&mutex);
    return 0;
}