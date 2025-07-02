#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <sys/types.h>
#include "timer.h"
#include "sim.h"

//function with symbol _ are for internal use only!
//static menuItem **items;
menuItem** _controlMenu(int size){//синглтон для доступа к меню
	static menuItem **items;//статический массив структур меню айтемс
	if (size >= 1){//size less of 0 means there is setMenu call
		if (size < 5 || size > 7)
			close_program("Size of menu must be between 5 and 7 items\n");
		setSizeMenu(size);
		char names[7][10] = {"Pizza","Salad","Hamburger","Spagetti","Pie","Milkshake","Falafel"};
		//menuItem **items = malloc(sizeof(menuItem*)*size);
		//We need to use shared memory according the task, but in this realisation we're using threads instead of a process, and it would be fine without it
		//items = getShmat(sizeof(menuItem**));
		//items = malloc(sizeof(menuItem**));
		items = malloc(sizeof(menuItem*) * size);//выделяем память через стандартный маллок
		for(int i = 0; i < size; ++i){
			//items[i] = malloc(sizeof(menuItem*)); // non thread-safe version
			items[i] = mmap(NULL, sizeof *items[i], PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1,0);//используем mmap для иницилизации шаред ресурса для мультипроцесс программы??
			items[i]->id = i+1;//just set up id of items in the loop
			items[i]->price = rand()%20 + 3;//set up price for item via rand
			items[i]->name = (char*)malloc(sizeof(names[i])+1);//для указателя на имя можно использовать маллок в данном случае??
			strcpy(items[i]->name,names[i]);
		}
		items[size] = NULL;//последний элемент нуль для удобства использования массива
	}
	return items;
}

void setMenu(int size){
	if (size < 1)
		close_program("Wrong argument for setMenu(int) call\n");
	_controlMenu(size);
}

menuItem** getMenu(){//прокси для вызова меню
	return _controlMenu(-1);
}

orderItem** _controlOrderBoard(int size){//синглтон для вызова списка заказов
	static orderItem **items;//храним статический указатель в соответсвии с паттерном Синглтон
	if (size > 0){//if parametr size <= 0 then it's call getOrderBoard()
		//we use threads, so we can get just malloc to allocate memory 
		items = malloc(sizeof(orderItem*)*size);//сам массив инициализируем с помощью маллок??
		for(int i = 0; i < size; ++i){
			//items[i] = malloc(sizeof(orderItem*));
			items[i] = mmap(NULL, sizeof *items[i], PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1,0);//выделяем память с помощью mmap для использования шаред ресурс
			items[i]->customerId = i;
			items[i]->itemId = -1; //newermind
			items[i]->amount = -1; //newermind
			items[i]->done = 1;    // 1 - finished, 0 - does not
		}	
		items[size] = NULL;//последний элемент массива нуль для удобства итерации по циклу
	}
	return items;
}

void setOrderBoard(int size){
	if (size < 1)
		close_program("Wrong argument for setOrderBoard(int) call\n");
	_controlOrderBoard(size);
}

orderItem** getOrderBoard(){//запрос массива на указатели заказов с помощью паттерна Прокси
	return _controlOrderBoard(-1);
}

int _controlSizeMenu(int size){//функция для того чтобы запомнить и получить размер созданного меню
	static int size_menu;
	if (size > 0)
		size_menu = size;
	return size_menu;
}

void setSizeMenu(int size){//прокси для установки размера меню
	_controlSizeMenu(size);
}

int getSizeMenu(){//прокси для запроса размера меню
	return _controlSizeMenu(-1);
}

void printMenu(menuItem **menu){//функция для распечатки меню. Не обязательно должна быть потокобезопасной так как вызывается всегда только из главного процесса и не вызывается из процессов клиентов и официантов
	//NOTE: we don't need to use thread-sage function printThreadMessage because printMenu is used only on main process!!
	int size = getSizeMenu();

	/*printThreadMessage*/printf("======================Menu list=====================\n");
	/*printThreadMessage*/printf("%-7s %-12s %-8s %-12s\n", "Id", "Name", "Price","Order");
	for(int i = 0; i < size; ++i){
		//if(menu[i] == NULL) break;
		/*printThreadMessage*/printf("%-7d %-12s %-8.2f %-12d\n",
			menu[i]->id,
			menu[i]->name,
			menu[i]->price,
			menu[i]->orders);
	}
	/*printThreadMessage*/printf("=====================================================\n");
}

void close_program(char *msg){//выход с кодом -3
	perror(msg);
	exit(-3);
}

static int *timerSim;//переменная которую проверяем в циклах процессов Клиент и Официант для контроля времени симуляции
void stopSim(){
	*timerSim = 0;
}

void initTimerSim(){//инициализация переменной для контроля времени симуляции?? почему она иницилизирована с помощью mmap - не знаю, возможно наверное было бы сделать ее обычной интеджер
	timerSim = mmap(NULL, sizeof *timerSim, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1,0);
	*timerSim = 1;
}

int controlSim(){//для удобства вызова, в целом можно использовать timerSim напрямую
	return *timerSim;
}

int isSimWorks(){//функция повтор того что выше?? забыл удалить
	return *timerSim;
}

//вся эта функция используется в версии для потоков, в реализации для процессов другая
pthread_mutex_t mutex_client_waiter;//не нужен в данной реализации??забыли удалить, осталась после реализации на потоках
void *th_foo_client(void *thread_id){//
	long num = (long)thread_id;//это номер процесса
	printThreadMessage("%.3f Customer %d: created PID %llu PPID %d\n", getTimeWork(),num,getpid(),getppid());
	//a)if not elapsed simulation time
	while(isSimWorks()){//проверка переменной которая отвечет за время работы симуляции
		sleep(2);
		
		//b)sleep for 3 to 6 seconds randomly
		int time_sleep = rand()%6+3;//ждем время 3-8 секунд
		sleep(time_sleep);
		//c)Read the menu (1 second);
		//d)If the previous order has not yet been done, loop to (a)
		int isContinue = 0;
		pthread_mutex_lock(&mutex_client_waiter);
		if(getOrderBoard()[num]->done == 0)
			isContinue = 1;//it's need because need to unlock mutex before Continue
		pthread_mutex_unlock(&mutex_client_waiter);
		if (isContinue)
			continue;
		//e)with the probability 0.5 client will order 
		if (rand()%2){
			
			//i)randomly choose item and amount
			int item = rand()%getSizeMenu();
			int amount = rand()%4+1;
			char name_item[20];
			//ii)write the order to the board
			pthread_mutex_lock(&mutex_client_waiter);
			getOrderBoard()[num]->itemId = item;
			getOrderBoard()[num]->amount = amount;
			getOrderBoard()[num]->done = 0;
			strcpy(name_item, getMenu()[item]->name);
			pthread_mutex_unlock(&mutex_client_waiter);
			printThreadMessage("%.3f Customer %d: reads a menu about %s(ordered, amount %d)\n", getTimeWork(),num, name_item, amount);
		}else{
		//f)with the probability 0.5 client does not order
		//we can choose random dish for this message, because it will not be ordered
		char name_item[20];
		pthread_mutex_lock(&mutex_client_waiter); 
		strcpy(name_item, getMenu()[rand()%getSizeMenu()]->name);
		pthread_mutex_unlock(&mutex_client_waiter);
		printThreadMessage("%.3f Customer %d: reads a menu about %s(doesn't want to order)\n", getTimeWork(),num, name_item);
		sleep(1);//there is sleep from (c)
		//g)loop to (a)
		}
		
	}
	printThreadMessage("%.3f Customer ID %d: PID %llu end work PPID %d\n",num, getTimeWork(), getpid(), getppid());
}

void *th_foo_waiter(void *thread_id){
	long num = (long)(thread_id);
	printThreadMessage("%.3f Waiter %d: created PID %llu PPID %d\n", getTimeWork(),num,getpid(),getppid());
	//a)if not elapsed simulation time
	while(isSimWorks()){
		//b)sleep for 1-2 seconds randomly
		sleep(1+rand()%2);
		//c)read an order from the "order board"
		pthread_mutex_lock(&mutex_client_waiter);
		orderItem **orders = getOrderBoard();
		//for(int i = 0;i < 
		int i = 0;
		while(orders[i] != NULL){
			//d) and e): if there is row that isn't Done
			if (orders[i]->done == 0){
				//i)add the amount ordered to the totals in main menu
				getMenu()[orders[i]->itemId]->orders += orders[i]->amount;
				//ii)mark the order as Done
				orders[i]->done = 1;
				printThreadMessage("%.3f Waiter %d: performs the order of customer ID %d (%d %s)\n", getTimeWork(), num,i,orders[i]->amount, getMenu()[orders[i]->itemId]->name);
			}
			i++;
		}
		pthread_mutex_unlock(&mutex_client_waiter);
	}
	printThreadMessage("%.3f Waiter ID %d: PID %llu end work PPID %d\n",num, getTimeWork(), getpid(), getppid());
}



//this version to print out from semaphore block instead of thread-safe function to avoiding nested semaphores
void printOneThreadMessage(const char *message, ...){//враппер для распечатки сообщений в формате принтФ
	va_list args;
	va_start(args,message);
	vprintf(message,args);
	va_end(args);
}

float getTotal(){//расчет суммы заказов в меню
	float total = 0.0;
	menuItem **menu = getMenu();
	for(int i = 0; i < getSizeMenu(); ++i)
		total += menu[i]->price * menu[i]->orders;
	return total;

}

int getCountItems(){//получение суммы количества заказов
	int total = 0;
	menuItem **menu = getMenu();
	for(int i = 0; i < getSizeMenu(); ++i)
		total += menu[i]-> orders;
	return total;
}


//семафор и его инициализация для общих ресурсов у Клиентов и Официантов
int semid_sim;
struct sembuf sb_sim;
unsigned short sem_values_sim[1] = {1};
void initSemSim(){
	semid_sim = semget(1230,2, IPC_CREAT | 0666);//параметры - 1230 - просто число, кей, как идентификатор.2 - количество семафоров?? .IPC_CREAT | 0666 - флаги
	if (semid_sim == -1)//проверки на ошибки
		close_program("Error initialise semid_sim in funciton initSemSim\n");
	if (semctl(semid_sim,0, SETVAL, sem_values_sim) == -1)
		close_program("Error semctl in funciton initSemSim\n");
	if (semctl(semid_sim,1, SETVAL, sem_values_sim) == -1)
		close_program("Error semctl print in funciton initSemSim\n");
}

//using vprintf instead of printf much more ease to pass arguments to print out
void printThreadMessage(const char *message, ...){//функция для распечатывания сообщений в многопоточной среде, реализована с помощью семафоров
	//sem lock
	sb_sim.sem_num = 1;
	sb_sim.sem_op = -1;
	sb_sim.sem_flg = 0;
	semop(semid_sim,&sb_sim,1);
		
	va_list args;
	va_start(args,message);
	vprintf(message,args);
	va_end(args);
	
	//sem unlock
	sb_sim.sem_op = 1;
	semop(semid_sim,&sb_sim,1);
	
}

void foo_client(int num){//код процесса клиента
	printThreadMessage("%.3f Customer %d: created PID %llu PPID %d\n", getTimeWork(),num,getpid(),getppid());
	//a)if not elapsed simulation time
	while(isSimWorks()){//проверка на окончание времени симуляции
		//b)sleep for 3 to 6 seconds randomly
		int time_sleep = real_random()%6+3;//ждем случайное время 3-6 секунд?? тут ошибка, на самом деле ждем 3-8 секунд
		
		sleep(time_sleep);
		if (!isSimWorks())//здесь проверяем на окончание время симуляции чтобы не ждать лишнее время
			break;//the restoraunt was closed waiter a client was sleeped
		//c)Read the menu (1 second);
		//d)If the previous order has not yet been done, loop to (a)
		int isContinue = 0;
		//sem lock
		//инициализация семафора
		sb_sim.sem_num = 0;
		sb_sim.sem_op = -1;
		sb_sim.sem_flg = 0;
		semop(semid_sim,&sb_sim,1);
		
		if(getOrderBoard()[num]->done == 0)//проверка согласно условию - если клиент уже что-то заказал но заказ еще не выполнен, то просто продолжаем ждать
			isContinue = 1;//it's need because need to unlock mutex before Continue
		//sem unlock
		sb_sim.sem_op = 1;
		semop(semid_sim,&sb_sim,1);
		
		if (isContinue)
			continue;
		//e)with the probability 0.5 client will order 
		if (rand()%2){//клиент делает заказ
			
			//i)randomly choose item and amount
			int item = rand()%getSizeMenu();
			int amount = rand()%4+1;
			char name_item[20];
			//ii)write the order to the board
			//sem lock
			//инициализируем семафор и меняем значения в общем ресурсе - меню и списке заказов
			sb_sim.sem_num = 0;
			sb_sim.sem_op = -1;
			sb_sim.sem_flg = 0;
			semop(semid_sim,&sb_sim,1);
			getOrderBoard()[num]->itemId = item;
			getOrderBoard()[num]->amount = amount;
			getOrderBoard()[num]->done = 0;
			strcpy(name_item, getMenu()[item]->name);
			//sem unlock
			//выполняем операцию Down для семафора
			sb_sim.sem_op = 1;
			semop(semid_sim,&sb_sim,1);
			
			printThreadMessage("%.3f Customer %d: reads a menu about %s(ordered, amount %d)\n", getTimeWork(),num, name_item, amount);
		}else{
		//f)with the probability 0.5 client does not order
		//we can choose random dish for this message, because it will not be ordered
		char name_item[20];
		//если клиент не захотел заказывать - просто так и пишем
		strcpy(name_item, getMenu()[rand()%getSizeMenu()]->name);
		printThreadMessage("%.3f Customer %d: reads a menu about %s(doesn't want to order)\n", getTimeWork(),num, name_item);
		sleep(1);//there is sleep from (c)
		//g)loop to (a)
		}
		
	}
	printThreadMessage("%.3f Customer ID %d: PID %llu end work PPID %d\n",num, getTimeWork(), getpid(), getppid());
}

void foo_waiter(int num){//функция работы процесса официантов
	printThreadMessage("%.3f Waiter %d: created PID %d PPID %d\n", getTimeWork(),num,getpid(),getppid());
	//a)if not elapsed simulation time
	while(isSimWorks()){//проверка на продолжение времени симуляции
		//b)sleep for 1-2 seconds randomly
		sleep(1+real_random()%2);
		if (!isSimWorks())//проверка на продолжение времени симуляции
			break;//the restoraunt was closed waiter a client was sleeped
		//c)read an order from the "order board"
		//sem lock
		//инициализируем семафор
		sb_sim.sem_num = 0;
		sb_sim.sem_op = -1;
		sb_sim.sem_flg = 0;
		semop(semid_sim,&sb_sim,1);
		//получение общего ресурса - списка заказов
		orderItem **orders = getOrderBoard();
		
		int i = 0;
		int amount = 0;
		char name[20];
		while(orders[i] != NULL){//проход в цикле по всем заказам
			//d) and e): if there is row that isn't Done
			if (orders[i]->done == 0){//нашли тот который еще не выполнен
				//i)add the amount ordered to the totals in main menu
				getMenu()[orders[i]->itemId]->orders += orders[i]->amount;
				//ii)mark the order as Done
				orders[i]->done = 1;//ТУТ ВЕСЬ КОД ПРОСТО КОПИРУЕТ ПОЛЯ ЗАКАЗА
				amount = orders[i]->amount;
				strcpy(name,getMenu()[orders[i]->itemId]->name);
				//printThreadMessage("%.3f Waiter %d: performs the order of customer ID %d (%d %s)\n", getTimeWork(), num,i,orders[i]->amount, getMenu()[orders[i]->itemId]->name);
				break;//we need to give work for other waiters if performed at least one order
			}	
			i++;
		}
		//sem unlock
		sb_sim.sem_op = 1;//отпускаем семафор
		semop(semid_sim,&sb_sim,1);
		if (orders[i] != NULL)// ?? здесь сомнительный момент, так как не удалось сделать распечатку внутри семафора(код выше) я просто вынес сюда. Если спросит, сказать что можно было и там, но просто решили что так удобнее, разницы нет и т.д
			printThreadMessage("%.3f Waiter %d: performs the order of customer ID %d (%d %s)\n", getTimeWork(), num,i,amount, name);
		
	}
	printThreadMessage("%.3f Waiter ID %d: PID %d end work PPID %d\n",num, getTimeWork(), getpid(), getppid());
}

void initTimerEndSim(int time){//для установки времени симуляции создаем дочерний процесс который спит заданное время и после этого меняет значение переменной которая отвечает за время симуляции
	//create timer to end simulation in child process
	initTimerSim();
	if (fork() == 0){
		sleep(time);
		stopSim();
		exit(0);
	}
}

int real_random(){//функция чтобы каждый запуск время ожидания в процессах было случайным
	//seed to random time sleep in every execute
	srandom(time(0) ^ (getpid()<<16));
	return rand();  
}

void clean_up_resourcecs(){//инкапсуляция всех функций освобождения памяти и ресурсов
	//clean up at a semaphores
	semctl(semid_sim, 0, IPC_RMID);
	//clean up at a shared memory
	clean_up_timer();
	munmap(timerSim, sizeof *timerSim);
	menuItem **menu = getMenu();
	for(int i = 0; i < getSizeMenu(); ++i)
		munmap(menu[i], sizeof *menu[i]);
	
	int i = 0;
	orderItem **orders = getOrderBoard();
	while(orders[i++] != NULL)
		munmap(orders[i], sizeof *orders[i]);
	//clean up at simple memory
	free(menu);
	free(orders);
}
