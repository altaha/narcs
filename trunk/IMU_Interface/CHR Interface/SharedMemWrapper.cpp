#include "SharedMemWrapper.h"
#include "SharedMem.h"
#include "UnmanagedClassHandling.h"

SharedMemWrapper::SharedMemWrapper(int size, wchar_t *name, bool creator)
{
	minitialize();
	sharedMem = new SharedMem(size,
							  name,
							  creator);
}

SharedMemWrapper::SharedMemWrapper(wchar_t *name, bool creator)
{
	minitialize();
	sharedMem = new SharedMem(name,
							  creator);
}

SharedMemWrapper::~SharedMemWrapper(void)
{
	delete sharedMem;
	mterminate();
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