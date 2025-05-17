#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdarg.h>
#include "timer.h"
#include "sim.h"

//function with symbol _ are for internal use only!
menuItem** _controlMenu(int size){
	static menuItem **items;
	if (size >= 0){//size less of 0 means there is setMenu call
		if (size < 5 || size > 7)
			close_program("Size of menu must be between 5 and 7 items\n");
		char names[7][10] = {"Pizza","Salad","Hamburger","Spagetti","Pie","Milkshake","Falafel"};
		//menuItem **items = malloc(sizeof(menuItem*)*size);
		//We need to use shared memory according the task, but in this realisation we're using threads instead of a process, and it would be fine without it
		items = getShmat(sizeof(menuItem**));
		for(int i = 0; i < size; ++i){
			items[i] = malloc(sizeof(menuItem*));
			items[i]->id = i+1;//just set up id of items in the loop
			items[i]->price = rand()%20 + 3;//set up price for item via rand
			items[i]->name = malloc(sizeof(names[i])+1);
			strcpy(items[i]->name,names[i]);
		}
		items[size] = NULL;
	}
	return items;
}

void setMenu(int size){
	_controlMenu(size);
}

menuItem** getMenu(){
	return _controlMenu(-1);
}

orderItem **initOrderBoard(int size){
	//we use threads, so we can get just malloc to allocate memory 
	orderItem **items = malloc(sizeof(orderItem**));
	for(int i = 0; i < size; ++i){
		items[i] = malloc(sizeof(orderItem*));
		items[i]->customerId = i;
		items[i]->itemId = -1; //newermind
		items[i]->amount = -1; //newermind
		items[i]->done = 1;    // 1 - finished, 0 - does not
	}
}

void printMenu(menuItem **menu){
	for(int i = 0; menu[i] != NULL; ++i){
		printf("%d %s\n",menu[i]->id, menu[i]->name);
	}
}

int simulation(int clients){
	while(clients > 0){
		//we are still working untill we have at least one client;
	}
	return 0;
}

void close_program(char *msg){
	perror(msg);
	exit(-3);
}

void alarmEndSimulation(){
	controlSim(0);
}

/*
float getTime(){
	static float internal_timer = (float)clock();
	internal_timer = internal_timer/CLOCKS_PER_SEC;
}
*/

pthread_mutex_t mutex_stop_sim;
int isSimWorks(){
	int justCheckSim = 1;
	return controlSim(justCheckSim);
}

void stopSim(){
	controlSim(0);
}

int controlSim(int val){
	static int sim_status = 1;
	if(val == 0){//we need to stop simulation and need to use mutex to change variable status
		pthread_mutex_lock(&mutex_stop_sim);
		sim_status = val;
		pthread_mutex_unlock(&mutex_stop_sim);
	}
	return sim_status;
}

void *th_foo_client(void *thread_id){
	int num = *(int *)thread_id;
	printThreadMessage("%f Customer %d: created PID %d PPID %d\n", getTimeWork(),num,getpid(),getppid());
	printf("THREAD %d CAN READ MENU: %s\n", num, getMenu()[3]->name);
	while(isSimWorks()){
		//printf("thread %d\n works", getpid());
	}
}

void *getShmat(int size){
	size_t SIZE_MENUITEM = size;
	key_t key;
	int shmid;
	menuItem *items;
	
	key = ftok(".", 1234);
	if (key == -1)
		close_program("ftok returned error");
	shmid = shmget(key, SIZE_MENUITEM, IPC_EXCL | 0666);
	if (shmid == -1)
		close_program("shmget returned error");
	items = shmat(shmid,NULL,0);
	return items;
}

pthread_mutex_t mutex_print;
//using vprintf instead of printf much more ease to pass arguments to print out
void printThreadMessage(const char *message, ...){
	pthread_mutex_lock(&mutex_print);
	va_list args;
	va_start(args,message);
	vprintf(message,args);
	va_end(args);
	pthread_mutex_unlock(&mutex_print);
}
