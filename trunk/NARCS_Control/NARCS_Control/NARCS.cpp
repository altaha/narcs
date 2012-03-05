#include "StdAfx.h"
#include "NARCS.h"

NARCS::NARCS(void) : _nActiveThreads(0)
{
	for (int i=0; i<NUM_THREADS; i++){
		this->_threads[i] = NULL;
	}
}


NARCS::~NARCS(void)
{
	this->stop_threads();
	for (int i=0; i<NUM_THREADS; i++)
	{
		if(this->_threads[i]!=NULL){
			delete this->_threads[i];
		}
	}
}

void NARCS::allocate_threads(void)
{
	for (int i=0; i<NUM_THREADS; i++){
		if (threadRequired[i]){
			switch (i){
				case KINECT_THREAD:
					_threads[i] = new IMU ( (Thread_type) i);
					break;
				case IMU_THREAD:
				case ARDUINO_THREAD:
				case ROBO_ARM_THREAD:
				default:
					break;
			}
		}
	}
}

void NARCS::start_threads(void)
{	
	for (int i=0; i<NUM_THREADS; i++)
	{
		if (threadRequired[i])
		{
			if(! _threads[i]->startThread() ){
				throw FAILED_THREAD_CREATE;
			}
			else{
				//this->_activeThreads[_nActiveThreads] = _threads[i];
				//this->_activeThreadHandles[_nActiveThreads] = _threads[i]->getHandle();
				_nActiveThreads++;
			}
		}
	}
}

void NARCS::stop_threads(void)
{
	printf("Terminating child threads\n");

	ThreadObj* activeThreads[NUM_THREADS];
	HANDLE activeThreadHandles[NUM_THREADS];

	DWORD nThreads=0;
	for (int i=0; i<NUM_THREADS; i++)
	{
		if (threadRequired[i] && this->_threads[i]!=NULL && this->_threads[i]->getHandle()!=NULL)
		{
			this->_threads[i]->stopThread();
			activeThreads[nThreads] = this->_threads[i];
			activeThreadHandles[nThreads] = this->_threads[i]->getHandle();
			nThreads++;
		}
	}
	
	DWORD timeout = 2000; //wait 2 seconds for threads to exit
	DWORD dead = WaitForMultipleObjects(nThreads,activeThreadHandles,TRUE,timeout);
	/*if(dead != WAIT_TIMEOUT)
	{
		_nActiveThreads--;
		if( !activeThreads[ dead - WAIT_OBJECT_0]->startThread() ){
			throw FAILED_THREAD_CREATE;
		}
	}*/
	for(int i=0; i<nThreads; i++)
	{
		CloseHandle(activeThreadHandles[i]);
	}
}

void NARCS::reincarnate(unsigned long timeout)
{
	if (timeout == -1){
		timeout = INFINITE;
	} else if (timeout < 0){
		throw INVALID_PARAM_VALUE;
	}

	ThreadObj* activeThreads[NUM_THREADS];
	HANDLE activeThreadHandles[NUM_THREADS];

	DWORD dead = 0;
	DWORD nThreads=0;
	for (int i=0; i<NUM_THREADS; i++)
	{
		if (threadRequired[i] && this->_threads[i]!=NULL && this->_threads[i]->getHandle()!=NULL)
		{
			activeThreads[nThreads] = this->_threads[i];
			activeThreadHandles[nThreads] = this->_threads[i]->getHandle();
			nThreads++;
		}
	}

	dead = WaitForMultipleObjects(nThreads,activeThreadHandles,FALSE,timeout);
	if(dead != WAIT_TIMEOUT)
	{
		_nActiveThreads--;
		if( !activeThreads[ dead - WAIT_OBJECT_0]->startThread() ){
			throw FAILED_THREAD_CREATE;
		}
	}
}
