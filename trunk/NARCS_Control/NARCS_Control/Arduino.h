#pragma once

#include "ThreadObj.h"

class Arduino :
	public ThreadObj
{
public:
	Arduino(Thread_type type);
	~Arduino(void);
	int threadMain(void);
	SOCKET ArduinoSocket;
};

