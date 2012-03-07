// This is the main DLL file.

#include "stdafx.h"

#include "SectionLib.h"

using namespace SectionLib;
using namespace msclr::interop;

// SharedMemoryHandle

bool SharedMemoryHandle::ReleaseHandle() {
	return ::CloseHandle(handle.ToPointer()) ? true : false;
}

// SharedMemory

SharedMemory::SharedMemory(int size, String ^name) {
	CreateOrOpen(size, name);
}

SharedMemory::SharedMemory(int size) {
	CreateOrOpen(size, nullptr);
}

SharedMemory::SharedMemory(String^ name) {
	CreateOrOpen(0, name);
}

void SharedMemory::CreateOrOpen(int size, String^ name) {
	this->m_Name = name;
	HANDLE h = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, 
		marshal_context().marshal_as<LPCWSTR, String^>(name));
	m_Handle = gcnew SharedMemoryHandle(h);
	if(m_Handle->IsInvalid)
		throw gcnew InvalidOperationException("Failed to create section object");
}

Stream^ SharedMemory::MapView(int offset, int size) {
	void* p = ::MapViewOfFile(m_Handle->GetHandle(), FILE_MAP_READ | FILE_MAP_WRITE, 0, offset, size);
	if(p == NULL)
		throw gcnew InvalidOperationException("Filed to map view");
	if(size == 0) {
		MEMORY_BASIC_INFORMATION info;
		::VirtualQuery(p, &info, sizeof(info));
		size = info.RegionSize;
	}
	return gcnew SharedMemoryStream(p, size);
}

void* SharedMemory::MapView_native(int offset, int size) {
	void* p = ::MapViewOfFile(m_Handle->GetHandle(), FILE_MAP_READ | FILE_MAP_WRITE, 0, offset, size);
	if(p == NULL)
		throw gcnew InvalidOperationException("Filed to map view");
	if(size == 0) {
		MEMORY_BASIC_INFORMATION info;
		::VirtualQuery(p, &info, sizeof(info));
		size = info.RegionSize;
	}
	return p;
}
