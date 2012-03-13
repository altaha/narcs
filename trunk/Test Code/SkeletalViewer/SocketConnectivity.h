// <adeel>
#pragma once


#include <winsock2.h>
#include <ws2tcpip.h>


#define POSITION_UPDATE_BUFFER_LENGTH  12


#pragma comment(lib, "Ws2_32.lib")


class SocketConnectivity
{
public:
	SocketConnectivity();
	~SocketConnectivity();

	void Initialize(HWND hWnd);
	bool RemoteComputerWantsPositionUpdate();
	void SendPositionUpdate(float x, float y, float z);

private:
	HWND    m_hWnd;
	SOCKET  m_armTrackingSocket;
	char    m_positionUpdateBuffer[POSITION_UPDATE_BUFFER_LENGTH];
};
//</adeel>