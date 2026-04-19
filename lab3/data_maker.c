#include <stdio.h>
#include <math.h>

int main(){
    FILE *fd1 = fopen("m.txt", "w");
    FILE *fd2 = fopen("v.txt", "w");
    if(fd1 == NULL || fd2 == NULL){
        fprintf(stderr, "File open error!\n");
        return -1;
    }
    int n = pow(10, 6);
    fprintf(fd1, "%d\n", n);
    for(int i = 0; i < n; i++){
        
    }
    return 0;
}