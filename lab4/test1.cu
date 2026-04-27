#include <stdio.h>
#include <cuda_runtime.h>

int main() {
    // 1. 查询GPU设备数量
    int deviceCount;
    cudaGetDeviceCount(&deviceCount);
    printf("找到 %d 个CUDA设备\n", deviceCount);
    
    // 2. 获取第一个GPU的详细信息
    if (deviceCount > 0) {
        cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop, 0);
        printf("设备名称: %s\n", prop.name);
        printf("计算能力: %d.%d\n", prop.major, prop.minor);
        printf("SM数量: %d\n", prop.multiProcessorCount);
        printf("每个SM最大线程数: %d\n", prop.maxThreadsPerMultiProcessor);
        printf("每个block最大线程数: %d\n", prop.maxThreadsPerBlock);
    }
    
    return 0;
}