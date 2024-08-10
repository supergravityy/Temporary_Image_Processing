#include "Processing.h"

int sharpening(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errCode)
{
	/*--------------------------------*/
	// 1. ���ϴ� Ŀ�� ����
	/*--------------------------------*/

	int option;

	while (1)
	{
		printf("The filter size is fixed at 3X3\n"); // ������ Ŀ���� �� Ŀ���� �ű⼭ �ű��̴�.
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
	// 2. ���ϴ� Ŀ���� ����
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
	// 3. Ŀ�� ����ȭ & ������ ���� ��ȿ���� Ȯ��
	/*--------------------------------*/

	// ������ ���ʹ� ������ 1�̴� -> ȭ�ҵ��� ��ü���� ���� �����ϵ�, �����ϵ��� �츰��

	normalize_filter(kernel, KERNEL33/3, 1);

	/*--------------------------------*/
	// 5. Ŀ�ε��� ������ y�� & x�� ��Ī�̱� ������, �и��� �������� �ʴ�
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
	// 6. �����ϱ�
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
	

	// ���� �������� Ŀ���̴�.

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
	// 4. ������ ������ Ŀ�ε��� ��� �����ͺô�. ���� Ŀ��
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

	// ������ ������� ����� �ۼ��Ǿ����� Ȯ��

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