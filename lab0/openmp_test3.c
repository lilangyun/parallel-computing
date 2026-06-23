#include <stdio.h>
#include <omp.h>

long num_steps = 1000000;  // compute numbers
long num_threads = 2;

int get_pi();

/* Using the private and critical words to get pi*/
int main(){
    double x, sum, pi = 0.0;
    omp_set_num_threads(num_threads);
    #pragma omp parallel private(x, sum) shared(pi)
    {
        int id = omp_get_thread_num();
        printf("I'm %d.\n", id);
        sum = 0.0;
        for(int i = id; i < num_steps; i += num_threads){
            x = 1.0 * (i + 0.5) * (1.0 / (double)num_steps);
            sum += 4.0 / (1.0 + x * x);
        }
        #pragma omp critical
            pi += sum;
    }
    printf("pi = %lf\n", pi * 1.0 / (double)num_steps);
    printf("-----------------------------------\n");
    get_pi();
    return 0;
}

/* Using the reduction to calculate pi*/
int get_pi(){
    double x, sum = 0.0, pi = 0.0;
    double step = 1.0 / (double)(num_steps);
    #pragma omp parallel private(x) reduction(+:sum)
    {
        int id = omp_get_thread_num();
        for(int i = id; i < num_steps; i += num_threads){
            x = 1.0 * (i + 0.5) * (1.0 / (double)num_steps);
            sum += 4.0 / (1.0 + x * x);
        }
    }
    printf("pi = %lf\n", sum * step);
    return 0;
}