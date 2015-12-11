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

	char aRecv[2048] = {0};	
	const char *aSend = "This is tcp client";
    while(1)
    {
    	checkError(send(iSocketFd, aSend, strlen(aSend), 0));
   		
   		printf("wait server data...\n");
    	int irecv = recv(iSocketFd, aRecv, sizeof(aRecv), 0);
		if(-1 == irecv){
			printf("recv err[%d]:%s\n", errno, strerror(errno));
			if(errno == EAGAIN){
				usleep(100);
				continue;
			} else {
				exit(1);
			}
		} else if (0 == irecv){
			printf("disconnect with server\n");
			close(iSocketFd);
			goto reconnect;
		}
		printf("server ip:%s, data:%s\n", inet_ntoa(server_addr.sin_addr), aRecv);
		sleep(1);
    }

	return 0;
}