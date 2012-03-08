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

	joint_data read_test;
	if( sharedMemory.Read( (void*)&read_test, STRUCT_SIZE, 5) ){
		printf("I read x:%f, y:%f, z:%f\n", read_test.x, read_test.y, read_test.z);
	}

	wprintf(sharedMemory.GetName());

	//terminate
	printf("Process 2 done\n");
	getchar();
	return 0;
}
