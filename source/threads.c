#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<string.h>
/* 
 * This is the counter value which is to
 * be incremented by all the threads created 
 * by your program
*/
int counter = 0;
pthread_mutex_t lock;

void *incrementCounter(void *args) {
	pthread_mutex_lock(&lock);
	counter += 1;
	pthread_mutex_unlock(&lock);
}

int parseInt(char const *input) {
	int value = 0;
	for(int i = 0; i < strlen(input); i++) {
		value = value * 10 + input[i] - '0';
		if(input[i] < '0' || input[i] > '9') return 0;
	}
	return value;
}


int main(int argc, char const *argv[]) {
	/* Write you code to create n threads here*/
	/* Each thread must increment the counter once and exit*/
	int threadCount = parseInt(argv[1]);
	pthread_t threads[threadCount];
	for(int i = 0; i < threadCount; i++) {
		pthread_create(&threads[i], NULL, incrementCounter, NULL);
	}
	for(int i = 0; i < threadCount; i++) {
		pthread_join(threads[i], NULL);
	}
	/* Make sure you reap all threads created by your program
	 before printing the counter*/
	printf("%d\n", counter);
	sleep(10000);
	return 0;
}