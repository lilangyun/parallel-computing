#include <stdio.h>

int main () {
    int start, end, n, numthread;
    int *x, *Ap, *p, *x, *r;
    int rho0, pAp;

    // 4.3 计算步长 alpha = rho0 / (p·Ap)
    double alpha = rho0 / pAp;
    // 4.4 更新解 x 和残差 r：x = x + alpha*p; r = r - alpha*Ap
    for (int i = start; i < n; i += numthread) {
        x[i] += alpha * p[i];
        r[i] -= alpha * Ap[i];
    }


    return 0 ;
}