#include "DoubleMovingAverage.h"
#include "UnmanagedClassHandling.h"


DoubleMovingAverage::DoubleMovingAverage(unsigned int size) : arraySize( size ),
															  nextItemIndex( 0 ),
															  movingAverage( 0 )
{
	minitialize();
	
	averageArray = new double [size];
	for(int i = 0; i < size; i++)
	{
		averageArray[i] = 0;
	}
}

DoubleMovingAverage::~DoubleMovingAverage(void)
{
	delete[] averageArray;
	mterminate();
}

void DoubleMovingAverage::addNextItem(double nextItem)
{
	averageArray[nextItemIndex] = nextItem;
	nextItemIndex = (nextItemIndex + 1) % arraySize;

	movingAverage = 0;
	for(int i = 0; i < arraySize; i++)
	{
		movingAverage += averageArray[i];
	}
	movingAverage = movingAverage / ( (double)(arraySize) );
}

double DoubleMovingAverage::getMovingAverage(void)
{
	return movingAverage;
}