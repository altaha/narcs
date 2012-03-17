#include <windows.h>
#include <_vcclrit.h>

using System::Console;
using System::Exception;

int minitialize() { 
      int retval = 0; 
      try 
      { 
            __crt_dll_initialize(); 
      } 
      catch(System::Exception^ e) 
      { 
            Console::WriteLine(e); 
            retval = 1; 
      } 
      return retval; 
} 

int mterminate() { 
      int retval = 0; 
      try 
      { 
            __crt_dll_terminate(); 
      } 
      catch(System::Exception^ e) 
      { 
            Console::WriteLine(e); 
            retval = 1; 
      } 
      return retval; 
} 