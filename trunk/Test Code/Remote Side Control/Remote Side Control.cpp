#include <afxwin.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <MMSystem.h>


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#import "C:\Program Files\CRS Robotics\ActiveRobot\ActiveRobot.dll"


#define DEFAULT_PORT  "62009"
#define ARM_SPEED  80
#define POISTION_UPDATE_BUFFER_LENGTH  12
//#define ADEEL_DEBUG

#ifdef ADEEL_DEBUG
#include <fstream>
#endif


using namespace ACTIVEROBOTLib;
using namespace std;


int main() {
	if FAILED(CoInitialize(NULL))
	{
		AfxMessageBox("CoInitialize() failed.");
		exit(1);
	}

	ICRSRobotPtr Robot =
					ICRSRobotPtr(__uuidof(CRSRobot));
	try
	{
		Robot->ControlGet();
		Robot->PutSpeed(ARM_SPEED);
		Robot->PutBlendMotion(-1);
		Robot->Ready();
		Robot->Finish(ftTight);
	}
	catch (_com_error MyError)
	{
		char WorkString[255];
		sprintf(WorkString, "The following error occurred during initialization --\n%s", (LPCTSTR) MyError.Description());
		AfxMessageBox(WorkString);
		Robot->ControlRelease();
		exit(1);
	}

	ICRSLocationPtr readyPosition = Robot->GetWorldLocation(
														ptActual);
	const float readyPositionX = readyPosition->Getx(),
		        readyPositionY = readyPosition->Gety(),
		        readyPositionZ = readyPosition->Getz();

	ICRSLocationPtr endEffectorPosition = 
							ICRSLocationPtr(__uuidof(CRSLocation));

	try
	{
		locFlags flags = readyPosition->GetFlags();
		flags = (locFlags)(flags | locBase);
		endEffectorPosition->PutFlags(flags);
		endEffectorPosition->PutClass(readyPosition->GetClass());
		endEffectorPosition->PutRobotType(locRCA255);
	}
	catch (_com_error MyError)
	{
		char WorkString[255];
		sprintf(WorkString, "The following error occurred during initialization --\n%s", (LPCTSTR) MyError.Description());
		AfxMessageBox(WorkString);
		Robot->ControlRelease();
		exit(1);
	}
	

    int iResult, iSendResult;

	WSADATA wsaData;	//to inialize Windows socket dll
	struct addrinfo *result=NULL, *ptr=NULL, hints;
	SOCKET ListenSocket = INVALID_SOCKET, ClientSocket = INVALID_SOCKET;


	//Initialize Winscok
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult) {
		printf("WSAStartup failed: %d\n", iResult);
		Robot->ControlRelease();
		cin.get();
		return -1;
	}

	//Creating socker IP address
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	//resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult) {
		printf("getaddrinfofailed: %d\n", iResult);
		WSACleanup();
		Robot->ControlRelease();
		cin.get();
		return -1;
	}
	
	//Create a SOCKET for the server to listen for client connections
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		Robot->ControlRelease();
		cin.get();
		return -1;
	}

	//Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		Robot->ControlRelease();
		cin.get();
		return -1;
	}

	freeaddrinfo(result);	// no longer need the address information

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		Robot->ControlRelease();
		cin.get();
		return -1;
	}
	
	printf("All good. Waiting for request\n");
	//Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		Robot->ControlRelease();
		cin.get();
		return -1;
	}

	printf("Accept succeeded\n\n");
	

	
	char position_update_buffer[POISTION_UPDATE_BUFFER_LENGTH];
	int curr_bytes_read = -1;
	unsigned int total_bytes_read = 0;
	char *curr_buffer_ptr = position_update_buffer;
	float endEffectorX,
		  endEffectorY,
		  endEffectorZ;
	
#ifdef ADEEL_DEBUG
	int moveStartTime;
	fstream debugOut("debug.txt", ios::out);
#endif
	while(true)
	{
		// Read main message contents
		curr_bytes_read = -1;
		total_bytes_read = 0;
		curr_buffer_ptr = position_update_buffer;
		while(total_bytes_read < POISTION_UPDATE_BUFFER_LENGTH)
		{
			curr_bytes_read = recv(ClientSocket,
								   curr_buffer_ptr,
								   POISTION_UPDATE_BUFFER_LENGTH - total_bytes_read,
								   0);
			if (curr_bytes_read == 0)
			{
				printf("Connection closing...\n");
				closesocket(ClientSocket);
				WSACleanup();
				Robot->ControlRelease();
				cin.get();
#ifdef ADEEL_DEBUG
				debugOut.close();
#endif
				return 0;
			}
			else if (curr_bytes_read < 0)
			{
				printf("recv failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				Robot->ControlRelease();
				cin.get();
#ifdef ADEEL_DEBUG
				debugOut.close();
#endif
				return -1;
			}

			total_bytes_read += curr_bytes_read;
			curr_buffer_ptr += curr_bytes_read;
		}

		memcpy_s( (void *)(&endEffectorX),
				  sizeof(endEffectorX),
			      (const void *)(position_update_buffer + 8),
				  4);
		memcpy_s( (void *)(&endEffectorY),
				  sizeof(endEffectorY),
				  (const void *)(position_update_buffer),
				  4);
		memcpy_s( (void *)(&endEffectorZ),
				  sizeof(endEffectorZ),
				  (const void *)(position_update_buffer + 4),
				  4);

		endEffectorX += readyPositionX;
		endEffectorY = endEffectorY * (-1);
		endEffectorY += readyPositionY;
		endEffectorZ += readyPositionZ;

		try
		{
			endEffectorPosition->Putx(endEffectorX);
			endEffectorPosition->Puty(endEffectorY);
			endEffectorPosition->Putz(endEffectorZ);
			//Robot->Stop();
#ifdef ADEEL_DEBUG
			moveStartTime = timeGetTime();
#endif
			Robot->Stop();
			Robot->Move(endEffectorPosition);
			//Robot->Finish(ftLoose);
#ifdef ADEEL_DEBUG
			debugOut << timeGetTime() - moveStartTime << endl;
			debugOut.flush();
#endif
		}
		catch (_com_error MyError)
		{
			char WorkString[255];
			sprintf(WorkString, "The following error occurred during initialization --\n%s", (LPCTSTR) MyError.Description());
			AfxMessageBox(WorkString);
			closesocket(ClientSocket);
			WSACleanup();
			Robot->ControlRelease();
			cin.get();
#ifdef ADEEL_DEBUG
			debugOut.close();
#endif
			return -1;
		}
	}

	/*
	// Receive until the peer shuts down the connection
	do {
		char recvbuf[DEFAULT_BUFLEN];
		iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			recvbuf[iResult] = NULL;
			printf("recvbuf = \"%s\"\n\n", recvbuf);

			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return -1;
			}
			printf("Bytes sent: %d\n", iSendResult);
			printf("sendbuf = \"%s\"\n\n", recvbuf);
		} else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return -1;
		}

	} while (iResult > 0);
	
	closesocket(ClientSocket);
	WSACleanup();
	cin.get();
	*/

	/*
	if FAILED(CoInitialize(NULL))
	{
		AfxMessageBox("CoInitialize() failed.");
		exit(1);
	}

	ICRSRobotPtr Robot =
					ICRSRobotPtr(__uuidof(CRSRobot));
	ICRSLocationPtr locA = 
					ICRSLocationPtr(__uuidof(CRSLocation));

	try
	{
		Robot->ControlGet();
		ICRSLocationPtr tempLoc = Robot->GetWorldLocation(
					ACTIVEROBOTLib::ptPositionType::ptActual);
		cout << "tempLoc->Getx() = " << tempLoc->Getx() << endl
			 << "tempLoc->Gety() = " << tempLoc->Gety() << endl
			 << "tempLoc->Getz() = " << tempLoc->Getz() << endl
			 << "tempLoc->Getxrot() = " << tempLoc->Getxrot()  << endl
			 << "tempLoc->Getyrot() = " << tempLoc->Getyrot() << endl
			 << "tempLoc->Getzrot() = " << tempLoc->Getzrot() << endl;

		tempLoc->Getx();



		stringstream coordinateStream(recvbuf);
		string xCoordString, yCoordString, zCoordString;
		
		coordinateStream >> xCoordString;
		coordinateStream >> yCoordString;
		coordinateStream >> zCoordString;

		stringstream xCoordStringToNumConverter(xCoordString),
					 yCoordStringToNumConverter(yCoordString),
					 zCoordStringToNumConverter(zCoordString);
		double xCoord, yCoord, zCoord;

		xCoordStringToNumConverter >> xCoord;
		yCoordStringToNumConverter >> yCoord;
		zCoordStringToNumConverter >> zCoord;


		//locA->Putx(280); // 304.778
		//locA->Puty(20);	// -0.0265
		//locA->Putz(550);  // 507.998
		locA->Putx(xCoord); // 304.778
		locA->Puty(yCoord);	// -0.0265
		locA->Putz(zCoord);  // 507.998
		locA->Putxrot(0);
		locA->Putyrot(0);
		locA->Putzrot(0);
		locA->PutFlags(tempLoc->GetFlags());
		locA->PutClass(
				ACTIVEROBOTLib::locClass::locCartesian);
		Robot->Move(locA);

		/*
		Robot->GripperOpen(50);
		Robot->GripperFinish();
		Robot->GripperClose(30);
		Robot->GripperFinish();
		//Robot->GripperDistance = 50;
		//Robot->GripperFinish();
		
		Robot->GripperStop();
		
	}
	catch (_com_error MyError)
	{
		char WorkString[255];
		sprintf(WorkString, "The following error occurred during initialization --\n%s", (LPCTSTR) MyError.Description());
		AfxMessageBox(WorkString);
		Robot->ControlRelease();
		exit(1);
	}

	Robot->ControlRelease();
	*/

	return 0;
}
