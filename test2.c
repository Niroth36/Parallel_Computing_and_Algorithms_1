#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Comparison function for qsort
int compare_int(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}


int main(int argc, char* argv[]) {
    int N;  // size of large array A
    
    int my_A[N/4];  // local array for each process
    int rank, num_procs;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

       // read in the value of n
   if (rank == 0) {
      printf("Enter the value of n: ");
      scanf("%d", &N);
   }

   int A[N];  // large array A
   
    // broadcast the value of n to all processes
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Seed the random number generator (only done by process 0)
    if (rank == 0) {
        srand(time(NULL));
    }

    // Generate 4 random datasets in A (only done by process 0)
    if (rank == 0) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < N/4; j++) {
                A[i*N/4 + j] = rand() % 100 + 1;  // random number from 1 to 100
            }
        }
    }

    // Divide the data in A among the processes using MPI_Scatter
    MPI_Scatter(A, N/4, MPI_INT, my_A, N/4, MPI_INT, 0, MPI_COMM_WORLD);

    // Each process sorts its local array using qsort
    qsort(my_A, N/4, sizeof(int), compare_int);

    // Print the sorted numbers to the terminal
    for (int i = 0; i < N/4; i++) {
        printf("Process %d: %d\n", rank, my_A[i]);
    }

    MPI_Finalize();
    return 0;
}

