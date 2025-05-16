#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "sim.h"
#include "timer.h"

int main(int argc , char ** argv){
	menuItem item;

	printf("%f\n", getTimeWork());
	if (argc != 5){
		perror("It's must to be 4 argumetns!!\n");
		return -3;
	}
	int time_simulation = 1;
	int count_waiters = 1;
	int count_clients = 1;

	//we're using signal SIGALRM to hande function for stop clients and waiters threads
	signal(SIGALRM, alarmEndSimulation);
	alarm(time_simulation);

	pthread_t waiters_th[count_waiters];
	pthread_t clients_th[count_clients];

	//initialization a clients threads
	for(int i = 0; i < count_clients; ++i){
		long t;
		pthread_create(&clients_th[i], NULL, th_foo_client,&t);
		pthread_join(clients_th[i],NULL);
	}


	//simulation(42);
	menuItem **menu = initMenu(6);
	printMenu(menu);


	printf("%f\n",getTimeWork());
	return 0;
}
