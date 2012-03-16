#include "SharedMemWrapper.h"
#include "SharedMem.h"


SharedMemWrapper::SharedMemWrapper(int size, TCHAR* name, bool creator) : sharedMem(new SharedMem(size, name, creator))
{
}

SharedMemWrapper::SharedMemWrapper(TCHAR* name, bool creator) : sharedMem(new SharedMem(name, creator))
{
}

SharedMemWrapper::~SharedMemWrapper(void)
{
	delete sharedMem;
}

bool SharedMemWrapper::Start(int offset)
{
	return sharedMem->Start(offset);
}

void SharedMemWrapper::writeBytes(const void* data, unsigned int size, unsigned int offset)
{
	sharedMem->writeBytes(data, size, offset);
}

void SharedMemWrapper::readBytes(void* outData, unsigned int size, unsigned int offset)
{
	sharedMem->readBytes(outData, size, offset);
}