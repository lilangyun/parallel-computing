#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

typedef struct Arg
{
    int rank;
    int step;
    long n;
    double x;
}Arg;

double sum;
pthread_mutex_t lock;

void *pthread_function(void *arg){
    Arg *l = (Arg*)arg;
    double x = l->x;
    double tmp = 0.0, it = 1.0;
    int j=1;
    for(int i=l->rank;i<=l->n;i+=l->step){
        for(;j<=2*i+1;j++){
            it = it*x/j;
        }
        if(i%2==0) tmp+=it;
        else tmp-=it;
    }
    pthread_mutex_lock(&lock);
    sum += tmp;
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main(int argc, char *argv[]){
    if(argc != 4){
        printf("Bad parameters!\n");
    }

    double x = atof(argv[1]);
    long n = atol(argv[2]);
    int m = atoi(argv[3]);

    sum = 0.0;
    pthread_mutex_init(&lock, NULL);

    pthread_t *tid = (pthread_t*)malloc(sizeof(pthread_t)*m);
    for(long i=0;i<m;i++){
        Arg *arg = (Arg*)malloc(sizeof(Arg));
        arg->rank=i;
        arg->step=m;
        arg->n=n;
        arg->x=x;
        pthread_create(&tid[i], NULL, pthread_function, (void *)arg);
    }
    for(int i=0;i<m;i++){
        pthread_join(tid[i], NULL);
    }
    printf("%lf\n", sum);
}