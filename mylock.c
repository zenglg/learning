#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int num = 0;

int lock = 0;

static void *pthread_fun(void *arg);
static void get_num(int *n);
static void add_num(int *n, int i);
static void put_num(int *n);
static void add(int i);
static void print(const char *str);

static void mylock_init(int *lock);
static void mylock(int *lock);
static void myunlock(int *lock);

int main(void)
{
	pthread_t ptid;

	mylock_init(&lock);

	pthread_create(&ptid, NULL, pthread_fun, NULL);

	add(19);

	pthread_join(ptid, NULL);

	print("main");

	return 0;
}

static void *pthread_fun(void *arg)
{
	int *num2 = (int *)arg;

	add(13);
}

static void get_num(int *n)
{
	sleep(1);

	*n = num;
}

static void add_num(int *n, int i)
{
	sleep(1);

	*n += i;
}

static void put_num(int *n)
{
	sleep(1);

	num = *n;
}

static void add(int i)
{
	int tmp;

	mylock(&lock);

	get_num(&tmp);

	add_num(&tmp, i);

	put_num(&tmp);

	myunlock(&lock);
}

static void print(const char *str)
{
	printf("%s:%d %s - %d\n", __func__, __LINE__, str, num);
}

#ifdef MYLOCK

static void mylock_init(int *lock)
{
	*lock = 0;
}

static void mylock(int *lock)
{
	while (*lock != 0) {
		sleep(1);
	}

	(*lock)++;
}

static void myunlock(int *lock)
{
	if (*lock < 1) {
		printf("%d: lock error, *lock: %d\n", pthread_self(), *lock);
		exit(-1);
	}

	(*lock)--;
}

#else

static void mylock_init(int *lock)
{
}

static void mylock(int *lock)
{
}

static void myunlock(int *lock)
{
}

#endif
