#pragma once

typedef struct KinectData
{
	float rightHandX;
	float rightHandY;
	float rightHandZ;
} KinectData;

typedef struct IMUData
{
	float pitch;
	float roll;
} IMUData;

typedef struct KinectAndIMUData
{
	KinectData kinectData;
	IMUData imuData;
} KinectAndIMUData;