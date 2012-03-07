#include "SharedMem.h"
#include <stdio.h>

//SharedMem
SharedMem::SharedMem(int size, TCHAR* name, bool creator)
	: m_Handle(NULL), m_Name(NULL), m_Size(0), m_Status(false), m_Buffer(NULL)
{
	CreateOrOpen(size, name, creator);
}

SharedMem::SharedMem(int size, bool creator)
	: m_Handle(NULL), m_Name(NULL), m_Size(0), m_Status(false), m_Buffer(NULL)
{
	CreateOrOpen(size, NULL, creator);
}

SharedMem::SharedMem(TCHAR* name, bool creator)
	: m_Handle(NULL), m_Name(NULL), m_Size(0), m_Status(false), m_Buffer(NULL)
{
	CreateOrOpen(0, name, creator);
}

SharedMem::~SharedMem(void)
{
	if (this->m_Status == true){
		UnmapViewOfFile(this->m_Buffer);
		CloseHandle(this->m_Handle);
	}
}

void SharedMem::CreateOrOpen(int size, TCHAR* name, bool creator)
{
	this->m_Name = name;
	this->m_Size = size;

	if(creator)
	{
		this->m_Handle = CreateFileMapping(
			INVALID_HANDLE_VALUE, //shared memory instead of mem-mapped file
			NULL, //default security (cannot be inherited by child processes)
			PAGE_READWRITE, //Allow read/write access
			0,
			size,
			name );	//Name of shared memory
		if (this->m_Handle == NULL){
			printf("Could not create shared memory object (%d)\n", GetLastError());
			throw 123; //TODO: link to a defined exception
		}
	}
	else{
		this->m_Handle = OpenFileMapping(
                   FILE_MAP_ALL_ACCESS,   // read/write access
                   FALSE,                 // do not inherit the name
                   name);        // name of shared memory object
		if (this->m_Handle == NULL){
			printf("Could not Open shared memory object (%d)\n", GetLastError());
			throw 123; //TODO: link to a defined exception
		}
	}
	return;
}

bool SharedMem::MapView_native(int offset, int size)
{
	LPVOID pBuf = MapViewOfFile(this->m_Handle, FILE_MAP_ALL_ACCESS, 0, offset, size);
	if(pBuf == NULL){
		printf("Could not map view of shared memory file (%d).\n", GetLastError());
		CloseHandle(this->m_Handle);
		throw 223; //TODO: link to a defined exception
		return false; //not getting here it seems
	}
	if(size == 0) {
		MEMORY_BASIC_INFORMATION info;
		VirtualQuery(pBuf, &info, sizeof(info));
		size = info.RegionSize;
	}
	this->m_Status = true;
	this->m_Size = size;
	this->m_Buffer = pBuf;
	return true;
}

bool SharedMem::Write( const LPVOID data, int size, int offset)
{
	if (!this->m_Status && (offset+size) > this->m_Size && size<=0)
		return false;
	else{
		memcpy(this->m_Buffer, data, size );
		return true;
	}
}
bool SharedMem::Read( LPVOID data, int size, int offset)
{
	if (!this->m_Status && (offset+size) > this->m_Size && size<=0)
		return false;
	else{
		memcpy(data, this->m_Buffer, size );
		return true;
	}
}

