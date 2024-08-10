#include "Processing.h"

int magnifying(BYTE* old_buffer, BYTE** new_buffer, BITMAPINFOHEADER* infoheader, BITMAPFILEHEADER* fileheader, int* errCode)
{
	// 양선형 보간법
	/*------------------------------------------------------*/
	// 1. 얼마의 크기로 확대할 것인지
	/*------------------------------------------------------*/

	int newWidth; int newHeight; DWORD newSize;

	printf("How much do you want to increase?\n\n");

	do
	{
		printf("The original BMP file's information : width -> %d, height -> %d\n", infoheader->width, infoheader->height);
		printf("The new size must bigger than original size and must be positive number!\n\n");
		printf("Input new width : ");
		scanf("%d", &newWidth);
		fflush(stdin);
		printf("\nInput new height : ");
		scanf("%d", &newHeight);
		fflush(stdin);
		puts("\n\n");
	} while (check_size_4M(&newWidth, &newHeight, infoheader));

	/*------------------------------------------------------*/
	// 2. 입력받은 정보에 맞춰서, 파일의 크기와 파일의 가로세로 너비를 계산한다.
	/*------------------------------------------------------*/

	newSize = newHeight * newWidth;

	printf("The new width : %d , The new height : %d, Finally new Imagesize : %u\n", newWidth, newHeight, newSize);

	/*---------------------------------------------*/
	// 3. 해당 계산값을 헤더로 넣어 올바르게 수정한다
	/*---------------------------------------------*/

	int oldWidth = infoheader->width;
	int oldHeight = infoheader->height;

	infoheader->width = newWidth;
	infoheader->height = newHeight;
	infoheader->ImageSize = newSize;
	fileheader->size = infoheader->ImageSize + fileheader->offset;

	/*---------------------------------------------*/
	// 4. 해당 계산값에 맞춰서 new_buffer의 크기를 재할당 해준다.
	/*---------------------------------------------*/

	BYTE* new_addr = realloc(*new_buffer, newSize);

	if (new_addr == NULL)
	{
		printf("realloc Error occured!\n");
		return 0;
	}

	*new_buffer = new_addr;

	/*---------------------------------------------*/
	// 5. 기존 이미지를 새 이미지에 매핑 해준다.
	/*---------------------------------------------*/

	double xRatio = (double)oldWidth / (double)newWidth;
	double yRatio = (double)oldHeight / (double)newHeight; // 새 이미지 좌표를 원본에 투영시키기 위한 확대비 (역방향 사상)

	int intX, intY;								// 원본버퍼에 적용될 좌표 매핑용 변수들
	double realX, realY;						// 원본버퍼에 적용될 좌표 매핑용 변수들
	double upLeft, upRight, downLeft, downRight;// 원본버퍼에 적용될 좌표 매핑용 변수들
	BYTE result;
	int newX, newY;
	double xDiff, yDiff;
	int nextX, nextY;

	// 양선형 보간법 시작

	for (newY = 0; newY < newHeight; newY++)
	{
		realY = ((double)newY) * yRatio;
		intY = (int)floor(realY);
		yDiff = realY - intY;

		nextY = (intY + 1 > oldHeight - 1) ? intY : intY + 1; // 원본이미지의 높이범위를 벗어나지 않게 처리

		for (newX = 0; newX < newWidth; newX++)
		{
			realX = ((double)newX) * xRatio;
			intX = (int)floor(realX);
			xDiff = realX - intX;

			nextX = (intX + 1 > oldWidth - 1) ? intX : intX + 1; // 원본이미지의 너비범위를 벗어나지 않게 처리

			upLeft = (double)old_buffer[intY * oldWidth + intX];
			upRight = (double)old_buffer[intY * oldWidth + nextX];
			downLeft = (double)old_buffer[nextY * oldWidth + intX];
			downRight = (double)old_buffer[nextY * oldWidth + nextX];
			// 사실 확대된 이미지의 좌표를 원본 이미지 좌표에 매핑시킨것이 intY+yDiff , intX+xDiff 이다.

			result = bilinear_interpolation(upLeft, upRight, downLeft, downRight, xDiff, yDiff);
			// 가중치라는 개념보다는 원본에 투영된 실 좌표와 근접 좌표들끼리의 비율계산 (양선형 보간법!!)
			(*new_buffer)[newY * newWidth + newX] = result;
		}
	}

	return 0;
}

int check_size_4M(int* newWidth, int* newHeight, BITMAPINFOHEADER* infoheader)
{
	int width_check = 1; int height_check = 1;
	DWORD maximum = 0xffffffff; int remain;

	if (*newWidth > infoheader->width && *newWidth > 0)
	{
		remain = (*newWidth) % 4;

		if (remain != 0) // 역시나 4의 배수가 맞는지를 확인해야 한다.
		{
			printf("New width is not a multiple of 4!\n");
			printf("Force to be a multiple of 4!\n\n");

			remain = 4 - remain;
			*newWidth += remain;
		}

		width_check = 0;
	}

	if (*newHeight > infoheader->height && *newHeight > 0)
		height_check = 0;

	if (!((DWORD)(*newWidth) * (DWORD)(*newHeight) < maximum)) // unsigned int 형의 표현가능한 크기보다 큰가?
	{
		printf("This size is larger than what unsigned integers can express!\n");
		printf("A maximum of unsigned inters can express by 0 ~ %d\n\n",maximum);
		width_check = 1; 
		height_check = 1;
	}
		

	return width_check || height_check;
}