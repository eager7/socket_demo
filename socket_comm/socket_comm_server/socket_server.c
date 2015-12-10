/****************************************************************************
 *
 * MODULE:             socket_server.c
 *
 * COMPONENT:          Utils interface
 *
 * REVISION:           $Revision:  0$
 *
 * DATED:              $Date: 2015-10-21 15:13:17 +0100 (Thu, 21 Oct 2015 $
 *
 * AUTHOR:             PCT
 *
 ****************************************************************************
 *
 * Copyright panchangtao@gmail.com B.V. 2015. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include "socket_server.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define DBG_SOCK 1 
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
static void *SocketServerHandleThread(void *arg);
/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
static tsSocketServer sSocketServer;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
teSocketStatus SocketServerInit(int iPort, char *psNetAddress)
{
    BLUE_vPrintf(DBG_SOCK, "SocketServerInit\n");

    memset(&sSocketServer, 0, sizeof(sSocketServer));
    signal(SIGPIPE, SIG_IGN);//ingnore signal interference
    
	struct sockaddr_in server_addr;  
	server_addr.sin_family = AF_INET;  
    if(NULL != psNetAddress)
    {
        server_addr.sin_addr.s_addr = inet_addr(psNetAddress);  /*just receive one address*/
    }
    else
    {
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);        /*receive any address*/
    }
	server_addr.sin_port = htons(iPort);

    if(-1 == (sSocketServer.iSocketFd = socket(AF_INET, SOCK_STREAM, 0)))
    {
        ERR_vPrintf(T_TRUE, "socket create error %s\n", strerror(errno));
        return E_SOCK_ERROR_CREATESOCK;
    }

    int on = 1;  /*SO_REUSEADDR port can used twice by program */
    if((setsockopt(sSocketServer.iSocketFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))<0) 
    {  
        ERR_vPrintf(T_TRUE,"setsockopt failed, %s\n", strerror(errno));  
        return E_SOCK_ERROR_SETSOCK;
    }  

    if(-1 == bind(sSocketServer.iSocketFd, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        ERR_vPrintf(T_TRUE,"bind socket failed, %s\n", strerror(errno));  
        return E_SOCK_ERROR_BIND;
    }

    if(-1 == listen(sSocketServer.iSocketFd, SOCKET_LISTEN_NUM))
    {
        ERR_vPrintf(T_TRUE,"listen socket failed, %s\n", strerror(errno));  
        return E_SOCK_ERROR_LISTEN;
    }

    //start accept thread
    BLUE_vPrintf(DBG_SOCK, "pthread_create\n");
    if(0 != pthread_create(&sSocketServer.pthSocketServer, NULL, SocketServerHandleThread, NULL))
    {
        ERR_vPrintf(T_TRUE,"pthread_create failed, %s\n", strerror(errno));  
        return E_SOCK_ERROR_PTHREAD_CREATE;
    }
    return E_SOCK_OK;
}

teSocketStatus SocketServerFinished()
{
    BLUE_vPrintf(DBG_SOCK, "Waiting SocketServerFinished...\n");
    
    sSocketServer.eState = E_THREAD_STOPPED;
    pthread_kill(sSocketServer.pthSocketServer, THREAD_SIGNAL);
    void *psThread_Result;
    if(0 != pthread_join(sSocketServer.pthSocketServer, &psThread_Result))
    {
        ERR_vPrintf(T_TRUE,"phread_join socket failed, %s\n", strerror(errno));  
        return E_SOCK_ERROR_JOIN;
    }

    BLUE_vPrintf(DBG_SOCK, " SocketServerFinished %s\n", (char*)psThread_Result);

    return E_SOCK_OK;
}
/****************************************************************************/
/***        Local    Functions                                            ***/
/****************************************************************************/
static void ThreadSignalHandler(int sig)
{
    BLUE_vPrintf(DBG_SOCK, "ThreadSignalHandler Used To Interrupt System Call\n");
}

static void *SocketServerHandleThread(void *arg)
{
    BLUE_vPrintf(DBG_SOCK, "SocketServerHandleThread\n");
    sSocketServer.eState = E_THREAD_RUNNING;
    signal(THREAD_SIGNAL, ThreadSignalHandler);
    
    int i = 0;
    tsSocketClient sSocketClient[SOCKET_LISTEN_NUM];
    memset(&sSocketClient, 0, sizeof(sSocketClient));
    for(i = 0; i < SOCKET_LISTEN_NUM; i++)
    {
        sSocketClient[i].iSocketFd = -1;
    }

    fd_set fdAll, fdSelect;
    FD_ZERO(&fdAll);                            /*Init FD*/
    FD_SET(sSocketServer.iSocketFd, &fdAll);    /*Add our sock into FD*/

    int iListenSock = sSocketServer.iSocketFd;
    while(sSocketServer.eState)
    {
        fdSelect = fdAll;                       /*Copy Fd, because this Fd will be zero after*/
        DBG_vPrintf(DBG_SOCK, "Waiting for Fd Changed\n");
		    
		/*struct timeval tv;
        tv.tv_sec= 0;         
        tv.tv_usec= 0;
        int iSelectResult = select(iListenSock + 1, &fdSelect, NULL, NULL, &tv);
		BLUE_vPrintf(T_TRUE, "select return %d\n", iSelectResult);*/
        int iSelectResult = select(iListenSock + 1, &fdSelect, NULL, NULL, NULL);
        switch(iSelectResult)
        {
            case E_SELECT_TIMEOUT:
            {
                ERR_vPrintf(T_TRUE,"socket select timeout\n");  
            }
            break;

            case E_SELECT_ERROR:
            {
                ERR_vPrintf(T_TRUE,"socket select failed, %s\n", strerror(errno));  
            }
            break;
            
            default:
            {
                if(FD_ISSET(sSocketServer.iSocketFd, &fdSelect))//accept event
                {
                    DBG_vPrintf(DBG_SOCK, "sSocketServer.iSocketFd Changed\n");
                    if(sSocketServer.u8NumConnClient >= SOCKET_LISTEN_NUM)
                    {
                        ERR_vPrintf(T_TRUE, "The Number of client is 10, do not allowed connect\n");
                        continue;
                    }
                    
                    for(i = 0; i < SOCKET_LISTEN_NUM; i++)
                    {
                        if(-1 == sSocketClient[i].iSocketFd)//not used
                        {
                            sSocketClient[i].iSocketLen = sizeof(sSocketClient[i].addrclient);
                            sSocketClient[i].iSocketFd = accept(sSocketServer.iSocketFd, 
                                (struct sockaddr*)&sSocketClient[i].addrclient, (socklen_t *)&sSocketClient[i].iSocketLen);
                            if(-1 == sSocketClient[i].iSocketFd)
                            {
                                ERR_vPrintf(T_TRUE, "socket accept error %s\n", strerror(errno));
                            }
                            else
                            {
                                YELLOW_vPrintf(DBG_SOCK, "A client already connected, [%d]-[%d]\n", i, sSocketClient[i].iSocketFd);
                                FD_SET(sSocketClient[i].iSocketFd, &fdAll); /*Add This Client Into Select FD*/
                                if(sSocketClient[i].iSocketFd > iListenSock)
                                {
                                    iListenSock = sSocketClient[i].iSocketFd;   /*Make Sure The Listen Fd is Biggest*/
                                }
                                sSocketServer.u8NumConnClient ++;
                                break;
                            }
                               
                        }
                    }
                }/*END if(FD_ISSET(*/

                for(i = 0; i < SOCKET_LISTEN_NUM; i++)  /*recive event or disconnect event*/
                {
                    if(-1 == sSocketClient[i].iSocketFd)
                    {
                        continue;
                    }
                    else if(FD_ISSET(sSocketClient[i].iSocketFd, &fdSelect))
                    {
                        BLUE_vPrintf(DBG_SOCK, "Socket Client[%d] Begin Recv Data...\n", i);
                        sSocketClient[i].iSocketLen = recv(sSocketClient[i].iSocketFd, 
                            sSocketClient[i].csClientData, sizeof(sSocketClient[i].csClientData), 0);
                        if(-1 == sSocketClient[i].iSocketLen)
                        {
                            ERR_vPrintf(T_TRUE, "socket recv error %s\n", strerror(errno));
                        }
                        else if(0 == sSocketClient[i].iSocketLen)   /*disconnect*/
                        {
                            ERR_vPrintf(T_TRUE, "The Client[%d] is disconnect, Closet It\n", i);
                            FD_CLR(sSocketClient[i].iSocketFd, &fdAll);    /*delete it from select fd*/
                            close(sSocketClient[i].iSocketFd);
                            sSocketClient[i].iSocketFd = -1;
                            sSocketServer.u8NumConnClient --;
                        }
                        else    /*recv event*/
                        {
                            YELLOW_vPrintf(DBG_SOCK, "Recv Data is [%d]--- %s\n", i, sSocketClient[i].csClientData);
                        }
                    }
                }
            }
            break;
        }
        sleep(0);
    }
    
    close(sSocketServer.iSocketFd);
    DBG_vPrintf(DBG_SOCK, "Exit SocketServerHandleThread\n");
    pthread_exit("Get Killed Signal");
}

