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
	int ForceValue=20;

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

	char szBuff[3] = {0}; //read serial buffer
	char szwBuff[1] = {0}; //write serial buffer
	DWORD dwBytesRead = 0;
	char state='r'; //initialize state to motor off
	while(true){
		if(!ReadFile(hSerial, szBuff, 3, &dwBytesRead, NULL)){
		//error occurred. Report to user.
			//cout<<"error\n";
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
		//scale the force reading
		if(ForceValue>1000)
			ForceValue=0;
		else if(ForceValue<1000 && ForceValue>699)
		{
			ForceValue=1000-ForceValue;
			ForceValue=( (ForceValue/300)*60 )+30;
		}
		else if(ForceValue<700)
			ForceValue=90;

		//now send the force value to the remote site
		sendBuffer=(char *)(&ForceValue);
		int currBytesWritten = send(ArduinoSocket,
										sendBuffer,
										sizeof(ForceValue),
										0);
		if (currBytesWritten == SOCKET_ERROR)
		{
			cout << "_tmain(...) - send(...) failed." << endl;
				
		}
		

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
	
	
	}
	

	CloseHandle(hSerial);
	return EXIT_SUCCESS;
}