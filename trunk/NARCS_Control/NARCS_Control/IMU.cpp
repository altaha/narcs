#include "StdAfx.h"
#include "IMU.h"


IMU::IMU(Thread_type type){
	setType(type);
}


IMU::~IMU(void)
{
}

int IMU::threadMain(void){
	printf("This is IMU Thread\n");
	return EXIT_SUCCESS;
}