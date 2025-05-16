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
float getTime();
void *th_foo_client();
void *th_foo_waiter();
void alarmEndSimulation();
int isSimWorks();
void stopSim();
int controlSim(int);
