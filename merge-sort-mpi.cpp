#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

// helper method for printing vectors
void print_vector(char* prompt, int *vector, int no_elems);

// merge sort method
void merge_sort(int *arr, int l, int r);

// helper method merge two halves
void merge(int *arr, int l, int m, int r);


int main(int argc, char** argv) {

    // get cli input for array size
    int elems = 10;
    if (argv[1])
        elems = atoi(argv[1]);

    // initial vector to be sorted & the final sorted vector
    int *unsorted = (int*)malloc(sizeof(int) * elems);
    int* sorted = NULL;

    // populate the unsorted vector with random numbers
    srand(time(NULL));
    for (int i = 0; i < elems; i++) {
        unsorted[i] = rand() % 100;
    }

    // init mpi
    int my_rank;
    int world_size;
    int root = 0;

    // time for each subprocess & the max time
    double my_time, max_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);   

    // Divide the unsorted array in equal-sized chunks
    // to be sent to sub processes
    int size = elems / world_size;

    // Send each divided subarray to each process
    int* sub_array = (int *)malloc(sizeof(int) * size);
    MPI_Scatter(unsorted, size, MPI_INT, sub_array, size, MPI_INT, root, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    my_time = MPI_Wtime();
    // Perform the mergesort on each process
    merge_sort(sub_array, 0, (size - 1));
    my_time = MPI_Wtime() - my_time;

    // Gather the sorted subarrays into one in the root process
    if (my_rank == root)
        sorted = (int *)malloc(sizeof(int) * elems);

    MPI_Gather(sub_array, size, MPI_INT, sorted, size, MPI_INT, root, MPI_COMM_WORLD);
    // get the max time for exec by a sub process
    MPI_Reduce(&my_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, root, MPI_COMM_WORLD);

    if (my_rank == root) {

        clock_t begin = clock();
        // final sort call to sort the sorted two arrays
        merge_sort(sorted, 0, (elems - 1));
        clock_t end = clock();

        printf("\nTotal # elements: %d\n", elems);

        // print only the first 10 elems - unsorted & sorted
        print_vector((char*)"Unsorted", unsorted, elems);      
        print_vector((char*)"Sorted", sorted, elems);

        // print the execution times
        printf("\nMax exec time by a process: %f s", max_time);
        double total_time = ((double)(end - begin) / (double)CLOCKS_PER_SEC) + max_time;
        printf("\nTotal exec time: %f s\n", total_time);
    }

    // finalize the MPI env
    MPI_Finalize();
}

// l is for left index and r is right index of the
// sub-array of arr to be sorted
// initially - l = 0, r = length of unsorted array
void merge_sort(int *arr, int l, int r)
{
    if (l < r) {
        // get the middle index
        int m = l + (r - l) / 2;

        // Sort first and second halves - divide
        merge_sort(arr, l, m);
        merge_sort(arr, m + 1, r);

        // merge the two halves - conquer
        merge(arr, l, m, r);
    }
}

// merge two halves together
void merge(int *arr, int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    // create temp arrays
    int* L = (int*)malloc(sizeof(int) * n1);
    int* R = (int*)malloc(sizeof(int) * n2);

    //Copy data to temp arrays L[] and R[]
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    // Merge the temp arrays back into arr[l..r]
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray

    // sort the subarrays while merging
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    //Copy the remaining elements of L[], if there are any
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    //Copy the remaining elements of R[], if there are any
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

// print a given vector
void print_vector(char* prompt, int *vector, int no_elems)
{
    char* prompt_add = (char *)" :";
    if (no_elems > 20) {
        no_elems = 10;
        prompt_add = (char *)" first 10 elems: ";
    }

    printf("\n%s %s\n", prompt, prompt_add);
    for (int i = 0; i < no_elems; i++) {
        printf(" %d,", vector[i]);
    }
    printf("\n");
}
