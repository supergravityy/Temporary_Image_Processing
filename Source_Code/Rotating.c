#include "Processing.h"

int rotating(BYTE* old_buffer, BYTE** new_buffer, BITMAPINFOHEADER* infoheader, BITMAPFILEHEADER* fileheader, int* errcode)
{
	// * ȭ���� ũ�⸦ ���� �̹����� �밢�� ũ�⸸ŭ Ű���ְ�, ������� �����̵� �����ٰ�
	// * �׸��� ȸ����ȯ�� �Ұ�
	// * �������� �̿��� Ȧ�� �޿��ٰ�

	/*--------------------------------*/
	// 1. ���۸� Ű���ֱ����� ������ ũ�� ���ϱ�
	/*--------------------------------*/

	int oldWidth = infoheader->width;
	int oldHeight = infoheader->height;

	int newWidth = (int)ceil(sqrt(2) * (double)infoheader->width);
	int newHeight = (int)ceil(sqrt(2) * (double)infoheader->height);
	// ������ RAW ũ���� �ϸ�, ȭ���� �Ϻΰ� �߸���. -> old buffer�� new buffer��ŭ Ű���ְ�, �ȼ������� ���߾����� �̵����Ѻ���.

	int remain = newWidth % 4;
	remain = 4 - remain;
	newWidth += remain;
	newHeight += remain;
	// ������ ��ǥ����̰�, �̷��� ���̰��� ������ȯ�� ������ ��ĥ �� �ִ�.

	int newSide = (newWidth > newHeight) ? newWidth : newHeight;
	// �̹��� ��ü�� ���簢���� �ƴϸ�, ȸ���ϸ� ������ �߸��� �ȴ�. -> ���簢������ ������ٰ�

	printf("\nSize specifications required for rotational conversion \n");
	printf("But we need padding to Width and Height! by multiple of 4!\n");
	printf("oldWidth(%d) * root(2) = newWidth(%d)\n", oldWidth, newWidth);
	printf("oldHeight(%d) * root(2) = newHeight(%d)\n", oldHeight, newHeight);

	int newSize = (int)pow(newSide, 2);
	printf("Consider non-square cases and grow the buffer into larger sides\n");
	printf("newSide : %d, newSize : %d\n", newSide, newSize);
	
	/*--------------------------------*/
	// 2. ���� ũ�⿡ �°� �� 3���� ���۸� ������ �Ѵ�
	// old���� : ���� RAW ������ ������
	// temp���� : ȸ����ȯ ��, ��� RAW �����͸� ������� ũ�⸦ ���� ����
	// new���� : ȸ����ȯ�� �� ���� -> ������ ���� �Ϻ�����
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
	// 3. ���� RAW�� ������� �����̵���Ű�� ���� ���
	/*-------------------------------*/

	int marginWidth = newSide - oldWidth;
	int marginHeight = newSide - oldHeight;
	int moveWidth = (int)round((double)marginWidth / 2);
	int moveHeight = (int)round((double)marginHeight / 2);

	printf("\n\nThis is temp_buffer specification \n");
	printf("marginWidth : %d , marginHeight : %d , moveWidth : %d, moveHeight : %d\n\n", marginWidth, marginHeight, moveWidth, moveHeight);

	move_RAWdata(old_buffer, temp_buffer, infoheader, fileheader, moveWidth, moveHeight, newSide);
	// �� �Լ����� ��������� ���� �����ȴ�
	printf("The extending process has been completed!\n\n");

	/*-----------------------------------*/
	// 4. �ݽð� �������� ��ŭ ȸ���� ���ΰ�? �������� �Է¹ޱ�
	/*-----------------------------------*/

	double angle;

	while (1)
	{
		printf("\nHow many angles do you want to enter? \nThe angles must always be integer : ");
		scanf("%lf", &angle);
		// ���� : double �������� %f�� ������ 4����Ʈ�� float ���·� �ޱ⿡ ��Ʋ����� ����� �޸𸮿��� �����Ⱚ�� ������ ���ۿ� ����.
		putchar('\n');

		if (isInteger(angle))
			break;
	}

	/*-----------------------------------*/
	// 5. Ȯ��� RAW �����͸� ������� ȸ����ȯ
	/*-----------------------------------*/

	rotate_RAWdata(temp_buffer, *new_buffer, infoheader, ANG2RAD(angle));
	// ����� double ���� �������� ������ �ȴ�. C�� �ﰢ�Լ��� ���ȸ��� �޴´�.

	/*-----------------------------------*/
	// 6. ���� �����ι���� �ƴ� ������ ����̱⿡, �ʿ������� hole�� �߻��� ���ۿ� ����.
	// �̸� ä���ֱ� ���� �����Լ��� ����
	/*-----------------------------------*/

	finding_holes(*new_buffer,infoheader);
	
release:
	if (temp_buffer != NULL) free(temp_buffer);

	return 0;
}

int move_RAWdata(BYTE* old_buffer, BYTE* temp_buffer, BITMAPINFOHEADER* infoheader, BITMAPFILEHEADER* fileheader, int moveWidth, int moveHeight, int newSide)
{
	// ���� ���ۿ��� temp ���۷� RAW �����͸� �����̵�
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

	// RAW�����͸� �Ű����� ��������� �����Ѵ�.
	infoheader->width = newSide;
	infoheader->height = newSide;
	infoheader->ImageSize = (DWORD)(pow(newSide, 2));
	fileheader->size = (DWORD)(infoheader->ImageSize) + fileheader->offset;

	return 0;
}

int isInteger(double angle)
{
	return floor(angle) == ceil(angle);
	// �������� �ø����� ������ ������� �Ǵ��� �� �ִ�.
}

int rotate_RAWdata(BYTE* temp_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, double radian)
{
	int Width = infoheader->width;
	int Height = infoheader->height;
	int tempX, tempY;

	double realX, realY;
	int amidWidth = Width / 2; int amidHeight = amidWidth; // ���簢���̱⿡

	double cosVal = cos(radian); double sinVal = sin(radian);
	// �̹��� ��ǥ������ ���� ������ ��ǥ������ y�� ��Ī������, RAW ���������� �Ųٷ� �Ǿ��ְ�
	// �� RAW �̹����� ȭ�鿡 �ҷ��ö��� ��Ģ������ ���� ������ȯ ������ �ᵵ ����� ����

	int newX, newY;

	printf("\nThe center location of Imnage : (X : %d , Y : %d)\n", amidWidth, amidHeight);
	printf("The cosVal : %lf, sinVal : %lf\n\n", cosVal, sinVal);

	// �̹����� �߽��� �������� ȸ���Ѵٴ� ���� ��������

	for (tempY = 0; tempY < Height; tempY++)
	{
		for (tempX = 0; tempX < Width; tempX++)
		{
			realX = (tempX - amidWidth) * cosVal - (tempY - amidHeight) * sinVal;
			realY = (tempX - amidWidth) * sinVal + (tempY - amidHeight) * cosVal;

			newX = (int)round(realX) + amidWidth;
			newY = (int)round(realY) + amidHeight;

			if (newX >= 0 && newX < Width && newY >= 0 && newY < Height) // ��ȿ�� ��ǥ�ΰ�?
				new_buffer[newY * Width + newX] = temp_buffer[tempY * Width + tempX];
		}
	}

	printf("A The image rotation has been completed to the angle you wanted!\n\n");

	return 0;
}

int finding_holes(BYTE* buffer, BITMAPINFOHEADER* infoheader)
{
	int X, Y;

	// RAW �ȼ����� ��ȸ�ϸ鼭 �ϴ� ��Ⱚ�� 0�� �ȼ��� ã�´�
	// 2������ ���ɼ��� �ִ�.
	// * �����Ҵ��Ҷ� �����ߴ� ���� or * ������ ���ο��� �߻��� Hole

	// Hole�� �߻��ϴ� ������ ��İ���� �� ��, �ݿø��� �Ͽ� ����ó���� �ϱ� �����̴�.
	// �׷��� ������ �������� x���� y���� ���������� hole�� �߻� �� �� ���ٰ� �Ǵ��ߴ�.

	for (Y = 0; Y < infoheader->height; Y++)
		for (X = 0; X < infoheader->width; X++)
			if (buffer[Y * infoheader->width + X] == 0)
				buffer[Y * infoheader->width + X] = MYnearest_interpolate(buffer, X, Y, infoheader->width, infoheader->height);

	printf("A hole searching and interpolation has been ended!\n\n");

	return 0;
}