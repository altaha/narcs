#pragma once

#include <math.h>
#include "ThreadObj.h"
#include "SharedMem.h"
#include "DataTransferStructs.h"
#include "MovingAverage.h"

class IMU :
	public ThreadObj
{
public:
	IMU(Thread_type type);
	~IMU(void);
	int threadMain(void);
	SharedMem* _sharedMem;
};

