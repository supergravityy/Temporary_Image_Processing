#include "Processing.h"

int parabola_processing(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errCode)
{
	/*--------------------------------*/
	// 1. ���ϴ� �Ķ󺼶� ó�� ����
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
	// 2. ���õ� ó���� �´� ������
	/*--------------------------------*/

	double temp;
	int denominator = MAX_BRIT_VAL / 2 + 1; // ���� ���ٷ� �������ؼ� �̷��� ��

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
	// CAP �Ķ󺼶� ó���� ���� �κ��� �� �����Ѵ�
	// ���� �κ��� �������� ��ü������ ����

	case 2:
		printf("\nYou select CUP parabola!\n");
		for (int idx = 0; idx < infoheader->ImageSize; idx++)
		{
			temp = MAX_DOB_BRIT_VAL * pow(old_buffer[idx] / ((double)denominator) - 1., 2);
			new_buffer[idx] = (BYTE)clipping((int)round(temp));
		}
		break;
	// CUP �Ķ󺼶� ó���� ��ο� �κ��� �� �����Ѵ�
	// ��ο� �κ��� �������� ��ü������ ����
	}
}