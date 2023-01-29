#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <string.h>

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

void merge_bottom(int my_A[], int my_keys[], int recv_keys[], int comm_size, int N){
    int local_n = N/comm_size;
    int temp_keys[2 * N/comm_size]; // temporary array used for merging
    int m_i, r_i, t_i;

    m_i = r_i = t_i = 0;
    while (t_i < local_n) {
        if (my_keys[m_i] <= recv_keys[r_i]) {
            temp_keys[t_i] = my_keys[m_i];
            t_i++;
            m_i++;
        } else {
            temp_keys[t_i] = recv_keys[r_i];
            t_i++;
            r_i++;
        }
    }

    for (m_i = 0; m_i < local_n; m_i++) {
        my_keys[m_i] = temp_keys[m_i];
    }

     memcpy(my_A, my_keys, N*sizeof(int));
}

void merge_top(int my_A[], int my_keys[], int recv_keys[], int comm_size, int N){
    int local_n = N/comm_size;
    int temp_keys[2 * N/comm_size]; // temporary array used for merging
    int m_i, r_i, t_i;

    m_i = local_n - 1;
    r_i = local_n - 1;
    t_i = 2 * local_n - 1;
    while (t_i >= local_n) {
        if (my_keys[m_i] >= recv_keys[r_i]) {
            temp_keys[t_i] = my_keys[m_i];
            t_i--;
            m_i--;
        } else {
            temp_keys[t_i] = recv_keys[r_i];
            t_i--;
            r_i--;
        }
    }

    for (int m_i = local_n; m_i < 2 * local_n; m_i++) {
        my_keys[m_i - local_n] = temp_keys[m_i];
    }

    memcpy(my_A, my_keys, N*sizeof(int));
}


int main(int argc, char* argv[]) {
    int N;  // size of large array A
    double start, finish;
    
    int partner, phase, my_rank, comm_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    // read in the value of n
   if (my_rank == 0) {
        printf("Enter a value: \n");
        fflush(stdout);  // flush the output buffer
        scanf("%d", &N);
   }

   int A[N];  // large array A
   int my_A[N/comm_size];  // local array for each process
   int receive_A[N/comm_size]; // partner array

    // Seed the random number generator (only done by process 0)
    if (my_rank == 0) {
        srand(time(NULL));
    }

    // Generate comm_size random datasets in A (only done by process 0)
    if (my_rank == 0) {
        for (int i = 0; i < comm_size; i++) {
            for (int j = 0; j < N/comm_size; j++) {
                A[i*N/comm_size + j] = rand() % 100 + 1;  // random number from 1 to 100
            }
        }
    }

    // broadcast the value of n to all processes
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Divide the data in A among the processes using MPI_Scatter
    MPI_Scatter(A, N/comm_size, MPI_INT, my_A, N/comm_size, MPI_INT, 0, MPI_COMM_WORLD);

    start = MPI_Wtime();
    // Each process sorts its local array using qsort
    qsort(my_A, N/comm_size, sizeof(int), cmpfunc);
    finish = MPI_Wtime();

    for (phase = 0; phase < comm_size; phase++) {
        partner = Compute_partner(phase, my_rank, comm_size);
        if(partner != MPI_PROC_NULL) { // if process not idle
            MPI_Sendrecv(my_A, N/comm_size, MPI_INT, partner, 0, receive_A, N/comm_size,
                 MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (my_rank < partner) {
                merge_bottom(my_A, my_A, receive_A, comm_size, N);
            } else {
                merge_top(my_A, my_A, receive_A, comm_size, N);
            }
            
        } else {
            // printf("This is phase %d, I am process %d and I will remain idle in this phase.\n", phase, my_rank);
        }
    }

    if (my_rank != 0) {
        MPI_Send(my_A, N/comm_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
        for (int p = 1; p < comm_size; p++) {
            MPI_Recv(receive_A, N/comm_size, MPI_INT, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    // Print the sorted numbers to the terminal
    // for (int i = 0; i < N/comm_size; i++) {
    //     printf("Process %d: %d\n", my_rank, my_A[i]);
    // }    

    printf("Elapsed time = %e\n", finish-start);

    MPI_Finalize();
    return 0;
}