#include "SyncObjsWrapper.h"
#include "SynchObjs.h"
#include "UnmanagedClassHandling.h"


/* MutexObjWrapper functions */
MutexObjWrapper::MutexObjWrapper(void)
{
	minitialize();
	mutexObj = new MutexObj();
}

MutexObjWrapper::~MutexObjWrapper(void)
{
	delete mutexObj;
	mterminate();
}

bool MutexObjWrapper::initMutex(void)
{
	return mutexObj->initMutex();
}

bool MutexObjWrapper::initNamedMutex(wchar_t *name, bool create)
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
EventObjWrapper::EventObjWrapper(void)
{
	minitialize();
	eventObj = new EventObj();
}

EventObjWrapper::~EventObjWrapper(void)
{
	delete eventObj;
	mterminate();
}

bool EventObjWrapper::initEvent(bool manual)
{
	return eventObj->initEvent(manual);
}

bool EventObjWrapper::initNamedEvent(wchar_t *name, bool creator, bool manual)
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