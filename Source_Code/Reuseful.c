#include "Processing.h"

void duplicate(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errcode)
{
	memcpy(new_buffer, old_buffer, infoheader->ImageSize);
	printf("Duplicating Image file has been completed! \n\n");
}

int compareDouble(double x, double y)
{
	double diff = x - y;
	double threshold = 1.0e-10;
	
	if (fabs(diff) <= threshold)
		return 0;

	return (diff > 0) ? 1 : -1;
}