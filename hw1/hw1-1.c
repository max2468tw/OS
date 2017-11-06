#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	int status;
	printf("Main Process ID : %d\n\n", getpid());
	pid_t pid1,pid2,pid3,pid4,pid5;
	pid1 = fork();
	if (pid1 < 0) {
		fprintf(stderr, "Fork Failed");
		return 1;
	}
	else if (pid1 == 0){
		printf("Fork1. I'm the child %d, my parent is %d\n",getpid(),getppid());
		pid2 = fork();
		if (pid2 < 0) {
			fprintf(stderr, "Fork Failed");
			return 1;
		}
		else if (pid2 == 0){
			printf("Fork2. I'm the child %d, my parent is %d\n",getpid(),getppid());
			pid4 = fork();
			if (pid4 < 0) {
				fprintf(stderr, "Fork Failed");
				return 1;
			}
			else if (pid4 == 0){
				printf("Fork3. I'm the child %d, my parent is %d\n",getpid(),getppid());
				return 0;
			}
			else {
				printf("Child%d Complete\n",wait(&status));
			}
			return 0;
		}
		else{
			pid3 = fork();
			if (pid3 < 0) {
				fprintf(stderr, "Fork Failed");
				return 1;
			}
			else if (pid3 == 0){
				printf("Fork2. I'm the child %d, my parent is %d\n",getpid(),getppid());
				pid5 = fork();
				if (pid5 < 0) {
					fprintf(stderr, "Fork Failed");
					return 1;
				}
				else if (pid5 == 0){
					printf("Fork3. I'm the child %d, my parent is %d\n",getpid(),getppid());
					return 0;
				}
				else {
					printf("Child%d Complete\n",wait(&status));
				}
				return 0;
			}
			else {
				printf("Child%d Complete\n",wait(&status));
			}
			printf("Child%d Complete\n",wait(&status));
		}
	}
	else {
		printf("Child%d Complete\n",wait(&status));
	}
	return 0;
}
