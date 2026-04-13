#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

int n = 10000;
int m = 4;
double sum;

void *pthread_function(void *arg){
    long index = (long)(arg);
    int num = n/m;
    long start = index;
    long end = index + num;
    double factor = 1.0;
    if(start%2==0){
        factor = - factor;
    }
    for(int i = start+1;i<=end;i++, factor=-factor){
        sum += factor*(1.0/(2*i-1));
    }
    return NULL;
}

int main(int argc, char *argv[]){
    sum = 0.0;

    // data

    // 创建m个线程
    pthread_t *tid = (pthread_t *)malloc(sizeof(pthread_t)*m);
    for(int i=0;i<m;i++){
        pthread_create(&tid[i],NULL,pthread_function,(void*)i);
    }
    for(int i=0;i<m;i++){
        pthread_join(tid[i],NULL);
    }
    printf("%lf\n", sum);
    return 0;
}