#include <iostream>
#include <stdio.h>
#include <Windows.h>

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

	sharedMemory.Write((void*)&test_data, STRUCT_SIZE, 5);

	//terminate
	printf("Creator process done\n");
	getchar();
	return 0;
}	
