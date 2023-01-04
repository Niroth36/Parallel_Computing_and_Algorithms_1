# include <stdio.h>
# include <stdlib.h>


int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int main () {
   int n, i, num;
   printf("Enter size of array\n");
   scanf("%d", &n);

   int array1[n], array2[n];

   for (i=1; i <= n; i++) {
      array1[i] = rand() % 100 + 1;
   }
   printf("%d", &n)

   qsort(array1, n, sizeof(int), cmpfunc);

   printf("\nAfter sorting the list is: \n");
   for( i = 0 ; i < n; i++ ) {   
      printf("%d ", array1[n]);
   }
  
   return(0);
}