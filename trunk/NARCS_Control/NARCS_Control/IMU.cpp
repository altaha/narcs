#include "StdAfx.h"
#include "IMU.h"
#include <iostream>

using namespace std;

#define PI 3.14159265

IMU::IMU(Thread_type type){
	setType(type);
}

IMU::~IMU(void)
{
	delete this->_sharedMem;
}

int IMU::threadMain(void)
{
	printf("This is a fake IMU Thread\n");
	printf("Just showing IPC with external process, and communicating with main thread in program\n");
	orient_data test_read;
	bool new_data = false;
	int counter =0;

	try{
		//Object used for communication within threads (globals initialized by main thread)
		thrdCommBlock* commBlock = &(globCommBlocks[KINECT_AND_IMU_THREAD]);
		EventObj* localEvent = &(commBlock->_event);
		MutexObj* localMutex = &(commBlock->_mutex);

		//IPC objects
		SharedMem IMUSharedMemory (TEXT("IMUSharedMemory"), false);
		MutexObj IMUSharedMemoryMutex;
		EventObj IMUSharedMemoryEvent;
		KinectAndHandOrientation kinectAndHandOrientation;
		kinectAndHandOrientation.HandOrientation.pitch = 0;
		kinectAndHandOrientation.HandOrientation.roll = 0;
		kinectAndHandOrientation.HandOrientation.yaw = 0;

		int samp_rate = 32; //Hz
		MovingAverage movAverage(4, 6); //set size and number of signals to average
		IMU_Data imu_raw_data;
		//IMU_Data imu_bias = {-2.74771, 15.00707, -20.7402, -0.02483, -0.0188, -1.047};
		IMU_Data imu_bias = {0, 0, 0, 0, 0, 0};
		IMU_Data imu_avg_data;
		HandOrientation orientation = {0,0,0};	
		// </Ahmed>

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
		while(!IMUSharedMemoryEvent.isValid() && !IMUSharedMemoryEvent.initNamedEvent(TEXT("IMUSharedMemoryEvent"), false,false) )
		{
			Sleep(500);
		}
		unsigned int samp_count = 0;

		while(!this->_stopThread){

			// <Ahmed> {Implementation of Orientation tracking}
			// get latest IMU data
			bool newImuData = false;
			if( waitEvent(IMUSharedMemoryEvent, 0) ){ //wait for new data
				if ( lockMutex(IMUSharedMemoryMutex, 2) ){
					IMUSharedMemory.readBytes((void *)(&imu_raw_data),
									  sizeof(IMU_Data),
									  0);
					newImuData = true;
				}
				unlockMutex(IMUSharedMemoryMutex);
			}
			if (newImuData){ //Got new data. Apply processing

				if (samp_count<samp_rate*2){ //calculate home posn bias
					imu_bias.x_gyro += imu_raw_data.x_gyro;
					imu_bias.y_gyro += imu_raw_data.y_gyro;
					imu_bias.z_gyro += imu_raw_data.z_gyro;
					imu_bias.x_accel += imu_raw_data.x_accel;
					imu_bias.y_accel += imu_raw_data.y_accel;
					imu_bias.z_accel += imu_raw_data.z_accel;
					samp_count++;
				}
				else{
					if (samp_count==samp_rate*2){ //calculate bias
						imu_bias.x_gyro /= samp_rate*2;
						imu_bias.y_gyro /= samp_rate*2;
						imu_bias.z_gyro /= samp_rate*2;
						imu_bias.x_accel = 0;
						imu_bias.y_accel = 0;
						imu_bias.z_accel = 0;
						samp_count++;
					}

					// compute moving average
					imu_avg_data.x_gyro = movAverage.addNextItem(imu_raw_data.x_gyro - imu_bias.x_gyro, 1); 
					imu_avg_data.y_gyro = movAverage.addNextItem(imu_raw_data.y_gyro - imu_bias.y_gyro, 2); 
					imu_avg_data.z_gyro = movAverage.addNextItem(imu_raw_data.z_gyro - imu_bias.z_gyro, 3); 
					imu_avg_data.x_accel = movAverage.addNextItem(imu_raw_data.x_accel, 4) ;
					imu_avg_data.y_accel = movAverage.addNextItem(imu_raw_data.y_accel, 5); 
					imu_avg_data.z_accel = movAverage.addNextItem(imu_raw_data.z_accel, 6); 

					// update orientation
					orientation.roll += imu_avg_data.y_gyro *(PI/(180*samp_rate));
					orientation.pitch  += imu_avg_data.x_gyro *(PI/(180*samp_rate));
					orientation.yaw   += imu_avg_data.z_gyro *(PI/(180*samp_rate));

					bool high_x = false, high_y = false, high_z = false;
					if( abs(imu_avg_data.x_accel)>0.9){
						high_x = true;
					}
					if( abs(imu_avg_data.y_accel)>0.9){
						high_y = true;
					}
					if( abs(imu_avg_data.z_accel)>0.9){
						high_z = true;
					}

					if(high_z && !high_x && !high_y){
						orientation.roll = imu_avg_data.z_accel < 0 ? 0 : PI;
						orientation.roll = 0;
						orientation.pitch = 0;
					}
					else if(high_x && !high_y && !high_z){
						orientation.roll = imu_avg_data.x_accel > 0 ? PI/2 : -PI/2;
					}
					else if(high_y && !high_x && !high_z){
						orientation.pitch = imu_avg_data.y_accel > 0 ? -PI/2 : PI/2;
					}

					#ifdef AHMED_DEBUG
					AhmedDebugRaw	<< imu_raw_data.time_s <<","
									<< imu_raw_data.x_gyro <<","
									<< imu_raw_data.y_gyro <<","
									<< imu_raw_data.z_gyro <<","
									<< imu_raw_data.x_accel <<","
									<< imu_raw_data.y_accel <<","
									<< imu_raw_data.z_accel << endl;

					AhmedDebugAvg	<< imu_raw_data.time_s <<","
									<< imu_avg_data.x_gyro <<","
									<< imu_avg_data.y_gyro <<","
									<< imu_avg_data.z_gyro <<","
									<< imu_avg_data.x_accel <<","
									<< imu_avg_data.y_accel <<","
									<< imu_avg_data.z_accel <<","
									<< orientation.roll <<","
									<< orientation.pitch <<","
									<< orientation.yaw << endl;

					AhmedDebugRaw.flush();
					AhmedDebugAvg.flush();
					#endif

					kinectAndHandOrientation.HandOrientation.pitch = orientation.pitch * 180/PI;
					kinectAndHandOrientation.HandOrientation.roll = orientation.roll * 180/PI;
					/*
					if( kinectAndHandOrientation.HandOrientation.pitch >90) {
						kinectAndHandOrientation.HandOrientation.pitch = 90;
					}else if( kinectAndHandOrientation.HandOrientation.pitch <-90) {
						kinectAndHandOrientation.HandOrientation.pitch = -90;
					}
					if( kinectAndHandOrientation.HandOrientation.roll >90) {
						kinectAndHandOrientation.HandOrientation.roll = 90;
					}else if( kinectAndHandOrientation.HandOrientation.roll <-90) {
						kinectAndHandOrientation.HandOrientation.roll = -90;
					}
					*/

					if( lockMutex(*localMutex, 10) ) //returns true when Mutex is obtained
					{
						commBlock->writeSingle(kinectAndHandOrientation.HandOrientation, 0);
						setEvent(*localEvent);//notify that new data has been written
						unlockMutex(*localMutex);
					}

				}
				counter++;
				if(counter > samp_rate/2)
				{
					// <debug>
					cout << "Roll = " << kinectAndHandOrientation.HandOrientation.roll << endl;
					cout << "Pitch = " << kinectAndHandOrientation.HandOrientation.pitch << endl;
					// </debug>
					counter = 0;
				}
			}
		}
		//exiting
	}
	catch(...){
		//Don't need anything special here. Since main thread is going to reincarnate any terminated threads
		printf("Exception in IMU thread (#: %ul), error:%ul\tExiting thread\n", GetCurrentThreadId(), GetLastError());
	}
	this->_active = false;
	return EXIT_SUCCESS;
}