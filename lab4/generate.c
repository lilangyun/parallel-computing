/* 生成数据 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <matrix_size_power>\n", argv[0]);
        fprintf(stderr, "Example: %s 3 (generates 10^3 = 1000 sized matrix)\n", argv[0]);
        return -1;
    }
    
    int power = atoi(argv[1]);
    int n = (int)pow(10, power);  // 矩阵维度 N = 10^power
    
    // 设置稀疏度（每行平均非零元素个数）
    int avg_nnz_per_row = 10;  // 平均每行10个非零元素
    int total_nnz = n * avg_nnz_per_row;
    
    // 动态分配数组
    int *row_ptr = (int *)malloc((n + 1) * sizeof(int));
    int *col_idx = (int *)malloc(total_nnz * sizeof(int));
    double *values = (double *)malloc(total_nnz * sizeof(double));
    double *b = (double *)malloc(n * sizeof(double));
    
    if (!row_ptr || !col_idx || !values || !b) {
        fprintf(stderr, "Memory allocation failed!\n");
        return -1;
    }
    
    // 设置随机种子
    srand(time(NULL));
    
    // 生成CSR格式的稀疏矩阵（对角占优的三对角为主，加一些随机非零元素）
    row_ptr[0] = 0;
    int nnz_index = 0;
    
    for (int i = 0; i < n; i++) {
        row_ptr[i] = nnz_index;
        
        // 确保每行至少有1个非零元素（对角线元素）
        int nnz_this_row = 1 + (rand() % (avg_nnz_per_row * 2)); // 1 到 avg_nnz_per_row*2 个非零
        
        // 记录本行已添加的列，避免重复
        int *cols_added = (int *)calloc(n, sizeof(int));
        
        for (int j = 0; j < nnz_this_row && nnz_index < total_nnz; j++) {
            int col;
            double value;
            
            if (j == 0) {
                // 第一个非零元素放在对角线位置，保证矩阵可逆
                col = i;
                // 对角占优：对角线元素大于同行其他元素之和
                value = (double)(rand() % 100 + 50); // 50-149
            } else {
                // 其他非零元素随机选择列
                do {
                    col = rand() % n;
                } while (col == i || cols_added[col] == 1); // 避免重复和对角线重复
                
                // 随机值，但绝对值小于对角线的一半（保证对角占优）
                value = (double)(rand() % 40 - 20); // -20 到 19
            }
            
            cols_added[col] = 1;
            col_idx[nnz_index] = col;
            values[nnz_index] = value;
            nnz_index++;
        }
        
        free(cols_added);
    }
    
    row_ptr[n] = nnz_index;
    
    // 如果实际非零元素少于预期，调整数组大小
    int actual_nnz = nnz_index;
    
    // 生成向量 b（随机值）
    for (int i = 0; i < n; i++) {
        b[i] = (double)(rand() % 100) / 10.0; // 0.0 到 9.9
    }
    
    // 写入文件
    char matrixName[100], vectorName[100];
    sprintf(matrixName, "matrix_%d.txt", n); 
    sprintf(vectorName, "vector_%d.txt", n);
    FILE *fd1 = fopen(matrixName, "w");
    FILE *fd2 = fopen(vectorName, "w");
    
    if (fd1 == NULL || fd2 == NULL) {
        fprintf(stderr, "File open error!\n");
        free(row_ptr);
        free(col_idx);
        free(values);
        free(b);
        return -1;
    }
    
    // 写入 matrix.txt
    fprintf(fd1, "%d\n", n);
    
    // 写入 row_ptr
    for (int i = 0; i <= n; i++) {
        fprintf(fd1, "%d", row_ptr[i]);
        if (i < n) fprintf(fd1, " ");
    }
    fprintf(fd1, "\n");
    
    // 写入 col_idx
    for (int i = 0; i < actual_nnz; i++) {
        fprintf(fd1, "%d", col_idx[i]);
        if (i < actual_nnz - 1) fprintf(fd1, " ");
    }
    fprintf(fd1, "\n");
    
    // 写入 values
    for (int i = 0; i < actual_nnz; i++) {
        fprintf(fd1, "%.1f", values[i]);
        if (i < actual_nnz - 1) fprintf(fd1, " ");
    }
    fprintf(fd1, "\n");
    
    // 写入 vector.txt
    for (int i = 0; i < n; i++) {
        fprintf(fd2, "%.1f", b[i]);
        if (i < n - 1) fprintf(fd2, " ");
    }
    fprintf(fd2, "%.1f", b[0]);
    for (int i = 1; i < n; i++) {
        fprintf(fd2, " %.1f", b[i]);
    }
    fprintf(fd2, "\n");

    printf("Data generation completed!\n");
    printf("Matrix size: %d x %d\n", n, n);
    printf("Non-zero elements: %d\n", actual_nnz);
    printf("Sparsity: %.2f%%\n", (1.0 - (double)actual_nnz / ((double)n * n)) * 100);
    printf("Files created: %s and %s\n", matrixName, vectorName);
    
    // 清理
    fclose(fd1);
    fclose(fd2);
    free(row_ptr);
    free(col_idx);
    free(values);
    free(b);
    
    return 0;
}