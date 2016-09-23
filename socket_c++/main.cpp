#include <iostream>
#include "mSocket/mSocket.hpp"
#include "mThread/mThread.hpp"
#include <signal.h>

using namespace std;
using namespace mSocket;
using namespace mThread;

#define UI_RED(x)       "\e[31;1m" x "\e[0m"
#define UI_GREEN(x)     "\e[32;1m" x "\e[0m"
#define UI_YELLOW(x)    "\e[33;1m" x "\e[0m"
#define UI_BLUE(x)      "\e[34;1m" x "\e[0m"
#define UI_PURPLE(x)    "\e[35;1m" x "\e[0m"

#define DBG_vPrintf(a,b,ARGS...)  do {  if (a) {printf(UI_BLUE	(b) , ## ARGS);} } while(0)
#define INF_vPrintf(a,b,ARGS...)  do {  if (a) {printf(UI_YELLOW(b) , ## ARGS);} } while(0)
#define NOT_vPrintf(a,b,ARGS...)  do {  if (a) {printf(UI_GREEN	(b) , ## ARGS);} } while(0)
#define WAR_vPrintf(a,b,ARGS...)  do {  if (a) {printf(UI_PURPLE(b) , ## ARGS);} } while(0)
#define ERR_vPrintf(a,b,ARGS...)  do {  if (a) {printf(UI_RED	("[%s:%d]" b) , __FILE__, __LINE__, ## ARGS);} } while(0)

static int main_loop = 1;

static void my_handler(int s){
    printf("Caught signal %d\n",s);
    main_loop = 0;
}

static void *server(void * info){
    Thread * pt = (Thread*)info;
    char receive[10] = {0};
    sleep(1);
    try {
        TCP server;
        TCP client;
        server.listen_on_port(10000);
        client = server.accept_client();

        while(pt->state() == E_THREAD_RUNNING) {
            cout<< pt->name() << ":loop[]..."  << endl;
            cout << "receiving ..." << endl;
            client.receive<char>(receive, 10);
            DBG_vPrintf(T_TRUE, "recv:%s\n", receive);
            sleep(1);
        }
    }catch (SocketException &e){
        cout << e << endl;
    }

    pt->exit();
    return NULL;
}

static void *client(void * info){
    Thread * pt = (Thread*)info;

    try {
        TCP client;
        while(T_TRUE) {
            try {
                client.connect_to(Address("127.0.0.1", 10000));
                break;
            }catch (SocketException &c){
                ERR_vPrintf(T_TRUE, "%s\n", c.what());
                cout << c << endl;
                cout << "try again" << endl;
                sleep(1);
            }
        }
        while(pt->state() == E_THREAD_RUNNING) {
            cout<< pt->name() << ":loop[]..."  << endl;
            cout << "sending ..." << endl;
            client.send<char>("hello", sizeof("hello"));
            sleep(1);
        }
    }catch (SocketException &e){
        cout << e << endl;
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
            cout << "main loop:" << endl;
            cout << thread_server << endl;
            cout << thread_client << endl;

            sleep(1);
        }
        cout << "exit main loop" << endl;
        thread_client.stop();
        thread_server.stop();
    }
    catch (ThreadException &e) {
        cout << e << endl;
    }



    return 0;
}