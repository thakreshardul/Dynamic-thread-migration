#include <stdio.h>
#include <unistd.h>

int main(){
	int counter=1;
	while(counter < 10){
		printf("%d\t",counter++);
		sleep(1);
		fflush(stdout);
	}
	return 0;
}
