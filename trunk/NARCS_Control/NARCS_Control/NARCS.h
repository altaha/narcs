#pragma once

#include "ThreadObj.h"
#include "IMU.h"

class NARCS
{
public:
	NARCS(void);
	~NARCS(void);

	//allocate threads
	void allocate_threads(void);

	void start_threads(void);

	//checks for dead threads upto timeout
	//Any dead threads are reincarnated
	//timeout in ms: -1 for infinite, 0 for nowait
	void reincarnate(unsigned long timeout);	

	ThreadObj* _threads[NUM_THREADS]; //handle to thread classes

private:
	int _nActiveThreads;

	void stop_threads();

};