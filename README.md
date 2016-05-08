# NARCS
Natural Robotic Arm Control Interface.

- Uses Kinect sensor and a wearable glove as inputs for a natural gesture based interface for humans to control robotic arms.
- The wearable glove uses a 3D orientation sensor to track hand orientation, and a force sensor to measure gripping action and force.
- The program controls a robotic arm remotely in real-time, using a TCP based message protocol.



======================================


## Project Structure:

This is where our Mechatronics FYDP project code resides

### Arduino:
This program will read values coming through the serial port


### NARCS_Control:

-The main program running on the control site computer.
-This includes communication with IMU, Kinect, ARDUINO, Networking with the Robotic Arm computer, etc.
-Program architecture setup to dedicate a thread to each of those functions

### IMU_Interface

-Code for the IMU interfacing GUI
-Based on manufacturer's source code
-Modified to supply orientation info to NARCS_Control through shared memory


### Kinect_GUI

-The Kinect GUI as provided in the Kinect SDK
-Will be mooching data from the program using shared memory


### SharedMemory

-Project demonstrating shared memory for C++ Win32 and managed C++
-Used for experimentation


### Test Code:

-This folder contains all test code.
