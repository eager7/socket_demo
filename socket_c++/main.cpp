#include <iostream>
#include "Socket.hpp"
#include <signal.h>

using namespace std;

static void my_handler(int s){
    printf("Caught signal %d\n",s);
    exit(1);
}

int main() {
    cout << "Hello, World!" << endl;
    signal (SIGINT,my_handler);

    try
    {
        mSocket::TCP server;

        server.listen_on_port(10000);
        mSocket::TCP client = server.accept_client();

        cout << "receiving ..." << endl;
        client.receive_file("output.bmp");
    }
    catch (mSocket::SocketException &e)
    {
        cout << e << endl;
    }

    return 0;
}