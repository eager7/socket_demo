#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h> //usleep

#define checkError(ret) do{if(-1==ret){printf("[%d]err:%s\n", __LINE__, strerror(errno));return -1;}}while(0)

int iSocketFd = 0;
int iNumberClient = 0;
int iSockClient[5] = {0};

int socket_init(char *netaddr, int port){
    int iSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    checkError(iSocketFd);

    int re = 1;
    checkError(setsockopt(iSocketFd, SOL_SOCKET, SO_REUSEADDR, &re, sizeof(re)));

    struct sockaddr_in server_addr;  
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;  
    if(NULL == netaddr){
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);/*receive any address*/       
    } else {
        server_addr.sin_addr.s_addr = inet_addr(netaddr);
    }
    server_addr.sin_port = htons(port);
    checkError(bind(iSocketFd, (struct sockaddr*)&server_addr, sizeof(server_addr)));

    checkError(listen(iSocketFd, 5));
    return iSocketFd;
}

int epoll_add(int iepollset, int isockfd, int state){
    struct epoll_event ev;
    ev.data.fd = isockfd;
    ev.events  = state;
    checkError(epoll_ctl(iepollset, EPOLL_CTL_ADD, isockfd, &ev));
    return 0;
}

int epoll_del(int iepollset, int isockfd, int state){
    struct epoll_event ev;
    ev.data.fd = isockfd;
    ev.events  = state;
    checkError(epoll_ctl(iepollset, EPOLL_CTL_DEL, isockfd, &ev));
    return 0;    
}

int epoll_modify(int iepollset, int isockfd, int state){
    struct epoll_event ev;
    ev.data.fd = isockfd;
    ev.events  = state;
    checkError(epoll_ctl(iepollset, EPOLL_CTL_MOD, isockfd, &ev));
    return 0;    
}

int handle_accept(int iepollset, struct epoll_event ev){
    struct sockaddr_in      client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t client_len =  sizeof(client_addr);

    if (ev.data.fd == iSocketFd){//server event
        int j = 0;
        for(j = 0; j < 5; j++){
            if(iSockClient[j] == 0){
                iSockClient[j] = accept(iSocketFd, (struct sockaddr*)&client_addr, &client_len);
                checkError(iSockClient[j]);
                printf("client connected %s\n", inet_ntoa(client_addr.sin_addr));
                checkError(epoll_add(iepollset, iSockClient[j], EPOLLIN | EPOLLET));

                iNumberClient++;
                if(iNumberClient >= 5){
                    checkError(epoll_del(iepollset, iSocketFd, EPOLLIN));
                }
                break;
            }                       
        }
    }    
    return 0;
}

int handle_client(int iepollset, struct epoll_event ev){
    int j = 0;
    char aRecv[2048] = {0}; 
    for(j = 0; j < 5; j++){
        if((iSockClient[j] != 0) && (iSockClient[j] == ev.data.fd)){
            int irecv = 0;
            checkError((irecv = recv(iSockClient[j], aRecv, sizeof(aRecv), 0)));
            if(0 == irecv){
                printf("client disconnect, close it\n");
                checkError(epoll_del(iepollset, iSockClient[j], EPOLLIN | EPOLLET));
                close(iSockClient[j]);

                iSockClient[j] = 0;
                iNumberClient--;
                if(iNumberClient < 5){
                    checkError(epoll_del(iepollset, iSocketFd, EPOLLIN | EPOLLET));
                }
            } else {
                printf("recv data:%s\n", aRecv);
                checkError(send(iSockClient[j], aRecv, sizeof(aRecv), 0));
            }            
        }
    }
    return 0;
}

int thread_new(void *fun){
    thread_t th;
    if(pthread_create(&th, NULL, fun, NULL)){
        printf("thread create err:%s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int main(int argc, char const *argv[])
{
	int i = 0;

	printf("this is tcp server thread demo\n");



    checkError((iSocketFd = socket_init(NULL, 7878)));

    int iEpollSet = 0;
    checkError((iEpollSet = epoll_create(65535)));
    checkError(epoll_add(iEpollSet, iSocketFd, EPOLLIN));

	struct epoll_event EpollEventList[5];
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
    				}
                    checkError(handle_accept(iEpollSet, EpollEventList[i]));
                    checkError(handle_client(iEpollSet, EpollEventList[i]));
                }
    		}
    		break;
    	}
    	sleep(0);
    }
	return 0;
}
