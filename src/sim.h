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

//menu
menuItem** _controlMenu(int);
void setMenu(int);
menuItem** getMenu();
//orders
orderItem** _controlOrderBoard(int);
void setOrderBoard(int);
orderItem** getOrderBoard();
//size menu - it would be easely to save this value then call getMenu with mutexs
int _controlSizeMenu(int);
void setSizeMenu(int);
int getSizeMenu();

void printMenu(menuItem**);
void close_program(char *);
float getTime();
//most work here
void *th_foo_client(void *);
void *th_foo_waiter(void *);

void alarmEndSimulation();
int isSimWorks();
void stopSim();
int controlSim(int);
//create a shared memory for struct menu
void *getShmat(int);
//display to output must be thread-safe operation
void printThreadMessage(const char*, ...);
//count result
float getTotal();
int getCountItems();
//client and waiter process
void foo_client();
void foo_waiter();
