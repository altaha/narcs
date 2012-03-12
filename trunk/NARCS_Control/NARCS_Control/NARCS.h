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

	//start running
	void start_threads(void);

	//checks for dead threads upto timeout
	//Any dead threads are reincarnated
	//timeout in ms: -1 for infinite, 0 for nowait
	void reincarnate(unsigned long timeout);	

private:
	int _nActiveThreads; //TODO: figure out if this is needed

	ThreadObj* _threads[NUM_THREADS]; //handle to thread classes
	void stop_threads();

};