#include "Processing.h"

int mid_filtering(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errCode)
{
	/*------------------------*/
	// 1. ��� ���� �̵�� ���͸��� ����ó���� ���弱�̴�
	// ������ �ȼ� �ϳ��ϳ��� ����
	/*------------------------*/

	for (int h = 0; h < infoheader->height; h++)
		for (int w = 0; w < infoheader->width; w++)
			new_buffer[h * infoheader->width + w] = sorting(old_buffer, infoheader->height, infoheader->width, h, w);

	return 0;
}

BYTE sorting(BYTE* old_buffer, int height, int width, int h, int w)
{
	/*------------------------*/
	// 2. Ŀ���� �ʹ� Ŀ���� �ȵǰ�, �ʹ� �۾Ƽ��� �ȵȴ�
	// �ʹ�ũ��, ������ �������� ��������, �ʹ� ������ ����ε� ������ ó���� �Ұ����ϴ�
	/*------------------------*/

	BYTE temp[25] = { 0 };
	int wrapped_h = 0, wrapped_w = 0, idx = 0;

	/*------------------------*/
	// 3. ������ �ȼ��ֺ��� ��� �ȼ��� ������ �迭�� ����
	/*------------------------*/

	for (int i = h - 2; i < h + 3; i++)
	{
		wrapped_h = circular_wrapping(i, height);

		for (int j = w - 2; j < w + 3; j++)
		{
			wrapped_w = circular_wrapping(j, width);
			temp[idx] = old_buffer[wrapped_h * width + wrapped_w];
			idx++;
		}
	}

	/*------------------------*/
	// 4. �迭 ������ ������, �迭�� ������������ �����Ѵ�
	/*------------------------*/
	for (int i = 0; i < 24; i++) // ������ �󸶱���?
		for (int j = 0; j<25-1-i; j++) // �ڴ� �̹� ������ �Ǿ��� ������ ��� X
		// ���� �ε����鸸 ���� ����
			if (temp[j + 1] < temp[j])
				swap(BYTE, temp[j], temp[j+1]); //��ȯ

	// ���� ���� ���⿡, �������Ļ��

	/*------------------------*/
	// 5. ������ �Ǿ��⿡, �߰����� ��ȯ
	/*------------------------*/

	return temp[13]; 
}