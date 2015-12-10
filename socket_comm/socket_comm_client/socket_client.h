/****************************************************************************
 *
 * MODULE:             socket_client.h
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


#ifndef __H_SOCKET_CLIENT_H_
#define __H_SOCKET_CLIENT_H_

#if defined __cplusplus
extern "C"{
#endif
/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <stdio.h>
#include <pthread.h>
#include "utils.h"
#include <netinet/in.h>
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define SOCKET_LISTEN_NUM 10
#define THREAD_SIGNAL SIGUSR1

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef enum
{
    E_SOCK_OK = 0,
    E_SOCK_ERROR,
    E_SOCK_ERROR_CREATESOCK,
    E_SOCK_ERROR_SETSOCK,
    E_SOCK_ERROR_CONNECT,
    E_SOCK_ERROR_LISTEN, 
    E_SOCK_ERROR_PTHREAD_CREATE, 
    E_SOCK_ERROR_JOIN, 
}teSocketStatus;

typedef enum
{
    E_SELECT_OK = 1,
    E_SELECT_TIMEOUT = 0,
    E_SELECT_ERROR = -1,
}teSelectResult;

typedef struct _tsSocketClient
{
    int iSocketFd;
    struct sockaddr_in addr_client;
    bool_t bIsConnected;
    pthread_t pthSocketClient;
    int iSocketLength;
    char csSocketData[MXBF];
    
}tsSocketClient;
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
teSocketStatus SocketClientInit(int iPort, char *psNetAddress);
teSocketStatus SocketClientFinished();
teSocketStatus SocketClientSendMsg(const char *msg);


#if defined __cplusplus
}
#endif
#endif

