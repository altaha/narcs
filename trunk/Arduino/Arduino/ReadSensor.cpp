// ReadSensor.cpp : main project file.

#include "stdafx.h"
#using <System.dll>

using namespace System;
using namespace System;
using namespace System::IO::Ports;

int main(array<System::String ^> ^args)
{
	String^ portName="COM25";
	int baudRate=9600;
	SerialPort^ arduino;
	arduino = gcnew SerialPort(portName, baudRate);
	
	arduino->Open();
	while(true)
	{
		
		String^ value = arduino->ReadLine();
		try{
			int intvalue=Convert::ToInt32(value,10);
		}
		catch(FormatException ^e){} catch(ArgumentOutOfRangeException ^e){}
		Console::WriteLine(value);
		
	}
	   
    return 0;
}
