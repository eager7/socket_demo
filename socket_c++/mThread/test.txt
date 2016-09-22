#include <iostream>
#include "mThread/mThread.hpp"

//using namespace std;
using namespace mThread;
ThreadQueue<int> Queue(5);
ThreadQueue<char> Queue2(5);

static void *loop1(void * info){
    Thread * pt = (Thread*)info;

    int i = 0;char a = 'a';
    while(pt->state() == E_THREAD_RUNNING) {
        i++;
        Queue.Enqueue(&i);
        Queue2.Enqueue(&a);
        cout<< pt->name() << ":loop["<< i <<"]..."  << endl;
        sleep(1);
    }
    pt->exit();
    return NULL;
}

static void *loop2(void * info){
    Thread * pt = (Thread*)info;

    int *i;char *a;
    while(pt->state() == E_THREAD_RUNNING) {
        Queue.Dequeue(&i);
        Queue2.Dequeue(&a);
        cout<< pt->name() << ":loop["<< *i <<"]["<< *a <<"]..."  << endl;
        sleep(2);
    }
    pt->exit();
    return NULL;
}

int main() {
    cout << "Hello, World!" << endl;
    try {
        Thread test1(loop1, "test1");
        test1.start();

        Thread test2(loop2, "test2");
        test2.start();

        Thread test3(loop2, "test3", NULL, E_THREAD_DETACHED);
        test3.start();
        sleep(10);

        test1.stop();
        test2.stop();
        test3.stop();
    }catch (ThreadException &e){
        cout << "ex:" << e << endl;
    }

    for (int i = 0; i < 5; ++i) {
        cout << "main loop" << endl;
        sleep(1);
    }

    cout << "exit main" << endl;
    return 0;
}