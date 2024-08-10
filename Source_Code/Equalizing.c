#include "Processing.h"


int histo_equalizing(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errCode)
{
	/*---------------------------------------*/
	// 1. 히스토그램 평활화에 필요한 초기화 과정
	/*---------------------------------------*/

	STASTICS bmp_Data; // 통계값을 확인하기위한 구조체 초기화

	double TEMP_ARR[MAX_BRIT_VAL+1] = { 0 }; 
	// 히스토그램 연산을 위한 배열 선언, 초기화
	// 이 배열의 인덱스번호 자체는 0~255까지의 밝기값과 똑같다.


	/*---------------------------------------*/
	// 2. 이미지의 픽셀값과 배열의 인덱스 번호가 서로 같기에, 빈도수를 설정한다.
	// 통계치를 구하고, 전체픽셀들 중, 밝기값에 대한 히스토그램을 출력
	/*---------------------------------------*/

	init_ARR(old_buffer, infoheader, TEMP_ARR, &bmp_Data);

	printf("\n-----------------------------------------");
	printf("\nmean : %.5f, variance : %.5f", bmp_Data.mean, bmp_Data.variance);
	printf("\n-----------------------------------------");
	printf("\n-----------------------------------------\n\n\n");

	write_hist(TEMP_ARR);


	/*for (int brit = 0; brit < MAX_BRIT_VAL; brit++)
		printf("(%d) %d   ", brit, (int)TEMP_ARR[brit]);*/

	/*---------------------------------------*/
	// 3. 히스토그램의 모든 빈도수를 CDF형태로 만든다
	// 이미지 크기로 나누어서, 전체 이미지에서 특정 밝기값이 나올 확률을 구한다. (정규화 과정)
	// 그리고, 해당 확률에 전체 밝기값을 곱하고, CDF화 시킨다
	// 마지막으로 이전 버퍼의 각 픽셀의 밝기값을 수정된 배열(처리된 밝기값)에 매핑하여 새 버퍼에 넣는다.
	/*---------------------------------------*/

	normalize_CDF(old_buffer, infoheader, TEMP_ARR, &bmp_Data);

	for (unsigned int idx = 0; idx < infoheader->ImageSize; idx++)
		new_buffer[idx] = (BYTE)TEMP_ARR[old_buffer[idx]];

	init_ARR(new_buffer, infoheader, TEMP_ARR, &bmp_Data);

	printf("\n-----------------------------------------");
	printf("\nmean : %.5f, variance : %.5f", bmp_Data.mean, bmp_Data.variance);
	printf("\n-----------------------------------------");
	printf("\n-----------------------------------------\n\n\n");

	write_hist(TEMP_ARR);

	/*for (int brit = 0; brit < MAX_BRIT_VAL; brit++)
		printf("(%d) %d   ", brit, (int)TEMP_ARR[brit]);*/

	printf("\n");


	return 0;
}

void init_ARR(BYTE* buffer, BITMAPINFOHEADER* infoheader, double* temp_arr, STASTICS* data)
{
	// 밝기값에 따라 빈도수 설정하기

	data->mean = 0; data->pow_sum =0; data->sum = 0; data->variance = 0;
	// 통계를 위한 구조체 전부 초기화

	for (unsigned int idx = 0; idx < infoheader->ImageSize; idx++)
	{
		for (int brit = 0; brit <= MAX_BRIT_VAL; brit++)
		{
			if (buffer[idx] == brit)
			{
				temp_arr[brit]++;
				data->sum += brit; // 모든 픽셀들의 밝기값의 합
				data->pow_sum += pow(brit, 2);
				break; // 픽셀의 밝기값을 찾았으니, 뛰어넘어버리자
			}
		}
	}

	data->mean = data->sum / infoheader->ImageSize;
	data->variance = (data->pow_sum / infoheader->ImageSize) - pow(data->mean, 2);
	// 히스토그램의 평균과 분산을 구해서 보기 위함
}

void normalize_CDF(BYTE* old_buffer, BITMAPINFOHEADER* infoheader, double* temp_arr, STASTICS* data)
{
	// 히스토그램의 모든 빈도수를 CDF로 만들자.
	// 밝기값에 따라 빈도수가 설정되었으니, 전체 픽셀수로 나눠서, 
	// 이미지에서 특정 밝기값을 뽑았을때의 전체 픽셀 중, 그 밝기값이 나올 확률을 구하자
	// 또한, 최대 밝기값을 곱하여, 새로운 밝기값으로 개념을 확장한다

	double scale_factor = (double)MAX_BRIT_VAL / (double)infoheader->ImageSize;
	int temp;
	double sum = 0;

	for (int brit = 0; brit <= MAX_BRIT_VAL; brit++)
	{
		sum += temp_arr[brit];
		temp = (int)round(sum * scale_factor); // 정규화 후, 자동반올림
		temp_arr[brit] = temp; // 다시 기존의 히스토그램 빈도수로 넣어놓기
	}
	// 이제 히스토그램 평활화가 거의 완료되었다.

	printf("\n\n");
}

void write_hist(double* temp_arr)
{
	static char status = 0;
	int count = 0;

	if (status == 0)
	{
		printf("\n\n---This is brightness value histogram of original---\n\n");
		status++;
	}
	else
		printf("\n\n---This is brightness value histogram of modified---\n\n");

	for (int brit = 0; brit <= MAX_BRIT_VAL; brit++)
	{
		count = (int)temp_arr[brit];
		if (brit >= 0 && brit < 10)
		{
			printf("%d   | ", brit);
			draw_bar(count);
		}

		else if (brit >= 10 && brit < 100)
		{
			printf("%d  | ", brit);
			draw_bar(count);
		}

		else
		{
			printf("%d | ", brit);
			draw_bar(count);
		}
	}

	printf("\n\n");
}