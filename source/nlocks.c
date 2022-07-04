#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

/* 
 * This is the data array which is to
 * be incremented by all the threads created 
 * by your program as specified in the problem state
*/
int data[10];
pthread_mutex_t lock[10];

void increment(int index) {
	pthread_mutex_lock(&lock[index]);
	data[index] += 1;
	pthread_mutex_unlock(&lock[index]);
}

void * incrementData(void *arg) {
	int index = *((int *)arg);

	int count = 1000;
	while(count-- > 0) {
		increment(index);
	}
}

int main(int argc, char const *argv[]) {
	/* Write you code to create 10 threads here*/
	/* Increment the data array as specified in the problem statement*/
	pthread_t threads[10];
	int index[10];
	for(int i = 0; i < 10; i++) {
		index[i] = i;
		pthread_create(&threads[i], NULL, incrementData, (void *)&index[i]);
	}

	for(int j = 0; j < 1000; j++) {
		for(int i = 0; i < 10; i++) {
			increment(i);
		}
	}
	
	for(int i = 0; i < 10; i++) {
		pthread_join(threads[i], NULL);
	}

	/* Make sure you reap all threads created by your program
	 before printing the counter*/
	for (int i = 0; i < 10; ++i) {
		printf("%d\n", data[i]);
	}
	sleep(10000);
	return 0;
}