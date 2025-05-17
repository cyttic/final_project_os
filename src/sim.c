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
	if (size >= 1){//size less of 0 means there is setMenu call
		if (size < 5 || size > 7)
			close_program("Size of menu must be between 5 and 7 items\n");
		setSizeMenu(size);
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
	if (size < 1)
		close_program("Wrong argument for setMenu(int) call\n");
	_controlMenu(size);
}

menuItem** getMenu(){
	return _controlMenu(-1);
}

orderItem** _controlOrderBoard(int size){
	static orderItem **items;
	if (size > 0){//if parametr size <= 0 then it's call getOrderBoard()
		//we use threads, so we can get just malloc to allocate memory 
		items = malloc(sizeof(orderItem**));
		for(int i = 0; i < size; ++i){
			items[i] = malloc(sizeof(orderItem*));
			items[i]->customerId = i;
			items[i]->itemId = -1; //newermind
			items[i]->amount = -1; //newermind
			items[i]->done = 1;    // 1 - finished, 0 - does not
		}	
	}
	return items;
}

void setOrderBoard(int size){
	if (size < 1)
		close_program("Wrong argument for setOrderBoard(int) call\n");
	_controlOrderBoard(size);
}

orderItem** getOrderBoard(){
	return _controlOrderBoard(-1);
}

int _controlSizeMenu(int size){
	static int size_menu;
	if (size > 0)
		size_menu = size;
	return size_menu;
}

void setSizeMenu(int size){
	_controlSizeMenu(size);
}

int getSizeMenu(){
	return _controlSizeMenu(-1);
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

pthread_mutex_t mutex_client_waiter;
void *th_foo_client(void *thread_id){
	int num = *(int *)thread_id;
	printThreadMessage("%f Customer %d: created PID %d PPID %d\n", getTimeWork(),num,getpid(),getppid());
	//printf("THREAD %d CAN READ MENU: %s\n", num, getMenu()[3]->name);
	//a)if not elapsed simulation time
	while(isSimWorks()){
		//b)sleep for 3 to 6 seconds randomly
		int time_sleep = rand()%6+3;
		sleep(time_sleep);
		//c)Read the menu (1 second);
		//d)If the previous order has not yet been done, loop to (a)
		int isContinue = 0;
		pthread_mutex_lock(&mutex_client_waiter);

		if(getOrderBoard()[num]->done == 0)
			isContinue = 1;//it's need because need to unlock mutex before Continue
		pthread_mutex_unlock(&mutex_client_waiter);
		if (isContinue)
			continue;
		//e)with the probability 0.5 client will order 
		if (rand()%2){
			//i)randomly choose item and amount
			int item = rand()%getSizeMenu();
			int amount = rand()%4+1;
			char name_item[20];
			//ii)write the order to the board
			pthread_mutex_lock(&mutex_client_waiter);
			getOrderBoard()[num]->itemId = item;
			getOrderBoard()[num]->amount = amount;
			getOrderBoard()[num]->done = 0;
			strcpy(name_item, getMenu()[item]->name);
			pthread_mutex_unlock(&mutex_client_waiter);
			printThreadMessage("%f Customer %d: reads a menu about %s(ordered, amount %d)\n", getTimeWork(),num, name_item, amount);
		}else{
		//f)with the probability 0.5 client does not order
		//we can choose random dish for this message, because it will not be ordered
		char name_item[20];
		pthread_mutex_lock(&mutex_client_waiter); 
		strcpy(name_item, getMenu()[rand()%getSizeMenu()]->name);
		pthread_mutex_unlock(&mutex_client_waiter);
		printThreadMessage("%f Customer %d: reads a menu about %s(doesn't want to order)\n", getTimeWork(),num, name_item);
		sleep(1);//there is sleep from (c)
		//g)loop to (a)
		}
	}
	printThreadMessage("%f Customer %d: PID %d end work PPID %d\n", getTimeWork(), getpid(), getppid());
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
