#pragma once

#include "stdafx.h"

/////////////////////////////////
class MutexObj
{
public:
	MutexObj(void);
	~MutexObj(void);

	bool initMutex(void);
	//create named mutexes. Used for inter-process sharing
	bool initNamedMutex( TCHAR* name, bool create);

	HANDLE getHandle(){
		return _handle;
	}
	bool isValid(void)
	{
		return _valid;
	}

private:
	HANDLE _handle; //handle to underlying Win32 Mutex Object
	bool _valid; //indicates whether Mutex has been successfully created
};

/////////////////////////////////
class EventObj
{
public:
	EventObj(void);
	~EventObj(void);

	bool initEvent(bool manual);
	//create named events. Used for inter-process sharing
	bool initNamedEvent(TCHAR* name, bool creator, bool manual);

	HANDLE getHandle(){
		return _handle;
	}
	bool isValid(void){
		return _valid;
	}
	bool isManual(void){
		return _manual;
	}

private:
	HANDLE _handle; //handle to underlying Win32 Event Object
	bool _valid; //indicates whether event has been successfully created
	bool _manual;	//indicate manually reset event
};

/////////////////////////////////
//Functions to operate on Mutexs and Events
/////////////////////////////////

//use INFINITE for infinite blocking waits
bool lockMutex(MutexObj& i_mutex, unsigned int timeout_ms);
void unlockMutex(MutexObj& i_mutex);

void setEvent(EventObj& i_event);
bool waitEvent(EventObj& i_event, unsigned int timeout_ms);
//reset only for events requiring manual reset (can check with isManual)
void resetEvent(EventObj& i_event);

bool waitEventAndMutex(EventObj& i_event, MutexObj& i_mutex, unsigned int timeout_ms);




/////////////////////////////////
//struct used for sharing data between threads
typedef struct thrdCommBlock
{
	unsigned char*	_pBuf;	//pointer to the start of buffer
	unsigned int	_size;	//size of buffer in bytes
	MutexObj		_mutex;//Mutex for synchronizing read/write
	EventObj		_event;//Event to indicate new data

	//Read and write to memory
	//size is number of bytes to read/write
	//offset is the offset in bytes in the shared memory to read/write to
	void writeBytes( const void* data, unsigned int size, unsigned int offset);
	void readBytes( void* outData, unsigned int size, unsigned int offset);
	
	template<typename Type> void writeSingle(const Type& data, unsigned int offset);
	template<typename Type> void readSingle(Type& outData, unsigned int offset);
	
	template<typename Type> void writeArray(const Type data[], unsigned int n, unsigned int offset);
	template<typename Type> void readArray(Type outData[], unsigned int n, unsigned int offset);

}thrdCommBlock;


/////////////////////////////////
//Template functions
template<typename Type> void thrdCommBlock::writeSingle(const Type& data, unsigned int offset)
{
	if ( sizeof(Type)+offset > this->_size ){
		printf("thrdCommBlock writeSingle Error: overflows\n");
		return;
	}
	else{
		memcpy( this->_pBuf+offset, (void*) &(data), sizeof(Type) );
	}
}
template<typename Type> void thrdCommBlock::readSingle(Type& outData, unsigned int offset)
{
	if ( sizeof(Type)+offset > this->_size ){
		printf("thrdCommBlock readSingle Error: overflows\n");
		return;
	}
	else{
		memcpy( (void*) &(outData), this->_pBuf+offset, sizeof(Type) );
	}
}
	
template<typename Type> void thrdCommBlock::writeArray(const Type data[], unsigned int n, unsigned int offset)
{
	if ( sizeof(Type)*n+offset > this->_size ){
		printf("thrdCommBlock writeArray Error: overflows\n");
		return;
	}
	else{
		memcpy( this->_pBuf+offset, (void*)(data), sizeof(Type)*n );
	}
}
template<typename Type> void thrdCommBlock::readArray(Type outData[], unsigned int n, unsigned int offset)
{
	if ( sizeof(Type)*n+offset > this->_size ){
		printf("thrdCommBlock readArray Error: overflows\n");
		return;
	}
	else{
		memcpy( (void*)(outData), this->_pBuf+offset, sizeof(Type)*n );
	}
}
