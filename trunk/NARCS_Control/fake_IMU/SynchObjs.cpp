#include "StdAfx.h"
#include "SynchObjs.h"

///////////////////////////////////////////////////////////////////////////////
MutexObj::MutexObj(void): _handle(NULL), _valid(false)
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
		printf("Create Mutex error: %ul\n", GetLastError());
		throw 123; //TODO
		return false;
	}
	_valid = true;
	return true;
}

bool MutexObj::initNamedMutex(TCHAR* name, bool create)
{
	if(create){
		_handle = CreateMutex(NULL, FALSE, name);
		if (_handle == NULL){
			printf("Create Mutex (named) error: %ul\n", GetLastError());
			throw 123; //TODO
			return false;
		}
	}else{
		_handle = OpenMutex(SYNCHRONIZE, FALSE, name);
		if (_handle == NULL){
			printf("Can't Open named Mutex: %ul\n", GetLastError());
			return false;
		}
	}
	_valid = true;
	return true;
}


///////////////////////////////////////////////////////////////////////////////////
EventObj::EventObj(void): _handle(NULL), _valid(false), _manual(false)
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
		printf("Create Event error: %ul\n", GetLastError());
		throw 123; //TODO
		return false;
	}
	_manual = manual;
	_valid = true;
	return true;
}

bool EventObj::initNamedEvent(TCHAR* name, bool creator, bool manual)
{
	if(creator){
		_handle = CreateEvent(NULL, manual, FALSE, name);
		if (_handle == NULL){
			printf("Create Mutex (named) error: %ul\n", GetLastError());
			throw 123; //TODO
			return false;
		}
	}else{
		_handle = OpenEvent(SYNCHRONIZE, FALSE, name);
		if (_handle == NULL){
			printf("Can't Open named Event: %ul\n", GetLastError());
			return false;
		}
	}
	_manual = manual;
	_valid = true;
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
			printf("lockMutex WaitForSingleObject error: %ul\n", GetLastError());
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
			printf("waitEvent WaitForSingleObject error: %ul\n", GetLastError());
			break;
	}
	throw 123;
	return false;
}
//only for manual-reset events (hopefully not used in proj)
void resetEvent(EventObj& i_event)
{
	if( !ResetEvent(i_event.getHandle()) ){
		printf("resetEvent error: %ul\n", GetLastError());
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
			printf("waitEventAndMutex WaitForMultipleObjects error: %ul\n", GetLastError());
			break;
	}
	throw 123;
	return false;
}


///////////////////////////////////////////////////////////////////////////////////
//Implementation of functions for reading/writing to thrdShareBuffers
void thrdCommBlock::writeBytes( const void* data, unsigned int size, unsigned int offset)
{
	if ( (offset+size)>this->_size){
		printf("thrdCommBlock writeBytes Error: overflows\n");
		return;
	}
	else{
		memcpy( this->_pBuf+offset, data, size );
	}
}
void thrdCommBlock::readBytes( void* outData, unsigned int size, unsigned int offset)
{
	if ( (offset+size)>this->_size){
		printf("thrdCommBlock readBytes Error: overflows\n");
		return;
	}
	else{
		memcpy( outData, this->_pBuf+offset, size );
		return;
	}
}
