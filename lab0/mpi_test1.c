#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
   const int nProcess = 8, localSize = 8, globalSize = nProcess * localSize;
   int globalData[globalSize], localData[localSize], count[localSize], localSum[1];
   int comSize, comRank, i;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &comSize);
   MPI_Comm_rank(MPI_COMM_WORLD, &comRank);
   if (comRank == 0) {
       for (i = 0; i < globalSize; i++) {
           globalData[i] = i; // 初始化全局数据
       }
   }
   MPI_Scatter(globalData, localSize, MPI_INT, localData, localSize, MPI_INT, 0, MPI_COMM_WORLD);
   for (i = 0; i < localSize; i++) {
       count[i] = 1; // 每个进程接收一个结果
   }
   MPI_Reduce_scatter(localData, localSum, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
   printf("Rank %d received result: %d\n", comRank, localSum[0]);
   MPI_Finalize();
   return 0;
}