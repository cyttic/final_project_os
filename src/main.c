#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
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

	//we're using signal SIGALRM to hande function for stop clients and waiters threads
	signal(SIGALRM, alarmEndSimulation);
	alarm(time_simulation);

	pthread_t waiters_th[count_waiters];
	pthread_t clients_th[count_clients];
	//create menu
	setMenu(size_menu);
	printMenu(getMenu());
	//create orders board
	setOrderBoard(count_clients);
	//initialization a clients threads
	printThreadMessage("%.3f Main process start creating sub-process\n", getTimeWork());
	//using type "long" in the loop For to avoid a warings
	for(long i = 0; i < count_client; ++i){
		int id = fork();
		if (id == 0){
			foo_client();
			exit();
		}else{
			wait(NULL);
		}
	}
	
	/*
	for(long i = 0; i < count_clients; ++i)
		pthread_create(&clients_th[i], NULL, th_foo_client,(void*)i);
	for(long i = 0; i < count_waiters; ++i)
		pthread_create(&waiters_th[i], NULL, th_foo_waiter,(void*)i);

	for(int i = 0; i < count_clients; ++i)
		pthread_join(clients_th[i],NULL);
	
	for(int i = 0; i < count_waiters; ++i)
		pthread_join(waiters_th[i],NULL);
		*/


	printMenu(getMenu());
	printThreadMessage("Total orders %d, for an amount %.2f NIL\n",getCountItems(), getTotal());
	printThreadMessage("%.3f Main ID %d end work\n",getTimeWork(), getpid());
	printThreadMessage("%.3f End of simulation\n");
	return 0;
}
