#include <iostream>
#include <stdio.h>
#include <Windows.h>

#include "SharedMem.h"

using namespace std;

typedef struct joint_data{
	double x;
	double y;
	double z;
} joint_data;

int STRUCT_SIZE = sizeof(joint_data);

#define BUF_SIZE 256

int main(){
	
	printf("Opening Named Shared Memory\n");

	TCHAR sharedMemName[]=TEXT("testSharedMemory");

	SharedMem sharedMemory = SharedMem::SharedMem(sharedMemName, false);

	if( !sharedMemory.Start(0) ){
		printf("Failed to open memory\n");
	}

	printf("Size of struct: %d\n", STRUCT_SIZE);
	printf("Size of memory is: %d\n", sharedMemory.GetSize() );


	////Event and Mutex Objects
	HANDLE ghWriteEvent = OpenEvent(
        SYNCHRONIZE,		//Standard access
        FALSE,				//Child processes dont inherit
        TEXT("SharedMemWriteEvent")  // object name
        );
	if (ghWriteEvent == NULL) 
    { 
        printf("CreateEvent failed (%d)\n", GetLastError());
        return 1;
    }

	HANDLE ghMutex = OpenMutex(
        SYNCHRONIZE,       //Standard access
        FALSE,             //Child processes dont inherit
        TEXT("SharedMemMutex")  // object name
		);
    if (ghMutex == NULL) 
    {
        printf("CreateMutex error: %d\n", GetLastError());
        return 1;
    }
	////////////////////////////////
	int loop_count = 0;
	while(loop_count < 1000)
	{
		HANDLE lpHandles[2];
		lpHandles[0] = ghWriteEvent;
		//lpHandles[0] = ghMutex;
		lpHandles[1] = ghMutex;
		DWORD rc = WaitForMultipleObjects(2, lpHandles, TRUE, 20000);
	//	DWORD rc = WaitForSingleObject(ghMutex, 10000);
		if(rc != WAIT_OBJECT_0){
			printf("WaitForMultipleObjects error: %d\n", GetLastError());
			return 1;
		}

		joint_data read_test;
		if( sharedMemory.Read( (void*)&read_test, STRUCT_SIZE, 5) ){
			printf("I read x:%f, y:%f, z:%f\n", read_test.x, read_test.y, read_test.z);
		}

		if( !ReleaseMutex( ghMutex) )
		{
			printf("ReleaseMutex failed (%d)\n", GetLastError());
			return 1;
		}
		loop_count++;
		printf("Consumer Loop: %d\n",loop_count);
	}

	wprintf(sharedMemory.GetName());

	//terminate
	printf("Process 2 done\n");
	getchar();
	CloseHandle(ghWriteEvent);
	CloseHandle(ghMutex);
	return 0;
}
