#pragma once

//#include "stdafx.h"
#include <stdio.h>
#include <tchar.h>

#include <comdef.h>
#include <windows.h>
//#include <process.h>
//using namespace System;
//using namespace System::Runtime::InteropServices;
//using namespace Microsoft::Win32::SafeHandles;
//using namespace System::IO;

/////////////////////////////////
class MutexObj
{
public:
	MutexObj(void);
	~MutexObj(void);

	bool initMutex(void);
	//create named mutexes. Used for inter-process sharing
	bool initNamedMutex(TCHAR* name, bool create);

	HANDLE getHandle(){
		return _handle;
	}
	bool isValid(void)
	{
		return _valid;
	}

private:
	HANDLE _handle; //handle to underlying Win32 Mutex Object
	bool _valid; //indicates whether Mutex has been successfully created
};

/////////////////////////////////
class EventObj
{
public:
	EventObj(void);
	~EventObj(void);

	bool initEvent(bool manual);
	//create named events. Used for inter-process sharing
	bool initNamedEvent(TCHAR* name, bool creator, bool manual);

	HANDLE getHandle(){
		return _handle;
	}
	bool isValid(void){
		return _valid;
	}
	bool isManual(void){
		return _manual;
	}

private:
	HANDLE _handle; //handle to underlying Win32 Event Object
	bool _valid; //indicates whether event has been successfully created
	bool _manual;	//indicate manually reset event
};

/////////////////////////////////
//Functions to operate on Mutexs and Events
/////////////////////////////////

//use INFINITE for infinite blocking waits
bool lockMutex(MutexObj& i_mutex, unsigned int timeout_ms);
void unlockMutex(MutexObj& i_mutex);

void setEvent(EventObj& i_event);
bool waitEvent(EventObj& i_event, unsigned int timeout_ms);
//reset only for events requiring manual reset (can check with isManual)
void resetEvent(EventObj& i_event);

bool waitEventAndMutex(EventObj& i_event, MutexObj& i_mutex, unsigned int timeout_ms);