#include <hip/hip_runtime.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
 
#define N 256 * 1024 * 1024

// 计算 y = a x^2 + b x + c

__global__ void cal(float *x, float *y, float a, float b, float c) {
  int tid = threadIdx.x + blockIdx.x * blockDim.x;
  if (tid < N) {
    y[tid] = a * x[tid] * x[tid] + b * x[tid] + c;
  }
}

int main() {
    float a = 4.0, b = 12.0, c = 9.0;
    int size = N * sizeof(float);

    float *x, *y;
    x = (float*) malloc(size);
    y = (float*) malloc(size);

    float *dx, *dy;
    hipMalloc(&dx, size);
    hipMalloc(&dy, size);

    timeval tb, te;
    double t_calc, t_check;
    
    for (int i = 0; i < N; i++) {
      x[i] = i;
    }
    hipMemcpy(dx, x, size, hipMemcpyHostToDevice);

    int block_size = 256;
    int grid_size = (N + block_size - 1) / block_size;

    gettimeofday(&tb, NULL);
    cal<<<grid_size, block_size>>>(dx, dy, a, b, c);
    hipDeviceSynchronize();
    gettimeofday(&te, NULL);
    t_calc = (te.tv_sec - tb.tv_sec) * 1000.0 + (te.tv_usec - tb.tv_usec) / 1000.0;

    bool correct = true;
    gettimeofday(&tb, NULL);
    hipMemcpy(y, dy, size, hipMemcpyDeviceToHost);
    for (int i = 0; i < N; i++) {
        double expected = (2 * x[i] + 3.0) * (2 * x[i] + 3.0);
        double mismatch = (y[i] - expected) / expected;
        if (mismatch > 1e-5 || mismatch < -1e-5) {
            printf("Wrong at %d: %f\n", i, y[i]);
            correct = false;
        }
    }
    if (correct) {
        printf("Correct!\n");
    } else {
        printf("Wrong!\n");
    }
    gettimeofday(&te, NULL);
    t_check = (te.tv_sec - tb.tv_sec) * 1000.0 + (te.tv_usec - tb.tv_usec) / 1000.0;

    printf("Calculation took %8.3f ms\n", t_calc);
    printf("Validation  took %8.3f ms\n", t_check);

    hipFree(dx);
    hipFree(dy);
    free(x);
    free(y);
    return 0;
}
