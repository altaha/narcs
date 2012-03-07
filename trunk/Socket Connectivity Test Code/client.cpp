#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>


#pragma comment(lib, "Ws2_32.lib")


#define SERVER_IP_ADDRESS_BYTE_1  172
#define SERVER_IP_ADDRESS_BYTE_2  21
#define SERVER_IP_ADDRESS_BYTE_3  19
#define SERVER_IP_ADDRESS_BYTE_4  242
#define SERVER_PORT  62009
#define DEFAULT_BUFLEN 512


using namespace std;


int main() {
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}


	SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}


	struct sockaddr_in serverAddress;
	
	ZeroMemory(&serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);
	//serverAddress.sin_addr.S_un.S_addr = htonl(SERVER_IP_ADDRESS_BYTE_4 | SERVER_IP_ADDRESS_BYTE_3 | SERVER_IP_ADDRESS_BYTE_2 | SERVER_IP_ADDRESS_BYTE_1);
	serverAddress.sin_addr.S_un.S_un_b.s_b1 = (unsigned char)(SERVER_IP_ADDRESS_BYTE_1);
	serverAddress.sin_addr.S_un.S_un_b.s_b2 = (unsigned char)(SERVER_IP_ADDRESS_BYTE_2);
	serverAddress.sin_addr.S_un.S_un_b.s_b3 = (unsigned char)(SERVER_IP_ADDRESS_BYTE_3);
	serverAddress.sin_addr.S_un.S_un_b.s_b4 = (unsigned char)(SERVER_IP_ADDRESS_BYTE_4);

	iResult = connect(ConnectSocket, (const struct sockaddr *)(&serverAddress), sizeof(serverAddress));
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		printf("Unable to connect to server: %ld\n", WSAGetLastError());
		WSACleanup();
		cin.get();
		return 1;
	}

	printf("Successfull Connection!\n\n");


	char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];

	// Send an initial buffer
	iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		cin.get();
		return 1;
	}

	printf("Bytes Sent: %ld\n", iResult);
	printf("sendbuf = \"%s\"\n", sendbuf);

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		cin.get();
		return 1;
	}

	cout<<"Sending suceeded!"<<endl<<endl;


	// Receive data until the server closes the connection
	iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
	if (iResult > 0) {
		printf("Bytes received: %d\n", iResult);
        recvbuf[iResult] = NULL;
		printf("recvbuf = \"%s\"\n\n", recvbuf);
	}
	else if (iResult == 0)
		printf("Connection closed\n");
	else
		printf("recv failed: %d\n", WSAGetLastError());


	closesocket(ConnectSocket);
	WSACleanup();
	printf("Closing connection\n");
	cin.get();
	return 0;
}