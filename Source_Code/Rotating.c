#include "Processing.h"

int rotating(BYTE* old_buffer, BYTE** new_buffer, BITMAPINFOHEADER* infoheader, BITMAPFILEHEADER* fileheader, int* errcode)
{
	// * 화면의 크기를 현재 이미지의 대각선 크기만큼 키워주고, 정가운데로 평행이동 시켜줄것
	// * 그리고 회전변환을 할것
	// * 보간법을 이용해 홀을 메워줄것

	/*--------------------------------*/
	// 1. 버퍼를 키워주기위한 버퍼의 크기 정하기
	/*--------------------------------*/

	int oldWidth = infoheader->width;
	int oldHeight = infoheader->height;

	int newWidth = (int)ceil(sqrt(2) * (double)infoheader->width);
	int newHeight = (int)ceil(sqrt(2) * (double)infoheader->height);
	// 기존의 RAW 크기대로 하면, 화면의 일부가 잘린다. -> old buffer도 new buffer만큼 키워주고, 픽셀값들을 정중앙으로 이동시켜보자.

	int remain = newWidth % 4;
	remain = 4 - remain;
	newWidth += remain;
	newHeight += remain;
	// 순전히 좌표계산이고, 이러한 높이값도 선형변환에 영향을 미칠 수 있다.

	int newSide = (newWidth > newHeight) ? newWidth : newHeight;
	// 이미지 자체가 정사각형이 아니면, 회전하면 무조건 잘리게 된다. -> 정사각형으로 만들어줄것

	printf("\nSize specifications required for rotational conversion \n");
	printf("But we need padding to Width and Height! by multiple of 4!\n");
	printf("oldWidth(%d) * root(2) = newWidth(%d)\n", oldWidth, newWidth);
	printf("oldHeight(%d) * root(2) = newHeight(%d)\n", oldHeight, newHeight);

	int newSize = (int)pow(newSide, 2);
	printf("Consider non-square cases and grow the buffer into larger sides\n");
	printf("newSide : %d, newSize : %d\n", newSide, newSize);
	
	/*--------------------------------*/
	// 2. 정한 크기에 맞게 총 3개의 버퍼를 가져야 한다
	// old버퍼 : 원본 RAW 데이터 보관용
	// temp버퍼 : 회전변환 후, 모든 RAW 데이터를 담기위한 크기를 갖는 버퍼
	// new버퍼 : 회전변환이 된 버퍼 -> 보간을 통해 완벽해짐
	/*--------------------------------*/

	BYTE* temp_buffer = (BYTE*)calloc(1, newSize);
	BYTE* new_addr = realloc(*new_buffer, sizeof(char) * newSize);

	if (new_addr == NULL)
	{
		printf("Cannot allocate memory section\n");
		*errcode = 2;
		return 0;
	}

	*new_buffer = new_addr;
	memset(*new_buffer, 0, newSize * sizeof(BYTE));

	/*-------------------------------*/
	// 3. 기존 RAW를 정가운데로 평행이동시키기 위한 계산
	/*-------------------------------*/

	int marginWidth = newSide - oldWidth;
	int marginHeight = newSide - oldHeight;
	int moveWidth = (int)round((double)marginWidth / 2);
	int moveHeight = (int)round((double)marginHeight / 2);

	printf("\n\nThis is temp_buffer specification \n");
	printf("marginWidth : %d , marginHeight : %d , moveWidth : %d, moveHeight : %d\n\n", marginWidth, marginHeight, moveWidth, moveHeight);

	move_RAWdata(old_buffer, temp_buffer, infoheader, fileheader, moveWidth, moveHeight, newSide);
	// 이 함수에서 헤더정보도 전부 수정된다
	printf("The extending process has been completed!\n\n");

	/*-----------------------------------*/
	// 4. 반시계 방향으로 얼만큼 회전할 것인가? 정수각도 입력받기
	/*-----------------------------------*/

	double angle;

	while (1)
	{
		printf("\nHow many angles do you want to enter? \nThe angles must always be integer : ");
		scanf("%lf", &angle);
		// 주의 : double 변수에서 %f로 받으면 4바이트인 float 형태로 받기에 리틀엔디안 방식의 메모리에선 쓰래기값을 포함할 수밖에 없다.
		putchar('\n');

		if (isInteger(angle))
			break;
	}

	/*-----------------------------------*/
	// 5. 확장된 RAW 데이터를 기반으로 회전변환
	/*-----------------------------------*/

	rotate_RAWdata(temp_buffer, *new_buffer, infoheader, ANG2RAD(angle));
	// 출력은 double 형인 라디안으로 나가게 된다. C의 삼각함수는 라디안만을 받는다.

	/*-----------------------------------*/
	// 6. 현재 역매핑방식이 아닌 정매핑 방식이기에, 필연적으로 hole이 발생할 수밖에 없다.
	// 이를 채워주기 위한 보간함수를 선언
	/*-----------------------------------*/

	finding_holes(*new_buffer,infoheader);
	
release:
	if (temp_buffer != NULL) free(temp_buffer);

	return 0;
}

int move_RAWdata(BYTE* old_buffer, BYTE* temp_buffer, BITMAPINFOHEADER* infoheader, BITMAPFILEHEADER* fileheader, int moveWidth, int moveHeight, int newSide)
{
	// 기존 버퍼에서 temp 버퍼로 RAW 데이터를 평행이동
	int newY, newX;
	int oldWidth = infoheader->width;
	int oldHeight = infoheader->height;

	for (int oldY = 0; oldY < oldHeight; oldY++)
	{
		newY = moveHeight + oldY;

		for (int oldX = 0; oldX < oldWidth; oldX++)
		{
			newX = moveWidth + oldX;
			temp_buffer[newY * newSide + newX] = old_buffer[oldY * oldWidth + oldX];
		}
	}

	// RAW데이터를 옮겼으니 헤더정보를 수정한다.
	infoheader->width = newSide;
	infoheader->height = newSide;
	infoheader->ImageSize = (DWORD)(pow(newSide, 2));
	fileheader->size = (DWORD)(infoheader->ImageSize) + fileheader->offset;

	return 0;
}

int isInteger(double angle)
{
	return floor(angle) == ceil(angle);
	// 내림수와 올림수가 같으면 정수라고 판단할 수 있다.
}

int rotate_RAWdata(BYTE* temp_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, double radian)
{
	int Width = infoheader->width;
	int Height = infoheader->height;
	int tempX, tempY;

	double realX, realY;
	int amidWidth = Width / 2; int amidHeight = amidWidth; // 정사각형이기에

	double cosVal = cos(radian); double sinVal = sin(radian);
	// 이미지 좌표공간은 기존 수학의 좌표공간과 y축 대칭이지만, RAW 파일형식이 거꾸로 되어있고
	// 그 RAW 이미지를 화면에 불러올때의 규칙때문에 기존 선형변환 공식을 써도 상관이 없다

	int newX, newY;

	printf("\nThe center location of Imnage : (X : %d , Y : %d)\n", amidWidth, amidHeight);
	printf("The cosVal : %lf, sinVal : %lf\n\n", cosVal, sinVal);

	// 이미지의 중심을 기점으로 회전한다는 것을 잊지말자

	for (tempY = 0; tempY < Height; tempY++)
	{
		for (tempX = 0; tempX < Width; tempX++)
		{
			realX = (tempX - amidWidth) * cosVal - (tempY - amidHeight) * sinVal;
			realY = (tempX - amidWidth) * sinVal + (tempY - amidHeight) * cosVal;

			newX = (int)round(realX) + amidWidth;
			newY = (int)round(realY) + amidHeight;

			if (newX >= 0 && newX < Width && newY >= 0 && newY < Height) // 유효한 좌표인가?
				new_buffer[newY * Width + newX] = temp_buffer[tempY * Width + tempX];
		}
	}

	printf("A The image rotation has been completed to the angle you wanted!\n\n");

	return 0;
}

int finding_holes(BYTE* buffer, BITMAPINFOHEADER* infoheader)
{
	int X, Y;

	// RAW 픽셀들을 순회하면서 일단 밝기값이 0인 픽셀을 찾는다
	// 2가지의 가능성이 있다.
	// * 동적할당할때 선언했던 공백 or * 정방향 매핑에서 발생한 Hole

	// Hole이 발생하는 이유는 행렬계산을 한 후, 반올림을 하여 정수처리를 하기 때문이다.
	// 그렇기 때문에 논리적으로 x방향 y방향 연속적으로 hole이 발생 할 수 없다고 판단했다.

	for (Y = 0; Y < infoheader->height; Y++)
		for (X = 0; X < infoheader->width; X++)
			if (buffer[Y * infoheader->width + X] == 0)
				buffer[Y * infoheader->width + X] = MYnearest_interpolate(buffer, X, Y, infoheader->width, infoheader->height);

	printf("A hole searching and interpolation has been ended!\n\n");

	return 0;
}