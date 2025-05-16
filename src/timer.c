#include <time.h>
#include <stdio.h>
#include "timer.h"

//keep the time of first start of program
struct timespec firstTime(){
	static struct timespec tmr;
	static int init = 0;
	if (init ==  0){
		clock_gettime(CLOCK_REALTIME, &tmr);
		init = 1;
	}
	return tmr;	
}

//count difference between current and first time of start
double getTimeWork(){
	struct timespec dif;
	clock_gettime(CLOCK_REALTIME, &dif);
	double result = (dif.tv_sec - firstTime().tv_sec) + (dif.tv_nsec - firstTime().tv_nsec)/1000000000.0;
	//this check "result >= 0.0" needs because first run of function returns -0.0
	return result >= 0.0 ? result: 0.0;
}
