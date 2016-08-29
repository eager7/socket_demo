#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>

int main()
{
    struct hostent *hptr;
    hptr = gethostbyname("firefly-z");
    if(NULL == hptr){
        printf("error:%s\n", strerror(errno));
        exit(-1);
    }
    printf("offical hostname:%s \n", hptr->h_name);

    char **pptr;
    for(pptr = hptr->h_aliases; *pptr != NULL; pptr++){
        printf("aliases:%s\n", *pptr);
    }

    char str[INET_ADDRSTRLEN]; 
    switch(hptr->h_addrtype){
        case (AF_INET):
            pptr = hptr->h_addr_list;
            for(; *pptr != NULL; pptr++){
                printf("addr:%s\n", inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
            }
        break;
        default:
        printf("unknow addr type\n");
        break;
    }
    return 0;
}

