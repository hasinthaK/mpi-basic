#include <stdlib.h>
#include <stdio.h>
#include <time.h>

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

    // initial vector to be sorted
    int *unsorted = (int*)malloc(sizeof(int) * elems);

    // populate teh unsorted vector with random numbers
    srand(time(NULL));
    for (int i = 0; i < elems; i++) {
        unsorted[i] = rand() % 100;
    }

    printf("Total elements: %d\n", elems);

    // print only the first 10 elems - unsorted
    print_vector((char*)"Unsorted", unsorted, elems);

    clock_t begin = clock();
    merge_sort(unsorted, 0, elems - 1);
    clock_t end = clock();

    // print only the first 10 elems - sorted
    print_vector((char*)"Sorted", unsorted, elems);

    // print the execution time
    printf("\nExecution duration: %f s", (double)(end - begin) / (double)CLOCKS_PER_SEC);
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
