#pragma once

//Define exception error codes
#define UNEXPECTED_ERROR 1
#define INVALID_THREAD_TYPE 2
#define FAILED_THREAD_CREATE 3
#define INVALID_PARAM_VALUE 4

//Thread Defines
#define NUM_THREADS	4 //plus primary thread

class NARCS; //forward declaration

typedef enum Thread_type {
	KINECT_AND_IMU_THREAD,	// thread for sending Kinect and IMU data to the remote side
	ARDUINO_THREAD,
	ROBO_ARM_THREAD,
} Thread_type;

static bool threadRequired[NUM_THREADS] =	//set which threads (modules) will be used
{
	false,	//KINECT_AND_IMU_THREAD
	true,	//ARDUINO_THREAD
	false,	//ROBO_ARM_THREAD
};


/////Other types
typedef struct orient_data{
	double roll;
	double pitch;
	double yaw;
} orient_data;


////Inter thread communication global objects
extern struct thrdCommBlock globCommBlocks[NUM_THREADS];
