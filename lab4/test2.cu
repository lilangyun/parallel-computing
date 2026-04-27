#include <stdio.h>
#include <cuda_runtime.h>

// __global__ 关键字：这个函数在GPU上执行，从CPU调用
__global__ void show_tid() {
    // 获取线程坐标（理解并行层次的关键）
    int tid = threadIdx.x;           // block内部的线程ID
    int bid = blockIdx.x;            // grid内部的block ID
    int bdim = blockDim.x;           // 每个block有多少线程
    
    printf("Block %d, Thread %d/%d\n", bid, tid, bdim);
}

int main() {
    // <<<grid_size, block_size>>> 语法
    // 启动 2 个block，每个block 4 个线程，总共 8 个GPU线程
    printf("启动配置：grid=2个block, block=4个线程\n");
    printf("预期看到：Block 0 的 Thread 0-3，Block 1 的 Thread 0-3\n\n");
    
    show_tid<<<2, 4>>>();
    
    cudaDeviceSynchronize();  // 等待GPU执行完成
  
    return 0;
}