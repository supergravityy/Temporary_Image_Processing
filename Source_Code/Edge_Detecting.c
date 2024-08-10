#include "Processing.h"

int edge_detecting(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errCode)
{
	/*---------------------------*/
	// 1. 원하는 커널 선택
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

		// 주의 : 1번,2번,4번,5번은 엣지검출에 필요한 전처리 과정(약간의 블러링 과정)을 포함하지 않기에
		// 먼저 블러링을 약소하게 (표준편차값을 작게) 걸어주고 작업할것

		// 나머지 커널은 그 자체적으로 블러링 효과를 가진다.

		// 주의2 : 로버츠와 프리윗,소벨

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
	// 2. 원하는 커널을 생성
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
	// 3. 생성된 커널로 이미지와 컨볼루션 하여 엣지검출
	/*---------------------------*/

	int Y, X;
	int Height = infoheader->height;
	int Width = infoheader->width;

	for (Y = 0; Y < Height; Y++)
		for (X = 0; X < Width; X++)
			new_buffer[Y * Width + X] = regular_cal(old_buffer, kernel_1st, X, Y, infoheader, Sizeside);


	if (option < 4) // 3이하는 두번 필터를 거침
	{
		for (Y = 0; Y < Height; Y++)
			for (X = 0; X < Width; X++)
				new_buffer[Y * Width + X] = regular_cal(old_buffer, kernel_2nd, X, Y, infoheader, Sizeside);
	}

	/*---------------------------*/
	// 4. 정리
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
		printf("It is a first Roberts kernel!"); // 첫번째

		double temp[KERNEL33] = { -1,0,0,0,1,0,0,0,0 };

		for (int idx = 0; idx < KERNEL33; idx++)
			roberts[idx] = temp[idx];

		tryNum++;

		return roberts;
	}

	else
	{
		printf("It is a second Roberts kernel!"); // 두번째

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
		printf("It is a first Prewitt kernel!"); // 첫번째

		double temp[KERNEL33] = { -1,0,1,-1,0,1,-1,0,1 };

		for (int idx = 0; idx < KERNEL33; idx++)
			prewitt[idx] = temp[idx];

		tryNum++;

		return prewitt;
	}

	else
	{
		printf("It is a second Prewitt kernel!"); // 두번째

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
		printf("It is a first Sobel kernel!"); // 첫번째

		double temp[KERNEL33] = { -1,0,1,-2,0,2,-1,0,1 };

		for (int idx = 0; idx < KERNEL33; idx++)
			sobel[idx] = temp[idx];

		tryNum++;

		return sobel;
	}

	else
	{
		printf("It is a second Sobel kernel!"); // 두번째

		double temp[KERNEL33] = { 1,2,1,0,0,0,-1,-2,-1 };

		for (int idx = 0; idx < KERNEL33; idx++)
			sobel[idx] = temp[idx];

		return sobel;
	}

	// 로버츠, 프리윗, 소벨필터는 1방향의 
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
	/*--- 커널 동적할당, 연산에 필요한 변수들 선언 ---*/

	double* LoG_kernel = (double*)malloc(sizeof(double) * KERNEL77);
	double sum = 0.;
	int Radius = Sizeside / 2;
	int Y, X;
	int idx;
	double Front, Back;

	/*--- 유효한 시그마 입력받기 ---*/

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

	/*--- 이 변수들을 가지고 2D 가우시안커널을 2차미분 (LoG) 만들기 ---*/

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
	// 엣지 디텍팅이기에, 합이 0이되어야 한다

	/*--- 합이 0이 아니라면, 강제로 0으로 만들어주기 ---*/

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
	/*--- 커널 동적할당, 연산에 필요한 변수들 선언 ---*/

	double* DoG_kernel = (double*)malloc(sizeof(double) * KERNEL77);
	int Radius = Sizeside / 2;

	/*--- 유효한 시그마 입력받기 ---*/

	double sigma;
	printf("DoG is a kernel for approximating LoG to speed up computation\n");
	printf("Which one would you set up the Gaussian sigma of LoG ?\n");
	printf("It automatically calculates the two sigma required for DoG!\n");
	// DoG는 원하는 시그마 값을 갖는 LoG를 완성하기 위해 식을 계산하여, 서로다른 시그마를 계산해서 얻어야 한다

	while (1)
	{
		printf("\nIt should be less than 7/6 (=%f), because one side of this kernel is 7!", (float)(7. / 6.));
		printf("Please enter a sigma value : ");

		scanf("%lf", &sigma);

		if (sigma < (double)(7. / 6.) && sigma > 0)
			break;

	}

	/*--- 해당 LoG 시그마 값에 맞는 두개의 시그마 값을 구하기 ---*/

	// DoG가 LoG를 근사화한것처럼 비슷하게 되려면, DoG와 LoG가 같은 제로크로싱을 가져야 한다. -> 변곡점이 같을것

	double sigma1, sigma2;

	sigma2 = sigma / (2.56 / 1.56 * log(2.56));
	sigma1 = sigma2 * 1.6; // 두 시그마의 비가 1.6:1 일것 (연구결과라니 구현은 불가능 할 것 같다)

	double Constant1 = 2 * M_PI * sigma1 * sigma1;
	double Constant2 = 2 * M_PI * sigma2 * sigma2;

	printf("\nTwo sigma values : sigma1 (%lf), sigma2 (%lf)\n", sigma1, sigma2);

	/*--- 이제 DoG 커널 구하기 ---*/

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


	/*--- 그러나 LoG 커널과 DoG 커널의 제로 크로싱만 같고, 크기 스케일은 다르다 ---*/

	// LoG 커널 중심 원소를 구해서 DoG 커널 중심 원소로 나눈값을 스케일 factor로 설정
	// 모든 DoG커널과의 원소에 곱해주기
	
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
	
	/*---------------------------------------해결완료 무시해도됨------------------------------------*/
	// 수정 : DoG 커널이 LoG 커널과 맞추기 위해서, 스케일링을 해도 LoG 커널이 될 수 없다.
	// 오히려 안한게 LoG 커널로 처리한 것과 비슷해진다 

	/*--- 합이 0이 아니라면, 강제로 0으로 만들어주기 ---*/


	while(fabs(sum) > 1e-10) // 단 한번으로는 완벽하게 0으로 만들수 없다. 부동소수점이라서
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