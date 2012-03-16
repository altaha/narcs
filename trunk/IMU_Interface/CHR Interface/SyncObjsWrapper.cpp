#include "SyncObjsWrapper.h"
#include "SynchObjs.h"


/* MutexObjWrapper functions */
MutexObjWrapper::MutexObjWrapper(void) : mutexObj(new MutexObj())
{
}

MutexObjWrapper::~MutexObjWrapper(void)
{
	delete mutexObj;
}

bool MutexObjWrapper::initMutex(void)
{
	return mutexObj->initMutex();
}

bool MutexObjWrapper::initNamedMutex(TCHAR* name, bool create)
{
	return mutexObj->initNamedMutex(name, create);
}

bool MutexObjWrapper::lockMutexWrapper(unsigned int timeout_ms)
{
	return lockMutex(*mutexObj, timeout_ms);
}

void MutexObjWrapper::unlockMutexWrapper(void)
{
	unlockMutex(*mutexObj);
}


/* EventObjWrapper functions */
EventObjWrapper::EventObjWrapper(void) : eventObj(new EventObj())
{
}

EventObjWrapper::~EventObjWrapper(void)
{
	delete eventObj;
}

bool EventObjWrapper::initEvent(bool manual)
{
	return eventObj->initEvent(manual);
}

bool EventObjWrapper::initNamedEvent(TCHAR* name, bool creator, bool manual)
{
	return eventObj->initNamedEvent(name, creator, manual);
}

void EventObjWrapper::setEventWrapper(void)
{
	setEvent(*eventObj);
}

bool EventObjWrapper::waitEventWrapper(unsigned int timeout_ms)
{
	return waitEvent(*eventObj, timeout_ms);
}

void EventObjWrapper::resetEventWrapper(void)
{
	resetEvent(*eventObj);
}