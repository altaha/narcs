// NARCS_Control.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "NARCS.h"
#include "ThreadObj.h"
#include "SynchObjs.h"
#include "IMU.h"

#define BUFF_SIZE 1024

thrdCommBlock globCommBlocks[NUM_THREADS];

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
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

		printf("Main thread starting\n");

		global = new NARCS;

		global->allocate_threads();

		global->start_threads();

		//bring to life anythread that dies. This should be in a while loop
		global->reincarnate(100000);

		getchar();
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
}

