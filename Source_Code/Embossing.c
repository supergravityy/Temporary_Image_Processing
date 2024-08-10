#include "Processing.h"

int embossing(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errCode)
{
	/*------------------------------*/
	// 1. ���ϴ� Ŀ�� �Է¹ޱ�
	/*------------------------------*/

	int option;

	while (1)
	{
		printf("Choose the filter you want\n");
		printf("1. From top left to bottom right \n");
		printf("2. From top right to bottom left\n");

		printf("\nChoose your kernel! : ");

		scanf("%d", &option);
		putchar('\n');

		if (option > 0 && option < 3)
			break;
	}

	double kernel[9];

	if (option == 1)
	{
		double temp[9] = { -1,-1,0,0,0,0,0,1,1 };
		for (int i = 0; i < 9; i++) 
			kernel[i] = temp[i];
	}
		
	else
	{
		double temp[9] = { 0,-1,-1,0,0,0,1,1,0 };
		for (int i = 0; i < 9; i++)
			kernel[i] = temp[i];
	}
		
	// �ȼ��� �밢�� �������� ���̸� ����Ѵ�, ���� ������ 0
	// �߰����� �ȼ�ó���� �Բ���� �밢�� �������� ���� ��µ��� ȿ���� �ش�
	
	/*------------------------------*/
	// 2. ������ ó�� ���
	/*------------------------------*/

	printf("\nNow embossing calculation is start!\n");

	int temp;
	for (int h = 0; h < infoheader->height; h++)
	{
		for (int w = 0; w < infoheader->width; w++)
		{
			temp = 128 + regular_cal(old_buffer, kernel, w, h, infoheader, 3);
			new_buffer[h * infoheader->width + w] = clipping(temp);
		}
	}
	
	// �ش� Ŀ���� ���� ������ 0�̱⿡, Ŀ�θ� ����ϰ� �Ǹ� ���� �����ð� ���� ȭ���� ���̰� �ȴ�. 
	// �̸� �����ϱ� ����, 128�� �����ְ� Ȥ�ø� �������� �����ϱ� ����, Ŭ���� ������ ��ģ��.

	return 0;
}