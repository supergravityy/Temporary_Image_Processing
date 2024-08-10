#include "Processing.h"

int edge_detecting(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errCode)
{
	/*---------------------------*/
	// 1. ���ϴ� Ŀ�� ����
	/*---------------------------*/

	int option;

	while (1)
	{
		printf("Filters have their own size!\n");
		printf("1. Roberts edge \n");
		printf("2. Prewitt edge\n");
		printf("3. Sobel edge\n");
		printf("4. 2nd Laplacian_1\n");
		printf("5. 2nd Laplacian_2\n");
		printf("6. Laplacian of Gaussian\n");
		printf("7. Difference of Gaussian\n");

		// ���� : 1��,2��,4��,5���� �������⿡ �ʿ��� ��ó�� ����(�ణ�� ���� ����)�� �������� �ʱ⿡
		// ���� ������ ����ϰ� (ǥ���������� �۰�) �ɾ��ְ� �۾��Ұ�

		// ������ Ŀ���� �� ��ü������ ���� ȿ���� ������.

		// ����2 : �ι����� ������,�Һ�

		printf("\nChoose your kernel! : ");

		scanf("%d", &option);
		putchar('\n');

		if (option > 0 && option < 8)
			break;
	}

	int Sizeside;

	if (option > 0 && option < 6)
		Sizeside = KERNEL33 / 3;

	else if (option >= 6 && option < 8)
		Sizeside = KERNEL77 / 7;

	/*---------------------------*/
	// 2. ���ϴ� Ŀ���� ����
	/*---------------------------*/

	double* kernel_1st = NULL;
	double* kernel_2nd = NULL;

	switch (option)
	{
	case 1:
		kernel_1st = gen_Roberts_kernel();
		kernel_2nd = gen_Roberts_kernel();
		break;

	case 2:
		kernel_1st = gen_Prewitt_kernel();
		kernel_2nd = gen_Prewitt_kernel();
		break;

	case 3:
		kernel_1st = gen_Sobel_kernel();
		kernel_2nd = gen_Sobel_kernel();
		break;

	case 4:
		kernel_1st = gen_2nd_Laplacian_kernel_1();
		break;

	case 5:
		kernel_1st = gen_2nd_Laplacian_kernel_2();
		break;

	case 6:
		kernel_1st = gen_LoG(Sizeside);
		break;

	case 7:
		kernel_1st = gen_DoG(Sizeside);
		break;
	}

	if (kernel_1st == NULL || (option <= 3 && kernel_2nd == NULL))
	{
		printf("memory allocation failed! at making kernel process!\n");
		*errCode = 2;
		goto release;
	}

	putchar('\n');
	putchar('\n');

	/*---------------------------*/
	// 3. ������ Ŀ�η� �̹����� ������� �Ͽ� ��������
	/*---------------------------*/

	int Y, X;
	int Height = infoheader->height;
	int Width = infoheader->width;

	for (Y = 0; Y < Height; Y++)
		for (X = 0; X < Width; X++)
			new_buffer[Y * Width + X] = regular_cal(old_buffer, kernel_1st, X, Y, infoheader, Sizeside);


	if (option < 4) // 3���ϴ� �ι� ���͸� ��ħ
	{
		for (Y = 0; Y < Height; Y++)
			for (X = 0; X < Width; X++)
				new_buffer[Y * Width + X] = regular_cal(old_buffer, kernel_2nd, X, Y, infoheader, Sizeside);
	}

	/*---------------------------*/
	// 4. ����
	/*---------------------------*/

release:

	if (kernel_1st != NULL)
		free(kernel_1st);

	if (kernel_2nd != NULL)
		free(kernel_2nd);

	return 0;
}

double* gen_Roberts_kernel()
{
	static tryNum = 0;

	double* roberts = (double*)malloc(sizeof(double) * KERNEL33);

	if (tryNum == 0)
	{
		printf("It is a first Roberts kernel!"); // ù��°

		double temp[KERNEL33] = { -1,0,0,0,1,0,0,0,0 };

		for (int idx = 0; idx < KERNEL33; idx++)
			roberts[idx] = temp[idx];

		tryNum++;

		return roberts;
	}

	else
	{
		printf("It is a second Roberts kernel!"); // �ι�°

		double temp[KERNEL33] = { 0,0,-1,0,1,0,0,0,0 };

		for (int idx = 0; idx < KERNEL33; idx++)
			roberts[idx] = temp[idx];

		return roberts;
	}
}

double* gen_Prewitt_kernel()
{
	static tryNum = 0;

	double* prewitt = (double*)malloc(sizeof(double) * KERNEL33);

	if (tryNum == 0)
	{
		printf("It is a first Prewitt kernel!"); // ù��°

		double temp[KERNEL33] = { -1,0,1,-1,0,1,-1,0,1 };

		for (int idx = 0; idx < KERNEL33; idx++)
			prewitt[idx] = temp[idx];

		tryNum++;

		return prewitt;
	}

	else
	{
		printf("It is a second Prewitt kernel!"); // �ι�°

		double temp[KERNEL33] = { 1,1,1,0,0,0,-1,-1,-1 };

		for (int idx = 0; idx < KERNEL33; idx++)
			prewitt[idx] = temp[idx];

		return prewitt;
	}
}

double* gen_Sobel_kernel()
{
	static tryNum = 0;

	double* sobel = (double*)malloc(sizeof(double) * KERNEL33);

	if (tryNum == 0)
	{
		printf("It is a first Sobel kernel!"); // ù��°

		double temp[KERNEL33] = { -1,0,1,-2,0,2,-1,0,1 };

		for (int idx = 0; idx < KERNEL33; idx++)
			sobel[idx] = temp[idx];

		tryNum++;

		return sobel;
	}

	else
	{
		printf("It is a second Sobel kernel!"); // �ι�°

		double temp[KERNEL33] = { 1,2,1,0,0,0,-1,-2,-1 };

		for (int idx = 0; idx < KERNEL33; idx++)
			sobel[idx] = temp[idx];

		return sobel;
	}

	// �ι���, ������, �Һ����ʹ� 1������ 
}

double* gen_2nd_Laplacian_kernel_1()
{
	double* Lp_kernel = (double*)malloc(sizeof(double) * KERNEL33);

	double temp[KERNEL33] = { 0,-1,0,-1,4,-1,0,-1,0 };

	for (int idx = 0; idx < KERNEL33; idx++)
		Lp_kernel[idx] = temp[idx];

	return Lp_kernel;
}

double* gen_2nd_Laplacian_kernel_2()
{
	double* Lp_kernel = (double*)malloc(sizeof(double) * KERNEL33);

	double temp[KERNEL33] = { -1,-1,-1,-1,8,-1,-1,-1,-1 };

	for (int idx = 0; idx < KERNEL33; idx++)
		Lp_kernel[idx] = temp[idx];

	return Lp_kernel;
}

double* gen_LoG(int Sizeside)
{
	/*--- Ŀ�� �����Ҵ�, ���꿡 �ʿ��� ������ ���� ---*/

	double* LoG_kernel = (double*)malloc(sizeof(double) * KERNEL77);
	double sum = 0.;
	int Radius = Sizeside / 2;
	int Y, X;
	int idx;
	double Front, Back;

	/*--- ��ȿ�� �ñ׸� �Է¹ޱ� ---*/

	double sigma;

	while (1)
	{
		printf("\nIt should be less than 7/6 (=%f), because one side of this kernel is 7!", (float)(7. / 6.));
		printf("\nPlease enter a sigma value : ");

		scanf("%lf", &sigma);

		if (sigma < (double)(7. / 6.))
			break;
	}

	double sigma_pow = pow(sigma, 2);
	double Constant = 1.0 / (2 * M_PI * sigma_pow);

	/*--- �� �������� ������ 2D ����þ�Ŀ���� 2���̺� (LoG) ����� ---*/

	printf("This is LoG kernel elements\n");

	for (Y = -Radius; Y <= Radius; Y++)
	{
		for (X = -Radius; X <= Radius; X++)
		{
			idx = (Y + Radius) * Sizeside + (X + Radius);

			Front = (X * X + Y * Y - 2 * sigma_pow) / (sigma_pow * sigma_pow);
			Back = exp(-(X * X + Y * Y) / (2 * sigma_pow));

			LoG_kernel[idx] = Constant * Front * Back;
			sum += LoG_kernel[idx];
		}
	}

	for (int y = 0; y < Sizeside; y++)
	{
		for (int x = 0; x < Sizeside; x++)
			printf("%lf\t", LoG_kernel[y * Sizeside + x]);

		putchar('\n');
	}

	printf("\nThe LoG kernel\'s' sum : %lf\n", sum);
	// ���� �������̱⿡, ���� 0�̵Ǿ�� �Ѵ�

	/*--- ���� 0�� �ƴ϶��, ������ 0���� ������ֱ� ---*/

	if (fabs(sum) > 1e-10)
	{
		printf("Kernel sum is not 0!\nForce it close to zero !\n");

		for (int y = 0; y < Sizeside; y++)
			for (int x = 0; x < Sizeside; x++)
				LoG_kernel[y * Sizeside + x] -= sum / (double)KERNEL77;

		printf("\nAdjusted LoG Kernel : \n");
		sum = 0.;

		for (int y = 0; y < Sizeside; y++)
		{
			for (int x = 0; x < Sizeside; x++)
			{
				printf("%lf\t", LoG_kernel[y * Sizeside + x]);
				sum += LoG_kernel[y * Sizeside + x];
			}
			putchar('\n');
		}

		printf("\nThe adjusted LoG kernel\'s' sum : %lf\n", sum);
	}

	return LoG_kernel;
}

double* gen_DoG(int Sizeside)
{
	/*--- Ŀ�� �����Ҵ�, ���꿡 �ʿ��� ������ ���� ---*/

	double* DoG_kernel = (double*)malloc(sizeof(double) * KERNEL77);
	int Radius = Sizeside / 2;

	/*--- ��ȿ�� �ñ׸� �Է¹ޱ� ---*/

	double sigma;
	printf("DoG is a kernel for approximating LoG to speed up computation\n");
	printf("Which one would you set up the Gaussian sigma of LoG ?\n");
	printf("It automatically calculates the two sigma required for DoG!\n");
	// DoG�� ���ϴ� �ñ׸� ���� ���� LoG�� �ϼ��ϱ� ���� ���� ����Ͽ�, ���δٸ� �ñ׸��� ����ؼ� ���� �Ѵ�

	while (1)
	{
		printf("\nIt should be less than 7/6 (=%f), because one side of this kernel is 7!", (float)(7. / 6.));
		printf("Please enter a sigma value : ");

		scanf("%lf", &sigma);

		if (sigma < (double)(7. / 6.) && sigma > 0)
			break;

	}

	/*--- �ش� LoG �ñ׸� ���� �´� �ΰ��� �ñ׸� ���� ���ϱ� ---*/

	// DoG�� LoG�� �ٻ�ȭ�Ѱ�ó�� ����ϰ� �Ƿ���, DoG�� LoG�� ���� ����ũ�ν��� ������ �Ѵ�. -> �������� ������

	double sigma1, sigma2;

	sigma2 = sigma / (2.56 / 1.56 * log(2.56));
	sigma1 = sigma2 * 1.6; // �� �ñ׸��� �� 1.6:1 �ϰ� (���������� ������ �Ұ��� �� �� ����)

	double Constant1 = 2 * M_PI * sigma1 * sigma1;
	double Constant2 = 2 * M_PI * sigma2 * sigma2;

	printf("\nTwo sigma values : sigma1 (%lf), sigma2 (%lf)\n", sigma1, sigma2);

	/*--- ���� DoG Ŀ�� ���ϱ� ---*/

	double GAU1, GAU2;
	int Y, X;
	int idx;
	double sum = 0.;
	double element;
	printf("\nThis is DoG kernel elements\n");
	printf("But it can NOT work as a LoG substitute perfect!\n");

	for (Y = -Radius; Y <= Radius; Y++)
	{
		for (X = -Radius; X <= Radius; X++)
		{
			idx = (Y + Radius) * Sizeside + (X + Radius);
			GAU1 = exp(-(X * X + Y * Y) / (2 * sigma1)) / Constant1;
			GAU2 = exp(-(X * X + Y * Y) / (2 * sigma2)) / Constant2;
			element = GAU1 - GAU2;
			DoG_kernel[idx] = element;
		}
	}

	printf("This is original DoG kernel\n");

	for (Y = 0; Y < Sizeside; Y++)
	{
		for (X = 0; X < Sizeside; X++)
		{
			printf("%lf\t", DoG_kernel[Y * Sizeside + X]);
			sum += DoG_kernel[Y * Sizeside + X];
		}

		putchar('\n');
	}
	printf("\nThe adjusted DoG kernel\'s' sum : %lf\n\n\n", sum);


	/*--- �׷��� LoG Ŀ�ΰ� DoG Ŀ���� ���� ũ�ν̸� ����, ũ�� �������� �ٸ��� ---*/

	// LoG Ŀ�� �߽� ���Ҹ� ���ؼ� DoG Ŀ�� �߽� ���ҷ� �������� ������ factor�� ����
	// ��� DoGĿ�ΰ��� ���ҿ� �����ֱ�
	
	printf("\n\nWe need to adjust the matrix scalar to be a substitute!\n");

	double sigma_pow = sigma * sigma;
	double Constant = 1.0/(2 * M_PI * sigma_pow);
	X = 0; Y = 0;
	double Front = (X * X + Y * Y - 2 * sigma_pow) / (sigma_pow * sigma_pow);
	double Back = exp(-(X * X + Y * Y) / (2 * sigma_pow));

	double center_LoG = Constant * Front * Back;

	double scail_factor = center_LoG / DoG_kernel[Radius * Sizeside + Radius];

	printf("This is LoG kernel\'s center element : %lf, and scail factor is %lf\n", center_LoG, scail_factor);
	printf("And This is rescailed DoG kernel elements\n");

	for (Y = 0; Y < Sizeside; Y++)
	{
		for (X = 0; X < Sizeside; X++)
		{
			DoG_kernel[Y * Sizeside + X] *= scail_factor;
			sum += DoG_kernel[Y * Sizeside + X];
		}
	}

	for (int y = 0; y < Sizeside; y++)
	{
		for (int x = 0; x < Sizeside; x++)
			printf("%lf\t", DoG_kernel[y * Sizeside + x]);

		putchar('\n');
	}

	printf("The sum is %lf\n\n\n", sum);
	
	/*---------------------------------------�ذ�Ϸ� �����ص���------------------------------------*/
	// ���� : DoG Ŀ���� LoG Ŀ�ΰ� ���߱� ���ؼ�, �����ϸ��� �ص� LoG Ŀ���� �� �� ����.
	// ������ ���Ѱ� LoG Ŀ�η� ó���� �Ͱ� ��������� 

	/*--- ���� 0�� �ƴ϶��, ������ 0���� ������ֱ� ---*/


	while(fabs(sum) > 1e-10) // �� �ѹ����δ� �Ϻ��ϰ� 0���� ����� ����. �ε��Ҽ����̶�
	{
		printf("Kernel sum is not 0!\nForce it close to zero!\n");

		for (int y = 0; y < Sizeside; y++)
		{
			for (int x = 0; x < Sizeside; x++)
			{
				DoG_kernel[y * Sizeside + x] -= sum / (double)KERNEL77;
			}
		}

		printf("\nAdjusted DoG Kernel:\n");
		sum = 0;

		for (int y = 0; y < Sizeside; y++)
		{
			for (int x = 0; x < Sizeside; x++)
			{
				printf("%lf\t", DoG_kernel[y * Sizeside + x]);
				sum += DoG_kernel[y * Sizeside + x];
			}
			putchar('\n');
		}

		printf("\nThe adjusted DoG kernel\'s' sum : %lf\n\n\n", sum);
	}

	return DoG_kernel;
}