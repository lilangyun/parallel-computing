#include <stdio.h>
#include <omp.h>

int main(){
    omp_set_num_threads(2);
    #pragma omp parallel
    #pragma omp sections
    {
        #pragma omp section
        printf("%d ", omp_get_thread_num());
        #pragma omp section
        printf("%d ", omp_get_thread_num());
    }

    #pragma omp parallel for schedule(static, 2)
        for(int i=0; i<4; i++)
        {
            printf("%d ", 2 + omp_get_thread_num());
        }
    return 0;
}