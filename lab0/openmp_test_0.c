#include <stdio.h>
#include <omp.h>
int main() {
    #pragma omp parallel num_threads(4)
    {
        int i = omp_get_thread_num();
    #pragma omp for
        for( int j=0; j<4; j++){
            printf("Hello from thread %d\n", i);
            // The output is just 4 lines, because the for loop is assigned for 4 threads to execute.
        }
    }
   return 0;
}