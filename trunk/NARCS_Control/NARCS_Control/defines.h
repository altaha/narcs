#pragma once

//Define exception error codes
#define INVALID_THREAD_TYPE 1

//Thread Defines
#define NUM_THREADS	5 //plus primary thread

class NARCS; //forward declaration

typedef enum Thread_type {
	KINECT_THREAD,		//Thread to get Kinect data
	IMU_THREAD,			//Thread for IMU
	ARDUINO_THREAD,
	ROBO_ARM_THREAD,
	LOGIC_THREAD,		//central thread that brings things together
} Thread_type;

static bool threadRequired[NUM_THREADS] =	//set which threads (modules) will be used
{
	true,	//KINECT_THREAD
	false,	//IMU_THREAD
	false,	//ARDUINO_THREAD
	false,	//ROBO_ARM_THREAD
	false,	//LOGIC_THREAD
};

//typedef struct Thread_config {
//	Thread_type	_thread_type;	/**< @brief Thread to launch. */
//	NARCS		*_object;		/**< @brief Blueberry class object. */
//}Thread_config;
