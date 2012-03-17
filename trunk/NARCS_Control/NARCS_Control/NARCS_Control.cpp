// NARCS_Control.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
//#include "NARCS.h"
//#include "ThreadObj.h"
#include "SharedMem.h"
#include "SynchObjs.h"
//#include "IMU.h"
// <adeel>
#include "KinectData.h"
// </adeel>


//#define ADEEL_DEBUG


#ifdef ADEEL_DEBUG
#include <iostream>
#include <fstream>
#endif



//#define BUFF_SIZE 1024


thrdCommBlock globCommBlocks[NUM_THREADS];


using namespace std;


typedef struct IMUData
{
	float pitch;
	float roll;
} IMUData;


/* 
  MAIN THREAD:
  ============

  Responsible for:
	- TODO:- Setting up all sockets
	- TODO:- Starting any additional threads
	- Responding to poistion and orientation update requests from the remote side
*/
int _tmain(int argc, _TCHAR* argv[])
{
	SharedMem IMUSharedMemory (TEXT("IMUSharedMemory"), false);
	MutexObj IMUSharedMemoryMutex;
	IMUData imuData;

	SharedMem kinectSharedMemory (TEXT("kinectSharedMemory"), false);
	MutexObj kinectSharedMemoryMutex;
	KinectData kinectData;


#ifdef ADEEL_DEBUG
	fstream debugOut("IMUDebug.txt", ios::out);
#endif

	while(!IMUSharedMemory.isValid())
	{
		if(!IMUSharedMemory.Start(0))
		{
			Sleep(500);
		}
	}
		
	while(!IMUSharedMemoryMutex.isValid() && !IMUSharedMemoryMutex.initNamedMutex(TEXT("IMUSharedMemoryMutex"), false))
	{
		Sleep(500);
	}

	while(!kinectSharedMemory.isValid())
	{
		if(!kinectSharedMemory.Start(0))
		{
			Sleep(500);
		}
	}
		
	while(!kinectSharedMemoryMutex.isValid() && !kinectSharedMemoryMutex.initNamedMutex(TEXT("kinectSharedMemoryMutex"), false))
	{
		Sleep(500);
	}

#ifdef ADEEL_DEBUG
	for(int i = 0; i < 500; i++)
	{
		if ( lockMutex(kinectSharedMemoryMutex, INFINITE) ){
			kinectSharedMemory.readBytes((void *)(&kinectData), sizeof(kinectData), 0);
			unlockMutex(kinectSharedMemoryMutex);
			cout << "x = " << kinectData.rightHandX
			     << ", y = " << kinectData.rightHandY
				 << ", z = " << kinectData.rightHandZ << endl;

			/*
			debugOut << "x = " << kinectData.rightHandX
					 << ", y = " << kinectData.rightHandY
					 << ", z = " << kinectData.rightHandZ << endl;
			debugOut.flush();
			*/
		}
		Sleep(50);
	}

	debugOut.close();
#endif

	return 0;









	/*
	NARCS* global;
	try
	{
		//allocate global commBlocks
		unsigned char shareBuffer[BUFF_SIZE*NUM_THREADS];
		for(int i=0; i<NUM_THREADS; i++){
			globCommBlocks[i]._mutex.initMutex();
			globCommBlocks[i]._event.initEvent(false);
			globCommBlocks[i]._size = BUFF_SIZE;
			globCommBlocks[i]._pBuf = shareBuffer+i*BUFF_SIZE;
		}

		printf("Starting threads\n");
		global = new NARCS;
		global->allocate_threads();
		global->start_threads();

		while(1) //loop infinitely
		{
			//read data from threads
			for(int i=0; i<NUM_THREADS; i++)
			{
				if(threadRequired[i])
				{
					orient_data test;
					//wait for new data
					if( waitEvent(globCommBlocks[i]._event, 10) )
					{
						 //get Mutex before reading
						if ( lockMutex(globCommBlocks[i]._mutex, 2) )
						{
							globCommBlocks[i].readSingle(test, 0);
							//release Mutex after reading
							unlockMutex(globCommBlocks[i]._mutex);
							printf("I read roll:%f, pitch:%f, yaw:%f\n",
									test.roll, test.pitch, test.yaw);
						}
					}
				}
			}

			//bring back to life any thread that dies
			global->reincarnate(0);
		}
	}
	catch (int e)
	{
		char * exceptionStr;
		switch (e){
		case UNEXPECTED_ERROR:
			exceptionStr = "Unexpected Error in Win32 calls\n";
			break;

		case INVALID_THREAD_TYPE:
			exceptionStr = "Specified thread type is invalid\n";
			break;

		case FAILED_THREAD_CREATE:
			exceptionStr = "Failed to create thread\n";
			break;

		case INVALID_PARAM_VALUE:
			exceptionStr = "Invalid parameter value specified to a function\n";
			break;

		default:
			exceptionStr = "Unknown int Exception: %d\n";
			break;
		}
		printf("Exception: %s",exceptionStr);
	}
	catch (exception& e)
	{
		printf("Standard exception: %s\n", e.what() );
	}
	catch (...){
		printf("Exception: unknown  type\n");
	}

	//Exit and cleanup Code
	delete global;
	return 0;
	*/
}