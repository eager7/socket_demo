#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h> //usleep
#include <arpa/inet.h>

int iSocketFd = 0;
int main(int argc, char *argv[])
{
	printf("udp client test\n");

	iSocketFd = socket(AF_INET, SOCK_DGRAM, 0);//create a ucp socket file
	if(-1 == iSocketFd){
		printf("create socket fd err:%s\n", strerror(errno));
		return -1;
	}

    struct timeval timeout={2,5};//timeout
    if(-1 == setsockopt(iSocketFd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout)) ||
      (-1 == setsockopt(iSocketFd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout)))){
    	printf("setsockopt err:%s\n", strerror(errno));
    	return -1;
    }

	//create server socket addr struct
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_port 	= 7878;
	server_addr.sin_family 	= AF_INET; 
	//server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//accept all interface of host
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //accept one interface of host
	socklen_t server_len = sizeof(server_addr);

	char aRecv[2048] = {0};
	while(1){
		printf("wait server data...\n");

		memset(aRecv, 0, sizeof(aRecv));
		if(-1 == recvfrom(iSocketFd, aRecv, sizeof(aRecv), 0, (struct sockaddr*)&server_addr, &server_len)){
			printf("recvfrom err[%d]:%s\n", errno, strerror(errno));
		}
		printf("changed timeout\n");
		static t = 1;
		timeout.tv_sec = t++;
		timeout.tv_usec = 0;
		if(-1 == setsockopt(iSocketFd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout)) ||
		  (-1 == setsockopt(iSocketFd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout)))){
			printf("setsockopt err:%s\n", strerror(errno));
			return -1;
		}

		//printf("server ipaddr:%s, data:%s\n", inet_ntoa(server_addr.sin_addr), aRecv);
		sleep(1);
	}
	return 0;
}
