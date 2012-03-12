// <adeel>
#include "stdafx.h"
#include "SocketConnectivity.h"
#include "SkeletalViewer.h"
#include <sstream>


//#define ADEEL_DEBUG

#ifdef ADEEL_DEBUG
#include <fstream>
#endif


#define REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_1  192
#define REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_2  168
#define REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_3  1
#define REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_4  1
#define ARM_TRACKING_SOCKET_PORT  62009


using namespace std;

#ifdef ADEEL_DEBUG
fstream debugOut("debug.txt", ios::out);
#endif


SocketConnectivity::SocketConnectivity() : m_hWnd( NULL ),
										   m_armTrackingSocket( INVALID_SOCKET )
										   
{
}

SocketConnectivity::~SocketConnectivity()
{
	if(m_armTrackingSocket != INVALID_SOCKET)
	{
		closesocket(m_armTrackingSocket);
	}
	WSACleanup();

	#ifdef ADEEL_DEBUG
	debugOut.flush();
	debugOut.close();
	#endif
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

void SocketConnectivity::SendPositionUpdate(float x, float y, float z)
{
    // copy coordinates into m_positionUpdateBuffer
	memcpy_s( (void *)(m_positionUpdateBuffer), 4, (const void *)(&x), sizeof(x) );
	memcpy_s( (void *)(m_positionUpdateBuffer + 4), 4, (const void *)(&y), sizeof(y) );
	memcpy_s( (void *)(m_positionUpdateBuffer + 8), 4, (const void *)(&z), sizeof(z) );


	// Send the message
	char *currStringPtr = m_positionUpdateBuffer;
	unsigned int totalBytesWritten = 0;
	int currBytesWritten = -1;
	while(totalBytesWritten < POSITION_UPDATE_BUFFER_LENGTH)
	{
		currBytesWritten = send(m_armTrackingSocket,
								currStringPtr,
								POSITION_UPDATE_BUFFER_LENGTH - totalBytesWritten,
								0);
		if (currBytesWritten == SOCKET_ERROR)
		{
			MessageBox(m_hWnd,
					   TEXT("SocketConnectivity::SendMessage(...):- send(...) failed"),
					   g_szAppTitle,
					   MB_OK | MB_ICONHAND);
			DestroyWindow(m_hWnd);
		}

		totalBytesWritten += currBytesWritten;
		currStringPtr += currBytesWritten;
	}

	#ifdef ADEEL_DEBUG
	debugOut << x << ", " << y << ", " << z << endl;
	debugOut.flush();
	#endif

	return;
}
// </adeel>