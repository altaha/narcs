#include "StdAfx.h"
#include "IMU.h"


IMU::IMU(Thread_type type){
	setType(type);
}

IMU::~IMU(void)
{
	delete this->_sharedMem;
}

int IMU::threadMain(void)
{
	printf("This is a fake IMU Thread\n");
	printf("Just showing IPC with external process, and communicating with main thread in program\n");
	orient_data test_read;
	try{
		//Objects used for inter-process communication
		SharedMem sharedMemory = SharedMem(TEXT("testSharedMemory"), false);
		EventObj ipcEvent = EventObj();
		MutexObj ipcMutex = MutexObj();
		//Object used for communication within threads
		this->commBlock = &(globCommBlocks[IMU_THREAD]);
		
		int loop_count = 0;
		while( !this->_stopThread )
		{
			//Open objects by name (objects created by other process)
			//retry if open fails (indicates other process not started yet)
			if( !sharedMemory.isValid() && !sharedMemory.Start(0) ){
				//wait one second before retrying
				Sleep(1000);
				continue;
			}
			if( !ipcEvent.isValid() && !ipcEvent.initNamedEvent(TEXT("SharedMemEvent"), false, false) ){
				Sleep(1000);
				continue;
			}
			if( !ipcMutex.isValid() && !ipcMutex.initNamedMutex(TEXT("SharedMemMutex"), false) ){
				Sleep(1000);
				continue;
			}

			//beyond this we can safely read from other process
			if( waitEvent(ipcEvent, 1000) ){ //wait for new data
				if ( lockMutex(ipcMutex, 500) ){
					sharedMemory.readSingle(test_read, 0);
					//sharedMemory.readArray(&test_read, 1, 0);
					//sharedMemory.readBytes((void*)&test_read, sizeof(test_read), 0);
					unlockMutex(ipcMutex);
					printf("loop %d, I read roll:%f, pitch:%f, yaw:%f\n", loop_count, test_read.roll, test_read.pitch, test_read.yaw);
				}
				loop_count++;
			}
			if(loop_count>50)
				break;
		}
		//exiting
		printf("IMU OUT\n");
	}
	catch(...){
		//Don't need anything special here. Since main thread is going to reincarnate any terminated threads
		printf("Exception in IMU thread (#: %ul), error:%ul\tExiting thread\n", GetCurrentThreadId(), GetLastError());
	}
	this->_active = false;
	return EXIT_SUCCESS;
}