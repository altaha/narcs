#include "StdAfx.h"

#include "SharedMem.h"

//SharedMem
SharedMem::SharedMem(int size, TCHAR* name, bool creator)
	: m_Handle(NULL), m_Status(false), m_Buffer(NULL)
{
	this->m_Name = name;
	this->m_Size = size;
	this->m_Creator = creator;
}

SharedMem::SharedMem(TCHAR* name, bool creator)
	: m_Handle(NULL), m_Status(false), m_Buffer(NULL)
{
	this->m_Name = name;
	this->m_Size = 0;
	this->m_Creator = creator;
}

SharedMem::~SharedMem(void)
{
	if (this->m_Status == true){
		UnmapViewOfFile(this->m_Buffer);
		CloseHandle(this->m_Handle);
	}
}

bool SharedMem::Start(int offset)
{
	if(!this->CreateOrOpen()){
		return false;
	}
	return this->MapView_native(offset);
}

bool SharedMem::CreateOrOpen()
{
	if(this->m_Creator)
	{
		this->m_Handle = CreateFileMapping(
			INVALID_HANDLE_VALUE, //shared memory instead of mem-mapped file
			NULL, //default security (cannot be inherited by child processes)
			PAGE_READWRITE, //Allow read/write access
			0,
			this->m_Size,
			this->m_Name );	//Name of shared memory
		if (this->m_Handle == NULL){
			printf("Could not create shared memory object: (%d)\n", GetLastError());
			throw 123; //TODO: link to a defined exception
			return false;
		}
	}
	else{
		this->m_Handle = OpenFileMapping(
                   FILE_MAP_ALL_ACCESS,   // read/write access
                   FALSE,                 // do not inherit the name
                   this->m_Name);        // name of shared memory object
		if (this->m_Handle == NULL){
			printf("Could not Open shared memory object: (%d)\n", GetLastError());
			return false; //TODO: link to a defined exception
		}
	}
	return true;
}

bool SharedMem::MapView_native(int offset)
{
	LPVOID pBuf = MapViewOfFile(this->m_Handle, FILE_MAP_ALL_ACCESS, 0, offset, this->m_Size);
	if(pBuf == NULL){
		printf("Could not map view of shared memory file: (%d).\n", GetLastError());
		CloseHandle(this->m_Handle);
		throw 223; //TODO: link to a defined exception
		return false; //not getting here it seems
	}
	if(this->m_Size == 0) {
		MEMORY_BASIC_INFORMATION info;
		VirtualQuery(pBuf, &info, sizeof(info));
		this->m_Size = info.RegionSize;
	}
	this->m_Status = true;
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
	if (!this->m_Status || (offset+size) > this->m_Size || size<=0)
		return false;
	else{
		memcpy(data, this->m_Buffer, size );
		return true;
	}
}

