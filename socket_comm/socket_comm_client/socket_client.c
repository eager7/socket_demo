/****************************************************************************
 *
 * MODULE:             socket_client.c
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

#include "socket_client.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define DBG_SOCK 1
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
static void *SocketClientHandlerThreadFun(void *arg);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
static tsSocketClient sSocketClient;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
teSocketStatus SocketClientInit(int iPort, char *psNetAddress)
{
    DBG_vPrintf(DBG_SOCK, "SocketClientInit\n");

    signal(SIGPIPE, SIG_IGN);//ingnore signal interference
    memset(&sSocketClient, 0, sizeof(sSocketClient));
    
    sSocketClient.addr_client.sin_family = AF_INET;
    if(NULL == psNetAddress)
    {
        sSocketClient.addr_client.sin_addr.s_addr = htons(INADDR_ANY);
    }
    else
    {
        sSocketClient.addr_client.sin_addr.s_addr = inet_addr(psNetAddress);
    }
    sSocketClient.addr_client.sin_port = htons(iPort);

    BLUE_vPrintf(DBG_SOCK, "Create Socket Handler Thread\n");
    if(0 != pthread_create(&sSocketClient.pthSocketClient, NULL, SocketClientHandlerThreadFun, NULL))
    {
        ERR_vPrintf(T_TRUE,"pthread_create failed, %s\n", strerror(errno));  
        return E_SOCK_ERROR_PTHREAD_CREATE;
    }
    
    return E_SOCK_OK;
}

teSocketStatus SocketClientFinished()
{
    DBG_vPrintf(DBG_SOCK, "SocketClientInit\n");
    close(sSocketClient.iSocketFd);

    pthread_kill(sSocketClient.pthSocketClient, THREAD_SIGNAL);
    void *psThread_Result;
    if(0 != pthread_join(sSocketClient.pthSocketClient, &psThread_Result))
    {
        ERR_vPrintf(T_TRUE,"phread_join socket failed, %s\n", strerror(errno));  
        return E_SOCK_ERROR_JOIN;
    }

    BLUE_vPrintf(DBG_SOCK, " SocketServerFinished %s\n", (char*)psThread_Result);
    return E_SOCK_OK;
}

teSocketStatus SocketClientSendMsg(const char *msg)
{
	if(NULL != msg)
		BLUE_vPrintf(DBG_SOCK, "SocketClientSendMsg:%s\n", msg);
	else
		return E_SOCK_ERROR;
	
	if (sSocketClient.iSocketFd > 0)
	{
		if(-1 == send(sSocketClient.iSocketFd, msg,strlen(msg),0))
		{
			RED_vPrintf(T_TRUE, "send data to client error:%s\n", strerror(errno));
			return E_SOCK_ERROR;
		}     
	}

	return E_SOCK_OK;
}
/****************************************************************************/
/***        Local    Functions                                            ***/
/****************************************************************************/
static void *SocketClientHandlerThreadFun(void *arg)
{
    BLUE_vPrintf(DBG_SOCK, "SocketClientHandlerThreadFun\n");
    
RECONNECT:
    sSocketClient.iSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sSocketClient.iSocketFd)
    {
        ERR_vPrintf(T_TRUE, "socket create error %s\n", strerror(errno));
        return NULL;
    }
    int on = 1;  /*SO_REUSEADDR port can used twice by program */
    if((setsockopt(sSocketClient.iSocketFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))<0) 
    {  
        ERR_vPrintf(T_TRUE,"setsockopt failed, %s\n", strerror(errno));  
        return NULL;
    }  
    
    while(1)
    {
        DBG_vPrintf(DBG_SOCK, "Try To Connect With Socket Server\n");
        if(0 == connect(sSocketClient.iSocketFd, (struct sockaddr*)&sSocketClient.addr_client, sizeof(sSocketClient.addr_client)))
        {
            BLUE_vPrintf(DBG_SOCK, "Client been Connected With Server Successfuliy\n");
            sSocketClient.bIsConnected = T_TRUE;
            break;
        }
        else
        {
            ERR_vPrintf(T_TRUE, "socket connect error: %s\n", strerror(errno));
            sleep(3);
        }
    }

    fd_set fdAll, fdSelect;
    FD_ZERO(&fdAll);
    FD_SET(sSocketClient.iSocketFd, &fdAll);
    while(1)
    {
        //struct timeval time_out = {3,0};
        fdSelect = fdAll;
        YELLOW_vPrintf(DBG_SOCK, "Waiting For Select Change....\n");
		memset(&sSocketClient.csSocketData, 0, sizeof(sSocketClient.csSocketData));
        int iSelectResult = select(sSocketClient.iSocketFd + 1, &fdSelect, NULL, NULL, NULL);
        switch(iSelectResult)
        {
            case (E_SELECT_TIMEOUT):
            {
                ERR_vPrintf(T_TRUE,"socket select timeout\n");  
            }
            break;
            case (E_SELECT_ERROR):
            {
                ERR_vPrintf(T_TRUE,"socket select failed, %s\n", strerror(errno));  
            }
            break;
            default:
            {
                if(FD_ISSET(sSocketClient.iSocketFd, &fdSelect))    /*recv or disconnect event*/
                {
                    sSocketClient.iSocketLength = recv(sSocketClient.iSocketFd, sSocketClient.csSocketData, sizeof(sSocketClient.csSocketData), 0);
                    if(-1 == sSocketClient.iSocketLength)   /*socket disconnect*/
                    {
                        ERR_vPrintf(T_TRUE,"Recv Error\n"); 
                        sleep(1);
                    }
                    else if(0 == sSocketClient.iSocketLength)
                    {
                        ERR_vPrintf(T_TRUE,"This Socket Is Disconnected, Will Retry Connect With Server\n"); 
                        close(sSocketClient.iSocketFd);
                        sleep(2);
                        goto RECONNECT;
                    }
                    else
                    {
                        PURPLE_vPrintf(DBG_SOCK, "Recv Data --- %s\n", sSocketClient.csSocketData);
                    }
                }
            }
            break;
        }
    }
    
    pthread_exit("Exit SocketClientHandlerThreadFun");
}
