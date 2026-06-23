#include <stdio.h>
#include <cuda.h>
#include "mpi.h"

#define NBIN 1000000  // 循环次数
#define NUM_BLOCK 10
#define NUM_THREAD 100

__global__ void cal_pi(float *sum, int nbin) {
    int idx = blockIdx.x*blockDim.x + threadIdx.x;
    float x;
    float step = 1.0 / (float)(nbin); 
    for (int i = idx; i < nbin; i += blockDim.x * gridDim.x) {
        x = (i + 0.5) * step;
        sum[idx] += 4.0 / (float)(1 + x * x);
    }
}

int main(int argc, char *argv[]) {
    int nproc, rank;
    float sum = 0.0;
    float *sumHost, *sumDevice;
    dim3 dimGrid(NUM_BLOCK, 1, 1);
    dim3 dimBlock(NUM_THREAD, 1, 1);
    MPI_INIT(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    size_t aSize = NUM_BLOCK * NUM_THREAD * sizeof(float);
    cudaMalloc(&sumDevice, aSize);
    sumHost = (float *)malloc(aSize);
    cudaMemset(sumDevice, 0, aSize);

    cal_pi<<<dimGrid, dimBlock>>>(sumDevice, NBIN);
    cudaMemcpy(sumHost, sumDevice, aSize, cudaMemcpyDeviceToHost);
    for (int i = 0; i < NUM_BLOCK * NUM_THREAD; i++) {
        sum += sumHost[i];
    }
    sum *= 1.0 / (float)(NBIN);

    free(sumHost);
    free(sumDevice);
    printf("myid = %d, my pi = %f.\n", rank, sum);

    MPI_Finalisze();
    return 0;
}