#pragma once

struct IMU_Data{
	double time_s;	
	double x_gyro;
	double y_gyro;
	double z_gyro;
	double x_accel;
	double y_accel;
	double z_accel;
};

typedef struct KinectData
{
	float rightHandX;
	float rightHandY;
	float rightHandZ;
} KinectData;

typedef struct HandOrientation
{
	float pitch;
	float roll;
	float yaw;
} HandOrientation;

typedef struct KinectAndHandOrientation
{
	KinectData kinectData;
	HandOrientation HandOrientation;
} KinectAndHandOrientation;

