#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>


void *thread_function()
{
     int i = 0;
     for (int y = 0; y < 100000000; y++) i++;
     printf("i = %i\n", i);
}

int main()
{
     pthread_t threads[10];
     int i = 0;

     double time_spent = 0.0;
     clock_t begin;
     clock_t end;

     begin = clock();
     for (int y = 0; y < 1000000000; y++) i++;
     printf("i = %d\n", i);
     end = clock();
     time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
     printf("The elapsed time in single thread is %f seconds\n", time_spent);

     begin = clock();
     for (int y = 0; y < 10; y++)
          if(!pthread_create(&threads[y], NULL, thread_function, NULL));

     end = clock();

     pthread_join(threads[0], NULL);
     pthread_join(threads[1], NULL);
     pthread_join(threads[2], NULL);
     pthread_join(threads[3], NULL);
     pthread_join(threads[4], NULL);
     pthread_join(threads[5], NULL);
     pthread_join(threads[6], NULL);
     pthread_join(threads[7], NULL);
     pthread_join(threads[8], NULL);
     pthread_join(threads[9], NULL);

     time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
     printf("The elapsed time with threads is %f seconds\n", time_spent);
     exit(EXIT_SUCCESS);
}

