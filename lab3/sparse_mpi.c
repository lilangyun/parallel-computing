#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "mpi.h"

void cg_serial(int n, const double* values, const int* row_ptr, const int* col_idx,
    const double* b, double* x, int max_iter, double tol);

int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Parameter falied!\n");
        exit(-1);
    }
    // MPI initialize
    MPI_Init(&argc, &argv);
    
    /* Input begin */
    FILE* matrix_fd = fopen(argv[1], "r");
    FILE* vector_fd = fopen(argv[2], "r");
    if (matrix_fd == NULL || vector_fd == NULL) {
        fprintf(stderr, "[ERROR] matrix.txt or vector.txt fails to be open!\n");
    }
    int n;  // matrix dimension
    double *values; int *row_ptr; int *col_idx;  // csr matrix
    double *x; double *b;
    double tol = 1e-5;
    fscanf(matrix_fd, "%d", &n);
    row_ptr = (int *)malloc((n+1)*sizeof(int));
    for(int i=0;i<n+1;i++){
        fscanf(matrix_fd, "%d", &row_ptr[i]);
    }
    values = (double *)malloc(row_ptr[n]*sizeof(double));
    col_idx = (int *)malloc(row_ptr[n]*sizeof(int));
    for(int i=0;i<row_ptr[n];i++){
        fscanf(matrix_fd, "%d", &col_idx[i]);
    }
    for(int i=0;i<row_ptr[n];i++){
        fscanf(matrix_fd, "%lf", &values[i]);
    }
    b = (double*)malloc(n*sizeof(double));
    for(int i=0;i<n;i++){
        fscanf(vector_fd, "%lf", &b[i]);
    }
    x = (double *)malloc(n*sizeof(double));
    /* Input end*/

    cg_serial(n, values, row_ptr, col_idx, b, x, 2000, tol);

    /* End Handler */
    free(values);
    free(row_ptr);
    free(col_idx);
    free(x);
    free(b);
    fclose(matrix_fd);
    fclose(vector_fd);
    MPI_Finalize();
    // MPI End
}

// 输入：n(矩阵维度), values(非零元素), row_ptr(行偏移), col_idx(列索引), b(右端向量),maxiter(最大迭代次数), tol(收敛精度)// 输出：x(解向量，函数内会覆盖初始值)
void cg_serial(int n, const double* values, const int* row_ptr, const int* col_idx,const double* b, double* x, int max_iter, double tol) {
    // 1. 内存分配并检查是否成功
    double *r = (double*)malloc(n * sizeof(double));double *p = (double*)malloc(n * sizeof(double));double *Ap = (double*)malloc(n * sizeof(double));
    if (r == NULL || p == NULL || Ap == NULL) {
        // 内存分配失败，直接返回
        return;
    }
    // 分配全局global_p
    double *global_p = (double *)malloc(n * sizeof(double));
    // 2. 初始化：x=0, r=b-Ax=b, p=r
    for (int i = 0; i < n; i++) {
        x[i] = 0.0;
        r[i] = b[i];
        p[i] = r[i];
        global_p[i] = r[i];
    }
    // 3. 计算初始残差内积 rho0 = r₀·r₀
    double rho0 = 0.0;
    for (int i = 0; i < n; i++) {
        rho0 += r[i] * r[i];
    }
    if (sqrt(rho0) < tol) { // 初始残差范数已满足精度，直接返回
        free(r); free(p); free(Ap);
        return;
    }

    // MPI Begining
    int myid, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    int block = n / num_procs;

    /* Assign recvcounts and displs */
    int *recvcounts = (int *)malloc(num_procs * sizeof(int));
    int *displs = (int *)malloc(num_procs * sizeof(int));
    for( int i = 0; i < num_procs; i++){
        recvcounts[i] = (i == (num_procs - 1)) ?  n - i * block : block;
        displs[i] = i * block;
    }
    /* Assign Ending */
    double *global_x = (myid == 0) ? (double *)malloc(n * sizeof(double)) : NULL;
    int start = myid * block;
    int end = (myid == num_procs - 1) ? n : start + block;
    double pAp = 0.0;
    double rho_new = 0.0;

    // 4. 迭代核心逻辑
    for (int iter = 0; iter < max_iter; iter++) {
        // 4.1 计算 Ap = A*p
        for (int i = start; i < end; i++) {
            // 初始化输出向量为 0
            Ap[i] = 0.0;
            // 遍历第 i 行的所有非零元素
            for (int j = row_ptr[i]; j < row_ptr[i+1]; j++) {
                Ap[i] += values[j] * global_p[col_idx[j]];
            }
        }
        // 4.2 计算 p·Ap，避免除以 0
        double my_pAp = 0.0;
        pAp = 0.0;
        for (int i = start; i < end; i++) {
            my_pAp += p[i] * Ap[i];
        }
        MPI_Allreduce(&my_pAp, &pAp, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        if (fabs(pAp) < 1e-15) { // 分母接近 0，停止迭代
            break;
        }
        // 4.3 计算步长 alpha = rho0 / (p·Ap)
        double alpha = rho0 / pAp;
        // 4.4 更新解 x 和残差 r：x = x + alpha*p; r = r - alpha*Ap
        for (int i = start; i < end; i++) {
            x[i] += alpha * p[i];
            r[i] -= alpha * Ap[i];
        }
        // 4.5 计算新残差内积并判断收敛
        double my_rho_new = 0.0;
        rho_new = 0.0;
        for (int i = start; i < end; i++) {
            my_rho_new += r[i] * r[i];
        }
        MPI_Allreduce(&my_rho_new, &rho_new, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        double res_norm = sqrt(rho_new); // 残差 L2 范数
        if (res_norm < tol) { // 满足收敛精度，提前退出
            if(!myid){
                printf("迭代 %d 次后收敛，残差范数：%.6e\n", iter+1, res_norm);
            }
            break;
        }
        // 4.6 计算 beta 并更新搜索方向 p：p = r + beta*p
        double beta = rho_new / rho0;
        for (int i = start; i < end; i++) {
            p[i] = r[i] + beta * p[i];
        }
        MPI_Allgatherv(p + start, end - start, MPI_DOUBLE, global_p, recvcounts, displs, MPI_DOUBLE, MPI_COMM_WORLD);
        // 4.7 更新 rho0 为下一次迭代使用
        rho0 = rho_new;
    }
    // 更新全局x值
    MPI_Gatherv(x + start, end - start, MPI_DOUBLE, global_x, recvcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if(!myid){
        FILE *fd = fopen("result.txt", "w");
        for(int i=0; i < n; i++){
            fprintf(fd, "%lf ", global_x[i]);
        }
        fprintf(fd, "\n");
    }
    // 5. 释放内存（避免内存泄漏）
    free(r);
    free(p);
    free(Ap);
}