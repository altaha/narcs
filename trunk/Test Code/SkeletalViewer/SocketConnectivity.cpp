// <adeel>
#include "stdafx.h"
#include "SocketConnectivity.h"
#include "SkeletalViewer.h"


using namespace std;


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
}


void SocketConnectivity::initialize(HWND hWnd)
{
	m_hWnd = hWnd;

	WSADATA wsaData;
	int errCode;

	// Initialize Winsock
	errCode = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (errCode != 0) {
		MessageBox(m_hWnd,
				   TEXT("SocketConnectivity::initialize(...):- WSAStartup(...) failed"),
				   g_szAppTitle,
				   MB_OK | MB_ICONHAND);
		DestroyWindow(m_hWnd);
		return;
	}


	m_armTrackingSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_armTrackingSocket == INVALID_SOCKET)
	{
		MessageBox(m_hWnd,
				   TEXT("SocketConnectivity::initialize(...):- socket(...) failed"),
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
				   TEXT("SocketConnectivity::initialize(...):- connect(...) failed"),
				   g_szAppTitle,
				   MB_OK | MB_ICONHAND);
		DestroyWindow(m_hWnd);
		return;
	}
}

/*
bool send_message(string message_string)
{
	string message_length_string = "";
	unsigned int prev_message_length = 0,
				 curr_message_length = message_string.size() +
				 	 	 	 	 	   message_length_string.size();

	// Determine the length of the message
	do
	{
		prev_message_length = curr_message_length;

		stringstream num_string_converter;
		num_string_converter << curr_message_length;
		num_string_converter >> message_length_string;

		curr_message_length = message_string.size() +
							  message_length_string.size();
	} while(prev_message_length != curr_message_length);

	string total_message_string = message_length_string + message_string;
	char *message_buffer = new char[curr_message_length + 1];
	strcpy(message_buffer, total_message_string.c_str());

	// Send the message
	char *curr_string_ptr = message_buffer;
	unsigned int total_bytes_written = 0;
	int curr_bytes_written = -1;
	bool connection_closed = false;
	while(total_bytes_written < curr_message_length)
	{
		curr_bytes_written = send(m_armTrackingSocket, sendbuf, (int) strlen(sendbuf), 0);
		if (iResult == SOCKET_ERROR) {
			closesocket(m_armTrackingSocket);
			WSACleanup();
			return -1;
		}

		curr_bytes_written = send(socket_descriptor,
								  curr_string_ptr,
								  curr_message_length - total_bytes_written,
								  0);
		if(curr_bytes_written < 0)
		{
			connection_closed = true;
			break;
		}

		total_bytes_written += curr_bytes_written;
		curr_string_ptr += curr_bytes_written;
	}

	delete[] message_buffer;
	if(connection_closed)
	{
		close(socket_descriptor);
		FD_CLR(socket_descriptor, &master_socket_descriptor_set);
		remove_any_database_entries(socket_descriptor);

		return false;
	}

	return true;
}
*/
// </adeel>