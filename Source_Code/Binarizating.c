#include "Processing.h"

int binarizating(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, unsigned int* errCode)
{
	// OTSU 알고리즘 -> 통계학을 이용해 배경과 피사체를 완벽히 분리할 수 있다


	/*----------------------------*/
	// 1. 제일먼저 히스토그램 초기화
	/*----------------------------*/

	STASTICS Data_image, Data_G1, Data_G2;

	double histogram[PXL_BRIT_NUM] = { 0 };

	init_ARR(old_buffer, infoheader, histogram, &Data_image); // 분산계산 다시보기
	
	init_Stastics(&Data_G1);
	init_Stastics(&Data_G2);

	/*----------------------------*/
	// 2. 원본의 통계값과 히스토그램 출력
	/*----------------------------*/

	printf("\n-----------------------------------------");
	printf("\nmean : %lf, variance : %lf", Data_image.mean, Data_image.variance);
	printf("\n-----------------------------------------");
	printf("\n-----------------------------------------\n\n\n");

	write_hist(histogram);

	printf("\n\n\n");
	for (int brit = 0; brit < PXL_BRIT_NUM; brit++)
		printf("(%d) %d   ", brit, (int)histogram[brit]);

	/*----------------------------*/
	// 3. 최적화를 위해 해당 히스토그램에서 유효한 범위를 찾고
	// 그 범위만큼의 배열 선언
	/*----------------------------*/

	int min = find_valid_Val('m', histogram);
	int max = find_valid_Val('M', histogram);
	printf("\n\nvalid Max Value : %d, valid Min Value : %d\n", max, min);

	int threshold=0; // 오츠 알고리즘에서 가장 핵심적인 역할
	int Diff = max - min; // 둘의 차이가 얼마인가?

	double* Variances = (double*)malloc(sizeof(double) * Diff);
	// 문턱값에 의한 두 클래스내의 분산을 저장할 용도

	if (Variances == NULL)
	{
		printf("Cannot allocate memory section!\n\n");
		*errCode = 2;
		return 0;
	}

	/*----------------------------*/
	// 4. 문턱값을 유효범위내에서 움직여가며, 두 클래스내의 분산 찾기
	/*----------------------------*/

	double var_G1,var_G2;
	int temp_Th;
	int Width = infoheader->width;
	int Height = infoheader->height;

	for (temp_Th = 0; temp_Th < Diff; temp_Th++)
	// 여기의 임시 문턱값은 분산배열의 인덱스 요소도 하기에 일부러 이렇게 선언
	{
		var_G1 = within_class_Var(histogram, 0, min + temp_Th - 1, Width, Height,&Data_G1);
		var_G2 = within_class_Var(histogram, min + temp_Th, max, Width, Height, &Data_G2);

		printf("At threshold(%d), G1 VAR : %lf, G2 VAR : %lf \n", temp_Th + min, var_G1, var_G2);
		Variances[temp_Th] = var_G1 + var_G2;
		printf("VAR within class : %lf\n", Variances[temp_Th]);

		init_Stastics(&Data_G1);
		init_Stastics(&Data_G2);
	}

	/*----------------------------*/
	// 5. 문턱값에 따른 클래스내 분산의 최소값을 찾고, 매칭되는 문턱값 탐색
	/*----------------------------*/

	// 일단 클래스내의 분산의 최소값이 두 클래스의 요소들이 얼마나 단단히 뭉쳐있냐의 뜻이기에 최소값을 찾아야함

	int idx=0;
	double Var_min = Variances[0];

	// 최소 분산값 찾기

	for (; idx < Diff; idx++)
		if (Var_min > Variances[idx])
			Var_min = Variances[idx];  

	// 이것과 매칭되는 threshold값 찾기

	for (idx = 0; idx < Diff; idx++)
	{
		if (Var_min == Variances[idx])
		{
			threshold = idx;  // 인덱스에 Min을 더하여 실제 임계값을 찾음
			break;
		}
	}

	threshold += min;

	printf("\n\nThe Minimum VAR : %lf, The threshold : %d\n\n", Var_min, threshold);

	/*----------------------------*/
	// 6. 문턱값을 기준으로 이진화
	/*----------------------------*/

	Otsu_final(old_buffer, new_buffer, (BYTE)threshold, infoheader);

	free(Variances);
	return 0;
}

int Otsu_final(BYTE* old_buffer, BYTE* new_buffer, BYTE threshold, BITMAPINFOHEADER* infoheader)
{
	int X, Y;
	int Width = infoheader->width; int Height = infoheader->height;
	for (Y = 0; Y < Height; Y++)

		for (X = 0; X < Width; X++)

			(old_buffer[Y * Width + X] > threshold) ? (new_buffer[Y * Width + X] = (BYTE)MAX_BRIT_VAL) : (new_buffer[Y * Width + X] = (BYTE)0);

	// 문턱값 아래의 픽셀값은 모두 0, 위의 픽셀은 모두 255 

	return 0;
}

int find_valid_Val(char mode, double* histogram)
{
	// 이미지의 유효한 밝기값의 범위를 찾기위한 함수
	int seeker;

	if (mode == 'm') // 최소 유효밝기값 탐색
	{
		for (seeker = 0; seeker < MAX_BRIT_VAL; seeker++)
		{
			if (histogram[seeker] != 0)
			{
				return seeker;
			}
		}
	}
	else // 최대 유효 밝기값 탐색
	{
		for (seeker = MAX_BRIT_VAL; seeker > 0; seeker--)
		{
			if (histogram[seeker] != 0)
			{
				return seeker;
			}
		}
	}
}

double within_class_Var(double* histogram, int min, int max, int Width, int Height, STASTICS* classData)
{
	int brit;

	for (brit = min; brit <= max; brit++)
	{
		classData->sum += histogram[brit]; // 통계값의 sum은 클래스 전체를 의미한다 -> 나누기 요소
		classData->mean += brit * histogram[brit];
		classData->pow_sum += pow(brit, 2) * histogram[brit];
	}

	if (classData->sum == 0)
		return 0; // 0나누기 피하기

	classData->mean /= classData->sum;
	classData->variance = (classData->pow_sum / classData->sum) - pow(classData->mean, 2);

	return classData->variance;
}

void init_Stastics(STASTICS* Data)
{
	Data->mean = 0;
	Data->pow_sum = 0;
	Data->sum = 0;
	Data->variance = 0;

	return;
}