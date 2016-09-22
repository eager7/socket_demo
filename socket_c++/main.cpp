#include <iostream>
#include "mSocket/mSocket.hpp"
#include "mThread/mThread.hpp"
#include <signal.h>

using namespace std;
using namespace mSocket;
using namespace mThread;

static int main_loop = 1;
static void my_handler(int s){
    printf("Caught signal %d\n",s);
    main_loop = 0;
}

static void *server(void * info){
    Thread * pt = (Thread*)info;
    char receive[10] = {0};

    TCP server;
    server.listen_on_port(10000);
    TCP client = server.accept_client();
    while(pt->state() == E_THREAD_RUNNING) {

        cout<< pt->name() << ":loop[]..."  << endl;
        cout << "receiving ..." << endl;
        client.receive<char>(receive, 10);
        DBG_vPrintf(T_TRUE, "recv:%s\n", receive);
        sleep(1);
    }

    pt->exit();
    return NULL;
}

static void *client(void * info){
    Thread * pt = (Thread*)info;

    TCP client;
    client.connect_to(Address("127.0.0.1", 10000));
    while(pt->state() == E_THREAD_RUNNING) {
        cout<< pt->name() << ":loop[]..."  << endl;
        cout << "sending ..." << endl;
        client.send<char>("hello", sizeof("hello"));
        sleep(1);
    }

    pt->exit();
    return NULL;
}

int main()
{
    cout << "Hello, World!" << endl;
    signal (SIGINT,my_handler);


    try {
        Thread thread_server(server, "tcp_server");
        thread_server.start();

        Thread thread_client(client, "tcp_client");
        thread_client.start();

        while (main_loop){
            cout << "main loop" << endl;
            sleep(1);
        }
        thread_client.stop();
        thread_server.stop();
    }
    catch (ThreadException &e) {
        cout << e << endl;
    }



    return 0;
}