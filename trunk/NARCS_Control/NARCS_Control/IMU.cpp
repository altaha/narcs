#include "StdAfx.h"
#include "IMU.h"


IMU::IMU(Thread_type type){
	setType(type);
}


IMU::~IMU(void)
{
}

int IMU::threadMain(void){

	try{
		printf("This is IMU Thread\n");
		while( !this->_stop ){
		}
		printf("IMU OUT\n");
	}
	catch(...){
		//Don't need anything special here. Since main thread is going to reincarnate any dead ones
		printf("Exception in thread: %ul\tExiting thread\n", GetCurrentThreadId());
	}
	this->_active = false;
	return EXIT_SUCCESS;
}