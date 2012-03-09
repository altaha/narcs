#include <afxwin.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")
#import "C:\Program Files\CRS Robotics\ActiveRobot\ActiveRobot.dll"

#define DEFAULT_PORT "62009"
#define DEFAULT_BUFLEN 512

using namespace ACTIVEROBOTLib;
using namespace std;

int main() {

	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;

	WSADATA wsaData;	//to inialize Windows socket dll
	struct addrinfo *result=NULL, *ptr=NULL, hints;
	SOCKET ListenSocket = INVALID_SOCKET, ClientSocket = INVALID_SOCKET;


	//Initialize Winscok
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
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
		return 1;
	}
	
	//Create a SOCKET for the server to listen for client connections
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	//Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);	// no longer need the address information

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	
	printf("All good. Waiting for request\n");
	//Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		cin.get();
		return 1;
	}

	printf("Accept succeeded\n\n");

	
	// Receive until the peer shuts down the connection
	do {
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
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
			printf("sendbuf = \"%s\"\n\n", recvbuf);
		} else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);

	closesocket(ClientSocket);
	WSACleanup();



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
		*/
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
	

	return 0;
}





/*
#include <afxwin.h>
#include <iostream>

#import "C:\Program Files\CRS Robotics\ActiveRobot\ActiveRobot.dll"
using namespace ACTIVEROBOTLib;
using namespace std;

int main()
{
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


		locA->Putx(280); // 304.778
		locA->Puty(20);	// -0.0265
		locA->Putz(550);  // 507.998
		locA->Putxrot(0);
		locA->Putyrot(0);
		locA->Putzrot(0);
		locA->PutFlags(tempLoc->GetFlags());
		locA->PutClass(
				ACTIVEROBOTLib::locClass::locCartesian);
		Robot->Move(locA);

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
	return 0;
}
*/