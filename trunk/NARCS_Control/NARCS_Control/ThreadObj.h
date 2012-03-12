#pragma once

#include "stdafx.h"

#include "SynchObjs.h"

//Executed by newly created threads. Points to specific function to execute
unsigned _stdcall thread_starting(void* param);

class ThreadObj
{
public:
	ThreadObj(void) :_active(false),_stopThread(false),_handle(NULL){}
	~ThreadObj(void)
	{}

	bool startThread(void);

	HANDLE getHandle()
	{
		return this->_handle;
	}

	void stopThread(void)
	{
		_stopThread = true;
	}

	void setType(Thread_type type)
	{
		_type = type;
	}

	Thread_type getType()
	{
		return _type;
	}

	virtual int threadMain() = 0;
	
protected:
	HANDLE _handle;
	bool _active;	//thread status( running or not)
	bool _stopThread;		//command thread to terminate
	Thread_type _type;

	thrdCommBlock* commBlock;
};

