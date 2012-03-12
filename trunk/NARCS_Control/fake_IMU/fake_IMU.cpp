#include "stdafx.h"

#include "SharedMem.h"
#include "SynchObjs.h"

#define BUF_SIZE 256

typedef struct orient_data{
	double roll;
	double pitch;
	double yaw;
} orient_data;

int STRUCT_SIZE = sizeof(orient_data);

int main(){
	
	orient_data test_data = { 0.5, 0.19, 3.2};

	printf("Creating Named Shared Memory\n");
	SharedMem sharedMemory = SharedMem(STRUCT_SIZE*10, TEXT("testSharedMemory"), true);
	sharedMemory.Start(0);

	//Create Event and Mutex Objects for synchronization
	EventObj ghWriteEvent = EventObj();
	ghWriteEvent.initNamedEvent(TEXT("SharedMemEvent"), true, false);

	MutexObj ghMutex = MutexObj();
	ghMutex.initNamedMutex(TEXT("SharedMemMutex"), true);
	////////////////////

	int loop_count = 0;
	printf("Hit a Key to start cycle\n");
	getchar();
	while(loop_count < 100)
	{
		//Steps:
		//1. Acquire Mutex
		//2. Write to shared memory
		//3. Set Event
		//4. Release Mutex

		//lockMutex(ghMutex, INFINITE) ) //Wait infinitely
		if( lockMutex(ghMutex, 1000) ) //returns true when Mutex is obtained
		{
			printf("I wrote roll:%f, pitch:%f, yaw:%f\n", test_data.roll, test_data.pitch, test_data.yaw);
			sharedMemory.writeSingle(test_data, 0);
			//sharedMemory.writeArray(&test_data, 1, 0);
			//sharedMemory.writeBytes((void*)&test_data, sizeof(test_data), 0);
			
			setEvent(ghWriteEvent);//notify that new data has been written

			unlockMutex(ghMutex);
		}
		
		loop_count++;
		printf("Producer Loop: %d\n",loop_count);
		test_data.roll += 1;
		test_data.pitch += 1;
		test_data.yaw += 1;
		Sleep(100);
	}

	//terminate
	printf("Fake IMU process done\n");
	getchar();
	return 0;
}	
