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
		global = new NARCS;

		global->allocate_threads();

		global->start_threads();

		printf("Main thread\n");
		getchar();
		return 0;
	}
	catch (int e)
	{
		printf("Exception\n");
		switch (e){
		case INVALID_THREAD_TYPE:
			printf("Specified thread type is invalid\n");
			break;

		default:
			printf("Unknown int Exception: %d\n",e);
			break;
		}
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
	return 0;
}

