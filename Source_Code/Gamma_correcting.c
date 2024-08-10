#include "Processing.h"

int gamma_correcting(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errCode)
{
	/*-----------------------------------*/
	// 1. 원하는 감마값 선택
	/*-----------------------------------*/

	double gamma;

	while (1)
	{
		printf("Tip1 : The best gamma value for the human eye is about 2.2\n");
		printf("Tip2 : output brit level > input brit level (Gamma < 1)\n");
		printf("Tip3 : output brit level < input brit level (Gamma > 1)\n");
		printf("input your Gamma Val! : ");

		scanf("%lf", &gamma);
		putchar('\n');

		if (gamma > 0 && gamma <= 2.2)
			break;

		// 왜 하필 감마값이 2.2가 중요하냐면, 
		// 인간의 시각시스템은 밝기를 선형적으로 인식하지 않고, 어두운곳에서의 변화를 더 민감하게 느끼기에
		// 디스플레이 장치에서 사람이 인식하는 밝기와 실제 디스플레이 밝기가 일치하게 함
	}

	/*-----------------------------------*/
	// 2. 감마값에 따른 계산수행
	/*-----------------------------------*/

	double normalizedPXL;
	double correctedPXL;

	for (int idx = 0; idx < infoheader->ImageSize; idx++)
	{
		normalizedPXL = old_buffer[idx] / MAX_DOB_BRIT_VAL;
		correctedPXL = pow(normalizedPXL, gamma);
		new_buffer[idx] = (BYTE)round(correctedPXL * MAX_DOB_BRIT_VAL);
	}
}