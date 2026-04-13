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
pthread_mutex_t mutex;

void *pthread_function(void *arg){
    Arg *l = (Arg*)arg;
    int start = l->rank;
    int step = l->step;
    long n = l->n;
    double x = l->x;
    double tmp = 0.0, it = 1.0;
    int j=1;
    for(int i=start;i<=n;i+=step){
        for(;j<=2*i+1;j++){
	    it=it*x/j;
        }
	if(i%2==0)tmp+=it;
	else tmp-=it;
    }
    pthread_mutex_lock(&mutex);
    sum += tmp;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int argc, char *argv[]){
    double x = atof(argv[1]);
    long n = atol(argv[2]);
    int m = atoi(argv[3]);
    sum = 0.0;

    pthread_t *pid = (pthread_t*)malloc(sizeof(pthread_t)*m);
    pthread_mutex_init(&mutex, NULL);
    for(long i=0;i<m;i++){
        Arg *arg = (Arg*)malloc(sizeof(Arg));
        arg->rank=i;
        arg->step=m;
        arg->n=n;
        arg->x=x;
        pthread_create(&pid[i], NULL, pthread_function, (void *)arg);
    }
    for(int i=0;i<m;i++){
        pthread_join(pid[i], NULL);
    }
    free(pid);
    printf("%lf\n", sum);
    printf("Hi\n");
}
