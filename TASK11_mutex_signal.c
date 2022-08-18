#include <stdio.h>
#include <pthread.h>

long a = 0;
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1;

void *thread_calc(void *args)
{
	int i, tmp;
	for (i = 0; i < 500010; i++) {
		pthread_mutex_lock(&m1);
		tmp = a;
		tmp ++;
		a = tmp;
		if (a >= 1000000) pthread_cond_signal(&cond1);
		pthread_mutex_unlock(&m1);
	}
	return NULL;
}
void *thread_wait(void *args)
{
	pthread_mutex_lock(&m1);
	pthread_cond_wait(&cond1, &m1);
	printf("a >= 1000000\n");
	pthread_mutex_unlock(&m1);
}
int main(void)
{
	int index[5];
	int i, i1;
	int *s;
	pthread_t thread[5];

	pthread_cond_init(&cond1, NULL);
	for (i = 0; i < 2; i++) {
		index[i] = i + 1;
		pthread_create(&thread[i], NULL, thread_calc, NULL);
	}
	pthread_create(&thread[2], NULL, thread_wait, NULL);
	for (i1 = 0; i1 < 2; i1++) {
		pthread_join(thread[i1], (void **) &s);
	}



}
