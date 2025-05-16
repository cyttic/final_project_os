#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sim.h"

menuItem** initMenu(){
	char names[7][10] = {"Pizza","Salad","Hamburger","Spagetti","Pie","Milkshake","Falafel"};
	menuItem **items = malloc(sizeof(*items)*7);
	for(int i = 0; i < 6; ++i){
		items[i] = malloc(sizeof(menuItem*));
		items[i]->id = i+1;//just set up id of items in the loop
		items[i]->price = rand()%20 + 3;//set up price for item via rand
		items[i]->name = malloc(sizeof(names[i])+1);
		strcpy(items[i]->name,names[i]);
	}
	items[7] = NULL;
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
