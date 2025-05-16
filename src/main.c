#include <stdio.h>
#include <unistd.h>
#include "sim.h"
#include "timer.h"

int main(int argc , char ** argv){
	menuItem item;

	printf("%f\n", getTimeWork());
	if (argc != 4){
		perror("It's must to be 3 argumetns!!\n");
		return -3;
	}
	//simulation(42);
	sleep(2);
	menuItem **menu = initMenu(6);
	printMenu(menu);

	printf("%f\n",getTimeWork());
	return 0;
}
