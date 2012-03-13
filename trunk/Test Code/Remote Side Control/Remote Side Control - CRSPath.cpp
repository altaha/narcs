#include <afxwin.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>


#pragma comment(lib, "Ws2_32.lib")
#import "C:\Program Files\CRS Robotics\ActiveRobot\ActiveRobot.dll"


#define DEFAULT_PORT  "62009"
#define ARM_SPEED  25
#define POSITION_UPDATE_BUFFER_LENGTH  12
#define END_EFFECTOR_PATH_ID  1
#define NUM_END_EFFECTOR_PATH_LOCS  3
#define POSITION_SCALE_FACTOR  0.5
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
		Robot->PutTrapezoidalProfileEnable(0);
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
	
	
	ICRSLocationPtr EndEffectorPathLocs[NUM_END_EFFECTOR_PATH_LOCS];

	for(int i = 0; i < NUM_END_EFFECTOR_PATH_LOCS; i++)
	{
		EndEffectorPathLocs[i] = 
						ICRSLocationPtr(__uuidof(CRSLocation));

		try
		{
			EndEffectorPathLocs[i]->PutFlags(readyPosition->GetFlags());
			EndEffectorPathLocs[i]->PutClass(readyPosition->GetClass());
			EndEffectorPathLocs[i]->PutRobotType(locRCA255);
		}
		catch (_com_error MyError)
		{
			char WorkString[255];
			sprintf(WorkString, "The following error occurred during initialization --\n%s", (LPCTSTR) MyError.Description());
			AfxMessageBox(WorkString);
			Robot->ControlRelease();
			exit(1);
		}
	}
	
	

    int iResult;

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
	

	
	char position_update_buffer[POSITION_UPDATE_BUFFER_LENGTH];
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
		ICRSPathPtr EndEffectorPath =
							ICRSPathPtr(__uuidof(CRSPath));

		for(int i = 0; i < NUM_END_EFFECTOR_PATH_LOCS; i++)
		{
			// Receive position update
			curr_bytes_read = -1;
			total_bytes_read = 0;
			curr_buffer_ptr = position_update_buffer;
			while(total_bytes_read < POSITION_UPDATE_BUFFER_LENGTH)
			{
				curr_bytes_read = recv(ClientSocket,
									   curr_buffer_ptr,
									   POSITION_UPDATE_BUFFER_LENGTH - total_bytes_read,
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

			//endEffectorX *= POSITION_SCALE_FACTOR;
			//endEffectorY *= POSITION_SCALE_FACTOR;
			//endEffectorZ *= POSITION_SCALE_FACTOR;
			endEffectorX += readyPositionX;
			endEffectorY = endEffectorY * (-1);
			endEffectorY += readyPositionY;
			endEffectorZ += readyPositionZ;

			try
			{
				EndEffectorPathLocs[i]->Putx(endEffectorX);
				EndEffectorPathLocs[i]->Puty(endEffectorY);
				EndEffectorPathLocs[i]->Putz(endEffectorZ);

				EndEffectorPath->Add(EndEffectorPathLocs[i], 0, 0, 0);
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
		} // for(int i = 0; i < NUM_END_EFFECTOR_PATH_LOCS; i++)

		try
		{
			//Robot->Finish(ftLoose);
			Robot->CTPath(END_EFFECTOR_PATH_ID,
						  EndEffectorPath,
						  1,
						  EndEffectorPath->Count,
						  ARM_SPEED);
			Robot->CTPathGo(END_EFFECTOR_PATH_ID);
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
		} // catch (_com_error MyError)
	} // while(true)

	return 0;
}
