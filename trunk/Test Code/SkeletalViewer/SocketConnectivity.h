// <adeel>
#pragma once


#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>


#pragma comment(lib, "Ws2_32.lib")


class SocketConnectivity
{
public:
	SocketConnectivity();
	~SocketConnectivity();

	void Initialize(HWND hWnd);
	void SendMessage(std::string messageString);

private:
	SOCKET m_armTrackingSocket;
	HWND m_hWnd;
};
//</adeel>