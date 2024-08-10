#include "Processing.h"

int binarizating(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, unsigned int* errCode)
{
	// OTSU �˰��� -> ������� �̿��� ���� �ǻ�ü�� �Ϻ��� �и��� �� �ִ�


	/*----------------------------*/
	// 1. ���ϸ��� ������׷� �ʱ�ȭ
	/*----------------------------*/

	STASTICS Data_image, Data_G1, Data_G2;

	double histogram[PXL_BRIT_NUM] = { 0 };

	init_ARR(old_buffer, infoheader, histogram, &Data_image); // �л��� �ٽú���
	
	init_Stastics(&Data_G1);
	init_Stastics(&Data_G2);

	/*----------------------------*/
	// 2. ������ ��谪�� ������׷� ���
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
	// 3. ����ȭ�� ���� �ش� ������׷����� ��ȿ�� ������ ã��
	// �� ������ŭ�� �迭 ����
	/*----------------------------*/

	int min = find_valid_Val('m', histogram);
	int max = find_valid_Val('M', histogram);
	printf("\n\nvalid Max Value : %d, valid Min Value : %d\n", max, min);

	int threshold=0; // ���� �˰��򿡼� ���� �ٽ����� ����
	int Diff = max - min; // ���� ���̰� ���ΰ�?

	double* Variances = (double*)malloc(sizeof(double) * Diff);
	// ���ΰ��� ���� �� Ŭ�������� �л��� ������ �뵵

	if (Variances == NULL)
	{
		printf("Cannot allocate memory section!\n\n");
		*errCode = 2;
		return 0;
	}

	/*----------------------------*/
	// 4. ���ΰ��� ��ȿ���������� ����������, �� Ŭ�������� �л� ã��
	/*----------------------------*/

	double var_G1,var_G2;
	int temp_Th;
	int Width = infoheader->width;
	int Height = infoheader->height;

	for (temp_Th = 0; temp_Th < Diff; temp_Th++)
	// ������ �ӽ� ���ΰ��� �л�迭�� �ε��� ��ҵ� �ϱ⿡ �Ϻη� �̷��� ����
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
	// 5. ���ΰ��� ���� Ŭ������ �л��� �ּҰ��� ã��, ��Ī�Ǵ� ���ΰ� Ž��
	/*----------------------------*/

	// �ϴ� Ŭ�������� �л��� �ּҰ��� �� Ŭ������ ��ҵ��� �󸶳� �ܴ��� �����ֳ��� ���̱⿡ �ּҰ��� ã�ƾ���

	int idx=0;
	double Var_min = Variances[0];

	// �ּ� �л갪 ã��

	for (; idx < Diff; idx++)
		if (Var_min > Variances[idx])
			Var_min = Variances[idx];  

	// �̰Ͱ� ��Ī�Ǵ� threshold�� ã��

	for (idx = 0; idx < Diff; idx++)
	{
		if (Var_min == Variances[idx])
		{
			threshold = idx;  // �ε����� Min�� ���Ͽ� ���� �Ӱ谪�� ã��
			break;
		}
	}

	threshold += min;

	printf("\n\nThe Minimum VAR : %lf, The threshold : %d\n\n", Var_min, threshold);

	/*----------------------------*/
	// 6. ���ΰ��� �������� ����ȭ
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

	// ���ΰ� �Ʒ��� �ȼ����� ��� 0, ���� �ȼ��� ��� 255 

	return 0;
}

int find_valid_Val(char mode, double* histogram)
{
	// �̹����� ��ȿ�� ��Ⱚ�� ������ ã������ �Լ�
	int seeker;

	if (mode == 'm') // �ּ� ��ȿ��Ⱚ Ž��
	{
		for (seeker = 0; seeker < MAX_BRIT_VAL; seeker++)
		{
			if (histogram[seeker] != 0)
			{
				return seeker;
			}
		}
	}
	else // �ִ� ��ȿ ��Ⱚ Ž��
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
		classData->sum += histogram[brit]; // ��谪�� sum�� Ŭ���� ��ü�� �ǹ��Ѵ� -> ������ ���
		classData->mean += brit * histogram[brit];
		classData->pow_sum += pow(brit, 2) * histogram[brit];
	}

	if (classData->sum == 0)
		return 0; // 0������ ���ϱ�

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