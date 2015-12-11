#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h> //usleep

#define checkError(ret) do{if(-1==ret){printf("[%d]err:%s\n", __LINE__, strerror(errno));exit(1);}}while(0)

int main(int argc, char const *argv[])
{
	int i = 0;
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

    int iSockClient[5] = {0};
	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(client_addr));
	socklen_t client_len = sizeof(client_addr);

	const char *aSend = "This is tcp server";
	char aRecv[2048] = {0};	

	fd_set fdSelect, fdAll;
	FD_ZERO(&fdAll);
	FD_SET(iSocketFd, &fdAll);
	int iSelectFd = iSocketFd;
	int iNumberClient = 0;
    while(1)
    {
    	printf("wait select changed ...\n");
    	fdSelect = fdAll;
    	int iRet = select(iSelectFd + 1, &fdSelect, NULL, NULL, NULL);
    	switch(iRet){
    		case (0):
    			printf("select timeout\n");
    		break;
    		case (-1):
    			printf("select error:%s\n", strerror(errno));
    		break;
    		default:{
    			printf("get select event \n");
    			if(FD_ISSET(iSocketFd, &fdSelect)){ //accept client connect
    				for (i = 0; i < 5; ++i) {
    					if (0 == iSockClient[i]) {
						    iSockClient[i] = accept(iSocketFd, (struct sockaddr*)&client_addr, &client_len);
							checkError(iSockClient[i]);	
							printf("client ipaddr:%s\n", inet_ntoa(client_addr.sin_addr));
							FD_SET(iSockClient[i], &fdAll);
							if (iSockClient[i] > iSelectFd){
								iSelectFd = iSockClient[i];
							}
							iNumberClient ++;
							if(iNumberClient >= 5){
								FD_CLR(iSocketFd, &fdAll);
							}
							break;
    					}
    				}
    			} else {
					for (i = 0; i < 5; ++i){
						if ((iSockClient[i] != 0) && FD_ISSET(iSockClient[i], &fdSelect)){
							int irecv = recv(iSockClient[i], aRecv, sizeof(aRecv), 0);
							checkError(irecv);
							if (0 == irecv){
								printf("this client[%d] disconnect, close it\n", i);
								FD_CLR(iSockClient[i], &fdAll);
								close(iSockClient[i]);
								iSockClient[i] = 0;
								iNumberClient --;
								if(iNumberClient < 5){
									FD_SET(iSocketFd, &fdAll);
								}
								break;
							}
							printf("recv client[%d] data:%s\n", i, aRecv);
							checkError(send(iSockClient[i], aSend, strlen(aSend), 0));
							break;
						}		
					}
					
				}
    		}
    		break;
    	}
    	sleep(0);
    }

	return 0;
}
