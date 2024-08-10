#include "Processing.h"

int histo_streching(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errCode)
{
	// 대비 스트래쳉은 이미지의 밝기범위를 확장하여 대비를 개선한다.
	// 이미지의 히스토그램에서 밝기값들이 단단하게 뭉쳐있으면, 최소 밝기값과 최대 밝기값을 찾는다
	// 이 스코프를 최대 밝기범위로 확장시킨다.

	/*-------------------------------------*/
	// 1. 새 밝기값을 계산하기 위한 인자들을 찾는다
	/*-------------------------------------*/

	BYTE denominator;
	BYTE factor[2]={ UCHAR_MAX, 0 };

	STASTICS bmp_Data;
	double temp_arr[MAX_BRIT_VAL+1] = { 0 };
	find_min_max(old_buffer,infoheader,factor);
	// 1번째 인덱스는 최소값, 0번째 인덱스는 최소값

	init_ARR(old_buffer, infoheader, temp_arr, &bmp_Data);
	write_hist(temp_arr);

	printf("\n-----------------------------------------");
	printf("\nmean : %.5f, variance : %.5f", bmp_Data.mean, bmp_Data.variance);
	printf("\n-----------------------------------------");
	printf("\n-----------------------------------------\n\n\n");

	denominator = factor[1] - factor[0]; // 분모 생성

	if (denominator==0) // 0으로 나눌수도 있기에
	{
		printf("This is not picture!\n All pixel has same brit_level!!\n");
		*errCode = 5;
		return 0;
	}

	printf("\nThe denominator : %d\n", denominator);

	/*-------------------------------------*/
	// 2. 공식을 완성한다
	// 픽셀의 새 밝기값 = (PXL_BRIT - MIN)/(DENOMINATOR)*MAX_PXL_BRIT
	// 모든 픽셀에 적용하여, 새버퍼에 할당한다
	/*-------------------------------------*/

	double scale_factor = (double)MAX_BRIT_VAL / denominator;
	for (unsigned int idx = 0; idx < infoheader->ImageSize; idx++)
	{
		int new_val = (int)round((old_buffer[idx] - factor[0]) * scale_factor);
		new_buffer[idx] = clipping(new_val);
	}
		

	init_ARR(new_buffer, infoheader, temp_arr, &bmp_Data);
	write_hist(temp_arr);

	printf("\n-----------------------------------------");
	printf("\nmean : %.5f, variance : %.5f", bmp_Data.mean, bmp_Data.variance);
	printf("\n-----------------------------------------");
	printf("\n-----------------------------------------\n\n\n");
	// 분산값은 전혀 변하지 않는다 -> 왜냐하면 모든 픽셀의 밝기값들에 대해서 곱셈형식의 연산이 수행되었기에
	// 평균은 변할지 몰라도, 분산의 정의에 따라서, 분산은 변하지 않는다.


	return 0;
}
 
void find_min_max(BYTE* old_buffer, BITMAPINFOHEADER* infoheader, BYTE* factor)
{
	// 0번은 최소, 1번은 최대

	for (unsigned int idx = 0; idx < infoheader->ImageSize; idx++)
	{
		if (factor[0] > old_buffer[idx])
			factor[0] = old_buffer[idx];

		if (factor[1] < old_buffer[idx])
			factor[1] = old_buffer[idx];
	}

	printf("\n\nThe MAX britnesss : %d\nThe MIN britness : %d\n\n", factor[1], factor[0]);
	
}