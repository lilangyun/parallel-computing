#include <stdio.h>
#include <cuda_runtime.h>

// 演示：block 内的线程通过共享内存合作，求数组的和
__global__ void shared_demo(int *result) {
    // __shared__ 变量：同一 block 内所有线程共享
    // 存储在片上缓存，访问速度极快（比全局内存快 100 倍以上）
    __shared__ int shared_data[256];
    
    int tid = threadIdx.y;      // 当前线程在 block 内的 ID (0~255)
    
    // 步骤1: 每个线程把自己的 tid 写入共享内存
    shared_data[tid] = tid;
    
    // 步骤2: 同步等待 —— 确保所有线程都写完共享内存
    // 如果没有这行，线程0可能读到其他线程还没写入的数据（脏数据）
    __syncthreads();
    
    // 步骤3: 让线程0负责把所有值加起来
    if (tid == 0) {
        int sum = 0;
        for (int i = 0; i < blockDim.y; i++) {
            sum += shared_data[i];   // 安全读取，其他线程已经写入完毕
        }
        result[0] = sum;
    }
}

int main() {
    int *host_result = (int*)malloc(sizeof(int));
    int *device_result;
    
    cudaMalloc(&device_result, sizeof(int));
    
    // 启动 1 个 block，256 个线程
    printf("启动配置：1 个 block，256 个线程\n");
    printf("Block 内有 256 个线程，共享同一个 shared_data[256] 数组\n\n");
    
    dim3 blocksize(1, 256, 1);
    shared_demo<<<1, blocksize>>>(device_result);
    
    // 把结果从 GPU 拷回 CPU
    cudaMemcpy(host_result, device_result, sizeof(int), cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
    
    printf("结果：0+1+2+...+255 = %d (期望 32640)\n", *host_result);
    
    free(host_result);
    cudaFree(device_result);
    
    return 0;
}