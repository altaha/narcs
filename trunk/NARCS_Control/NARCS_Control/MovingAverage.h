#pragma once

class MovingAverage
{
public:
	MovingAverage(unsigned int size, unsigned int num_signals);
	~MovingAverage(void);

	double addNextItem(double nextItem, unsigned int signal ); //adds a new item and retuns mov average
	double getMovingAverage(unsigned int signal);

private:
	double **averageArray;
	const unsigned int avg_size;
	const unsigned int nSignals;
	unsigned int* nextItemIndex;
	double* movingAverage;
};