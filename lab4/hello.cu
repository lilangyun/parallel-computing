#include <stdio.h>
#include <cuda_runtime.h>

__global__ void add(int *a, int *b, int *c) {
    c[0] = a[0] + b[0];
}

int main() {
    printf("=== CUDA Computation Test ===\n");
    
    // Host 数据
    int a = 10, b = 20, c = 0;
    
    // Device 指针
    int *d_a, *d_b, *d_c;
    
    // 分配 GPU 内存
    cudaMalloc(&d_a, sizeof(int));
    cudaMalloc(&d_b, sizeof(int));
    cudaMalloc(&d_c, sizeof(int));
    
    // 拷贝到 GPU
    cudaMemcpy(d_a, &a, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, &b, sizeof(int), cudaMemcpyHostToDevice);
    
    printf("Before kernel: a=%d, b=%d, c=%d\n", a, b, c);
    
    // 启动 kernel
    add<<<1, 1>>>(d_a, d_b, d_c);
    
    // 等待完成
    cudaDeviceSynchronize();
    
    // 拷贝结果回 CPU
    cudaMemcpy(&c, d_c, sizeof(int), cudaMemcpyDeviceToHost);
    
    printf("After kernel: a=%d, b=%d, c=%d\n", a, b, c);
    
    // 清理
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);
    
    // 检查错误
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess) {
        printf("CUDA Error: %s\n", cudaGetErrorString(err));
    } else {
        printf("No CUDA errors.\n");
    }
    
    return 0;
}