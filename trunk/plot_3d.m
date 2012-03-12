clear;
clc;

kinect_data = csvread('kinect_data.csv',0,0);

x = kinect_data(:,1);
y = kinect_data(:,2);
z = kinect_data(:,3);

plot3(x,y,z);

xlabel('X-axis [mm]', 'fontSize', 12);
ylabel('Y-axis [mm]', 'fontSize', 12);
zlabel('Z-axis [mm]', 'fontSize', 12);

%AZ = 30;
%EL = 40;
%view(AZ, EL); %Allows adjustment of 3d viewing angle