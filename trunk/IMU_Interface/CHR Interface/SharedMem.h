// SharedMem.h

//Class implementation of Shared Memory Object
//Only supports creation of one File view per fileMapObject

#pragma once

#include <stdio.h>
#include <tchar.h>

#include <comdef.h>
#include <Windows.h>

class SharedMem{

public:
	SharedMem(int size, TCHAR* name, bool creator);
	SharedMem(TCHAR* name, bool creator);

	~SharedMem(void);

	//get shared memory from system
	//offset provides an offset where the shared memory view starts from
	bool Start(int offset);

	TCHAR* GetName() {
		return m_Name;
	}
	int GetSize() {
		return m_Size;
	}
	bool isValid() {
		return m_Status;
	}

	//Read and write to memory
	//data is pointer to data
	//size is number of bytes to read/write
	//offset is the offset in bytes in the shared memory to read/write to
	void writeBytes( const void* data, unsigned int size, unsigned int offset);
	void readBytes( void* outData, unsigned int size, unsigned int offset);

private:
	bool CreateOrOpen();
	bool MapView_native(int offset);

	bool m_Creator; //true: creator of shared memory
	TCHAR* m_Name;
	HANDLE m_Handle; //Shared Memory Object handle
	LPVOID m_Buffer; //poitner to start of shared memory
	unsigned int m_Size;
	bool m_Status;	//indicates if a file view was created
					//ie. Can read/write to shared memory
};