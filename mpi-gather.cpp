#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


int main(int argc, char** argv) {
    
    int myid, nprocs, root;
    int *sendarray, *recv_buffer = NULL;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    //get the rank of the root process
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    // get the MPI world size - num of processes
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    //init the process params
    root = 0;
    int nums_count = 100;

    // if this process is the root process
    // allocate memory for subprocess data collection
    if (myid == root) {
        recv_buffer = (int *)malloc(sizeof(int) * nprocs * nums_count);
    }

    // init seed for random generator
    srand(time(NULL));

    // init the sendarray with random values (1-10)
    sendarray = (int*)malloc(sizeof(int) * nums_count);
    for (int i = 0; i < nums_count; i++) {
        sendarray[i] = (rand() % 10);
    }

    // gather data from all the sub processes
    // send if process is not the root
    // gather if the process is the root
    MPI_Gather(sendarray, nums_count, MPI_INT, recv_buffer, nums_count, MPI_INT, root, MPI_COMM_WORLD);

    // Print initial values from the recev_buff
    if (myid == root) {
        // print the head/some values from the root process which are gathered from sub processes
        for (int i = 0; i < 10; i++) {
            printf("recev_buffer head: %d\n", recv_buffer[i]);
        }
    }

    // sync all processes
    MPI_Barrier(MPI_COMM_WORLD);

    // Finalize the MPI environment.
    MPI_Finalize();

}
