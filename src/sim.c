#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <sys/types.h>
#include "timer.h"
#include "sim.h"

//function with symbol _ are for internal use only!
//static menuItem **items;
menuItem** _controlMenu(int size){
	static menuItem **items;
	if (size >= 1){//size less of 0 means there is setMenu call
		if (size < 5 || size > 7)
			close_program("Size of menu must be between 5 and 7 items\n");
		setSizeMenu(size);
		char names[7][10] = {"Pizza","Salad","Hamburger","Spagetti","Pie","Milkshake","Falafel"};
		//menuItem **items = malloc(sizeof(menuItem*)*size);
		//We need to use shared memory according the task, but in this realisation we're using threads instead of a process, and it would be fine without it
		//items = getShmat(sizeof(menuItem**));
		//items = malloc(sizeof(menuItem**));
		items = malloc(sizeof(menuItem*) * size);
		for(int i = 0; i < size; ++i){
			//items[i] = malloc(sizeof(menuItem*)); // non thread-safe version
			items[i] = mmap(NULL, sizeof *items[i], PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1,0);
			items[i]->id = i+1;//just set up id of items in the loop
			items[i]->price = rand()%20 + 3;//set up price for item via rand
			items[i]->name = (char*)malloc(sizeof(names[i])+1);
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
		items = malloc(sizeof(orderItem*)*size);
		for(int i = 0; i < size; ++i){
			//items[i] = malloc(sizeof(orderItem*));
			items[i] = mmap(NULL, sizeof *items[i], PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1,0);
			items[i]->customerId = i;
			items[i]->itemId = -1; //newermind
			items[i]->amount = -1; //newermind
			items[i]->done = 1;    // 1 - finished, 0 - does not
		}	
		items[size] = NULL;
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

int semid_printMenu;
struct sembuf sb_printMenu;
unsigned short sem_values_printMenu[1] = {1};

void initSemPrint(){
	semid_printMenu = semget(1235,1, IPC_CREAT | 0666);
	if (semid_printMenu == -1)
		close_program("Error initialise semid_printMenu in funciton initSemPrint\n");
	if (semctl(semid_printMenu,0, SETALL, sem_values_printMenu) == -1)
		close_program("Error semctl in funciton initSemPrint\n");
}

void printMenu(menuItem **menu){
	//NOTE: we don't need to use thread-sage function printThreadMessage because printMenu is used only on main process!!
	int size = getSizeMenu();
	
	sb_printMenu.sem_num = 0;
	sb_printMenu.sem_op = -1;
	sb_printMenu.sem_flg = 0;
	semop(semid_printMenu,&sb_printMenu,1);

	/*printThreadMessage*/printf("======================Menu list=====================\n");
	/*printThreadMessage*/printf("%-7s %-12s %-8s %-12s\n", "Id", "Name", "Price","Order");
	for(int i = 0; i < size; ++i){
		//if(menu[i] == NULL) break;
		/*printThreadMessage*/printf("%-7d %-12s %-8.2f %-12d\n",
			menu[i]->id,
			menu[i]->name,
			menu[i]->price,
			menu[i]->orders);

//	for(int i = 0; menu[i] != NULL; ++i){
//		printf("%d %s\n",menu[i]->id, menu[i]->name);
	}
	/*printThreadMessage*/printf("=====================================================\n");
	sb_printMenu.sem_op = 1;
	semop(semid_printMenu,&sb_printMenu,1);
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

/*
float getTime(){
	static float internal_timer = (float)clock();
	internal_timer = internal_timer/CLOCKS_PER_SEC;
}
*/
static int *timerSim;
void stopSim(){
	*timerSim = 0;
}

void initTimerSim(){
	timerSim = mmap(NULL, sizeof *timerSim, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1,0);
	*timerSim = 1;
}

int controlSim(){
	return *timerSim;
}

int isSimWorks(){
	return *timerSim;
}

pthread_mutex_t mutex_client_waiter;
void *th_foo_client(void *thread_id){
	long num = (long)thread_id;
	printThreadMessage("%.3f Customer %d: created PID %llu PPID %d\n", getTimeWork(),num,getpid(),getppid());
	//a)if not elapsed simulation time
	while(isSimWorks()){
		sleep(2);
		
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
			printThreadMessage("%.3f Customer %d: reads a menu about %s(ordered, amount %d)\n", getTimeWork(),num, name_item, amount);
		}else{
		//f)with the probability 0.5 client does not order
		//we can choose random dish for this message, because it will not be ordered
		char name_item[20];
		pthread_mutex_lock(&mutex_client_waiter); 
		strcpy(name_item, getMenu()[rand()%getSizeMenu()]->name);
		pthread_mutex_unlock(&mutex_client_waiter);
		printThreadMessage("%.3f Customer %d: reads a menu about %s(doesn't want to order)\n", getTimeWork(),num, name_item);
		sleep(1);//there is sleep from (c)
		//g)loop to (a)
		}
		
	}
	printThreadMessage("%.3f Customer ID %d: PID %llu end work PPID %d\n",num, getTimeWork(), getpid(), getppid());
}

void *th_foo_waiter(void *thread_id){
	long num = (long)(thread_id);
	printThreadMessage("%.3f Waiter %d: created PID %llu PPID %d\n", getTimeWork(),num,getpid(),getppid());
	//a)if not elapsed simulation time
	while(isSimWorks()){
		//b)sleep for 1-2 seconds randomly
		sleep(1+rand()%2);
		//c)read an order from the "order board"
		pthread_mutex_lock(&mutex_client_waiter);
		orderItem **orders = getOrderBoard();
		//for(int i = 0;i < 
		int i = 0;
		while(orders[i] != NULL){
			//d) and e): if there is row that isn't Done
			if (orders[i]->done == 0){
				//i)add the amount ordered to the totals in main menu
				getMenu()[orders[i]->itemId]->orders += orders[i]->amount;
				//ii)mark the order as Done
				orders[i]->done = 1;
				printThreadMessage("%.3f Waiter %d: performs the order of customer ID %d (%d %s)\n", getTimeWork(), num,i,orders[i]->amount, getMenu()[orders[i]->itemId]->name);
			}
			i++;
		}
		pthread_mutex_unlock(&mutex_client_waiter);
	}
	printThreadMessage("%.3f Waiter ID %d: PID %llu end work PPID %d\n",num, getTimeWork(), getpid(), getppid());
}


void *getShmat(int size){
	size_t SIZE_MENUITEM = size;
	key_t key;
	int shmid;
	menuItem *items;
	
	key = ftok("/tmp", 1234);
	if (key == -1)
		close_program("ftok returned error");
	shmid = shmget(key, SIZE_MENUITEM, IPC_CREAT | 0666);
	if (shmid == -1)
		close_program("shmget returned error");
	items = shmat(shmid,NULL,0);
	return items;
}

int semid_printTh;
struct sembuf sb_printTh;
unsigned short sem_values_printTh[1] = {1};
void initPrintTh(){
	semid_printTh = semget(12364,1, IPC_CREAT | 0666);
	if (semid_printTh == -1)
		close_program("Error initialise semid_printTh in funciton initPrintTh\n");
	if (semctl(semid_printTh,0, SETALL, sem_values_printTh) == -1)
		close_program("Error semctl in funciton initPrintTh\n");
}


//this version to print out from semaphore block instead of thread-safe function to avoiding nested semaphores
void printOneThreadMessage(const char *message, ...){
	va_list args;
	va_start(args,message);
	vprintf(message,args);
	va_end(args);
}

float getTotal(){
	float total = 0.0;
	menuItem **menu = getMenu();
	for(int i = 0; i < getSizeMenu(); ++i)
		total += menu[i]->price * menu[i]->orders;
	return total;

}

int getCountItems(){
	int total = 0;
	menuItem **menu = getMenu();
	for(int i = 0; i < getSizeMenu(); ++i)
		total += menu[i]-> orders;
	return total;
}


int semid_sim;
struct sembuf sb_sim;
unsigned short sem_values_sim[1] = {1};
void initSemSim(){
	semid_sim = semget(1230,2, IPC_CREAT | 0666);
	if (semid_sim == -1)
		close_program("Error initialise semid_sim in funciton initSemSim\n");
	if (semctl(semid_sim,0, SETVAL, sem_values_sim) == -1)
		close_program("Error semctl in funciton initSemSim\n");
	if (semctl(semid_sim,1, SETVAL, sem_values_sim) == -1)
		close_program("Error semctl print in funciton initSemSim\n");
}

//using vprintf instead of printf much more ease to pass arguments to print out
void printThreadMessage(const char *message, ...){
	/*
	sb_printTh.sem_num = 0;
	sb_printTh.sem_op = -1;
	sb_printTh.sem_flg = 0;
	semop(semid_printTh,&sb_printTh,1);
	
	va_list args;
	va_start(args,message);
	vprintf(message,args);
	va_end(args);
	
	sb_printTh.sem_op = 1;
	semop(semid_printTh,&sb_printTh,1);
	*/
	
	/*
	//sem lock
	sb_sim.sem_num = 1;
	sb_sim.sem_op = -1;
	sb_sim.sem_flg = 0;
	semop(semid_sim,&sb_sim,1);
	*/
		
	
	va_list args;
	va_start(args,message);
	vprintf(message,args);
	va_end(args);
	
	/*
	//sem unlock
	sb_sim.sem_op = 1;
	semop(semid_sim,&sb_sim,1);
	*/
	
}

void foo_client(int num){
	printThreadMessage("%.3f Customer %d: created PID %llu PPID %d\n", getTimeWork(),num,getpid(),getppid());
	//a)if not elapsed simulation time
	while(isSimWorks()){
		//b)sleep for 3 to 6 seconds randomly
		int time_sleep = real_random()%6+3;
		
		sleep(time_sleep);
		//c)Read the menu (1 second);
		//d)If the previous order has not yet been done, loop to (a)
		int isContinue = 0;
		//sem lock
		sb_sim.sem_num = 0;
		sb_sim.sem_op = -1;
		sb_sim.sem_flg = 0;
		semop(semid_sim,&sb_sim,1);
		
		if(getOrderBoard()[num]->done == 0)
			isContinue = 1;//it's need because need to unlock mutex before Continue
		//sem unlock
		sb_sim.sem_op = 1;
		semop(semid_sim,&sb_sim,1);
		
		if (isContinue)
			continue;
		//e)with the probability 0.5 client will order 
		if (rand()%2){
			
			//i)randomly choose item and amount
			int item = rand()%getSizeMenu();
			int amount = rand()%4+1;
			char name_item[20];
			//ii)write the order to the board
			//sem lock
			sb_sim.sem_num = 0;
			sb_sim.sem_op = -1;
			sb_sim.sem_flg = 0;
			semop(semid_sim,&sb_sim,1);
			getOrderBoard()[num]->itemId = item;
			getOrderBoard()[num]->amount = amount;
			getOrderBoard()[num]->done = 0;
			strcpy(name_item, getMenu()[item]->name);
			//sem unlock
			sb_sim.sem_op = 1;
			semop(semid_sim,&sb_sim,1);
			
			printThreadMessage("%.3f Customer %d: reads a menu about %s(ordered, amount %d)\n", getTimeWork(),num, name_item, amount);
		}else{
		//f)with the probability 0.5 client does not order
		//we can choose random dish for this message, because it will not be ordered
		char name_item[20];
		strcpy(name_item, getMenu()[rand()%getSizeMenu()]->name);
		printThreadMessage("%.3f Customer %d: reads a menu about %s(doesn't want to order)\n", getTimeWork(),num, name_item);
		sleep(1);//there is sleep from (c)
		//g)loop to (a)
		}
		
	}
	printThreadMessage("%.3f Customer ID %d: PID %llu end work PPID %d\n",num, getTimeWork(), getpid(), getppid());
}

void foo_waiter(int num){
	printThreadMessage("%.3f Waiter %d: created PID %d PPID %d\n", getTimeWork(),num,getpid(),getppid());
	//a)if not elapsed simulation time
	while(isSimWorks()){
		//b)sleep for 1-2 seconds randomly
		sleep(1+real_random()%2);
		//c)read an order from the "order board"
		//sem lock
		
		sb_sim.sem_num = 0;
		sb_sim.sem_op = -1;
		sb_sim.sem_flg = 0;
		semop(semid_sim,&sb_sim,1);
		
		orderItem **orders = getOrderBoard();
		
		int i = 0;
		int amount = 0;
		char name[20];
		while(orders[i] != NULL){
			//d) and e): if there is row that isn't Done
			if (orders[i]->done == 0){
				//i)add the amount ordered to the totals in main menu
				getMenu()[orders[i]->itemId]->orders += orders[i]->amount;
				//ii)mark the order as Done
				orders[i]->done = 1;
				amount = orders[i]->amount;
				strcpy(name,getMenu()[orders[i]->itemId]->name);
				//printThreadMessage("%.3f Waiter %d: performs the order of customer ID %d (%d %s)\n", getTimeWork(), num,i,orders[i]->amount, getMenu()[orders[i]->itemId]->name);
				break;//we need to give work for other waiters if performed at least one order
			}	
			i++;
		}
		//sem unlock
		sb_sim.sem_op = 1;
		semop(semid_sim,&sb_sim,1);
		if (orders[i] != NULL)
			printThreadMessage("%.3f Waiter %d: performs the order of customer ID %d (%d %s)\n", getTimeWork(), num,i,amount, name);
		
	}
	printThreadMessage("%.3f Waiter ID %d: PID %d end work PPID %d\n",num, getTimeWork(), getpid(), getppid());
}

void initTimerEndSim(int time){
	//create timer to end simulation in child process
	initTimerSim();
	if (fork() == 0){
		sleep(time);
		stopSim();
		exit(0);
	}
}

int real_random(){
	srandom(time(0) ^ (getpid()<<16));
	return rand();  
}
