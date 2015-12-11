#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
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

	int iEpollSet = epoll_create(65535);
	checkError(iEpollSet);
	struct epoll_event EpollEvent, EpollEventList[5];
	EpollEvent.data.fd = iSocketFd;
	EpollEvent.events = EPOLLIN;	//use to accept
	checkError(epoll_ctl(iEpollSet, EPOLL_CTL_ADD, iSocketFd, &EpollEvent));

	int iNumberClient = 0;
    while(1)
    {
    	printf("wait epoll changed ...\n");
    	int iRet = epoll_wait(iEpollSet, EpollEventList, 5, -1);
    	switch(iRet){
    		case (0):
    			printf("select timeout\n");
    		break;
    		case (-1):
    			printf("select error:%s\n", strerror(errno));
    		break;
    		default:{
    			printf("get epoll events[%d]\n", iRet);
    			for(i = 0; i < iRet; i++){
    				if((EpollEventList[i].events & EPOLLERR) || (EpollEventList[i].events & EPOLLHUP)){
    					printf("fd occured err:%s\n", strerror(errno));
    					continue;
    				} else if (EpollEventList[i].data.fd == iSocketFd){//server event
    					int j = 0;
    					for(j = 0; j < 5; j++){
     						if(iSockClient[j] == 0){
     							iSockClient[j] = accept(iSocketFd, (struct sockaddr*)&client_addr, &client_len);
     							checkError(iSockClient[j]);
     							printf("client connected %s\n", inet_ntoa(client_addr.sin_addr));
     							EpollEvent.data.fd = iSockClient[j];
     							EpollEvent.events = EPOLLIN | EPOLLET;/*read ,Ede-Triggered, close*/
     							checkError(epoll_ctl(iEpollSet, EPOLL_CTL_ADD, iSockClient[j], &EpollEvent));
     							iNumberClient++;
     							if(iNumberClient >= 5){
     								checkError(epoll_ctl(iEpollSet, EPOLL_CTL_DEL, iSocketFd, &EpollEvent));
     							}
     							break;
     						} 						
    					}
    				} else {
    					int j = 0;
    					for(j = 0; j < 5; j++){
    						if((iSockClient[j] != 0) && (EpollEventList[i].data.fd == iSockClient[j])){
    							int irecv = recv(iSockClient[j], aRecv, sizeof(aRecv), 0);
    							checkError(irecv);
    							if(0 == irecv){
    								printf("client disconnect, close it\n");
    								checkError(epoll_ctl(iEpollSet, EPOLL_CTL_DEL, iSockClient[j], &EpollEvent));
    								close(iSockClient[j]);
    								iSockClient[j] = 0;
    								iNumberClient--;
    								if(iNumberClient < 5){
    									EpollEvent.data.fd = iSocketFd;
    									EpollEvent.events = EPOLLIN;
    									checkError(epoll_ctl(iEpollSet, EPOLL_CTL_ADD, iSocketFd, &EpollEvent));
    								}
    							} else {
    								printf("recv data:%s\n", aRecv);
    								checkError(send(iSockClient[j], aSend, strlen(aSend), 0));
    							}
    						}
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
