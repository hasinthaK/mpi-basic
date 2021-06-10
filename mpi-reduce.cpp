#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

// subprocess execution
void work() {
    for (int i = 0; i < INT16_MAX; i++) {
        // simulate workload
    }
}

int main(int argc, char** argv) {
    
    int myrank, nprocs, root;
    int *sendarray, *recv_buffer = NULL;
    double mytime, maxtime, mintime, avgtime;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    //get the rank of the root process
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    // get the MPI world size - num of processes
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    //init the process params
    root = 0;


    // execute the processes in each process & collect time stats
    // get time just before work section for each process
    mytime = MPI_Wtime();
    
    // execute the processes
    work();

    // calculate the execution time taken for each process
    mytime = MPI_Wtime() - mytime;

    //compute max, min, and average timing statistics & receive to root process variables
    MPI_Reduce(&mytime, &maxtime, 1, MPI_DOUBLE, MPI_MAX, root, MPI_COMM_WORLD);
    MPI_Reduce(&mytime, &mintime, 1, MPI_DOUBLE, MPI_MIN, root, MPI_COMM_WORLD);
    MPI_Reduce(&mytime, &avgtime, 1, MPI_DOUBLE, MPI_SUM, root, MPI_COMM_WORLD);

    // display the subprocess time stats from root process
    if (myrank == root) {
        avgtime /= nprocs;
        printf("Min: %lf, Max: %lf, Avg: %lf\n", mintime, maxtime, avgtime);
    }

    // sync all processes
    MPI_Barrier(MPI_COMM_WORLD);

    // Finalize the MPI environment.
    MPI_Finalize();

}


