#pragma once

#include "defines.h"

class MutexObj
{
public:
	MutexObj(void);
	~MutexObj(void);

	bool initMutex(void);
	//create named mutexes. Used for inter-process sharing
	bool initNamedMutex( TCHAR* name, bool create);

	HANDLE getHandle(void)
	{
		return _handle;
	}

private:
	HANDLE _handle; //handle to underlying Win32 Mutex Object
};

class EventObj
{
public:
	EventObj(void);
	~EventObj(void);

	bool initEvent(bool manual);
	//create named events. Used for inter-process sharing
	bool initNamedEvent(bool manual, TCHAR* name, bool create);

	HANDLE getHandle(void)
	{
		return _handle;
	}
	bool isManual(void){
		return _manual;
	}

private:
	HANDLE _handle; //handle to underlying Win32 Event Object
	bool _manual;	//indicate manually reset event
};

//use INFINITE for infinite blocking waits
bool lockMutex(MutexObj& i_mutex, unsigned int timeout_ms);
void unlockMutex(MutexObj& i_mutex);

void setEvent(EventObj& i_event);
bool waitEvent(EventObj& i_event, unsigned int timeout_ms);
//reset only for events requiring manual reset (can check with isManual)
void resetEvent(EventObj& i_event);

bool waitEventAndMutex(EventObj& i_event, MutexObj& i_mutex, unsigned int timeout_ms);