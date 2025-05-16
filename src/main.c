#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "sim.h"
#include "timer.h"

int main(int argc , char ** argv){
	menuItem item;

	printf("%f\n", getTimeWork());
	if (argc != 5){
		perror("It's must to be 4 argumetns!!\n");
		return -3;
	}
	int count_waiters = 1;
	int count_clients = 1;

	pthread_t waiters_th[count_waiters];
	pthread_t clients_th[count_clients];

	//initialization a clients threads
	for(int i = 0; i < count_clients; ++i){
		long t;
		if (pthread_create(&clients_th[i], NULL, th_foo_client,&t))
			close_program("Unsucessful attempt to create a thread for client number %d\n",i);
	}


	//simulation(42);
	sleep(2);
	menuItem **menu = initMenu(6);
	printMenu(menu);

	printf("%f\n",getTimeWork());
	return 0;
}
