#include "StdAfx.h"
#include "Arduino.h"
#include <iostream>


Arduino::Arduino(Thread_type type){
	setType(type);
	SOCKET ArduinoSocket = INVALID_SOCKET;
}

Arduino::~Arduino(void)
{
	if(ArduinoSocket != INVALID_SOCKET)
	{
		closesocket(ArduinoSocket);
	}
	//WSACleanup();
}

using namespace std;
int Arduino::threadMain(void)
{
	char *sendBuffer = NULL;
	char *recvBuffer = NULL;
	int errCode;
	int ForceValue=0;
	int retCode = 0;

	HANDLE hSerial;
	std::string a="COM8";
	hSerial = CreateFile( L"\\\\.\\COM8",
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

	char szBuff[3] = {0}; //read serial buffer
	char szwBuff[1] = {0}; //write serial buffer
	DWORD dwBytesRead = 0;
	char state='r'; //initialize state to motor off
	char *currBuffPtr = NULL;
	unsigned int totalBytesWritten = 0;
	int currBytesWritten = -1;
	while(true){
		if(!ReadFile(hSerial, szBuff, 3, &dwBytesRead, NULL)){
			cout << "Arduino Thread - ReadFile(...) failed." << endl;
			goto FAILURE;
		}
		char check=(char)szBuff[0];
		
		ForceValue = ( int )( ( ( szBuff[2] & 0xff )  << 8 ) | ( szBuff[1] & 0xff ) );
		if(check !='c')
		{
			//cout << "BOO YAKASHA" << endl;
			if(!ReadFile(hSerial, szBuff, 1, &dwBytesRead, NULL)){
				//error occurred. Report to user.
				//cout<<"error\n";
			}
			
		}
		else
		{
			//scale the force reading
			if(ForceValue>1000)
			{
				ForceValue=0;
			}
			else if(ForceValue <= 1000 && ForceValue>699)
			{
				ForceValue=1000-ForceValue;
				ForceValue = ( int )( ( ( (double)(ForceValue) / (double)(300) )* (double)(40) )+ (double)(30) );
			}
			else if(ForceValue<700)
			{
				ForceValue=70;
			}
			// now send the force value to the remote site
			currBuffPtr = (char *)(&ForceValue);
			totalBytesWritten = 0;
			currBytesWritten = -1;
			while(totalBytesWritten < sizeof(ForceValue))
			{
				currBytesWritten = send(ArduinoSocket,
										currBuffPtr,
										sizeof(ForceValue) - totalBytesWritten,
										0);
				if (currBytesWritten == SOCKET_ERROR)
				{
					cout << "Arduino Thread - send(...) failed." << endl;
					goto FAILURE;
				}

				totalBytesWritten += currBytesWritten;
				currBuffPtr += currBytesWritten;
			}

		}
		

		/*
		//get motor values from other side
		recv(ArduinoSocket, recvBuffer, 1, 0);
		szwBuff[0]=recvBuffer[0];
		errCode = WSAGetLastError();
		if( errCode == 0 )
		{
			cout<<"error in receiving motor data"<<endl;
		}

		//write to the serial if state changes
		if(szwBuff[0]!=state)
		{
			if(!WriteFile(hSerial, szwBuff, 1, &dwBytesRead, NULL)){
				//error occurred. Report to user.
				//cout<<"error on write\n";
			}
			cout<<"state changed :"<<szwBuff[0]<<endl;
		}
		state=szwBuff[0]; //change the state
		*/
	
	}

	retCode = 0;
	goto SUCCESS;
	
FAILURE:
	retCode = -1;

SUCCESS:

	CloseHandle(hSerial);
	return retCode;
}