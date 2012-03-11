#pragma once

#include "ThreadObj.h"
#include "SharedMem.h"

class IMU :
	public ThreadObj
{
public:
	IMU(Thread_type type);
	~IMU(void);
	int threadMain(void);
	SharedMem* _sharedMem;
};

