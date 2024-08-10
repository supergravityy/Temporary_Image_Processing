#include "Processing.h"

int gamma_correcting(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errCode)
{
	/*-----------------------------------*/
	// 1. ���ϴ� ������ ����
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

		// �� ���� �������� 2.2�� �߿��ϳĸ�, 
		// �ΰ��� �ð��ý����� ��⸦ ���������� �ν����� �ʰ�, ��ο�������� ��ȭ�� �� �ΰ��ϰ� �����⿡
		// ���÷��� ��ġ���� ����� �ν��ϴ� ���� ���� ���÷��� ��Ⱑ ��ġ�ϰ� ��
	}

	/*-----------------------------------*/
	// 2. �������� ���� ������
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