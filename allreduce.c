/*  Cpt S 411, Introduction to Parallel Computing
 *  Trent Bultsma, Tanner Tegman, Programming project 2
 */

#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

struct AllReduceResult {
    int localSum;
    int globalSum;
};

int computeLocalSum(int count) {
    int sum = 0;
    for (int i = 0; i < count; i++) {
        sum += rand() % 100;
    }
    return sum;
}

struct AllReduceResult allReduceNaive(int rank, int p, int n) {
    const int localSum = computeLocalSum(n / p);
    int sum = localSum;
    int receiveBuf[100];
    int sendBuf[100];

    const int srcRank = rank - 1;
    const int destRank = rank + 1;

    // sum pass

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

    // distribute pass

    if (destRank < p) { // not the end
        MPI_Status status;
        MPI_Recv(receiveBuf, 1, MPI_INTEGER, destRank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        int recievedSum = receiveBuf[0];
        sum = recievedSum;
    }
    if (srcRank >= 0) { // not the start
        sendBuf[0] = sum;
        MPI_Send(sendBuf, 1, MPI_INTEGER, srcRank, 0, MPI_COMM_WORLD);
    }

    struct AllReduceResult result;
    result.localSum = localSum;
    result.globalSum = sum;
    return result;
}

struct AllReduceResult allReduceHypercubic(int rank, int p, int n) {
    
    // TODO implement

    struct AllReduceResult result;
    result.localSum = 0;
    result.globalSum = 0;
    return result;
}

struct AllReduceResult allReduceMPI(int rank, int p, int n) {
    
    // TODO implement

    struct AllReduceResult result;
    result.localSum = 0;
    result.globalSum = 0;
    return result;
}

int main(int argc,char *argv[]) {
    int rank,p;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&p);

    if (p > 1 && p % 2 != 0) {
        printf("ERROR: p must be a power of 2\n");
        return 1;
    }

    srand(rank + 1);
    int n = 1;
    const int maxN = pow(2,20);

    printf("-- Naive approach --\n");

    n = 1;
    while (n <= maxN) {
        if (n <= p || n % p != 0) {
            n *= 2;
            continue;
        }
        struct AllReduceResult naiveResult = allReduceNaive(rank, p, n);
        printf("Rank = %d: n = %d local sum = %d global sum = %d\n",
            rank,n,naiveResult.localSum,naiveResult.globalSum);
        n *= 2;
    }

    // TODO uncomment this block when the algorithms are implemented and add timing functionality

    /*
    printf("-- Hypercubic approach --\n");

    n = 1;
    while (n <= maxN) {
        if (n <= p || n % p != 0) {
            n *= 2;
            continue;
        }
        struct AllReduceResult naiveResult = allReduceHypercubic(rank, p, n);
        printf("Rank = %d: n = %d local sum = %d global sum = %d\n",
            rank,n,naiveResult.localSum,naiveResult.globalSum);
        n *= 2;
    }

    printf("-- MPI approach --\n");

    n = 1;
    while (n <= maxN) {
        if (n <= p || n % p != 0) {
            n *= 2;
            continue;
        }
        struct AllReduceResult naiveResult = allReduceMPI(rank, p, n);
        printf("Rank = %d: n = %d local sum = %d global sum = %d\n",
            rank,n,naiveResult.localSum,naiveResult.globalSum);
        n *= 2;
    }
    */

    MPI_Finalize();
}