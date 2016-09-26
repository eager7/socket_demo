/****************************************************************************
 *
 * MODULE:             mThread
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

#ifndef THREAD_CPP_MTHREAD_H
#define THREAD_CPP_MTHREAD_H

#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <vector>
#include <fstream>

#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

using namespace std;

namespace mThread
{
#define THREAD_SIGNAL SIGUSR1

    typedef enum{
        T_TRUE = 0x01,
        T_FALSE = 0x00,
    }bool_t;
    typedef enum {
        E_THREAD_JOINABLE = 0x01,      /**< Thread is created so that it can be waited on and joined */
        E_THREAD_DETACHED = 0x02,      /**< Thread is created detached so all resources are automatically free'd at exit. */
    } detach_t;
    typedef enum {
        E_THREAD_STOPPED = 0x00,   /**< Thread stopped */
        E_THREAD_RUNNING = 0x01,   /**< Thread running */
        E_THREAD_STOPPING = 0x02,  /**< Thread signaled to stop */
    } state_t;
    typedef void *(*thread_loop)(void *arg);

    class ThreadException : public exception
    {
    private:
        string _error;
    public:
        ThreadException(const string &error_str);
        ~ThreadException() throw();

        virtual const char *what() const throw();
        friend ostream &operator << (ostream &out, ThreadException &e);
    };

    struct ThreadInfo
    {
    public:
        string name;
        state_t state;
        detach_t detach;
        pthread_t thread_id;

        ThreadInfo():
                name(NULL),state(E_THREAD_STOPPED),
                detach(E_THREAD_JOINABLE),thread_id(0) { }
        ThreadInfo(string name, detach_t detach):
                name(name),state(E_THREAD_STOPPED),detach(detach),thread_id(0) { }
    };

    template <class T>
    class ThreadQueue{
    public:
        pthread_mutex_t mutex;
        pthread_cond_t cond_space_available;
        pthread_cond_t cond_data_available;

        T **apvBuffer;
        unsigned int length;
        unsigned int front;
        unsigned int rear;

        ThreadQueue(unsigned int len):length(len),front(0),rear(0){
            this->apvBuffer = (T**)malloc(sizeof(void *) * len);
            if (NULL == apvBuffer){
                throw ThreadException("[malloc] can't new memory");
            }

            pthread_mutex_init(&this->mutex, NULL);
            pthread_cond_init(&this->cond_data_available, NULL);
            pthread_cond_init(&this->cond_space_available, NULL);
        }
        ~ThreadQueue(){
            if(NULL != this->apvBuffer) free(this->apvBuffer);

            pthread_mutex_destroy(&this->mutex);
            pthread_cond_destroy(&this->cond_data_available);
            pthread_cond_destroy(&this->cond_space_available);
        }

        void Enqueue(T *pvData){
            pthread_mutex_lock(&this->mutex);
            while (((this->rear + 1)%this->length) == this->front)
                pthread_cond_wait(&this->cond_space_available, &this->mutex);
            this->apvBuffer[this->rear] = pvData;

            this->rear = (this->rear+1) % this->length;

            pthread_mutex_unlock(&this->mutex);
            pthread_cond_broadcast(&this->cond_data_available);
        }
        void Dequeue(T **ppvData){
            pthread_mutex_lock(&this->mutex);
            while (this->front == this->rear)
                pthread_cond_wait(&this->cond_data_available, &this->mutex);
            *ppvData = this->apvBuffer[this->front];

            this->front = (this->front + 1) % this->length;
            pthread_mutex_unlock(&this->mutex);
            pthread_cond_broadcast(&this->cond_space_available);
        }
    };

    class CommonThread{
    protected:
        bool_t _create;
        void install_signal();

    private:
        ThreadInfo thread_info;
        static void thread_signal_handler(int sig);
        static void* start_thread(void* arg);

    public:
        thread_loop func;
        CommonThread();
        CommonThread(CommonThread &t);
        CommonThread(string name = "None", detach_t detach = E_THREAD_JOINABLE, thread_loop func = NULL);

        void start();
        void stop();
        void exit();
        virtual void run(void *arg) = 0;

        string name(){return this->thread_info.name;}
        state_t state(){return this->thread_info.state;}
        pthread_t thread_id(){return this->thread_info.thread_id;}

        friend ostream& operator<< (ostream &out, CommonThread &thread);
    };
    class Thread : public CommonThread
    {
    public:
        Thread(thread_loop target=NULL, string name = "None", detach_t detach = E_THREAD_JOINABLE);
        void run(void *arg);
    };
}


#endif //THREAD_CPP_MTHREAD_H
