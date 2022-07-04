#include <pthread.h>
#include <stdbool.h>

struct barrier_t {
	int totalThreads;
	int threadEntryCount;
	int threadLeaveCount;
	bool barrierEntry;
	pthread_mutex_t lock;
	pthread_mutex_t leaveLock;
	pthread_mutex_t waitLock;
	pthread_cond_t leaveCond;
	pthread_cond_t waitCond;
};

void barrier_init(struct barrier_t *b, int i);
void barrier_wait(struct barrier_t *b);