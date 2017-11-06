#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
int main()
{
    int shmid ;
    int *shmaddr ;
    struct shmid_ds buf ;
    int flag = 0 ;
    int pid ;
	int N;
	int N4;
	unsigned int sum1, sum4;
	printf("Dimension: ");
	scanf("%d",&N);
	N4 = N/4+(N%4!=0);
	int SIZE = N*N*4*3;
    shmid = shmget(IPC_PRIVATE, SIZE, IPC_CREAT|0600 ) ;
    if ( shmid < 0 )
    {
            perror("get shm  ipc_id error") ;
            return -1 ;
    }
    pid = fork() ;
    if ( pid == 0 )
    {
        shmaddr = (int *)shmat( shmid, NULL, 0 ) ;
        if ( (long)shmaddr == -1 )
        {
            perror("shmat addr error") ;
            return -1 ;

        }
		unsigned int i;
		for (i = 0; i < N*N; i++){
			*(shmaddr+i) = i;
		}
		shmdt( shmaddr ) ;
        return  0;
    } else if ( pid > 0) {
        sleep(3) ;
		struct timeval start, end;
        flag = shmctl( shmid, IPC_STAT, &buf) ;
        if ( flag == -1 )
        {
            perror("shmctl shm error") ;
            return -1 ;
        }
        shmaddr = (int *) shmat(shmid, NULL, 0 ) ;
        if ( (long)shmaddr == -1 )
        {
            perror("shmat addr error") ;
            return -1 ;

        }
		int i,j,k;
		sum1 = 0;
		gettimeofday(&start,0);
		for (i = 0; i < N; i++){
			for(j = 0;j < N; j++){
				for(k = 0;k < N; k++){
					*(shmaddr+N*N+i+j*N) += (*(shmaddr+k+j*N))*(*(shmaddr+k*N+i));
				}
			}
		}
		gettimeofday(&end,0);
		sum1 = 0;
		for (i = 0; i < N; i++){
			for(j = 0;j < N; j++){
				sum1+=*(shmaddr+N*N+i+j*N);
			}
		}
		printf("1-process, checksum = %u\n",sum1);
		int sec = end.tv_sec-start.tv_sec;
		int usec = end.tv_usec-start.tv_usec;
		printf("elapsed %f ms\n",sec*1000+(usec/1000.0));
		gettimeofday(&start,0);
		pid_t pid1,pid2,pid3;
		pid1 = fork();
		if (pid1 < 0) {
			fprintf(stderr, "Fork Failed");
			return 1;
		}
		else if (pid1 == 0){
			pid2 = fork();
			if (pid2 < 0) {
				fprintf(stderr, "Fork Failed");
				return 1;
			}
			else if (pid2 == 0){
				pid3 = fork();
				if (pid3 < 0) {
					fprintf(stderr, "Fork Failed");
					return 1;
				}
				else if (pid3 == 0){
					for (i = 0; i < N; i++){
						for(j = 0;j < N4; j++){
							for(k = 0;k < N; k++){
								*(shmaddr+N*N*2+i+j*N) += (*(shmaddr+k+j*N))*(*(shmaddr+k*N+i));
							}
						}
					}
					return 0;
				}
				else {
					for (i = 0; i < N; i++){
						for(j = N4;j < N4*2; j++){
							for(k = 0;k < N; k++){
								*(shmaddr+N*N*2+i+j*N) += (*(shmaddr+k+j*N))*(*(shmaddr+k*N+i));
							}
						}
					}
					wait(NULL);
					//printf("P1 Complete\n");
				}
				return 0;
			}
			else {
				for (i = 0; i < N; i++){
					for(j = N4*2;j < N4*3; j++){
						for(k = 0;k < N; k++){
							*(shmaddr+N*N*2+i+j*N) += (*(shmaddr+k+j*N))*(*(shmaddr+k*N+i));
						}
					}
				}
				wait(NULL);
				//printf("P2 Complete\n");
			}
			return 0;
		}
		else {
			for (i = 0; i < N; i++){
				for(j = N4*3;j < N; j++){
					for(k = 0;k < N; k++){
						*(shmaddr+N*N*2+i+j*N) += (*(shmaddr+k+j*N))*(*(shmaddr+k*N+i));
					}
				}
			}
			//printf("P4 Complete\n");
			wait(NULL);
			//printf("P3 Complete\n");
		}
		gettimeofday(&end,0);
		sleep(1) ;
		sum4 = 0;
		for (i = 0; i < N; i++){
			for(j = 0;j < N; j++){
				sum4+=*(shmaddr+N*N*2+i+j*N);
			}
		}
		printf("4-process, checksum = %u\n",sum4);
		sec = end.tv_sec-start.tv_sec;
		usec = end.tv_usec-start.tv_usec;
		printf("elapsed %f ms\n",sec*1000+(usec/1000.0));
		wait(NULL);
		shmdt( shmaddr );
        shmctl(shmid, IPC_RMID, NULL) ;
    }else{
		perror("fork error") ;
        shmctl(shmid, IPC_RMID, NULL) ;
    }

    return 0 ;
}
