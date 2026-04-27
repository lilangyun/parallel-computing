#include <stdio.h>
#include <omp.h>

int main(){
    omp_set_num_threads(2);
    int sum = 0;
    #pragma omp parallel
    {
        #pragma omp sections
        {
            #pragma omp section
            printf("%d ", omp_get_thread_num());
            #pragma omp section
            printf("%d ", omp_get_thread_num());
        }

        #pragma omp for schedule(static, 2)
            for(int i=0; i<4; i++)
            {
                printf("%d ", 1000 + omp_get_thread_num());
            }
        #pragma omp for
            for(int i=0; i<100000; i++)
            {
            #pragma omp atomic
                sum++;
            }
        #pragma omp master
        {
            printf("\n%d\n", sum);
        }
    }

    return 0;
}