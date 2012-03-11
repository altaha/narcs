// <adeel>
#include "stdafx.h"
#include "SocketConnectivity.h"
#include "SkeletalViewer.h"
#include <sstream>
// <debug>
#include <fstream>
// </debug>


#define REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_1  192
#define REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_2  168
#define REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_3  1
#define REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_4  1
#define ARM_TRACKING_SOCKET_PORT  62009


using namespace std;

// <debug>
fstream debugOut("debug.txt", ios::out);
// </debug>


SocketConnectivity::SocketConnectivity() : m_armTrackingSocket( INVALID_SOCKET ),
										   m_hWnd( NULL )
{
}

SocketConnectivity::~SocketConnectivity()
{
	if(m_armTrackingSocket != INVALID_SOCKET)
	{
		closesocket(m_armTrackingSocket);
	}
	WSACleanup();

	// <debug>
	debugOut.flush();
	debugOut.close();
	// </debug>
}

void SocketConnectivity::Initialize(HWND hWnd)
{
	m_hWnd = hWnd;

	WSADATA wsaData;
	int errCode;

	// Initialize Winsock
	errCode = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (errCode != 0) {
		MessageBox(m_hWnd,
				   TEXT("SocketConnectivity::Initialize(...):- WSAStartup(...) failed"),
				   g_szAppTitle,
				   MB_OK | MB_ICONHAND);
		DestroyWindow(m_hWnd);
		return;
	}


	m_armTrackingSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_armTrackingSocket == INVALID_SOCKET)
	{
		MessageBox(m_hWnd,
				   TEXT("SocketConnectivity::Initialize(...):- socket(...) failed"),
				   g_szAppTitle,
				   MB_OK | MB_ICONHAND);
		DestroyWindow(m_hWnd);
		return;
	}


	struct sockaddr_in remoteSideComputerAddress;
	
	ZeroMemory(&remoteSideComputerAddress, sizeof(remoteSideComputerAddress));
	remoteSideComputerAddress.sin_family = AF_INET;
	remoteSideComputerAddress.sin_port = htons(ARM_TRACKING_SOCKET_PORT);
	remoteSideComputerAddress.sin_addr.S_un.S_un_b.s_b1 = (unsigned char)(REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_1);
	remoteSideComputerAddress.sin_addr.S_un.S_un_b.s_b2 = (unsigned char)(REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_2);
	remoteSideComputerAddress.sin_addr.S_un.S_un_b.s_b3 = (unsigned char)(REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_3);
	remoteSideComputerAddress.sin_addr.S_un.S_un_b.s_b4 = (unsigned char)(REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_4);

	errCode = connect(m_armTrackingSocket, (const struct sockaddr *)(&remoteSideComputerAddress), sizeof(remoteSideComputerAddress));
	if (errCode == SOCKET_ERROR) {
		MessageBox(m_hWnd,
				   TEXT("SocketConnectivity::Initialize(...):- connect(...) failed"),
				   g_szAppTitle,
				   MB_OK | MB_ICONHAND);
		DestroyWindow(m_hWnd);
		return;
	}
}

void SocketConnectivity::SendMessage(string messageString)
{
	string messageLengthString = "";
	unsigned int prevMessageLength = 0,
				 currMessageLength = messageString.size() +
				 	 	 	 	 	 messageLengthString.size();

	// Determine the length of the message
	do
	{
		prevMessageLength = currMessageLength;

		stringstream numStringConverter;
		numStringConverter << currMessageLength;
		numStringConverter >> messageLengthString;

		currMessageLength = messageString.size() +
							messageLengthString.size();
	} while(prevMessageLength != currMessageLength);

	string totalMessageString = messageLengthString + messageString;
	char *messageBuffer = new char[currMessageLength + 1];
	strcpy(messageBuffer, totalMessageString.c_str());

	// Send the message
	char *currStringPtr = messageBuffer;
	unsigned int totalBytesWritten = 0;
	int currBytesWritten = -1;
	while(totalBytesWritten < currMessageLength)
	{
		currBytesWritten = send(m_armTrackingSocket,
								currStringPtr,
								currMessageLength - totalBytesWritten,
								0);
		if (currBytesWritten == SOCKET_ERROR)
		{
			break;
		}

		totalBytesWritten += currBytesWritten;
		currStringPtr += currBytesWritten;
	}

	// <debug>
	debugOut << messageBuffer << endl;
	debugOut.flush();
	// </debug>

	delete[] messageBuffer;
	if(currBytesWritten == SOCKET_ERROR)
	{
		MessageBox(m_hWnd,
				   TEXT("SocketConnectivity::SendMessage(...):- send(...) failed"),
				   g_szAppTitle,
				   MB_OK | MB_ICONHAND);
		DestroyWindow(m_hWnd);
	}

	return;
}
// </adeel>