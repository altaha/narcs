// Arduino_Unmanaged.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include <iostream>
#include <string>
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hSerial;
	std::string a="COM25";
	hSerial = CreateFile( L"\\\\.\\COM25",
						GENERIC_READ | GENERIC_WRITE,
						0,
						0,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						0);
	if(hSerial==INVALID_HANDLE_VALUE){
		if(GetLastError()==ERROR_FILE_NOT_FOUND){
		//serial port does not exist. Inform user.
			cout<<"error create"<<endl;
		}
	//some other error occurred. Inform user.
	}

	DCB dcbSerialParams = {0};
	dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams)) {
	//error getting state
	}
	dcbSerialParams.BaudRate=CBR_9600;
	dcbSerialParams.ByteSize=8;
	dcbSerialParams.StopBits=ONESTOPBIT;
	dcbSerialParams.Parity=NOPARITY;
	if(!SetCommState(hSerial, &dcbSerialParams)){
	//error setting serial port state
	}

	COMMTIMEOUTS timeouts={0};
	timeouts.ReadIntervalTimeout=50;
	timeouts.ReadTotalTimeoutConstant=50;
	timeouts.ReadTotalTimeoutMultiplier=10;
	timeouts.WriteTotalTimeoutConstant=50;
	timeouts.WriteTotalTimeoutMultiplier=10;
	if(!SetCommTimeouts(hSerial, &timeouts)){
	//error occureed. Inform user
	}

	char szBuff[2 + 0] = {0};
	DWORD dwBytesRead = 0;
	while(true){
		if(!ReadFile(hSerial, szBuff, 2, &dwBytesRead, NULL)){
		//error occurred. Report to user.
			cout<<"error\n";
		}
		
		int val = ( int )( ( ( szBuff[0] & 0xff )  << 8 ) | ( szBuff[1] & 0xff ) );
		if( val > 1023 )
		{
			cout << "BOO YASHAKA" << endl;
			if(!ReadFile(hSerial, szBuff, 1, &dwBytesRead, NULL)){
				//error occurred. Report to user.
				cout<<"error\n";
			}
			
		}
		cout << "val = " << val << endl;

	}
	
	CloseHandle(hSerial);
	return 0;
}

