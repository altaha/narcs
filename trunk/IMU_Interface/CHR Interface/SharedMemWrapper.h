#pragma once


#include <tchar.h>


extern class SharedMem;


class SharedMemWrapper
{
public:
	SharedMemWrapper(int size, TCHAR* name, bool creator);
	SharedMemWrapper(TCHAR* name, bool creator);

	~SharedMemWrapper(void);

	//get shared memory from system
	//offset provides an offset where the shared memory view starts from
	bool Start(int offset);
	
	//Read and write to memory
	//data is pointer to data
	//size is number of bytes to read/write
	//offset is the offset in bytes in the shared memory to read/write to
	void writeBytes(const void* data, unsigned int size, unsigned int offset);
	void readBytes(void* outData, unsigned int size, unsigned int offset);

private:
	SharedMem *sharedMem;
};