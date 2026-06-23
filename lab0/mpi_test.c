#include <stdio.h>
#include <mpi.h>

/* Using mpi node to get pi*/
double f(double x)
{
    return 4.0 / (double)(1.0 + x * x);
}

int main(int argc, char *argv[])
{
    int myid, num_procs;
    long long num_steps = 1000000;
    double x;
    double step = 1.0 / (double)(num_steps);  // 步长
    double pi, mypi = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    for(int i = myid + 1; i <= num_steps; i += num_procs)
    {
        x = (i - 0.5) * 1.0 * step;
        mypi += f(x);
    }
    mypi *= step;
    printf("My  pi is: %.10lf.\n", mypi);
    fflush(stdout);
    MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    if(myid == 0)
    {
        printf("The pi is: %.10lf.\n", pi);
    }
    
    MPI_Finalize();
    return 0;
}