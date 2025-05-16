typedef struct{
	int id;
	char *name;
	float price;
	int orders;
}menuItem;

int simulation(int);
menuItem** initMenu();
void printMenu(menuItem**);
