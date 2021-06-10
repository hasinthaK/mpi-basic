#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>


#define NCOLS 8
int main(int argc, char** argv) {
    
    // init variables
    int myrank, nprocs, root;

    // resulting vectors - final & each process
    float A[NCOLS], Apart[NCOLS];
    // initial matrix
    float B[NCOLS][NCOLS], Bpart[NCOLS];
    // initial vector
    float C[NCOLS], Cpart[1];
    // resulting vector for serial computation
    float A_exact[NCOLS];

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    //get the rank of the root process
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    // get the MPI world size - num of processes
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    //init the process params
    root = 1;

    // init operands in the root process
    // populate the initial matrix(B) & vector(C) with random numbers
    if (myrank == root) {
        for(int row = 0; row < NCOLS; row++) {
            for (int col = 0; col < NCOLS; col++) {
                B[row][col] = row + col + 2;
            }
            C[row] = row + 3;
        }
    }

    // sync all processes
    MPI_Barrier(MPI_COMM_WORLD);

    // scatter the matrix B by rows
    MPI_Scatter(B, NCOLS, MPI_FLOAT, Bpart, NCOLS, MPI_FLOAT, root, MPI_COMM_WORLD);

    // scatter the vector C by parts
    MPI_Scatter(C, 1, MPI_FLOAT, Cpart, 1, MPI_FLOAT, root, MPI_COMM_WORLD);

    // perform the vector-scalar multiplication in each subprocess
    // & assign the result to corresponding Apart
    // recieved vector - Bpart, recieved scalar - Cpart[0]
    for (int i = 0; i < NCOLS; i++) {
        Apart[i] = Bpart[i] * Cpart[0];
    }

    // get the above parallely computed vector-scalar multiplication
    // reduced to matrix A - get the summation of each process multiplication
    MPI_Reduce(Apart, A, NCOLS, MPI_FLOAT, MPI_SUM, root, MPI_COMM_WORLD);

    // print the resulting matrix A from root process
    if (myrank == root) {
        printf("Parallel execution result: \n");
        for (int row = 0; row < NCOLS; row++) {
            printf("A[%d] = %g\n", row, A[row]);
        }

        printf("\nSequintial execution result: \n");
        // compute sequintially
        for(int row = 0; row < NCOLS; row++) {
            A_exact[row] = 0.0;
            for (int col = 0; col < NCOLS; col++) {
                A_exact[row] += C[col] * B[col][row];
            }
        }

        for (int row = 0; row < NCOLS; row++) {
            printf("A_exact[%d] = %g\n", row, A_exact[row]);
        }
    }
        

    // Finalize the MPI environment.
    MPI_Finalize();

}

