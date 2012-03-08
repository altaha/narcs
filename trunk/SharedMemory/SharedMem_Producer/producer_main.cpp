#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <WinBase.h>

#include "SharedMem.h"

using namespace std;

#define BUF_SIZE 256

typedef struct joint_data{
	double x;
	double y;
	double z;
} joint_data;

int STRUCT_SIZE = sizeof(joint_data);

int main(){
	
	printf("Creating Named Shared Memory\n");

	TCHAR sharedMemName[]=TEXT("testSharedMemory");

	SharedMem sharedMemory = SharedMem::SharedMem(STRUCT_SIZE*10, sharedMemName, true);

	sharedMemory.Start(0);

	joint_data test_data = { 0.5, 0.19, 3.2};

	//Create Event and Mutex Objects for synchronization
	HANDLE ghWriteEvent = CreateEvent( 
        NULL,               // default security attributes
        FALSE,               // automatic-reset event
        FALSE,              // initial state is nonsignaled
        TEXT("SharedMemWriteEvent")  // object name
        );
	if (ghWriteEvent == NULL) 
    { 
        printf("CreateEvent failed (%d)\n", GetLastError());
        return 1;
    }

	HANDLE ghMutex = CreateMutex( 
        NULL,              // default security attributes
        FALSE,             // initially owned
        TEXT("SharedMemMutex")  // object name
		);
    if (ghMutex == NULL) 
    {
        printf("CreateMutex error: %d\n", GetLastError());
        return 1;
    }
	////////////////////

	int loop_count = 0;
	printf("Hit a Key to start cycle\n");
	getchar();
	while(loop_count < 1000)
	{
		DWORD dwWaitResult = WaitForSingleObject( 
				ghMutex,    // handle to mutex
				INFINITE);  // no time-out interval
		switch (dwWaitResult) 
		{
			// The thread got ownership of the mutex
			case WAIT_OBJECT_0: 
				printf("Acquired Mutex!!!!!\n");
				sharedMemory.Write((void*)&test_data, STRUCT_SIZE, 5);
				break; 

			// The thread got ownership of an abandoned mutex
			// The database is in an indeterminate state
			default: 
				break; 
		}
		if (! SetEvent(ghWriteEvent) ) 
		{
			printf("SetEvent failed (%d)\n", GetLastError());
			return 1;
		}
		if( !ReleaseMutex( ghMutex) )
		{
			printf("ReleaseMutex failed (%d)\n", GetLastError());
			return 1;
		}
		loop_count++;
		printf("Producer Loop: %d\n",loop_count);
		test_data.x += 1;
		test_data.y += 1;
		test_data.z += 1;
		Sleep(50);
	}

	//terminate
	printf("Creator process done\n");
	getchar();
	CloseHandle(ghWriteEvent);
	CloseHandle(ghMutex);
	return 0;
}	
