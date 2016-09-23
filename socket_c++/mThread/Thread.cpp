/****************************************************************************
 *
 * MODULE:             Thread
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
    static int _signal = 0;
    Thread::Thread(thread_loop func, string name, detach_t detach) : CommonThread(name, detach, func){
        if(_signal == 0){
            this->install_signal();
            _signal = 1;
        }
        this->start();
    }

    void Thread::run(void *arg) {
        while (this->state() == E_THREAD_RUNNING){
            cout << "Thread::run--" << this->name() << endl;
            sleep(1);
        }
    }
}
