#include "cs_thread.h"
#include <stdbool.h>

struct Repositioning {
	char player; 		// T for turtle and H for hare
	int time; 		// At what time god interrupt's
	int distance;		// How much does god move any of the player. 
							// distance can be negetive or positive.
							// using this distance if any of players position is less than zero then bring him to start line.
							// If more than finish_distance then make him win.
							// If time is after completion of game than you can ignore it as we will already have winner.
};

struct race {
	
	//	Don't change these variables.
	//	speeds are unit distance per unit time.
	int printing_delay;
	int tortoise_speed;					// speed of Turtle
	int hare_speed;						// speed of hare
	int hare_sleep_time; 				// how much time does hare sleep (in case he decides to sleep)
	int hare_turtle_distance_for_sleep; // minimum lead which hare has on turtle after which hare decides to move
										// Any lead greater than this distance and hare will ignore turtle and go to sleep
	int finish_distance;				// Distance between start and finish line
	struct Repositioning* reposition;	// pointer to array containing Randomizer's decision
	int repositioning_count;			// number of elements in array of repositioning structure
	
	//	Add your custom variables here.
	int time;
	char winner;
	int repositionIndex;
	int harePosition;
	int turtlePosition;
	int hareWakeUpTime;
	pthread_mutex_t lock;
	pthread_cond_t initDone;
	pthread_cond_t randDone;
	pthread_cond_t hareDone;
	pthread_cond_t turtleDone;
	pthread_cond_t reportDone;
	bool randWait;
	bool hareWait;
	bool turtleWait;
	bool reportWait;
	bool initWait;
};


void* Turtle(void *race);
void* Hare(void *race);
void* Randomizer(void *race);
void* Report(void *race);

char init(struct race *race) {
	race->time = 0;
	race->winner = '-';
	race->repositionIndex = 0;
	race->harePosition = 0;
	race->turtlePosition = 0;
	race->hareWakeUpTime = 0;
	race->randWait = true;
	race->hareWait = true;
	race->turtleWait = true;
	race->reportWait = true;
	race->initWait = true;
	pthread_t turtle, hare, randomizer, report;
	pthread_create(&turtle, NULL, Turtle, (void *) race);
	pthread_create(&hare, NULL, Hare, (void *) race);
	pthread_create(&randomizer, NULL, Randomizer, (void *) race);
	pthread_create(&report, NULL, Report, (void *) race);

	do {
		race->randWait = false;
		pthread_cond_signal(&(race->initDone));
		while(race->initWait == true) {
			pthread_cond_wait(&(race->reportDone), &(race->lock));
		}
		race->initWait = true;
		race->time += 1;
		pthread_mutex_unlock(&(race->lock));
	} while(race->winner == '-');
	race->randWait = false;
	race->hareWait = false;
	race->turtleWait = false;
	race->reportWait = false;

	pthread_cond_signal(&(race->initDone));
	pthread_cond_signal(&(race->randDone));
	pthread_cond_signal(&(race->hareDone));
	pthread_cond_signal(&(race->turtleDone));

	pthread_join(turtle, NULL);
	pthread_join(hare, NULL);
	pthread_join(randomizer, NULL);
	pthread_join(report, NULL);

	return race->winner;
}

void* Turtle(void *arg) {
	struct race *race = (struct race *)arg;
	while(race->winner == '-') {
		while(race->turtleWait == true) {
			pthread_cond_wait(&(race->hareDone), &(race->lock));
		}
		race->turtleWait = true;

		// process
		if(race->winner == '-') { // randomizer did not decide winner, hence decide after turtle moves
			race->turtlePosition += race->tortoise_speed;
			if(race->turtlePosition < 0) race->turtlePosition = 0; // if speed is negative
			if(race->turtlePosition >= race->finish_distance) {
				race->turtlePosition = race->finish_distance;
				race->winner = 'T';
			} else if(race->harePosition == race->finish_distance) {
				race->winner = 'H';
			}
		}
		
		race->reportWait = false;
		pthread_cond_signal(&(race->turtleDone));
		pthread_mutex_unlock(&(race->lock));
	}
	return NULL;
  
}

void* Hare(void *arg) {
	struct race *race = (struct race *)arg;
	while(race->winner == '-') {
		while(race->hareWait == true) {
			pthread_cond_wait(&(race->randDone), &(race->lock));
		}
		race->hareWait = true;

		// process
		if(race->winner == '-' && race->time >= race->hareWakeUpTime) {
			if(race->harePosition - race->turtlePosition > race->hare_turtle_distance_for_sleep) {// hare decides at begining of time unit to sleep or not
				race->hareWakeUpTime = race->time + race->hare_sleep_time;
			} else {
				race->harePosition += race->hare_speed;
				if(race->harePosition < 0) race->harePosition = 0;
				if(race->harePosition >= race->finish_distance) race->harePosition = race->finish_distance;
			}
		}

		race->turtleWait = false;
		pthread_cond_signal(&(race->hareDone));
		pthread_mutex_unlock(&(race->lock));
	}
	return NULL;
}


void* Randomizer(void *arg) {
	struct race *race = (struct race *)arg;
	while(race->winner == '-') {
		while(race->randWait == true) {
			pthread_cond_wait(&(race->initDone), &(race->lock));
		}
		race->randWait = true;

		// process
		if(race->winner == '-' && race->repositionIndex < race->repositioning_count && race->time == race->reposition[race->repositionIndex].time) {
			char player = race->reposition[race->repositionIndex].player;
			int distance = race->reposition[race->repositionIndex].distance;
			if(race->finish_distance == 0) { // randomizer is called when winner is not decided.
				race->winner = 'T';
			} else if(player == 'T') {
				race->turtlePosition += distance;
				if(race->turtlePosition < 0) race->turtlePosition = 0;
				if(race->turtlePosition >= race->finish_distance){
					race->turtlePosition = race->finish_distance;
					race->winner = 'T';
				}
				
			} else if(player == 'H') {
				race->harePosition += distance;
				if(race->harePosition < 0) race->harePosition = 0;
				if(race->harePosition >= race->finish_distance){
					race->harePosition = race->finish_distance;
					race->winner = 'H';
				}
			}
			race->repositionIndex += 1;
		}

		race->hareWait = false;
		pthread_cond_signal(&(race->randDone));
		pthread_mutex_unlock(&(race->lock));
	}
	return NULL;
}

void* Report(void *arg) {
	struct race *race = (struct race *)arg;
	while(race->winner == '-') {
		while(race->reportWait == true) {
			pthread_cond_wait(&(race->turtleDone), &(race->lock));
		}
		race->reportWait = true;
		// process
		if((race->time+1) % race->printing_delay == 0) {
			printf("Time Unit Completed: %d, Hare at: %d, Turtle at: %d\n", race->time+1, race->harePosition, race->turtlePosition);
		}

		race->initWait = false;
		pthread_cond_signal(&(race->reportDone));
		pthread_mutex_unlock(&(race->lock));
	}
	return NULL;
}
