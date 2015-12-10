#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h> //usleep

#define checkError(ret) do{if(-1==ret){printf("[%d]err:%s\n", __LINE__, strerror(errno));exit(1);}}while(0)

int main(int argc, char const *argv[])
{
	printf("this is tcp demo\n");

	int iSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	checkError(iSocketFd);

	int re = 1;
	checkError(setsockopt(iSocketFd, SOL_SOCKET, SO_REUSEADDR, &re, sizeof(re)));

	struct sockaddr_in server_addr;  
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;  
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);        /*receive any address*/
    server_addr.sin_port = htons(7878);
    checkError(bind(iSocketFd, (struct sockaddr*)&server_addr, sizeof(server_addr)));

    checkError(listen(iSocketFd, 5));

    int iSockClient;
	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(client_addr));
	socklen_t client_len = sizeof(client_addr);

reconnect:
	iSockClient = accept(iSocketFd, (struct sockaddr*)&client_addr, &client_len);
	checkError(iSockClient);	
	printf("client ipaddr:%s\n", inet_ntoa(client_addr.sin_addr));

	const char *aSend = "This is tcp server";
	char aRecv[2048] = {0};	
    while(1)
    {
    	printf("wait client data...\n");
    	int irecv = recv(iSockClient, aRecv, sizeof(aRecv), 0);
		if(-1 == irecv){
			printf("recv err:%s\n", strerror(errno));
			if(errno == EAGAIN){
				usleep(100);
				continue;
			} else {
				exit(1);
			}
		} else if (0 == irecv){
			printf("disconnect with client\n");
			close(iSockClient);
			goto reconnect;
		}
		printf("recv client ip:%s, data:%s\n", inet_ntoa(client_addr.sin_addr), aRecv);

		checkError(send(iSockClient, aSend, strlen(aSend), 0));
		sleep(1);
    }

	return 0;
}
