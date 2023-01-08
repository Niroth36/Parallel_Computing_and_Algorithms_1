#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

// Comparison function for qsort
int cmpfunc(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}


// Compute partner function
int Compute_partner(int phase, int my_rank, int comm_size) {
    int partner;
    if (phase % 2 == 0) {           // Even phase
        if (my_rank % 2 != 0) {     // Odd rank
            partner = my_rank - 1;
        } else {                    // Even rank
            partner = my_rank + 1;
        }
    } else {                        // Odd phase
        if (my_rank % 2 != 0) {     // Odd rank
            partner = my_rank + 1;
        } else {                    // Even rank
            partner = my_rank - 1;
        }
    }
    if (partner == -1 || partner == comm_size){
        partner = MPI_PROC_NULL;
    }
    return partner;
}

int main(int argc, char* argv[]) {
    int N;  // size of large array A
    
    
    int partner, phase, my_rank, comm_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

       // read in the value of n
   if (my_rank == 0) {
      scanf("%d", &N);
   }

   int A[N];  // large array A
   int my_A[N/4];  // local array for each process

    // broadcast the value of n to all processes
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Seed the random number generator (only done by process 0)
    if (my_rank == 0) {
        srand(time(NULL));
    }

    // Generate 4 random datasets in A (only done by process 0)
    if (my_rank == 0) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < N/4; j++) {
                A[i*N/4 + j] = rand() % 100 + 1;  // random number from 1 to 100
            }
        }
    }

    // Divide the data in A among the processes using MPI_Scatter
    MPI_Scatter(A, N/4, MPI_INT, my_A, N/4, MPI_INT, 0, MPI_COMM_WORLD);

    // Each process sorts its local array using qsort
    qsort(my_A, N/4, sizeof(int), cmpfunc);

    for (phase = 0; phase < comm_size; phase++) {
        partner = Compute_partner(phase, my_rank, comm_size);
        if(partner != MPI_PROC_NULL) { // if process not idle
            printf("This is phase %d, I am process %d and I will exchange with process %d\n", phase, my_rank, partner);
        } else {
            printf("This is phase %d, I am process %d and I will remain idle in this phase.\n", phase, my_rank);
        }
    }

    // // Print the sorted numbers to the terminal
    // for (int i = 0; i < N/4; i++) {
    //     printf("Process %d: %d\n", my_rank, my_A[i]);
    // }

    MPI_Finalize();
    return 0;
}

