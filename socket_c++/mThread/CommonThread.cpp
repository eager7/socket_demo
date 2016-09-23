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
    CommonThread::CommonThread(): _create(T_FALSE), thread_info(), func(NULL){ }

    CommonThread::CommonThread(CommonThread &t): _create(T_FALSE), thread_info(), func(NULL) {
        this->thread_info = t.thread_info;
        this->_create = t._create;
    }

    CommonThread::CommonThread(string name,  detach_t detach, thread_loop func)
            : _create(T_FALSE), thread_info(name, detach), func(func) {
        this->thread_info = ThreadInfo(name, detach);
    }

    void CommonThread::install_signal() {
        /* Set up sigmask to receive configured signal in the main thread.
        * All created threads also get this signal mask, so all threads
        * get the signal. But we can use pthread_signal to direct it at one.
        */
        struct sigaction sa;
        sa.sa_handler = thread_signal_handler;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);
        cout << "install signal" << endl;
        if (sigaction(THREAD_SIGNAL, &sa, NULL) == -1) {
            throw ThreadException("[signal] can't install thread signal");
        }
    }

    void CommonThread::start() {
        if(!this->_create){
            if(pthread_create(&this->thread_info.thread_id, NULL, start_thread, (void*)this) != 0) {
                stringstream error;
                error << "[create] create thread failed:" << strerror(errno) << endl;
                throw ThreadException(error.str());
            }
            if (this->thread_info.detach == E_THREAD_DETACHED) {
                cout << "Detached Thread:" << this->thread_info.name << endl;
                if (pthread_detach(this->thread_info.thread_id)) {
                    throw ThreadException("[detach] can't create detached thread");
                }
            }
            this->_create = T_TRUE;
            this->thread_info.state = E_THREAD_RUNNING;
        }
    }

    //call in father thread
    void CommonThread::stop() {
        this->thread_info.state = E_THREAD_STOPPING;
        /* Send signal to the thread to kick it out of any system call it was in */
        pthread_kill(this->thread_info.thread_id, THREAD_SIGNAL);
        if (this->thread_info.detach == E_THREAD_JOINABLE){
            if(pthread_join(this->thread_info.thread_id, NULL)){
                stringstream error;
                error << "[join] pthread_join error:" << strerror(errno) << endl;
                throw ThreadException(error.str());
            }
        }
        this->thread_info.state = E_THREAD_STOPPED;
        cout << "Stop Thread:" << this->name() << endl;
    }

    //call in child thread
    void CommonThread::exit() {
        cout << "Exit Thread:" << this->name() << endl;
        this->thread_info.state = E_THREAD_STOPPED;
        pthread_exit(NULL);
    }

    void* CommonThread::start_thread(void *arg) {
        CommonThread *thread = (CommonThread*)arg;
        if(thread->func == NULL){
            cout << "run" << endl;
            thread->run(thread);
        } else {
            cout << "func" << endl;
            thread->func(thread);
        }

        thread->exit();
        return NULL;
    }

    /** Signal handler to receive THREAD_SIGNAL.
     *  This is just used to interrupt system calls such as recv() and sleep().
     */
    void CommonThread::thread_signal_handler(int sig) {
        cout << "Signal received:" << sig << endl;
    }


    ostream& operator<< (ostream &out, CommonThread &thread) {
        out << thread.name() << ":" << thread.state() << ":" << thread.thread_id();
        return out;
    }
}

