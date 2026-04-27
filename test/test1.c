#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
    int n, comSize, rank;
    long long sum = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    n = atoi(argv[1]);
    int step = n / comSize;
    int start = rank * step;
    int end = (rank == comSize - 1) ? n : start + step;
    long long tmp = 0;
    #pragma omp parallel for schedule(static, 4)
    for(int i = start; i <= end; i++) {
        tmp += pow(i, 3); 
    }
    MPI_Reduce(&tmp, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if(rank == 0) {
        printf("Sum = %lld.\n", sum);
    }
    MPI_Finalize();
    return 0;
}