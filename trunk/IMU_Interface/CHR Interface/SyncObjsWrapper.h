#pragma once


#include <tchar.h>


extern class MutexObj;
extern class EventObj;

#define INFINITE_WAIT  0xFFFFFFFF


class MutexObjWrapper
{
public:
	MutexObjWrapper(void);
	~MutexObjWrapper(void);

	bool initMutex(void);
	//create named mutexes. Used for inter-process sharing
	bool initNamedMutex(TCHAR* name, bool create);

	//use INFINITE_WAIT for infinite blocking waits
	bool lockMutexWrapper(unsigned int timeout_ms);
	void unlockMutexWrapper(void);

private:
	MutexObj *mutexObj;
};


class EventObjWrapper
{
public:
	EventObjWrapper(void);
	~EventObjWrapper(void);

	bool initEvent(bool manual);
	//create named events. Used for inter-process sharing
	bool initNamedEvent(TCHAR* name, bool creator, bool manual);

	void setEventWrapper(void);
	bool waitEventWrapper(unsigned int timeout_ms);
	//reset only for events requiring manual reset (can check with isManual)
	void resetEventWrapper(void);

private:
	EventObj *eventObj;
};