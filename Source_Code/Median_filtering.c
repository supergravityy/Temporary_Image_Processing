#include "Processing.h"

int mid_filtering(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errCode)
{
	/*------------------------*/
	// 1. 어떻게 보면 미디언 필터링도 영역처리의 연장선이다
	// 영상의 픽셀 하나하나를 선택
	/*------------------------*/

	for (int h = 0; h < infoheader->height; h++)
		for (int w = 0; w < infoheader->width; w++)
			new_buffer[h * infoheader->width + w] = sorting(old_buffer, infoheader->height, infoheader->width, h, w);

	return 0;
}

BYTE sorting(BYTE* old_buffer, int height, int width, int h, int w)
{
	/*------------------------*/
	// 2. 커널은 너무 커서도 안되고, 너무 작아서도 안된다
	// 너무크면, 영상의 디테일이 떨어지고, 너무 작으면 제대로된 노이즈 처리가 불가능하다
	/*------------------------*/

	BYTE temp[25] = { 0 };
	int wrapped_h = 0, wrapped_w = 0, idx = 0;

	/*------------------------*/
	// 3. 선택한 픽셀주변의 모든 픽셀을 정렬할 배열에 삽입
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
	// 4. 배열 삽입이 끝나면, 배열을 오름차순으로 정렬한다
	/*------------------------*/
	for (int i = 0; i < 24; i++) // 기준은 얼마까지?
		for (int j = 0; j<25-1-i; j++) // 뒤는 이미 정렬이 되었기 때문에 고려 X
		// 앞의 인덱스들만 갖고 정렬
			if (temp[j + 1] < temp[j])
				swap(BYTE, temp[j], temp[j+1]); //교환

	// 정렬 수가 적기에, 버블정렬사용

	/*------------------------*/
	// 5. 정렬이 되었기에, 중간값을 반환
	/*------------------------*/

	return temp[13]; 
}