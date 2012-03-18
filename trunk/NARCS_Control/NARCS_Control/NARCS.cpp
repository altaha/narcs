#include "StdAfx.h"
#include "NARCS.h"
#include "Arduino.h"

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
				case KINECT_AND_IMU_THREAD:
					_threads[i] = new IMU ( (Thread_type) i);
					break;
				case ARDUINO_THREAD:
					_threads[i] = new Arduino ( (Thread_type) i);
					break;
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

	int nThreads=0;
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
	//TODO: handle case if threads don't die after 2 seconds of waiting
	//Must terminate threads
	/*if(dead == WAIT_TIMEOUT)
	{
	}*/
	for(int i=0; i<nThreads; i++)
	{
		CloseHandle(activeThreadHandles[i]);
	}
}

void NARCS::reincarnate(unsigned long timeout)
{
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

ThreadObj* NARCS::getHandle(int i) 
{
	return _threads[(Thread_type) i];
}