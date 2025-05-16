typedef struct{
	int id;
	char *name;
	float price;
	int orders;
}menuItem;

int simulation(int);
menuItem** initMenu(int);
void printMenu(menuItem**);
void close_program(char *);
