#include "StdAfx.h"
#include "ThreadObj.h"

bool ThreadObj::startThread(void){
	this->_running = true;

	//_handle = (HANDLE) _beginthreadex(NULL, 0, &thread_launcher, config, 0, &_threadId[0]);
	this->_handle = (HANDLE) _beginthreadex(NULL, 0, &thread_starting, dynamic_cast<void*>(this), 0, NULL);
	return true;
}

unsigned _stdcall thread_starting(void* param){

	ThreadObj* thread = static_cast<ThreadObj*>(param);

	return thread->threadMain();
}