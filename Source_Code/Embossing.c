#include "Processing.h"

int embossing(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errCode)
{
	/*------------------------------*/
	// 1. 원하는 커널 입력받기
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
		
	// 픽셀의 대각선 방향으로 차이를 계산한다, 역시 총합은 0
	// 추가적인 픽셀처리와 함께라면 대각선 방향으로 빛이 드는듯한 효과를 준다
	
	/*------------------------------*/
	// 2. 엠보싱 처리 계산
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
	
	// 해당 커널은 현재 총합이 0이기에, 커널만 통과하게 되면 엣지 디텍팅과 같은 화면을 보이게 된다. 
	// 이를 방지하기 위해, 128을 더해주고 혹시모를 오버런을 방지하기 위해, 클리핑 과정을 거친다.

	return 0;
}