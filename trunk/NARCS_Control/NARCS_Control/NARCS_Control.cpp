// NARCS_Control.cpp : Defines the entry point for the console application.
//

// <adeel>
#include <winsock2.h>
#include <ws2tcpip.h>
// </adeel>
#include "stdafx.h"
// <adeel>
#include <conio.h>
// </adeel>
#include "NARCS.h"
//#include "ThreadObj.h"
#include "SharedMem.h"
#include "SynchObjs.h"
#include "Arduino.h"
//#include "IMU.h"
// <adeel>
#include "DataTransferStructs.h"
#include <iostream>


#define REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_1  192
#define REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_2  168
#define REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_3  1
#define REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_4  1
#define DEFAULT_PORT  62009


#pragma comment(lib, "Ws2_32.lib")
// </adeel>


//#define ADEEL_DEBUG


#ifdef ADEEL_DEBUG
#include <fstream>
#endif


//#define BUFF_SIZE 1024


thrdCommBlock globCommBlocks[NUM_THREADS];


using namespace std;


/* 
  MAIN THREAD:
  ============

  Responsible for:
	- TODO:- Setting up all sockets
	- TODO:- Starting any additional threads
	- Responding to poistion and orientation update requests from the remote side
*/
int _tmain(int argc, _TCHAR* argv[])
{
	SOCKET kinectAndIMUSocket = INVALID_SOCKET;
	NARCS* global = NULL;


	SharedMem kinectSharedMemory (TEXT("kinectSharedMemory"), false);
	MutexObj kinectSharedMemoryMutex;
	SharedMem IMUSharedMemory (TEXT("IMUSharedMemory"), false);
	MutexObj IMUSharedMemoryMutex;
	KinectAndIMUData kinectAndIMUData;


	while(!kinectSharedMemory.isValid())
	{
		if(!kinectSharedMemory.Start(0))
		{
			Sleep(500);
		}
	}
		
	while(!kinectSharedMemoryMutex.isValid() && !kinectSharedMemoryMutex.initNamedMutex(TEXT("kinectSharedMemoryMutex"), false))
	{
		Sleep(500);
	}

	while(!IMUSharedMemory.isValid())
	{
		if(!IMUSharedMemory.Start(0))
		{
			Sleep(500);
		}
	}
		
	while(!IMUSharedMemoryMutex.isValid() && !IMUSharedMemoryMutex.initNamedMutex(TEXT("IMUSharedMemoryMutex"), false))
	{
		Sleep(500);
	}


	int retCode = 0;
	WSADATA wsaData;
	
	// Initialize Winsock
	retCode = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (retCode != 0)
	{
		cout << "_tmain(...) - WSAStartup(...) failed." << endl;
		goto FAILURE;
	}
	
	kinectAndIMUSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (kinectAndIMUSocket == INVALID_SOCKET)
	{
		cout << "_tmain(...) - socket(...) failed." << endl;
		goto FAILURE;
	}


	struct sockaddr_in remoteSideComputerAddress;
	
	ZeroMemory(&remoteSideComputerAddress, sizeof(remoteSideComputerAddress));
	remoteSideComputerAddress.sin_family = AF_INET;
	remoteSideComputerAddress.sin_port = htons(DEFAULT_PORT);
	remoteSideComputerAddress.sin_addr.S_un.S_un_b.s_b1 =
												(unsigned char)(REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_1);
	remoteSideComputerAddress.sin_addr.S_un.S_un_b.s_b2 =
												(unsigned char)(REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_2);
	remoteSideComputerAddress.sin_addr.S_un.S_un_b.s_b3 =
												(unsigned char)(REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_3);
	remoteSideComputerAddress.sin_addr.S_un.S_un_b.s_b4 =
												(unsigned char)(REMOTE_SIDE_COMPUTER_IP_ADDRESS_BYTE_4);

	retCode = connect(kinectAndIMUSocket,
					  (const struct sockaddr *)(&remoteSideComputerAddress),
					  sizeof(remoteSideComputerAddress));
	if (retCode == SOCKET_ERROR)
	{
		cout << "_tmain(...) - connect(...) failed." << endl;
		goto FAILURE;
	}

	// make kinectAndIMUSocket non-blocking
	u_long socketMode = 1;
	ioctlsocket(kinectAndIMUSocket, FIONBIO, &socketMode);


	
	// allocate threads
	try
	{
		printf("Starting threads\n");
		global = new NARCS();
		global->allocate_threads();
	}
	catch (int e)
	{
		char * exceptionStr;
		switch (e){
		case UNEXPECTED_ERROR:
			exceptionStr = "Unexpected Error in Win32 calls\n";
			break;

		case INVALID_THREAD_TYPE:
			exceptionStr = "Specified thread type is invalid\n";
			break;

		case FAILED_THREAD_CREATE:
			exceptionStr = "Failed to create thread\n";
			break;

		case INVALID_PARAM_VALUE:
			exceptionStr = "Invalid parameter value specified to a function\n";
			break;

		default:
			exceptionStr = "Unknown int Exception: %d\n";
			break;
		}
		printf("Exception: %s",exceptionStr);
	}
	catch (exception& e)
	{
		printf("Standard exception: %s\n", e.what() );
	}
	catch (...){
		printf("Exception: unknown  type\n");
	}

	// Creating Arduino Socket
	Arduino* ArduinoHandle = (Arduino*)(global->getHandle(ARDUINO_THREAD));
	
	ArduinoHandle->ArduinoSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ArduinoHandle->ArduinoSocket == INVALID_SOCKET)
	{
		cout << "_tmain(...) - socket(...) failed." << endl;
		goto FAILURE;
	}

	retCode = connect(ArduinoHandle->ArduinoSocket,
					  (const struct sockaddr *)(&remoteSideComputerAddress),
					  sizeof(remoteSideComputerAddress));
	if (retCode == SOCKET_ERROR)
	{
		cout << "_tmain(...) - connect(...) failed." << endl;
		goto FAILURE;
	}

#ifdef ADEEL_DEBUG
	fstream debugOut("IMUDebug.txt", ios::out);
#endif

	char recvBuffer[1];
	char *currBuffPtr = NULL;
	unsigned int totalBytesWritten = 0;
	int currBytesWritten = -1;

	cout << "Program Running." << endl
		 << "Enter any key to exit program." << endl;
	while(true)
	{
		// wait for an update request from the remote side
		while(true)
		{
			recv(kinectAndIMUSocket, recvBuffer, 1, 0);

			retCode = WSAGetLastError();
			if( retCode == 0 )
			{
				break;
			}
			else if( retCode != WSAEWOULDBLOCK )
			{
				cout << "_tmain(...) - recv(...) failed." << endl;
				goto FAILURE;
			}
		}

		// get latest Kinect data
		lockMutex(kinectSharedMemoryMutex, INFINITE);
		kinectSharedMemory.readBytes((void *)(&kinectAndIMUData.kinectData),
									 sizeof(kinectAndIMUData.kinectData),
									 0);
		unlockMutex(kinectSharedMemoryMutex);
		
		// get latest IMU data
		lockMutex(IMUSharedMemoryMutex, INFINITE);
		IMUSharedMemory.readBytes((void *)(&kinectAndIMUData.imuData),
								  sizeof(kinectAndIMUData.imuData),
								  0);
		unlockMutex(IMUSharedMemoryMutex);
		
		// send the kinect and IMU update
		currBuffPtr = (char *)(&kinectAndIMUData);
		totalBytesWritten = 0;
		currBytesWritten = -1;
		while(totalBytesWritten < sizeof(kinectAndIMUData))
		{
			currBytesWritten = send(kinectAndIMUSocket,
									currBuffPtr,
									sizeof(kinectAndIMUData) - totalBytesWritten,
									0);
			if (currBytesWritten == SOCKET_ERROR)
			{
				cout << "_tmain(...) - send(...) failed." << endl;
				goto FAILURE;
			}

			totalBytesWritten += currBytesWritten;
			currBuffPtr += currBytesWritten;
		}

		// exit program if a key was pressed
		if(_kbhit() != 0)
		{
			break;
		}
	}





#ifdef ADEEL_DEBUG
	for(int i = 0; i < 500; i++)
	{
		if ( lockMutex(kinectSharedMemoryMutex, INFINITE) ){
			kinectSharedMemory.readBytes((void *)(&kinectAndIMUData.kinectData), sizeof(kinectAndIMUData.kinectData), 0);
			unlockMutex(kinectSharedMemoryMutex);
			cout << "x = " << kinectAndIMUData.kinectData.rightHandX
			     << ", y = " << kinectAndIMUData.kinectData.rightHandY
				 << ", z = " << kinectAndIMUData.kinectData.rightHandZ << endl;

			/*
			debugOut << "x = " << kinectAndIMUData.kinectData.rightHandX
					 << ", y = " << kinectAndIMUData.kinectData.rightHandY
					 << ", z = " << kinectAndIMUData.kinectData.rightHandZ << endl;
			debugOut.flush();
			*/
		}
		Sleep(50);
	}

	debugOut.close();
#endif


	retCode = 0;
	goto SUCCESS;

FAILURE:
	retCode = -1;

SUCCESS:
	if(kinectAndIMUSocket != INVALID_SOCKET)
	{
		closesocket(kinectAndIMUSocket);
	}
	WSACleanup();

	if(global != NULL)
	{
		delete global;
	}

	cin.get();
	return retCode;
}



	/*
	NARCS* global;
	try
	{
		//allocate global commBlocks
		unsigned char shareBuffer[BUFF_SIZE*NUM_THREADS];
		for(int i=0; i<NUM_THREADS; i++){
			globCommBlocks[i]._mutex.initMutex();
			globCommBlocks[i]._event.initEvent(false);
			globCommBlocks[i]._size = BUFF_SIZE;
			globCommBlocks[i]._pBuf = shareBuffer+i*BUFF_SIZE;
		}

		printf("Starting threads\n");
		global = new NARCS;
		global->allocate_threads();
		global->start_threads();

		while(1) //loop infinitely
		{
			//read data from threads
			for(int i=0; i<NUM_THREADS; i++)
			{
				if(threadRequired[i])
				{
					orient_data test;
					//wait for new data
					if( waitEvent(globCommBlocks[i]._event, 10) )
					{
						 //get Mutex before reading
						if ( lockMutex(globCommBlocks[i]._mutex, 2) )
						{
							globCommBlocks[i].readSingle(test, 0);
							//release Mutex after reading
							unlockMutex(globCommBlocks[i]._mutex);
							printf("I read roll:%f, pitch:%f, yaw:%f\n",
									test.roll, test.pitch, test.yaw);
						}
					}
				}
			}

			//bring back to life any thread that dies
			global->reincarnate(0);
		}
	}
	catch (int e)
	{
		char * exceptionStr;
		switch (e){
		case UNEXPECTED_ERROR:
			exceptionStr = "Unexpected Error in Win32 calls\n";
			break;

		case INVALID_THREAD_TYPE:
			exceptionStr = "Specified thread type is invalid\n";
			break;

		case FAILED_THREAD_CREATE:
			exceptionStr = "Failed to create thread\n";
			break;

		case INVALID_PARAM_VALUE:
			exceptionStr = "Invalid parameter value specified to a function\n";
			break;

		default:
			exceptionStr = "Unknown int Exception: %d\n";
			break;
		}
		printf("Exception: %s",exceptionStr);
	}
	catch (exception& e)
	{
		printf("Standard exception: %s\n", e.what() );
	}
	catch (...){
		printf("Exception: unknown  type\n");
	}

	//Exit and cleanup Code
	delete global;
	return 0;

}	*/