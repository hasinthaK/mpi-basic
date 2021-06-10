#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>


#define SIZE 5
int main(int argc, char** argv) {
    
    // init variables
    int nprocs, myrank, send_recv_count, root;
    float sendbuf[SIZE][SIZE] = { {1.0, 2.0, 3.0, 4.0, 25.0}, {5.0, 6.0, 7.0, 8.0, 30.0},
                                {9.0, 10.0, 11.0, 12.0, 40.0}, {13.0, 14.0, 15.0, 16.0, 50.0}, {17.0, 18.0, 19.0, 20.0, 21.0} };
    float recvbuf[SIZE];

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    //get the rank of the root process
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    // get the MPI world size - num of processes
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    //init the process params
    root = 1;
    send_recv_count = SIZE;

    // scatter the sendbuffer value sets across processes
    MPI_Scatter(sendbuf, send_recv_count, MPI_FLOAT, recvbuf, send_recv_count, MPI_FLOAT, root, MPI_COMM_WORLD);

    // print the recieved values from subprocesses
    printf("rank= %d Results: %f %f %f %f, %f\n", myrank, recvbuf[0], recvbuf[1], recvbuf[2], recvbuf[3], recvbuf[4]);

    // sync all processes
    MPI_Barrier(MPI_COMM_WORLD);

    // Finalize the MPI environment.
    MPI_Finalize();

}


