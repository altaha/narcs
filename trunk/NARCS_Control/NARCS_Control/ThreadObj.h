#pragma once

#include "stdafx.h"

//Executed by newly created threads. Points to specific function to execute
unsigned _stdcall thread_starting(void* param);

class ThreadObj
{
public:
	ThreadObj(void) :_running(false),_handle(NULL),_status(NULL) {}
	~ThreadObj(void)
	{}

	bool startThread(void);

	void setType(Thread_type type){
		_type = type;
	}
	Thread_type getType(){
		return _type;
	}

	virtual int threadMain() = 0;
	
private:
	HANDLE _handle;
	bool _running; //determines if thread should be running
	bool _status;	//thread status: active, terminated, etc;
	Thread_type _type;
};

