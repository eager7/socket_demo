#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h> //usleep

int udp_server_init(char *netAddr, int port);

int iSocketFd = 0;

int main(int argc, char *argv[])
{
	printf("udp server simple demo\n");

	if(0 != udp_server_init(NULL, 7878)){
		printf("udp_server_init error\n");
		exit(1);
	}

	//create a client addr
	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(client_addr));
	socklen_t client_len = sizeof(client_addr);

	char aRecv[2048] = {0};
	while(1){
		printf("wait client data...\n");
		memset(aRecv, 0, sizeof(aRecv));
		int irecv = recvfrom(iSocketFd, aRecv, sizeof(aRecv), 0, (struct sockaddr*)&client_addr, &client_len);
		if(-1 == irecv){
			printf("recvfrom err:%s\n", strerror(errno));
			if(errno == EAGAIN){
				usleep(100);
				continue;
			} else {
				exit(1);
			}
		}

		printf("client ipaddr:%s, data:%s\n", inet_ntoa(client_addr.sin_addr), aRecv);
		const char *aSend = "This is udp server";
		int isend = sendto(iSocketFd, aSend, strlen(aSend), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
		if(-1 == isend){
			printf("sendto client err:%s\n", strerror(errno));
		}
		sleep(1);
	}
	return 0;
}

int udp_server_init(char *netAddr, int port)
{
	iSocketFd = socket(AF_INET, SOCK_DGRAM, 0);//create a ucp socket file
	if(-1 == iSocketFd){
		printf("create socket fd err:%s\n", strerror(errno));
		return -1;
	}

	int on = 1;
    struct timeval timeout={2,5};//timeout
    if((-1 == setsockopt(iSocketFd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))) || //allow up serval server program
       (-1 == setsockopt(iSocketFd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout))) ||
       (-1 == setsockopt(iSocketFd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout)))){
    	printf("setsockopt err:%s\n", strerror(errno));
    	return -1;
    }

	//create socket addr struct
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family 	= AF_INET; 
	server_addr.sin_port 	= port;
	if(NULL == netAddr){
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//accept all interface of host
	} else {
		server_addr.sin_addr.s_addr = inet_addr(netAddr); //accept one interface of host
	}

	if(-1 == bind(iSocketFd, (struct sockaddr*)&server_addr, sizeof(server_addr))){
		printf("bind error:%s\n", strerror(errno));
		return -1;
	}

	return 0;
}