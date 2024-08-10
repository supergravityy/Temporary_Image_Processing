#include "Processing.h"

int inverting(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errCode)
{
	/*--------------------*/
	// �̹��� ������ �׳� �����̹����� 0bxxxxxxxx �� ��ü������ not ������ �ϸ� ������
	/*--------------------*/

	for (int h = 0; h < infoheader->height; h++)
		for (int w = 0; w < infoheader->width; w++)
			new_buffer[h * infoheader->width + w] = ~old_buffer[h * infoheader->width + w];

	return 0;
}