
/*
#include <time.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include "timer.h"

// Semaphore for tmr access
static int semid_timer = -1;
static struct sembuf sb_timer;

void initTimerSem() {
    semid_timer = semget(12368, 1, IPC_CREAT | 0666);
    if (semid_timer == -1) {
        perror("semget failed for timer");
        //exit(1);
    }
    unsigned short sem_val[1] = {1};
    if (semctl(semid_timer, 0, SETALL, sem_val) == -1) {
        perror("semctl failed for timer");
        //exit(1);
    }
}

//keep the time of first start of program
static struct timespec *tmr;
static int *innt;
static int innt2;
struct timespec *firstTime(){
	static int init = 0;
	
	if (init ==  0){
		tmr = mmap(NULL, sizeof *tmr, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1,0);
		clock_gettime(CLOCK_REALTIME, tmr);
		init = 1;
		
		innt = mmap(NULL, sizeof *innt, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1,0);
		innt2 = 1;
	}
	
	return tmr;	
}

//count difference between current and first time of start
double getTimeWork(){
	
	// Lock semaphore for tmr read
    sb_timer.sem_num = 0;
    sb_timer.sem_op = -1;
    sb_timer.sem_flg = 0;
    if (semop(semid_timer, &sb_timer, 1) == -1) {
        //perror("semop lock failed for timer");
        //exit(1);
    }
	
	innt = &innt2;
	innt2 += 1;
	
	struct timespec dif;
	clock_gettime(CLOCK_REALTIME, &dif);
	//double result = (dif.tv_sec - firstTime().tv_sec) + (dif.tv_nsec - firstTime().tv_nsec)/1000000000.0;
	double result = (dif.tv_sec - firstTime()->tv_sec) +  (dif.tv_nsec - firstTime()->tv_nsec)/1000000.0;
	
	// Unlock semaphore
    sb_timer.sem_op = 1;
    if (semop(semid_timer, &sb_timer, 1) == -1) {
        //perror("semop unlock failed for timer");
        //exit(1);
    }
	//this check "result >= 0.0" needs because first run of function returns -0.0
	//return result >= 0.0 ? result: 0.0;
	return *innt;
	
}
*/

#include <time.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>

static int semid_timer = -1;
static struct sembuf sb_timer;
static struct timespec *tmr = NULL;

void initTimerSem() {
    semid_timer = semget(12368, 1, IPC_CREAT | 0666);
    if (semid_timer == -1) {
        perror("semget failed");
        exit(1);
    }
    unsigned short sem_val[1] = {1};
    if (semctl(semid_timer, 0, SETALL, sem_val) == -1) {
        perror("semctl failed");
        exit(1);
    }
}

void initSharedTimer() {
    tmr = mmap(NULL, sizeof *tmr, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (tmr == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }
    clock_gettime(CLOCK_REALTIME, tmr);
}

double getTimeWork() {
    // lock
    sb_timer.sem_num = 0;
    sb_timer.sem_op = -1;
    sb_timer.sem_flg = 0;
    if (semop(semid_timer, &sb_timer, 1) == -1) {
        perror("semop lock failed");
        exit(1);
    }

    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    double result = (now.tv_sec - tmr->tv_sec) + (now.tv_nsec - tmr->tv_nsec) / 1e9;

    // unlock
    sb_timer.sem_op = 1;
    if (semop(semid_timer, &sb_timer, 1) == -1) {
        perror("semop unlock failed");
        exit(1);
    }

    return result >= 0.0 ? result : 0.0;
}

