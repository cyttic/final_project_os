
#include <time.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

static int semid_timer = -1;
static struct sembuf sb_timer;
static struct timespec *tmr = NULL;

void initTimerSem() {//инициализация семафора таймера
    semid_timer = semget(12368, 1, IPC_CREAT | 0666);
    if (semid_timer == -1) {
        perror("semget failed");
        exit(1);
    }
    unsigned short sem_val[1] = {1};
    if (semctl(semid_timer, 0, SETALL, sem_val) == -1) {
        perror("semctl failed");
        exit(1);
    }
}

void initSharedTimer() {//инициализация общего ресурса с помощью mmap - времени начала симуляции от которого будем брать разницу
    tmr = mmap(NULL, sizeof *tmr, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (tmr == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }
    clock_gettime(CLOCK_REALTIME, tmr);//используем стандартную функцию
}

double getTimeWork() {//функция возвращает время
    // lock
    //инициализация семафора
    sb_timer.sem_num = 0;
    sb_timer.sem_op = -1;
    sb_timer.sem_flg = 0;
    if (semop(semid_timer, &sb_timer, 1) == -1) {
        perror("semop lock failed");
        exit(1);
    }

    struct timespec now;//запрос текущего времени для вычисления разницы
    clock_gettime(CLOCK_REALTIME, &now);
    double result = (now.tv_sec + now.tv_nsec/1e7) - (tmr->tv_sec + tmr->tv_nsec / 1e7);

    // unlock
    //отпускаем семафор
    sb_timer.sem_op = 1;
    if (semop(semid_timer, &sb_timer, 1) == -1) {
        perror("semop unlock failed");
        exit(1);
    }

    return result >= 0.0 ? result : 0.0;
}

void clean_up_timer(){//очистка ресурсов
	munmap(tmr, sizeof *tmr);
}

