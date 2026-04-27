#include <stdio.h>
#include <cuda_runtime.h>

int main() {
    int deviceCount;
    cudaError_t err;
    
    err = cudaGetDeviceCount(&deviceCount);
    printf("cudaGetDeviceCount 返回值: %d\n", err);
    printf("设备数量: %d\n", deviceCount);
    
    if (err != cudaSuccess) {
        printf("错误信息: %s\n", cudaGetErrorString(err));
    }
    
    if (deviceCount > 0) {
        cudaDeviceProp prop;
        err = cudaGetDeviceProperties(&prop, 0);
        printf("获取设备属性返回值: %d\n", err);
        printf("设备名称: %s\n", prop.name);
        printf("计算能力: %d.%d\n", prop.major, prop.minor);
    }
    
    return 0;
}