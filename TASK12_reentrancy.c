#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <malloc.h>

#define T_QUANT 500

pthread_once_t once = PTHREAD_ONCE_INIT;
pthread_key_t key;

void dest(void *buff)
{
	free(buff);
}

void key_create(void)
{
	pthread_key_create(&key, dest);
}

char *error_str(int no)
{
	char *buff;
	char s[255];
	pthread_once(&once, key_create);

	buff = pthread_getspecific(key);
	if (buff == NULL) {
		buff = malloc(255);
		pthread_setspecific(key, buff);
	}
	sprintf(s, "%s %i", "Error", no);
	strcpy(buff, s);
	printf("%s\n", buff);

	return buff;
}
void *thread_func(void *args)
{
	int *arg = (int*) args;
	error_str(*arg);
}

int main(void)
{
	int index[T_QUANT];
	int i, i1;
	int *s;
	pthread_t thread[T_QUANT];

	for (i = 0; i < T_QUANT; i++) {
		index[i] = i + 1;

		pthread_create(&thread[i], NULL, thread_func, (void*) &index[i]);
	}
	
	for (i1 = 0; i1 < T_QUANT; i1++) {
		pthread_join(thread[i1], (void **) &s);
	}
	return 0;
}