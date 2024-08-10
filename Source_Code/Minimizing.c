#include "Processing.h"

int minimizing(BYTE* old_buffer, BYTE** new_buffer, BITMAPINFOHEADER* infoheader, BITMAPFILEHEADER* fileheader,int* errCode)
{
	/*---------------------------------------------*/
	// 1. 제일먼저 할것은 얼마의 크기로 축소할 것인지이다. 사용자에게 입력을 받는다 (0~100)
	/*---------------------------------------------*/
	
	int newWidth; int newHeight; int newSize;
	
	printf("How much do you want to reduce?\n");
	
	do
	{
		printf("The original BMP file's information : width -> %d , height -> %d\n", infoheader->width, infoheader->height);
		printf("The new size must bigger than old size and must be positive number!\n\n");
		printf("Input new width : ");
		scanf("%d", &newWidth);
		printf("\nInput new height : ");
		scanf("%d", &newHeight);
		puts("\n\n");
	} while (check_size_4m(&newWidth, &newHeight,infoheader));
	
	/*---------------------------------------------*/
	// 2. 그 크기에 맞춰서, 파일의 크기와 파일의 가로세로 비율을 계산한다.
	/*---------------------------------------------*/

	newSize = newWidth * newHeight;

	printf("The new width : %d , The new height : %d, Finally new Imagesize : %d\n", newWidth, newHeight, newSize);

	double width_Ratio = (double)infoheader->width / (double)newWidth;
	double height_Ratio = (double)infoheader->height / (double)newHeight;

	printf("The width ratio : %.3f, The height ratio : %.3f\n", width_Ratio, height_Ratio);

	/*---------------------------------------------*/
	// 3. 해당 계산값을 헤더로 넣어 올바르게 수정한다
	/*---------------------------------------------*/

	int oldWidth = infoheader->width; // 원본 raw 파일의 인덱스 계산에 필요하다.

	infoheader->width = newWidth;
	infoheader->height = newHeight;
	infoheader->ImageSize = newSize;
	fileheader->size = infoheader->ImageSize + fileheader->offset;

	/*---------------------------------------------*/
	// 4. 해당 계산값에 맞춰서 new_buffer의 크기를 재할당 해준다.
	/*---------------------------------------------*/

	BYTE* new_addr = realloc(*new_buffer, sizeof(char) * newSize);

	if (new_addr == NULL)
	{
		printf("realloc Error Occured! \n");
		*errCode = 2;
		return 0;
	}

	*new_buffer = new_addr; // 힙 영역이어서 로컬함수 스코프에서 처리해도 상관이 없다.

	/*---------------------------------------------*/
	// 5. new_buffer의 좌표값을 기준으로 하여, 새로운 크기 스펙과 계산했던 비율을 이용
	// old_buffer의 좌표값에 매핑하여, new_buffer에 채워넣는다.
	/*---------------------------------------------*/

	int tempX; int tempY;

	for (int y = 0; y < newHeight; y++)
	{
		tempY = (int)floor(y * height_Ratio);
		// ratio = old/new 이기에, 여기에 new를 곱해주면, old의 인덱스가 나온다.

		for (int x = 0; x < newWidth; x++)
		{
			tempX = (int)floor(x * width_Ratio);
			(*new_buffer)[y * newWidth + x] = old_buffer[tempY * oldWidth + tempX];
			// 제일먼저 포인터먼저 들어가 준다음, 배열 문법을 사용하자!!!
		}
	}
	
	return 0;
}

int check_size_4m(int* newWidth, int* newHeight, BITMAPINFOHEADER* infoheader)
{
	int width_check = 1; int height_check = 1;
	int remain = 0; 

	if (*newWidth < infoheader->width && *newWidth > 0)
	{
		remain = (*newWidth) % 4;

		if (remain != 0) // 이미지 읽기 최적화를 위해서 너비는 언제나 4의 배수일것!!!
		{
			if (*newWidth < (infoheader->width) / 2) // 0에 가까울 확률 높음
			{
				remain = 4 - remain;
				*newWidth += remain;
			}
			else // 이전 너비 크기에 가까울 확률이 높다
				*newWidth -= remain;
		}

		width_check = 0;
	}
		

	if (*newHeight < infoheader->height && *newHeight > 0)
		height_check = 0;

	return width_check || height_check;
}