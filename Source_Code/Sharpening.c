#include "Processing.h"

int sharpening(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errCode)
{
	/*--------------------------------*/
	// 1. 원하는 커널 선택
	/*--------------------------------*/

	int option;

	while (1)
	{
		printf("The filter size is fixed at 3X3\n"); // 어차피 커널이 더 커져도 거기서 거기이다.
		printf("1. Fully high-frequency pass filter \n");
		printf("2. High-frequency pass filter\n");
		printf("3. Fully high-frequency filter with fractional values\n");

		printf("\nChoose your kernel! : ");

		scanf("%d", &option);
		putchar('\n');

		if (option > 0 && option < 4)
			break;
	}

	/*--------------------------------*/
	// 2. 원하는 커널을 생성
	/*--------------------------------*/

	double* kernel = NULL;

	switch (option)
	{
	case 1:
		kernel = gen_fHF_kernel();
		break;

	case 2:
		kernel = gen_HF_kernel();
		break;
	case 3:
		kernel = gen_fHFfs_kernel();
		break;
	}

	if (kernel == NULL)
	{
		printf("memory allocation failed! at making kernel process!\n");
		*errCode = 2;
		goto release;
	}

	/*--------------------------------*/
	// 3. 커널 정규화 & 필터의 합이 유효한지 확인
	/*--------------------------------*/

	// 샤프닝 필터는 총합이 1이다 -> 화소들의 전체적인 밝기는 유지하되, 디테일들을 살린다

	normalize_filter(kernel, KERNEL33/3, 1);

	/*--------------------------------*/
	// 5. 커널들은 무조건 y축 & x축 대칭이긴 하지만, 분리가 가능하지 않다
	/*--------------------------------*/

	int result = 0;

	int X, Y;
	int Width = infoheader->width;
	int Height = infoheader->height;

	for (Y = 0; Y < Height; Y++)
	{
		for (X = 0; X < Width; X++)
		{
			new_buffer[Y * Width + X] = regular_cal(old_buffer,kernel,X,Y,infoheader, KERNEL33 / 3);
		}
	}

	/*--------------------------------*/
	// 6. 정리하기
	/*--------------------------------*/

release:

	if (kernel != NULL); free(kernel);
	return 0;
}

double* gen_fHF_kernel()
{
	double* fHF_kernel = (double*)malloc(sizeof(double) * KERNEL33);
	
	double temp[KERNEL33] = { -1.,-1.,-1.,-1.,9.,-1.,-1.,-1.,-1. };

	for (int i = 0; i < KERNEL33; i++) 
		fHF_kernel[i] = temp[i];
	

	// 가장 안정적인 커널이다.

	return fHF_kernel;
}

double* gen_HF_kernel()
{
	double* HF_kernel = (double*)malloc(sizeof(double) * KERNEL33);
	
	double temp[KERNEL33] = { 0.,-1.,0.,-1.,5.,-1.,0.,-1.,0. };

	for (int i = 0; i < KERNEL33; i++) 
		HF_kernel[i] = temp[i];
	

	return HF_kernel;
}

double* gen_fHFfs_kernel()
{
	/*--------------------------------*/
	// 4. 유명한 샤프닝 커널들을 몇가지 가져와봤다. 고정 커널
	/*--------------------------------*/

	double* fHFfs_kernel = (double*)malloc(sizeof(double) * KERNEL33);

	double temp[KERNEL33] = { -1. / 3.,1. / 3.,-1. / 3.,-1. / 3.,7. / 3.,-1. / 3. ,-1. / 3. ,1. / 3. ,-1. / 3. };

	for (int i = 0; i < KERNEL33; i++)
		fHFfs_kernel[i] = temp[i];

	return fHFfs_kernel;
}

void normalize_filter(double* kernal, int Sidesize, int coeff)
{
	int scailing = (int)pow(coeff, 2);
	double sum = 0.0;

	for (int i = 0; i < Sidesize; i++)
	{
		for (int j = 0; j < Sidesize; j++)
		{
			kernal[i * Sidesize + j] /= scailing;
			sum += kernal[i * Sidesize + j];
		}
	}

	// 필터의 계수들이 제대로 작성되었는지 확인

	printf("\nFilter coefficient \n\n");

	for (int i = 0; i < Sidesize; i++)
	{
		for (int j = 0; j < Sidesize; j++)
			printf("%lf\t", *(kernal + i * Sidesize + j));

		printf("\n");
	}
		

	if (sum == 1)
		printf("\nThe sum of the kernels is 1!\n\n");

	else
		printf("\nThe sum of the kernels is NOT 1!\n\n");
}