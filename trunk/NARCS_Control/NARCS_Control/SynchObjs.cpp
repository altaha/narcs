#include "StdAfx.h"
#include "SynchObjs.h"

///////////////////////////////////////////////////////////////////////////////
MutexObj::MutexObj(void): _handle(NULL)
{
}

MutexObj::~MutexObj(void)
{
	if( _handle!= NULL){
		CloseHandle(this->_handle);
	}
}

bool MutexObj::initMutex()
{
	_handle = CreateMutex(NULL, FALSE, NULL);
	if (_handle == NULL){
		printf("Create Mutex error: %d\n", GetLastError());
		throw 123; //TODO
		return false;
	}
	return true;
}

bool MutexObj::initNamedMutex(TCHAR* name, bool create)
{
	if(create){
		_handle = CreateMutex(NULL, FALSE, name);
		if (_handle == NULL){
			printf("Create Mutex (named) error: %d\n", GetLastError());
			throw 123; //TODO
			return false;
		}
	}else{
		_handle = OpenMutex(SYNCHRONIZE, FALSE, name);
		if (_handle == NULL){
			printf("Can't Open named Mutex: %d\n", GetLastError());
			return false;
		}
	}
	return true;
}


///////////////////////////////////////////////////////////////////////////////////
EventObj::EventObj(void): _handle(NULL), _manual(false)
{
}

EventObj::~EventObj(void)
{
	if( _handle!= NULL){
		CloseHandle(this->_handle);
	}
}

bool EventObj::initEvent(bool manual)
{
	_handle = CreateEvent(NULL, manual, FALSE, NULL);
	if (_handle == NULL){
		printf("Create Event error: %d\n", GetLastError());
		throw 123; //TODO
		return false;
	}
	_manual = manual;
	return true;
}

bool EventObj::initNamedEvent(bool manual, TCHAR* name, bool create)
{
	if(create){
		_handle = CreateEvent(NULL, manual, FALSE, name);
		if (_handle == NULL){
			printf("Create Mutex (named) error: %d\n", GetLastError());
			throw 123; //TODO
			return false;
		}
	}else{
		_handle = OpenEvent(SYNCHRONIZE, FALSE, name);
		if (_handle == NULL){
			printf("Can't Open named Event: %d\n", GetLastError());
			return false;
		}
	}
	_manual = manual;
	return true;
}


///////////////////////////////////////////////////////////////////////////////////
//use INFINITE for infinite blocking waits
bool lockMutex(MutexObj& i_mutex, unsigned int timeout_ms)
{
	//set timeout to INFINITE for blocking wait
	DWORD rc = WaitForSingleObject(i_mutex.getHandle(), timeout_ms);
	switch (rc){
		case WAIT_OBJECT_0: //acquired
			return true;

		case WAIT_ABANDONED:
			//previous owner terminated before releasing lock
			//caller of this function gets lock, but maybe indicates that other guy is dead
			return true;

		case WAIT_TIMEOUT: //Timed out without obtaining lock
			return false;

		default:
			printf("lockMutex WaitForSingleObject error: %d\n", GetLastError());
			break;
	}
	throw 123;
	return false;
}
void unlockMutex(MutexObj& i_mutex)
{
	if( !ReleaseMutex(i_mutex.getHandle()) )
	{
		printf("ReleaseMutex failed (%d)\n", GetLastError());
		throw 123; //TODO
	}
}

void setEvent(EventObj& i_event)
{
	if (! SetEvent(i_event.getHandle()) ) 
	{
		printf("SetEvent failed (%d)\n", GetLastError());
		throw 123; //TODO
	}
}
bool waitEvent(EventObj& i_event, unsigned int timeout_ms)
{
	//set timeout to INFINITE for blocking wait
	DWORD rc = WaitForSingleObject(i_event.getHandle(), timeout_ms);
	switch (rc){
		case WAIT_OBJECT_0: //acquired
			return true;

		case WAIT_TIMEOUT: //Timed out without obtaining lock
			return false;

		default:
			printf("waitEvent WaitForSingleObject error: %d\n", GetLastError());
			break;
	}
	throw 123;
	return false;
}
//only for manual-reset events (hopefully not used in proj)
void resetEvent(EventObj& i_event)
{
	if( !ResetEvent(i_event.getHandle()) ){
		printf("resetEvent error: %d\n", GetLastError());
		throw 123;
	}
}

bool waitEventAndMutex(EventObj& i_event, MutexObj& i_mutex, unsigned int timeout_ms)
{
	HANDLE lpHandles [2] = { i_event.getHandle(), i_mutex.getHandle() };

	DWORD rc = WaitForMultipleObjects(2, lpHandles, TRUE, timeout_ms);
	switch (rc){
		case WAIT_OBJECT_0: //success. acquired mutex
			return true;

		case WAIT_ABANDONED:
			//The mutex is abandoned (probably means previous owner died)
			//still success. acquired mutex
			return true;

		case WAIT_TIMEOUT: //Timed out without satisfying waits
			return false;

		default:
			printf("waitEventAndMutex WaitForMultipleObjects error: %d\n", GetLastError());
			break;
	}
	throw 123;
	return false;
}
