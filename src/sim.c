#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "sim.h"

menuItem** initMenu(int size){
	if (size < 5 || size > 7)
		close_program("Size of menu must be between 5 and 7 items\n");
	char names[7][10] = {"Pizza","Salad","Hamburger","Spagetti","Pie","Milkshake","Falafel"};
	menuItem **items = malloc(sizeof(*items)*size);
	for(int i = 0; i < size; ++i){
		items[i] = malloc(sizeof(menuItem*));
		items[i]->id = i+1;//just set up id of items in the loop
		items[i]->price = rand()%20 + 3;//set up price for item via rand
		items[i]->name = malloc(sizeof(names[i])+1);
		strcpy(items[i]->name,names[i]);
	}
	items[size] = NULL;
	return items;
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

/*
float getTime(){
	static float internal_timer = (float)clock();
	internal_timer = internal_timer/CLOCKS_PER_SEC;
}
*/

void th_foo_client(){
	printf("hello Gena!\n");
}
