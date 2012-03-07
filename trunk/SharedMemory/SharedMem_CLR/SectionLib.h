// SectionLib.h

#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Microsoft::Win32::SafeHandles;
using namespace System::IO;

namespace SectionLib {
	private ref class SharedMemoryHandle : public CriticalHandleMinusOneIsInvalid {
	protected:
		virtual bool ReleaseHandle() override;

	public:
		SharedMemoryHandle(HANDLE h) {
			handle = (IntPtr)h;
		}

		property virtual bool IsInvalid {
			bool get() override {
				return handle == IntPtr::Zero;
			}
		}

	internal:
		HANDLE GetHandle() {
			return handle.ToPointer();
		}
	};

	public ref class SharedMemory {
	public:
		SharedMemory(int size, String^ name);
		SharedMemory(int size);
		SharedMemory(String^ name);

		Stream^ MapView(int offset, int size);
		void* MapView_native(int offset, int size);

	protected:
		void CreateOrOpen(int size, String^ name);

	private:
		String^ m_Name;
		SharedMemoryHandle^ m_Handle;
	};

	private ref class SharedMemoryStream : public UnmanagedMemoryStream {
	public:
		SharedMemoryStream(void* p, int size) : UnmanagedMemoryStream((unsigned char*)(m_ptr = p), size, size, FileAccess::ReadWrite) {
		}

		~SharedMemoryStream() {
			::UnmapViewOfFile(m_ptr);
			m_ptr = NULL;
		}

	private:
		void* m_ptr;
	};
}
