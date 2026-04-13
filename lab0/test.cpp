#include<stdio.h>
#include<stdlib.h>
using namespace std;

int main(void){
    double x;
    long long n;
    scanf("%lf %d",&x, &n);

    double sum = 0.0;
    double tmp = x;
    for(int i=0;i<=n;i++){
        if(i%2==1){
            sum -= tmp;
        }
        else{
            sum += tmp;
        }
        tmp = tmp*x*x/((2*i+2)*(2*i+3));
    }
    printf("%lf\n",sum);
    return 0;
}