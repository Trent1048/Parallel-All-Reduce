/*  Cpt S 411, Introduction to Parallel Computing
 *  Trent Bultsma, Tanner Tegman, Programming project 2
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int computeLocalSum(int count) {
    int sum = 0;
    for (int i = 0; i < count; i++) {
        sum += rand() % 100;
    }
    return sum;
}

void allReduceNaive(int rank, int p, int n) {
    const int localSum = computeLocalSum(n / p);
    int sum = localSum;
    int receiveBuf[1];
    int sendBuf[1];

    const int srcRank = rank - 1;
    const int destRank = rank + 1;

    if (srcRank >= 0) { // not the start
        MPI_Status status;
        MPI_Recv(receiveBuf, 1, MPI_INTEGER, srcRank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        int recievedSum = receiveBuf[0];
        sum += recievedSum;
    }
    if (destRank < p) { // not the end
        sendBuf[0] = sum;
        MPI_Send(sendBuf, 1, MPI_INTEGER, destRank, 0, MPI_COMM_WORLD);
    }

    printf("Rank = %d: local sum = %d\n",rank,localSum);
    printf("Rank = %d: global sum = %d\n",rank,sum);
}

int main(int argc,char *argv[]) {
    int rank,p;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&p);

    int n = p * 10;
    srand(rank + 1);

    allReduceNaive(rank, p, n);

    MPI_Finalize();
}