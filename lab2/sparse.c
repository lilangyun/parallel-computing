#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<pthread.h>
#include <assert.h>

void cg_serial(int n, int max_iter, double tol);

void *pthread_function(void *val);

void barrier();

typedef struct padded_double
{
    double data;
    char padding[56];
}padded_double;  // avoid flash sharing

int n;  // matrix dimension

int pn = 2;  // thread num
pthread_t * tid;

double *values;int *row_ptr;int *col_idx;  // csr matrix 

double *b; 
padded_double *x; padded_double *r; padded_double *p; padded_double *Ap;  //  avoid flash sharing
padded_double *tt; padded_double *ttt;

double rho0 = 0.0; double pAp = 0.0; double rho_new = 0.0;  //  dot

int num = 0;  // for synchronization

double tol = 1e-5;

pthread_mutex_t mutex; pthread_cond_t cond;

int stop = 0;  //  a stop signal
int recved = 0;  // a recved signal

int main(int argc, char* argv[]){
    // check argument
    if(argc != 4){
        printf("Parameter falied!\n");
        exit(-1);
    }
    FILE* matrix_fd = fopen(argv[1], "r");
    FILE* vector_fd = fopen(argv[2], "r");
    pn = atoi(argv[3]);

    // scan data
    fscanf(matrix_fd, "%d", &n);
    row_ptr = (int *)malloc((n+1)*sizeof(int));
    for(int i=0;i<n+1;i++){
        fscanf(matrix_fd, "%d", &row_ptr[i]);
    }
    values = (double *)malloc(row_ptr[n]*sizeof(double));
    col_idx = (int *)malloc(row_ptr[n]*sizeof(int));
    for(int i=0;i<row_ptr[n];i++){
        fscanf(matrix_fd, "%d", &col_idx[i]);
    }
    for(int i=0;i<row_ptr[n];i++){
        fscanf(matrix_fd, "%lf", &values[i]);
    }
    b = (double*)malloc(n*sizeof(double));
    for(int i=0;i<n;i++){
        fscanf(vector_fd, "%lf", &b[i]);
    }

    // cg
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    x = (padded_double *)malloc(n*sizeof(padded_double));

    // pthread_create
    tid = (pthread_t *)malloc(pn*sizeof(pthread_t));
    for(int i=0;i<pn;i++){
        pthread_create(&tid[i], NULL, pthread_function, (void *)i);
    }

    cg_serial(n, 1000, 1e-15);

    // Join pthread
    void *status;
    for(int i=0;i<pn;i++){
        pthread_join(tid[i], &status);
    }

    // print
    FILE *fd = fopen("result.txt", "w");
    for(int i=0;i<n;i++){
        fprintf(fd, "%lf ", x[i].data);
    }
    printf("\n");

    // Handle "tail"
    free(tid);
    free(values);
    free(row_ptr);
    free(col_idx);
    free(x);
    free(b);
    fclose(matrix_fd);
    fclose(vector_fd);

    return 0;
}

void cg_serial(int n, int max_iter, double tol){
    // allocate
    r   = (padded_double *)malloc(n* sizeof(padded_double));
    p   = (padded_double *)malloc(n* sizeof(padded_double));
    Ap  = (padded_double *)malloc(n* sizeof(padded_double));
    tt  = (padded_double *)malloc(pn*sizeof(padded_double));
    ttt = (padded_double *)malloc(pn*sizeof(padded_double));
    if(r == NULL || p == NULL || Ap == NULL || tt == NULL || ttt == NULL){
        return ;
    }

    // initialize data
    for(int i=0;i<n;i++){
        x[i].data = 0.0;
        r[i].data = b[i];
        p[i].data = r[i].data;
    }
    // end

    for(int i=0;i<n;i++){
        rho0 += r[i].data*r[i].data;
    }
    if(sqrt(rho0) < tol){
        free(r);
        free(p);
        free(Ap);
        free(tt);
        free(ttt);
        return;
    }
    
    for(int iter=0;iter<max_iter;iter++){
        // reset
        pAp = 0.0;
        rho_new = 0.0;
        for(int i=0;i<pn;i++){
            tt[i].data = 0;
            ttt[i].data = 0;
        }
        // end

        // iter start
        barrier();
        // end

        // spmv_csr
        // end

        // pAp
        barrier();
        for(int i=0;i<pn;i++){
            pAp += tt[i].data;
        }
        barrier();
        // end

        // update x and r
        // end

        // rho_new
        barrier();
        for(int i=0;i<pn;i++){
            rho_new += ttt[i].data;
        }
        barrier();
        // end

        // update p
        // end

        // check
        if(fabs(pAp) < 1e-15){
            break;
        }
        double res_norm=sqrt(rho_new);
        if(res_norm < tol){
            printf("Converted after %d iterations, ridual norm: %.6e\n", iter+1, res_norm);
            break;
        }
        // end

        rho0 = rho_new;
    }

    // send stop
    stop = 1;
    barrier();
    // end

    free(r);
    free(p);
    free(Ap);
    free(tt);
    free(ttt);
}

void *pthread_function(void *val){
    // pass arg
    long v = (long)val;
    int begin = v*(n/pn); 
    int end = (v == pn-1) ? n : begin + n/pn;
    // n = 3, pn = 2, begin = 0, end = 1; begin = 1, end = 3

    // infinite loop
    while(1){
        // sychronize the iter    
        barrier();
        if(stop){
            break;
        }

        // spmv_csr(n, values, row_ptr, col_idx, p, Ap);
        for(int i=begin;i<end;i++){
            p[i].data=0.0;
        }
        for(int i=begin;i<end;i++){
            for(int j=row_ptr[i]; j<row_ptr[i+1]; j++){
                // 计算A*p[begin, end)
                Ap[i].data += p[col_idx[j]].data * values[j];
            }
        }

        // pAp
        for(int i = begin; i < end; i++){
            // p[begin, end) * Ap[begin, end)
            tt[v].data += p[i].data * Ap[i].data;
        }
        barrier();
        barrier();

        // update x and r
        double alpha = rho0 / pAp;
        for(int i = begin; i < end; i++){
            x[i].data += alpha * p[i].data;
            r[i].data -= alpha * Ap[i].data;
        }

        // rho_new
        for(int i = begin; i < end; i++){
            ttt[v].data += r[i].data*r[i].data;
        }
        barrier();
        barrier();

        // update p
        double beta = rho_new / rho0;
        for(int i = begin; i < end; i++){
            p[i].data = r[i].data + beta * p[i].data;
        }

    }

    return NULL;
}

void barrier(){
    pthread_mutex_lock(&mutex);
    num++;
    if(num == pn + 1){
        num = 0;
        pthread_cond_broadcast(&cond);
    }
    else{
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}