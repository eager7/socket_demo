#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h> //usleep
#include <signal.h>

#define checkError(ret) do{if(-1==ret){printf("[%d]err:%s\n", __LINE__, strerror(errno));exit(1);}}while(0)

int main(int argc, char const *argv[])
{
	printf("this is a tcp client demo\n");

	signal(SIGPIPE, SIG_IGN);//ingnore signal interference
	int iSocketFd = 0;
reconnect: 
	iSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	checkError(iSocketFd);

	int re = 1;
	checkError(setsockopt(iSocketFd, SOL_SOCKET, SO_REUSEADDR, &re, sizeof(re)));

	struct sockaddr_in server_addr;  
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;  
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");        /*receive any address*/
    //server_addr.sin_addr.s_addr = inet_addr("10.128.118.43");
    server_addr.sin_port = htons(7878);
	while(1){
		printf("conncet to server...\n");
    	if(-1 != connect(iSocketFd, (struct sockaddr *)&server_addr, sizeof(server_addr))){
    		break;
    	}	
    	sleep(1);
	}

	char aRecv[2048] = {0};	
	const char aSend[1024*10] = {0};
	int size = 0;
    while(1)
    {
		size ++;
		sprintf(aSend, "%d", size);
    	checkError(send(iSocketFd, aSend, sizeof(aSend), 0));
   		
   		printf("send size:%d\n", size);
		sleep(1);
    }

	return 0;
}