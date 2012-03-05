#include "StdAfx.h"
#include "NARCS.h"

NARCS::NARCS(void)
{
}


NARCS::~NARCS(void)
{
}

void NARCS::allocate_threads(void)
{
	for (int i=0; i<NUM_THREADS; i++){
		if (threadRequired[i]){
			switch (i){
				case KINECT_THREAD:
					_threads[i] = new IMU ( (Thread_type) i);
					break;
				case IMU_THREAD:
				case ARDUINO_THREAD:
				case ROBO_ARM_THREAD:
				default:
					break;
			}
		}
	}
}

void NARCS::start_threads(void){
	for (int i=0; i<NUM_THREADS; i++){
		if (threadRequired[i]){
			_threads[i]->startThread();
		}
	}
}
