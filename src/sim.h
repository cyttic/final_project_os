typedef struct{
	int id;
	char *name;
	float price;
	int orders;
}menuItem;

typedef struct{
	int customerId;
	int itemId;
	int amount;
	int done;
}orderItem;

int simulation(int);
//menuItem** initMenu(int);
menuItem** _controlMenu(int);
void setMenu(int);
menuItem** getMenu();
orderItem **initOrderBoard(int);
void printMenu(menuItem**);
void close_program(char *);
float getTime();
void *th_foo_client();
void *th_foo_waiter();
void alarmEndSimulation();
int isSimWorks();
void stopSim();
int controlSim(int);
//create a shared memory for struct menu
void *getShmat(int);
//display to output must be thread-safe operation
void printThreadMessage(const char*, ...);
