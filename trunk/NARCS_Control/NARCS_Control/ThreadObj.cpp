#include "StdAfx.h"
#include "ThreadObj.h"

bool ThreadObj::startThread(void)
{
	HANDLE temp = NULL;
	this->_stop = false;

	temp = (HANDLE) _beginthreadex(NULL, 0, &thread_starting, dynamic_cast<void*>(this), 0, NULL);

	int rc = (uintptr_t)temp;
	if (rc == 0){
		//failed to create thread
		return false;
	}else{

		this->_handle = temp;
		this->_active = true;
		return true;
	}
}

unsigned _stdcall thread_starting(void* param){

	ThreadObj* thread = static_cast<ThreadObj*>(param);

	return thread->threadMain();
}