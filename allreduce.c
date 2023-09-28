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
    int receiveBuf[100];
    int sendBuf[100];
    
    struct timeval startTime, stopTime;

    gettimeofday(&startTime, NULL);	
    const int localSum = computeLocalSum(n / p);
    int sum = localSum;
	gettimeofday(&stopTime, NULL);
    int localSumTime = (stopTime.tv_sec - startTime.tv_sec) * 1000000 + stopTime.tv_usec - startTime.tv_usec;
    if (rank == 0) {
        printf("-Naive- n = %d local sum time = %d ",n,localSumTime);
    }

    gettimeofday(&startTime, NULL);	
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

    gettimeofday(&stopTime, NULL);
    int parallelTime = (stopTime.tv_sec - startTime.tv_sec) * 1000000 + stopTime.tv_usec - startTime.tv_usec;
    if (rank == 0) {
        printf("total time = %d\n",parallelTime + localSumTime);
    }

    struct AllReduceResult result;
    result.localSum = localSum;
    result.globalSum = sum;    
    return result;
}

struct AllReduceResult allReduceHypercubic(int rank, int p, int n) 
{
    struct timeval startTime, stopTime;
    int partnerRank, sum, elapsedTime;
    int receiveBuf[100], sendBuf[100];

    struct AllReduceResult result;
    result.localSum = 0;
    result.globalSum = 0;

	gettimeofday(&startTime, NULL);	
    const int localSum = computeLocalSum(n / p);
	gettimeofday(&stopTime, NULL);	
    
    elapsedTime = (stopTime.tv_sec - startTime.tv_sec) * 1000000 + stopTime.tv_usec - startTime.tv_usec;
    
    if (rank == 0){
        printf("Hypercubic localSum elapsedTime: %d\n", elapsedTime);
    }
    
	gettimeofday(&startTime, NULL);	
    sum = localSum;

    for (int t = 0; t < log2(p); t++) {
        sendBuf[0] = sum;
        partnerRank = rank ^ t;
        MPI_Status status;
        MPI_Sendrecv(sendBuf, 1, MPI_INTEGER, partnerRank, 0,
            receiveBuf, 1, MPI_INTEGER, partnerRank, MPI_ANY_TAG,
            MPI_COMM_WORLD, &status);    
        int receivedSum = receiveBuf[0];
        // result.localSum += sendBuf[0] + receiveBuf[0];
        sum = receivedSum;
    }

	gettimeofday(&startTime, NULL);	
    elapsedTime = (stopTime.tv_sec - startTime.tv_sec) * 1000000 + stopTime.tv_usec - startTime.tv_usec;

    if (rank == 0){
        printf("Hypercubic Reduce elapsedTime: %d\n", elapsedTime);
    }
    
    result.localSum = localSum;
    result.globalSum = sum;
    return result;
}

struct AllReduceResult allReduceMPI(int rank, int p, int n) {
    struct timeval startTime, stopTime;
    int receiveBuf[100], sendBuf[100], sum = 0, elapsedTime;
    struct AllReduceResult result;
    result.localSum = 0;
    result.globalSum = 0;

	gettimeofday(&startTime, NULL);	
    const int localSum = computeLocalSum(n / p);
	gettimeofday(&stopTime, NULL);	
    
    if (rank == 0){
        printf("MPI localSum elapsedTime: %d\n", elapsedTime);
    }

    sum = localSum;
    
    sendBuf[0] = localSum;
	gettimeofday(&startTime, NULL);	
    MPI_Allreduce(sendBuf, receiveBuf, 1, MPI_INTEGER, MPI_SUM, MPI_COMM_WORLD);
    int receivedSum = receiveBuf[0];
    sum = receivedSum;
	gettimeofday(&stopTime, NULL);	

    if (rank == 0){
        printf("MPI Reduce elapsedTime: %d\n", elapsedTime);
    }

    result.localSum = localSum;
    result.globalSum = sum;
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

    if (rank == 0) {
        printf("-- Naive approach --\n");
    }

    n = 1;
    while (n <= maxN) {
        if (n <= p || n % p != 0) {
            n *= 2;
            continue;
        }
        struct AllReduceResult naiveResult = allReduceNaive(rank, p, n);
        //printf("-Naive- Rank = %d: n = %d local sum = %d global sum = %d\n",
        //    rank, n, naiveResult.localSum, naiveResult.globalSum);
        n *= 2;
    }

    if (rank == 0) {
        printf("---- End Naive Approach ----\n"); 
        printf("-- Hypercubic approach --\n");
    }

    n = 1;
    while (n <= maxN) {
        if (n <= p || n % p != 0) {
            n *= 2;
            continue;
        }
        struct AllReduceResult hypercubicResult = allReduceHypercubic(rank, p, n);
        //printf("-Hyper- Rank = %d: n = %d local sum = %d global sum = %d\n",
        //    rank, n, hypercubicResult.localSum, hypercubicResult.globalSum);
        n *= 2;
    }

    if (rank == 0) {
        printf("---- End Hypercubic Approach ----\n"); 
        printf("-- MPI approach --\n");
    }

    n = 1;
    while (n <= maxN) {
        if (n <= p || n % p != 0) {
            n *= 2;
            continue;
        }
        struct AllReduceResult mpiResult = allReduceMPI(rank, p, n);
        //printf("-MPI- Rank = %d: n = %d local sum = %d global sum = %d\n",
        //    rank, n, mpiResult.localSum, mpiResult.globalSum);
        n *= 2;
    }

    if (rank == 0) {
        printf("---- End MPI Approach ----\n"); 
    }

    MPI_Finalize();
}