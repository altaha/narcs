#pragma once

ref class DoubleMovingAverage
{
public:
	DoubleMovingAverage(unsigned int size);
	~DoubleMovingAverage(void);

	void addNextItem(double nextItem);
	double getMovingAverage(void);

private:
	double *averageArray;
	const unsigned int arraySize;
	unsigned int nextItemIndex;
	double movingAverage;
};