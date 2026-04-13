#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <assert.h>

void spmv_csr(int n, const double* values, const int* row_ptr, const int* col_idx,
    const double* x, double* y);

void cg_serial(int n, const double* values, const int* row_ptr, const int* col_idx,
    const double* b, double* x, int max_iter, double tol);

int n;  // matrix dimension

double *values;int *row_ptr;int *col_idx;  // csr matrix 

double *x;double *b; 

double tol = 1e-5;

int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Parameter falied!\n");
        exit(-1);
    }

    FILE* matrix_fd = fopen(argv[1], "r");
    FILE* vector_fd = fopen(argv[2], "r");
    // FILE* matrix_fd = fopen("matrix.txt", "r");
    // FILE* vector_fd = fopen("vector.txt", "r");

    fscanf(matrix_fd, "%d", &n);

    row_ptr = (int *)malloc((n+1)*sizeof(int));

    for(int i=0;i<n+1;i++){
        fscanf(matrix_fd, "%d", &row_ptr[i]);
    }

    values = (double *)malloc(row_ptr[n]*sizeof(double));
    col_idx = (int *)malloc(row_ptr[n]*sizeof(int));
    
    for(int i=0;i<row_ptr[n];i++){
        fscanf(matrix_fd, "%d", &col_idx[i]);
        // printf("%d ",col_idx[i]);
    }
    for(int i=0;i<row_ptr[n];i++){
        fscanf(matrix_fd, "%lf", &values[i]);
    }

    b = (double*)malloc(n*sizeof(double));
    for(int i=0;i<n;i++){
        fscanf(vector_fd, "%lf", &b[i]);
    }

    // cg
    x = (double *)malloc(n*sizeof(double));
    cg_serial(n, values, row_ptr, col_idx, b, x, 100, 1e-5);

    // print
    for(int i=0;i<n;i++){
        printf("%lf ", x[i]);
    }
    printf("\n");

    free(values);
    free(row_ptr);
    free(col_idx);
    free(x);
    free(b);

    fclose(matrix_fd);
    fclose(vector_fd);
}

// 输入：n(矩阵维度), values(非零元素), row_ptr(行偏移), col_idx(列索引), b(右端向量),maxiter(最大迭代次数), tol(收敛精度)// 输出：x(解向量，函数内会覆盖初始值)
void cg_serial(int n, const double* values, const int* row_ptr, const int* col_idx,const double* b, double* x, int max_iter, double tol) {
    // 1. 内存分配并检查是否成功
    double *r = (double*)malloc(n * sizeof(double));double *p = (double*)malloc(n * sizeof(double));double *Ap = (double*)malloc(n * sizeof(double));
    if (r == NULL || p == NULL || Ap == NULL) {
        // 内存分配失败，直接返回
        return;
    }
    // 2. 初始化：x=0, r=b-Ax=b, p=r
    for (int i = 0; i < n; i++) {
        x[i] = 0.0;
        r[i] = b[i];
        p[i] = r[i];
    }
    // 3. 计算初始残差内积 rho0 = r₀·r₀
    double rho0 = 0.0;
    for (int i = 0; i < n; i++) {
        rho0 += r[i] * r[i];
    }
    // 初始残差范数已满足精度，直接返回
    if (sqrt(rho0) < tol) {
        free(r); free(p); free(Ap);return;
    }
    // 4. 迭代核心逻辑
    for (int iter = 0; iter < max_iter; iter++) {
        // 4.1 计算 Ap = A*p
        spmv_csr(n, values, row_ptr, col_idx, p, Ap);
        // 4.2 计算 p·Ap，避免除以 0
        double pAp = 0.0;
        for (int i = 0; i < n; i++) {pAp += p[i] * Ap[i];}
        if (fabs(pAp) < 1e-15) { // 分母接近 0，停止迭代
            break;
        }
        // 4.3 计算步长 alpha = rho0 / (p·Ap)
        double alpha = rho0 / pAp;
        // 4.4 更新解 x 和残差 r：x = x + alpha*p; r = r - alpha*Ap
        for (int i = 0; i < n; i++) {
            x[i] += alpha * p[i];
            r[i] -= alpha * Ap[i];
        }
        // 4.5 计算新残差内积并判断收敛
        double rho_new = 0.0;
        for (int i = 0; i < n; i++) {rho_new += r[i] * r[i];}
        double res_norm = sqrt(rho_new); // 残差 L2 范数
        if (res_norm < tol) { // 满足收敛精度，提前退出
            printf("迭代 %d 次后收敛，残差范数：%.6e\n", iter+1, res_norm);break;
        }
        // 4.6 计算 beta 并更新搜索方向 p：p = r + beta*p
        double beta = rho_new / rho0;for (int i = 0; i < n; i++) {p[i] = r[i] + beta * p[i];}
        // 4.7 更新 rho0 为下一次迭代使用
        rho0 = rho_new;
    }
    // 5. 释放内存（避免内存泄漏）
    free(r);
    free(p);
    free(Ap);
}

// 稀疏矩阵-向量乘法 (CSR 格式)
// 输入：n(矩阵维度), values(非零元素值), row_ptr(行偏移), col_idx(列索引), x(输入向量)
// 输出：y(输出向量，y = A*x)
void spmv_csr(int n, const double* values, const int* row_ptr, const int* col_idx,const double* x, double* y) {
    // 初始化输出向量为 0
    for (int i = 0; i < n; i++) 
    {
        y[i] = 0.0;
    }
    // CSR 格式矩阵乘向量核心逻辑
    for (int i = 0; i < n; i++) {
        // 遍历第 i 行的所有非零元素
        for (int j = row_ptr[i]; j < row_ptr[i+1]; j++) {
            y[i] += values[j] * x[col_idx[j]];
        }
    }
}