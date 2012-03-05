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

private:
	ThreadObj* _threads[NUM_THREADS]; //handle to thread classes


};