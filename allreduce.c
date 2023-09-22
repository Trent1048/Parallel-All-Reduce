/*  Cpt S 411, Introduction to Parallel Computing
 *  Trent Bultsma, Tanner Tegman, Programming project 2
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int localSum(int count) {
    time_t t;
    srand((unsigned) time(&t));

    int sum = 0;
    for (int i = 0; i < count; i++) {
        sum += rand() % 100;
    }
    return sum;
}

int main(int argc,char *argv[]) {
    int rank,p;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&p);

    printf("my rank = %d\n",rank);
    printf("Rank = %d: number of processes = %d\n",rank,p);

    printf("Local sum = %d\n",localSum(1));

    MPI_Finalize();
}