#include "Processing.h"

int magnifying(BYTE* old_buffer, BYTE** new_buffer, BITMAPINFOHEADER* infoheader, BITMAPFILEHEADER* fileheader, int* errCode)
{
	// �缱�� ������
	/*------------------------------------------------------*/
	// 1. ���� ũ��� Ȯ���� ������
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
	// 2. �Է¹��� ������ ���缭, ������ ũ��� ������ ���μ��� �ʺ� ����Ѵ�.
	/*------------------------------------------------------*/

	newSize = newHeight * newWidth;

	printf("The new width : %d , The new height : %d, Finally new Imagesize : %u\n", newWidth, newHeight, newSize);

	/*---------------------------------------------*/
	// 3. �ش� ��갪�� ����� �־� �ùٸ��� �����Ѵ�
	/*---------------------------------------------*/

	int oldWidth = infoheader->width;
	int oldHeight = infoheader->height;

	infoheader->width = newWidth;
	infoheader->height = newHeight;
	infoheader->ImageSize = newSize;
	fileheader->size = infoheader->ImageSize + fileheader->offset;

	/*---------------------------------------------*/
	// 4. �ش� ��갪�� ���缭 new_buffer�� ũ�⸦ ���Ҵ� ���ش�.
	/*---------------------------------------------*/

	BYTE* new_addr = realloc(*new_buffer, newSize);

	if (new_addr == NULL)
	{
		printf("realloc Error occured!\n");
		return 0;
	}

	*new_buffer = new_addr;

	/*---------------------------------------------*/
	// 5. ���� �̹����� �� �̹����� ���� ���ش�.
	/*---------------------------------------------*/

	double xRatio = (double)oldWidth / (double)newWidth;
	double yRatio = (double)oldHeight / (double)newHeight; // �� �̹��� ��ǥ�� ������ ������Ű�� ���� Ȯ��� (������ ���)

	int intX, intY;								// �������ۿ� ����� ��ǥ ���ο� ������
	double realX, realY;						// �������ۿ� ����� ��ǥ ���ο� ������
	double upLeft, upRight, downLeft, downRight;// �������ۿ� ����� ��ǥ ���ο� ������
	BYTE result;
	int newX, newY;
	double xDiff, yDiff;
	int nextX, nextY;

	// �缱�� ������ ����

	for (newY = 0; newY < newHeight; newY++)
	{
		realY = ((double)newY) * yRatio;
		intY = (int)floor(realY);
		yDiff = realY - intY;

		nextY = (intY + 1 > oldHeight - 1) ? intY : intY + 1; // �����̹����� ���̹����� ����� �ʰ� ó��

		for (newX = 0; newX < newWidth; newX++)
		{
			realX = ((double)newX) * xRatio;
			intX = (int)floor(realX);
			xDiff = realX - intX;

			nextX = (intX + 1 > oldWidth - 1) ? intX : intX + 1; // �����̹����� �ʺ������ ����� �ʰ� ó��

			upLeft = (double)old_buffer[intY * oldWidth + intX];
			upRight = (double)old_buffer[intY * oldWidth + nextX];
			downLeft = (double)old_buffer[nextY * oldWidth + intX];
			downRight = (double)old_buffer[nextY * oldWidth + nextX];
			// ��� Ȯ��� �̹����� ��ǥ�� ���� �̹��� ��ǥ�� ���ν�Ų���� intY+yDiff , intX+xDiff �̴�.

			result = bilinear_interpolation(upLeft, upRight, downLeft, downRight, xDiff, yDiff);
			// ����ġ��� ���亸�ٴ� ������ ������ �� ��ǥ�� ���� ��ǥ�鳢���� ������� (�缱�� ������!!)
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

		if (remain != 0) // ���ó� 4�� ����� �´����� Ȯ���ؾ� �Ѵ�.
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

	if (!((DWORD)(*newWidth) * (DWORD)(*newHeight) < maximum)) // unsigned int ���� ǥ�������� ũ�⺸�� ū��?
	{
		printf("This size is larger than what unsigned integers can express!\n");
		printf("A maximum of unsigned inters can express by 0 ~ %d\n\n",maximum);
		width_check = 1; 
		height_check = 1;
	}
		

	return width_check || height_check;
}