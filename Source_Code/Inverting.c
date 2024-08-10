#include "Processing.h"

int inverting(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errCode)
{
	/*--------------------*/
	// 이미지 반전은 그냥 기존이미지의 0bxxxxxxxx 에 전체적으로 not 연산을 하면 끝난다
	/*--------------------*/

	for (int h = 0; h < infoheader->height; h++)
		for (int w = 0; w < infoheader->width; w++)
			new_buffer[h * infoheader->width + w] = ~old_buffer[h * infoheader->width + w];

	return 0;
}