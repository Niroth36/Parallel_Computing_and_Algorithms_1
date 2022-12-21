# include <stdio.h>
# include <stdlib.h>

int ages[10];

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int main () {
   int n, i, num;

   for (i=1; i <= 10; i++) {
      ages[i] = rand() % 100 + 1;
   }


   qsort(ages, 10, sizeof(int), cmpfunc);

   printf("\nAfter sorting the list is: \n");
   for( n = 0 ; n < 10; n++ ) {   
      printf("%d ", ages[n]);
   }
  
   return(0);
}