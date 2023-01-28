#include <stdio.h>
#include <stdlib.h>

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)b - *(int*)a );
}

int main() {
   int n, i, j, num;
   int *a, *b, *c;  // pointers to store the arrays

   // read in the value of n
   printf("Enter the value of n: ");
   scanf("%d", &n);

   // allocate memory for the arrays
   a = malloc(n * sizeof(int));
   b = malloc(n * sizeof(int));
   c = malloc(2 * n * sizeof(int));

   // generate random numbers for the first array
   for (i=0; i < n; i++) {
      a[i] = rand() % 100 + 1;  // generate a random number between 1 and 100
   }

   // generate random numbers for the second array
   for (i=0; i < n; i++) {
      b[i] = rand() % 100 + 1;  // generate a random number between 1 and 100
   }

   // sort both arrays in descending order
   qsort(a, n, sizeof(int), cmpfunc);
   qsort(b, n, sizeof(int), cmpfunc);

   printf("a array\n");
   for (i=0; i < n; i++) {
      printf("%d ", a[i]);
   }

   printf("\nb array\n");
   for (i=0; i < n; i++) {
      printf("%d ", b[i]);
   }

   // create the union of the two arrays
   i = 0;  // index for a
   j = 0;  // index for b
   num = 0;  // index for c
   while (i < n && j < n) {
      if (a[i] > b[j]) {
         c[num] = a[i];
         i++;
      } else if (a[i] < b[j]) {
         c[num] = b[j];
         j++;
      } else {
         c[num] = a[i];
         num++;
         c[num] = b[j];
         i++;
         j++;
      }
      num++;
   }

   // print the first n numbers of the first array (which now contain the n largest numbers)
   printf("\nThe first %d numbers of the two arrays are: ", n);
   for (i=0; i < n; i++) {
      a[i] = c[i];
      printf("%d ", a[i]);
   }
   printf("\n");

   free(a);
   free(b);
   free(c);

   return 0;
}
