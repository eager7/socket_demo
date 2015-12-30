#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

int main()
{
    printf("getaddrinfo program\n");

	struct addrinfo hint, *result;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	
	if(-1 == getaddrinfo(NULL, "8080", &hint, &result)){
		printf("getaddrinfo err:%s", strerror(errno));
		return -1;
	}
	
	char addr_test[256] = {0};
	if(NULL == inet_ntop(AF_INET, &result->ai_addr, addr_test, result->ai_addrlen)){
		printf("inet_ntop err:%s", strerror(errno));
		return -1;		
	}
	printf("addr_test = %s\n", addr_test);
    
    //accept
    
    return 0;
}

