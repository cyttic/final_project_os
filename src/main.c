#include <stdio.h>

int main(int argc , char ** argv){
	if (argc != 4){
		perror("It's must to be 3 argumetns!!\n");
		return -3;
	}
	return 0;
}
