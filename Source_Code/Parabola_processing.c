#include "Processing.h"

int parabola_processing(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errCode)
{
	/*--------------------------------*/
	// 1. 원하는 파라볼라 처리 선택
	/*--------------------------------*/

	int option;

	while (1)
	{
		printf("1. CAP parabola processing\n");
		printf("2. CUP prarbola processing\n");
		printf("Input mode you want! : ");

		scanf("%d", &option);
		putchar('\n');

		if (option > 0 && option <= 2)
			break;
	}

	/*--------------------------------*/
	// 2. 선택된 처리에 맞는 계산수행
	/*--------------------------------*/

	double temp;
	int denominator = MAX_BRIT_VAL / 2 + 1; // 식을 한줄로 쓰기위해서 이렇게 함

	switch (option)
	{
	case 1:
		printf("\nYou select CAP parabola!\n");
		for (int idx = 0; idx < infoheader->ImageSize; idx++)
		{
			temp = MAX_DOB_BRIT_VAL - MAX_DOB_BRIT_VAL * pow(old_buffer[idx] / ((double)denominator) - 1., 2);
			new_buffer[idx] = (BYTE)clipping((int)round(temp));
		}
		break;
	// CAP 파라볼라 처리는 밝은 부분을 더 강조한다
	// 밝은 부분을 기준으로 입체적으로 보임

	case 2:
		printf("\nYou select CUP parabola!\n");
		for (int idx = 0; idx < infoheader->ImageSize; idx++)
		{
			temp = MAX_DOB_BRIT_VAL * pow(old_buffer[idx] / ((double)denominator) - 1., 2);
			new_buffer[idx] = (BYTE)clipping((int)round(temp));
		}
		break;
	// CUP 파라볼라 처리는 어두운 부분을 더 강조한다
	// 어두운 부분을 기준으로 입체적으로 보임
	}
}