#include <iostream>
#include <stdio.h>
#include <Windows.h>

using namespace std;

#define BUF_SIZE 256

int main(){
	
	printf("Opening Named Shared Memory\n");

	HANDLE fileMapObj;
	//TCHAR sharedMemName[]=TEXT("testSharedMemory");
	TCHAR sharedMemName[]=TEXT("TestSharedMemCLI");

	//Create file mapping object
	fileMapObj = OpenFileMapping(
                   FILE_MAP_ALL_ACCESS,   // read/write access
                   FALSE,                 // do not inherit the name
                   sharedMemName);        // name of mapping object
	if (fileMapObj == NULL){
		printf("Could not open file mapping object (%d)\n", GetLastError());
		return 1;
	}

	//Create file view
	void* pBuf; //pointer to shared memory
	pBuf = MapViewOfFile(fileMapObj,   // handle to map object
					FILE_MAP_ALL_ACCESS, // read/write permission
					0,
					0,
					BUF_SIZE);
	if (pBuf == NULL)
	{
		printf("Could not map view of file (%d).\n", GetLastError());
		CloseHandle(fileMapObj);
		return 1;
	}
	/////Done opening named shared memory



	//Read from shared memory
	printf("I read %s\n",pBuf);

	//terminate
	printf("Process 2 done\n");
	getchar();
	UnmapViewOfFile(pBuf);
	CloseHandle(fileMapObj);
	return 0;
}
