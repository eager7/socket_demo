/****************************************************************************
 *
 * MODULE:             mThreadException
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

namespace mThread
{
    ThreadException::ThreadException(const string &error_str) {
        this->_error = error_str;
    }

    ThreadException::~ThreadException() throw(){ }

    const char *ThreadException::what() const throw(){
        return this->_error.c_str();
    }

    ostream &operator << (ostream &out, mThread::ThreadException &e){
        out << e.what();
        return out;
    }
}