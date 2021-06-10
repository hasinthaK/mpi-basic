#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define NCOLS 5

// helper method for printing matrices
void print_matrix(char* prompt, float matrix[NCOLS][NCOLS])
{
    printf("\n%s\n", prompt);
    for (int i = 0; i < NCOLS; i++) {
        for (int j = 0; j < NCOLS; j++) {
            printf(" %g", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char** argv) {

    // init variables
    int myrank, nprocs, root;

    // resulting matrix & computed vector by each process
    float A[NCOLS][NCOLS], Apart[NCOLS];
    // initial matrix & received vector by each process
    float B[NCOLS][NCOLS], Bpart[NCOLS];
    // initial matrix2 - accessed by every process
    float C[NCOLS][NCOLS];
    // resulting matrix for serial computation
    float A_exact[NCOLS][NCOLS];


    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    //get the rank of the root process
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    // get the MPI world size - num of processes
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    //init the process params
    root = 1;

    // populate the initial matrix(B) & matrix2(C) with random numbers
    srand(time(NULL)); // seed for random num generator below - distinct values
    for (int row = 0; row < NCOLS; row++) {
        for (int col = 0; col < NCOLS; col++) {
            B[row][col] = rand() % 10;
            C[row][col] = rand() % 5;
        }
    }

    // scatter the matrix B by rows
    MPI_Scatter(B, NCOLS, MPI_FLOAT, Bpart, NCOLS, MPI_FLOAT, root, MPI_COMM_WORLD);

    // perform the vector-scalar multiplication in each subprocess
    // & assign the result to corresponding Apart
    // recieved vector - Bpart, multiply by each scalar value of C
    float sum = 0.0;
    for (int i = 0; i < NCOLS; i++) {
        for (int j = 0; j < NCOLS; j++) {
            sum += Bpart[j] * C[j][i];
        }
        Apart[i] = sum;
        sum = 0.0;
    }

    // get the above parallely computed vector-scalar multiplication
    // reduced to matrix A - get the summation of each process multiplication
    //MPI_Reduce(Apart, A, NCOLS * NCOLS, MPI_FLOAT, MPI_SUM, root, MPI_COMM_WORLD);
    MPI_Gather(Apart, NCOLS, MPI_FLOAT, A, NCOLS, MPI_FLOAT, root, MPI_COMM_WORLD);

    // sync all processes
    MPI_Barrier(MPI_COMM_WORLD);

    // Finalize the MPI environment.
    MPI_Finalize();

    // print the resulting matrix A from root process
    if (myrank == root) {
        printf("Original matrices: \n");
        print_matrix((char*)"B = ", B);
        print_matrix((char*)"C = ", C);

        printf("Parallel execution result: \n");
        print_matrix((char *)"A = ", A);

        printf("\nSequintial execution result: \n");
        // compute sequintially
        for (int i = 0; i < NCOLS; i++) {
            for (int j = 0; j < NCOLS; j++) {
                A_exact[i][j] = 0.0;
                for (int k = 0; k < NCOLS; k++) {
                    A_exact[i][j] += B[i][k] * C[k][j];
                }
            }
        }

        print_matrix((char*)"A exact = ", A_exact);
    }

}


