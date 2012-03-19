#include "stdafx.h"
#include "MovingAverage.h"

MovingAverage::MovingAverage(unsigned int size, unsigned int num_signals)
														: avg_size( size ),
														nSignals( num_signals )
{
	nextItemIndex = new unsigned int[avg_size];
	movingAverage = new double[avg_size];
	averageArray = new double* [nSignals];
	for(int i = 0; i < nSignals; i++)
	{
		averageArray[i] = new double [avg_size];
		nextItemIndex[i] = 0;
		movingAverage[i] = 0;
		for(int j = 0; j < avg_size; j++){
			averageArray[i][j] = 0;
		}
	}
}

MovingAverage::~MovingAverage(void)
{
	for (int i = 0; i < nSignals; i++)
	{
		delete[] averageArray[i];
	}
	delete[] averageArray;
	delete[] nextItemIndex;
	delete[] movingAverage;
}

double MovingAverage::addNextItem(double nextItem, unsigned int signal)
{
	signal= signal-1;
	int nextIndex = nextItemIndex[signal];
	averageArray[signal][nextIndex] = nextItem;
	nextItemIndex[signal] = (nextItemIndex[signal] + 1) % avg_size;

	movingAverage[signal] += nextItem/avg_size;
	movingAverage[signal] -= averageArray[signal][nextItemIndex[signal]]/avg_size;

	return movingAverage[signal];
}

double MovingAverage::getMovingAverage( unsigned int signal)
{
	return movingAverage[signal];
}