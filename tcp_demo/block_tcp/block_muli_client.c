#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h> //usleep
#include <signal.h>

#define NUM_CLIENT 10
#define checkError(ret) do{if(-1==ret){printf("[%d]err:%s\n", __LINE__, strerror(errno));exit(1);}}while(0)

#define UI_BLACK(x)     "\e[30;1m"x"\e[0m"
#define UI_RED(x)       "\e[31;1m"x"\e[0m"
#define UI_GREEN(x)     "\e[32;1m"x"\e[0m"
#define UI_YELLOW(x)    "\e[33;1m"x"\e[0m"
#define UI_BLUE(x)      "\e[34;1m"x"\e[0m"
#define UI_PURPLE(x)    "\e[35;1m"x"\e[0m"
#define UI_CYAN(x)      "\e[36;1m"x"\e[0m"
#define UI_WHITE(x)     "\e[37;1m"x"\e[0m"

#define DBG_vPrintf(a,b,ARGS...)    do {  if (a) printf(b, ## ARGS);} while(0)
#define BLACK_vPrintf(a,b,ARGS...)  do {  if (a) {printf(UI_BLACK    ("[PCT_%d]") b, __LINE__, ## ARGS);} } while(0)
#define RED_vPrintf(a,b,ARGS...)    do {  if (a) {printf(UI_RED      ("[PCT_%d]") b, __LINE__, ## ARGS);} } while(0)
#define GREEN_vPrintf(a,b,ARGS...)  do {  if (a) {printf(UI_GREEN    ("[PCT_%d]") b, __LINE__, ## ARGS);} } while(0)
#define YELLOW_vPrintf(a,b,ARGS...) do {  if (a) {printf(UI_YELLOW   ("[PCT_%d]") b, __LINE__, ## ARGS);} } while(0)
#define BLUE_vPrintf(a,b,ARGS...)   do {  if (a) {printf(UI_BLUE     ("[PCT_%d]") b, __LINE__, ## ARGS);} } while(0)
#define PURPLE_vPrintf(a,b,ARGS...) do {  if (a) {printf(UI_PURPLE   ("[PCT_%d]") b, __LINE__, ## ARGS);} } while(0)
#define CYAN_vPrintf(a,b,ARGS...)   do {  if (a) {printf(UI_CYAN     ("[PCT_%d]") b, __LINE__, ## ARGS);} } while(0)
#define WHITE_vPrintf(a,b,ARGS...)  do {  if (a) {printf(UI_WHITE    ("[PCT_%d]") b, __LINE__, ## ARGS);} } while(0)
	
	
	
int iSocketFd[NUM_CLIENT];
	
int main(int argc, char const *argv[])
{
	int i = 0;
	printf("this is a tcp client demo\n");

	signal(SIGPIPE, SIG_IGN);//ingnore signal interference
	
	int re = 1;
	struct sockaddr_in server_addr;  
	char aRecv[2048] = {0};	
	char aSend[1024] = {0};
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;  
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);        /*receive any address*/
	server_addr.sin_port = htons(7878);
	
	for (i = 0; i < NUM_CLIENT; i ++){
		iSocketFd[i] = -1;
		iSocketFd[i] = socket(AF_INET, SOCK_STREAM, 0);
		checkError(iSocketFd[i]);
		checkError(setsockopt(iSocketFd[i], SOL_SOCKET, SO_REUSEADDR, &re, sizeof(re)));
		while(1){
			printf("socket[%d], conncet to server...\n", i);
			if(-1 != connect(iSocketFd[i], (struct sockaddr *)&server_addr, sizeof(server_addr))){
				break;
			}
			sleep(1);
		}
		BLUE_vPrintf(1, "Socket[%d] Connected with Server Success\n", i);
		sleep(1);
		sprintf(aSend, "This is tcp client%d", i);
    	checkError(send(iSocketFd[i], aSend, strlen(aSend), 0));
	}
	sleep(10);
	printf("Now Close Client One by One\n");
	for (i = 0; i < NUM_CLIENT; i ++){
		if (iSocketFd[i] != -1){
			close(iSocketFd[i]);
		}
		sleep(1);
		GREEN_vPrintf(1, "Close Client[%d]\n", i);
	}

#if 0
    while(1)
    {
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
#endif
	return 0;
}