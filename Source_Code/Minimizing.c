#include "Processing.h"

int minimizing(BYTE* old_buffer, BYTE** new_buffer, BITMAPINFOHEADER* infoheader, BITMAPFILEHEADER* fileheader,int* errCode)
{
	/*---------------------------------------------*/
	// 1. ���ϸ��� �Ұ��� ���� ũ��� ����� �������̴�. ����ڿ��� �Է��� �޴´� (0~100)
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
	// 2. �� ũ�⿡ ���缭, ������ ũ��� ������ ���μ��� ������ ����Ѵ�.
	/*---------------------------------------------*/

	newSize = newWidth * newHeight;

	printf("The new width : %d , The new height : %d, Finally new Imagesize : %d\n", newWidth, newHeight, newSize);

	double width_Ratio = (double)infoheader->width / (double)newWidth;
	double height_Ratio = (double)infoheader->height / (double)newHeight;

	printf("The width ratio : %.3f, The height ratio : %.3f\n", width_Ratio, height_Ratio);

	/*---------------------------------------------*/
	// 3. �ش� ��갪�� ����� �־� �ùٸ��� �����Ѵ�
	/*---------------------------------------------*/

	int oldWidth = infoheader->width; // ���� raw ������ �ε��� ��꿡 �ʿ��ϴ�.

	infoheader->width = newWidth;
	infoheader->height = newHeight;
	infoheader->ImageSize = newSize;
	fileheader->size = infoheader->ImageSize + fileheader->offset;

	/*---------------------------------------------*/
	// 4. �ش� ��갪�� ���缭 new_buffer�� ũ�⸦ ���Ҵ� ���ش�.
	/*---------------------------------------------*/

	BYTE* new_addr = realloc(*new_buffer, sizeof(char) * newSize);

	if (new_addr == NULL)
	{
		printf("realloc Error Occured! \n");
		*errCode = 2;
		return 0;
	}

	*new_buffer = new_addr; // �� �����̾ �����Լ� ���������� ó���ص� ����� ����.

	/*---------------------------------------------*/
	// 5. new_buffer�� ��ǥ���� �������� �Ͽ�, ���ο� ũ�� ����� ����ߴ� ������ �̿�
	// old_buffer�� ��ǥ���� �����Ͽ�, new_buffer�� ä���ִ´�.
	/*---------------------------------------------*/

	int tempX; int tempY;

	for (int y = 0; y < newHeight; y++)
	{
		tempY = (int)floor(y * height_Ratio);
		// ratio = old/new �̱⿡, ���⿡ new�� �����ָ�, old�� �ε����� ���´�.

		for (int x = 0; x < newWidth; x++)
		{
			tempX = (int)floor(x * width_Ratio);
			(*new_buffer)[y * newWidth + x] = old_buffer[tempY * oldWidth + tempX];
			// ���ϸ��� �����͸��� �� �ش���, �迭 ������ �������!!!
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

		if (remain != 0) // �̹��� �б� ����ȭ�� ���ؼ� �ʺ�� ������ 4�� ����ϰ�!!!
		{
			if (*newWidth < (infoheader->width) / 2) // 0�� ����� Ȯ�� ����
			{
				remain = 4 - remain;
				*newWidth += remain;
			}
			else // ���� �ʺ� ũ�⿡ ����� Ȯ���� ����
				*newWidth -= remain;
		}

		width_check = 0;
	}
		

	if (*newHeight < infoheader->height && *newHeight > 0)
		height_check = 0;

	return width_check || height_check;
}