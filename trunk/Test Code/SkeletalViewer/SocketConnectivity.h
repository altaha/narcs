// <adeel>
#pragma once


#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <WinDef.h>

#pragma comment(lib, "Ws2_32.lib")


#define REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_1  192
#define REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_2  168
#define REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_3  1
#define REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_4  1
#define ARM_TRACKING_SOCKET_PORT  62009


class SocketConnectivity
{
public:
	SocketConnectivity();
	~SocketConnectivity();

	void initialize(HWND hWnd);
	bool send_message(std::string message_string);

private:
	SOCKET m_armTrackingSocket;
	HWND m_hWnd;
};
//</adeel>