/*  Cpt S 411, Introduction to Parallel Computing
 *  Trent Bultsma, Tanner Tegman, Programming project 2
 */

#include <mpi.h>
#include <stdio.h>
#include <sys/time.h>

int main(int argc,char *argv[]) {
    int rank,p;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&p);

    printf("my rank = %d\n",rank);
    printf("Rank = %d: number of processes = %d\n",rank,p);

    // TODO add code in here

    MPI_Finalize();
}