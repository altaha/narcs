#pragma once

#include "ThreadObj.h"

class IMU :
	public ThreadObj
{
public:
	IMU(Thread_type type);
	~IMU(void);
	int threadMain(void);
};

