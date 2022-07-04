#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "barrier.h"

void barrier_init(struct barrier_t * barrier, int nthreads) {
	barrier->barrierEntry = false;
	barrier->totalThreads = nthreads;
	barrier->threadLeaveCount = nthreads;	// so that very first thread can enter
	pthread_mutex_lock(&(barrier->leaveLock));
	return;
}

void barrier_wait(struct barrier_t *barrier) {
	pthread_mutex_lock(&(barrier->lock));
	if(barrier->barrierEntry == false) {	// first thread enters
		while(barrier->threadLeaveCount < barrier->totalThreads){// wait till every thread leave
			pthread_cond_wait(&(barrier->leaveCond), &(barrier->leaveLock));
		}
		pthread_mutex_unlock(&(barrier->leaveLock));
		barrier->barrierEntry = true;
		barrier->threadEntryCount = 0;
		barrier->threadLeaveCount = 0;
	}
	barrier->threadEntryCount += 1;
	if(barrier->threadEntryCount == barrier->totalThreads) {	// last thread enters 
		pthread_mutex_lock(&(barrier->waitLock));
		pthread_cond_broadcast(&(barrier->waitCond));
		barrier->barrierEntry = false;

	}
	pthread_mutex_unlock(&(barrier->lock));
	
	while(barrier->threadEntryCount < barrier->totalThreads) {
		pthread_cond_wait(&(barrier->waitCond), &(barrier->waitLock));
	}
	pthread_mutex_unlock(&(barrier->waitLock));

	pthread_mutex_lock(&(barrier->leaveLock));
	barrier->threadLeaveCount += 1;
	if(barrier->threadLeaveCount == barrier->totalThreads) 
		pthread_cond_signal(&(barrier->leaveCond));
	pthread_mutex_unlock(&(barrier->leaveLock));
	return;
}