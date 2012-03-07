// SharedMem_CLR.cpp : main project file.

#include "stdafx.h"
#include "SectionLib.h"
#include <stdio.h>

using namespace System;
using namespace SectionLib;

#define BUF_SIZE 256

int main(array<System::String ^> ^args)
{
    Console::WriteLine(L"Hello World");

	SharedMemory^ sharedMem = gcnew SharedMemory(BUF_SIZE, "TestSharedMemCLI");

	void* pBuf; //pointer to shared memory
	pBuf = sharedMem->MapView_native(0,BUF_SIZE);

	//Write to shared memory
	char testMem[] = "Here is my message to other processes";
	memcpy(pBuf, testMem, (strlen(testMem) * sizeof(char)) );

	//terminate
	printf("Creator process done\n");
	getchar();
	//UnmapViewOfFile(pBuf);
	//CloseHandle(fileMapObj);

    return 0;
}
