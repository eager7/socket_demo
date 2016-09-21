#include <iostream>
#include "mSocket.hpp"
#include <signal.h>

using namespace std;
using namespace mSocket;

static void my_handler(int s){
    printf("Caught signal %d\n",s);
    exit(1);
}

int main()
{
    cout << "Hello, World!" << endl;
    signal (SIGINT,my_handler);

    char receive[10] = {0};
    try {
        TCP server;

        server.listen_on_port(10000);

        Address addr = server.address();
        cout << addr << endl;
        //cout << server.address() <<endl;

        TCP client = server.accept_client();

        cout << "receiving ..." << endl;
        client.receive<char>(receive, 10);
    }
    catch (SocketException &e) {
        cout << e << endl;
    }

    return 0;
}