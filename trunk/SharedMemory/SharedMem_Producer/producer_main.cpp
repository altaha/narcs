#include <iostream>
#include <stdio.h>
#include <Windows.h>

using namespace std;

#define BUF_SIZE 256

int main(){
	
	printf("Creating Named Shared Memory\n");

	HANDLE fileMapObj;
	TCHAR sharedMemName[]=TEXT("testSharedMemory");

	//Create file mapping object
	fileMapObj = CreateFileMapping(
			INVALID_HANDLE_VALUE, //shared memory instead of mem-mapped file
			NULL, //default security (cannot be inherited by child processes)
			PAGE_READWRITE, //Allow read/write access
			0,
			BUF_SIZE,
			sharedMemName );	//Name of shared memory
	if (fileMapObj == NULL){
		printf("Could not create file mapping object (%d)\n", GetLastError());
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
	//////Done creating named shared memory


	//Write to shared memory
	char testMem[] = "Here is my message to other processes";
	memcpy(pBuf, testMem, (strlen(testMem) * sizeof(char)) );

	//terminate
	printf("Creator process done\n");
	getchar();
	UnmapViewOfFile(pBuf);
	CloseHandle(fileMapObj);
	return 0;
}
