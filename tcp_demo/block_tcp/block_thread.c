#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h> //usleep
#include <signal.h>
#include <pthread.h>

#define checkError(ret) do{if(-1==ret){printf("[%d]err:%s\n", __LINE__, strerror(errno));exit(1);}}while(0)
static void *SendMsgThread(void *arg);
int iSocketFd = 0;

int main(int argc, char const *argv[])
{
	printf("this is a tcp client demo\n");

	signal(SIGPIPE, SIG_IGN);//ingnore signal interference
	iSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	checkError(iSocketFd);

	int re = 1;
	checkError(setsockopt(iSocketFd, SOL_SOCKET, SO_REUSEADDR, &re, sizeof(re)));

	struct sockaddr_in server_addr;  
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;  
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);        /*receive any address*/
    //server_addr.sin_addr.s_addr = inet_addr("10.128.118.43");
    server_addr.sin_port = htons(7878);
	while(1){
		printf("conncet to server...\n");
    	if(-1 != connect(iSocketFd, (struct sockaddr *)&server_addr, sizeof(server_addr))){
    		break;
    	}	
    	sleep(1);
	}

	pthread_t send_thread;
	if (pthread_create(&send_thread, NULL, SendMsgThread, NULL))
	{
		printf("err:%s\n", strerror(errno));
		return -1;
	}

	const char *aSend = "012345678987654321001234567898765432100123456789876543210\n";
    while(1)
    {
    	checkError(send(iSocketFd, aSend, strlen(aSend), 0));
		sleep(0);
    }

	return 0;
}


static void *SendMsgThread(void *arg)
{
	printf("SendMsgThread\n");
	char *sendstr = "012345678987654321001234567898765432100123456789876543210\n";
	while(1){
		checkError(send(iSocketFd, sendstr, strlen(sendstr), 0));
		sleep(0);
	}
	pthread_exit(0);
}