/****************************************************************************
 *
 * MODULE:             CommonThread
 *
 * COMPONENT:          steerMotor define
 *
 * REVISION:           $Revision:  0.1$
 *
 * DATED:              $Date: 2016-09-06 15:13:17
 *
 * AUTHOR:             PCT
 *
 ****************************************************************************
 *
 * Copyright changtao.pan@kuang-chi.com B.V. 2016. All rights reserved
 *
 ***************************************************************************/


#include "mThread.hpp"
using namespace std;

namespace mThread
{
    Thread::Thread(): _create(T_FALSE), thread_info(){ }

    Thread::Thread(Thread &t): _create(T_FALSE), thread_info() {
        this->thread_info = t.thread_info;
        this->func = t.func;
        this->_create = t._create;
    }

    Thread::Thread(thread_loop func, string name, void *pthread_data, detach_t detach)
            : _create(T_FALSE), thread_info(name, detach, pthread_data) {
        this->func = func;
        this->thread_info = ThreadInfo(name, detach, pthread_data);
        this->install_signal();
        cout << "Constructor:[name:" << this->thread_info.name << "][pthread_data:" <<
                this->thread_info.pthread_data << "][detach:" << this->thread_info.detach << "]" << endl;
    }

    void Thread::install_signal() {
        {
            /* Set up sigmask to receive configured signal in the main thread.
            * All created threads also get this signal mask, so all threads
            * get the signal. But we can use pthread_signal to direct it at one.
            */
            struct sigaction sa;
            sa.sa_handler = thread_signal_handler;
            sa.sa_flags = 0;
            sigemptyset(&sa.sa_mask);

            if (sigaction(THREAD_SIGNAL, &sa, NULL) == -1) {
                throw ThreadException("[signal] can't install thread signal");
            }
        }
    }

    void Thread::create(detach_t detach, void *info) {
        if(!this->_create){
            if(pthread_create(&this->thread_info.thread_id, NULL, this->func, info) != 0) {
                throw ThreadException("[create] create thread failed");
            }
            if (detach == E_THREAD_DETACHED)
            {
                cout << "Detached Thread:" << this->thread_info.name << endl;
                if (pthread_detach(this->thread_info.thread_id))
                {
                    throw ThreadException("[detach] can't create detached thread");
                }
            }
            this->_create = T_TRUE;
        }
    }

    void Thread::start() {
        if(this->func == NULL){
            throw ThreadException("[start] there has no loop func");
        }
        this->create(E_THREAD_JOINABLE, this);
        this->thread_info.state = E_THREAD_RUNNING;
    }

    void Thread::stop() {
        this->thread_info.state = E_THREAD_STOPPING;
        /* Send signal to the thread to kick it out of any system call it was in */
        pthread_kill(this->thread_info.thread_id, THREAD_SIGNAL);
        if (this->thread_info.detach == E_THREAD_JOINABLE){
            WAR_vPrintf(dbg, "Join Thread\n");
            if(pthread_join(this->thread_info.thread_id, NULL)){
                throw ThreadException("[join] pthread_join error");
            }
        }
        this->thread_info.state = E_THREAD_STOPPED;
        WAR_vPrintf(dbg, "Stop Thread: %ld\n", this->thread_id());
    }

    void Thread::exit() {
        WAR_vPrintf(dbg, "Exit Thread: %ld\n", this->thread_id());
        this->thread_info.state = E_THREAD_STOPPED;
        pthread_exit(NULL);
    }

    /** Signal handler to receive THREAD_SIGNAL.
     *  This is just used to interrupt system calls such as recv() and sleep().
     */
    void Thread::thread_signal_handler(int sig) {
        DBG_vPrintf(T_TRUE, "Signal %d received\n", sig);
    }

    Thread::~Thread() { }
}
