#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int main(int argc, char* argv[]) {
   int n, i, j, num;
   int *A, *my_A;  // pointers to store the arrays
   int my_rank, comm_size;

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
   MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

   // check that there are 4 processes
   if (comm_size != 4) {
      if (my_rank == 0) {
         printf("Error: number of processes must be 4\n");
      }
      MPI_Finalize();
      return 0;
   }

   // read in the value of n
   if (my_rank == 0) {
      printf("Enter the value of n: ");
      scanf("%d", &n);
   }

   // broadcast the value of n to all processes
   MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

   // allocate memory for the arrays
   A = malloc(n * sizeof(int));
   my_A = malloc((n/4) * sizeof(int));

   // generate random numbers for the array A
   if (my_rank == 0) {
      for (i=0; i < n; i++) {
         A[i] = rand() % 100 + 1;  // generate a random number between 1
      }
   }

   // scatter the data in A to the processes
   MPI_Scatter(A, n/4, MPI_INT, my_A, n/4, MPI_INT, 0, MPI_COMM_WORLD);

   // sort the local data using qsort
   qsort(my_A, n/4, sizeof(int), cmpfunc);

   // gather the sorted data back to A
   MPI_Gather(my_A, n/4, MPI_INT, A, n/4, MPI_INT, 0, MPI_COMM_WORLD);

   // print the sorted data, if rank is 0
   if (my_rank == 0) {
      printf("Sorted data: ");
      for (i=0; i < n; i++) {
         printf("%d ", A[i]);
      }
      printf("\n");
   }

   MPI_Finalize();
   return 0;
}