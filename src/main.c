#include <stdio.h>
#include "sim.h"

int main(int argc , char ** argv){
	menuItem item;


	if (argc != 4){
		perror("It's must to be 3 argumetns!!\n");
		return -3;
	}
	simulation(42);
	return 0;
}
