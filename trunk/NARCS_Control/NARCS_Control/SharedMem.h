// SharedMem.h

//Class implementation of Shared Memory Object
//Only supports creation of one File view per fileMapObject

#pragma once

#include <Windows.h>

class SharedMem{

public:
	SharedMem(int size, TCHAR* name, bool creator);
	SharedMem(TCHAR* name, bool creator);

	~SharedMem(void);

	//get shared memory from system
	//offset provides an offset where the shared memory view starts from
	bool Start(int offset);

	//Read and write to memory
	//data is pointer to data
	//size is number of bytes to read/write
	//offset is the offset in bytes in the shared memory to read/write to
	bool Write( const LPVOID data, int size, int offset);
	bool Read( LPVOID data, int size, int offset);

	HANDLE GetHandle() {
		return m_Handle;
	}
	TCHAR* GetName() {
		return m_Name;
	}
	LPVOID GetBuffer() {
		return m_Buffer;
	}
	int GetSize() {
		return m_Size;
	}
	bool GetStatus() {
		return m_Status;
	}

private:
	bool CreateOrOpen();
	bool MapView_native(int offset);

	bool m_Creator; //true: creator of shared memory
	TCHAR* m_Name;
	HANDLE m_Handle; //Shared Memory Object handle
	LPVOID m_Buffer; //poitner to start of shared memory
	int	m_Size;
	bool m_Status;	//indicates if a file view was created
					//ie. Can read/write to shared memory
};
