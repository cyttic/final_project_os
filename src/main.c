#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <wait.h>
#include "sim.h"
#include "timer.h"


int main(int argc , char ** argv){
	menuItem item;

	if (argc != 5){
		perror("It's must to be 4 argumetns!!\n");
		return -3;
	}
	int time_simulation = atoi(argv[1]);
	int size_menu       = atoi(argv[2]);
	int count_clients   = atoi(argv[3]);
	int count_waiters   = atoi(argv[4]);

	initTimerEndSim(time_simulation);
	initTimerSem();
	initSharedTimer();

	//create menu
	setMenu(size_menu);
	printMenu(getMenu());
	//create orders board
	setOrderBoard(count_clients);
	//initialization a clients threads
	printThreadMessage("%.3f Main process start creating sub-process\n", getTimeWork());
	//using type "long" in the loop For to avoid a warings
	pid_t pid_arr_cl[count_clients];
	for(long i = 0; i < count_clients; ++i){
		if ((pid_arr_cl[i] = fork()) == 0){
			foo_client(i);
			exit(i);
		}else{
		}
	}
	
	pid_t pid_arr_waiters[count_waiters];
	for(long i = 0; i < count_waiters; ++i){
		if ((pid_arr_waiters[i] = fork()) == 0){
			foo_waiter(i);
			exit(i);
		}else{
		}
	}

	pid_t stat;
	for(int i= 0; i< count_clients; ++i)
		waitpid(pid_arr_cl[i], &stat,0);
	for(int i = 0; i < count_waiters; ++i)
		waitpid(pid_arr_waiters[i], &stat,0);

	printMenu(getMenu());
	printThreadMessage("Total orders %d, for an amount %.2f NIS\n",getCountItems(), getTotal());
	printThreadMessage("%.3f Main ID %d end work\n",getTimeWork(), getpid());
	printThreadMessage("%.3f End of simulation\n");
	clean_up_resourcecs();
	return 0;
}
