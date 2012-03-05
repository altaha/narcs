// NARCS_Control.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "NARCS.h"
#include "ThreadObj.h"
#include "IMU.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	NARCS* global;
	try
	{
		printf("Main thread starting\n");

		global = new NARCS;

		global->allocate_threads();

		global->start_threads();

		global->reincarnate(2000);

		getchar();
	}
	catch (int e)
	{
		char * exceptionStr;
		switch (e){
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
	printf("Exiting program\n");
	delete global;
	getchar();
	return 0;
}

