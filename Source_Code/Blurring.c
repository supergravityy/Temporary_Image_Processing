#include "Processing.h"

int blurring(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errcCode)
{
	// 순서도는 가우시안 커널을 기준으로 작성되었습니다! 

	/*------------------------------------*/
	// 1. 사용자에게 원하는 커널을 선택받는다
	/*------------------------------------*/

	int option;

	while (1)
	{
		printf("1. Average kernel\n");
		printf("2. Custom gaussian kernel\n");

		printf("\nChoose your kernel! : ");

		scanf("%d", &option);
		putchar('\n');

		if (option > 0 && option < 3)
			break;
	}

	int kernel_size = 0;
	int formula_seed = 0;

	/*------------------------------------*/
	// 2. 커널 크기를 입력받는다
	// 커널크기는 특유의 영역처리 때문에, 홀수의 제곱배이기 때문에, x에 들어갈 수를 입력받는다
	/*------------------------------------*/

	while (1) // 커널크기를 수열이라고 하면  일반항은 (2x-1)^2 이다!
	{
		printf("\nThis is how the kernel size general term is configured : (2x-1)^2\n");
		printf("To suppress other possible errors, limit the kernel size to 100 or less\n");
		printf("And kernel size must bigger than 1\n");
		printf("Please input your kernel size to X! : ");

		if (scanf("%d", &formula_seed) != 1)
		{
			// 소수같은 것을 입력했을경우, 입력 버퍼를 비우고 다시 시도
			printf("Invalid input\n");
			while (getchar() != '\n'); // fflush(stdin) 을 해도 무한루프가 계속되기에 어쩔수 없이 전부 비워준다.
			continue;
		}

		kernel_size = (int)floor(pow(2 * formula_seed - 1, 2));
		printf("\nYour kernel size is (2 * %d - 1) ^2 = %d\n", formula_seed, kernel_size);

		if (kernel_size > 1 && kernel_size < 100)
		{
			printf("Your kernel size is valid!\n");
			break;
		}

		else
			printf("Invalid kernel size! Please retry!\n\n\n ");
	}

	int Sizeside = (int)floor(sqrt(kernel_size));

	/*------------------------------------*/
	// 3. 원하는 커널을 생성시킨다
	/*------------------------------------*/

	double* kernel = NULL;
	BYTE* temp_buffer = NULL;
	double* vector_buffer = NULL;

	switch (option)
	{
	case 1:
		kernel = gen_AVG_kernel(kernel_size);
		break;

	case 2:
		kernel = gen_GAU_kernel(kernel_size);
		break;
	}

	if (kernel == NULL)
	{
		printf("memory allocation failed! at making kernel process!\n");
		goto release;
	}

	/*------------------------------------*/
	// 6. 커널이 제대로 완성되어있는지 확인한다.
	/*------------------------------------*/

	double checkSum = 0;
	putchar('\n');
	putchar('\n');

	// 총합이 1인지 확인

	for (int i = 0; i < kernel_size; i++)
	{
		if (i % Sizeside == 0)
			putchar('\n');
		printf("%lf ", kernel[i]);
		checkSum += kernel[i];
	}

	if (!compareDouble(checkSum,1.))
	{
		printf("\n\nThe sum of the kernels is 1!\nIt won't affect the overall pixel brightness in any way!\n");
		printf("Overall, your kernel is valid for blurring kernel!\n\n");
	}

	else
	{
		printf("\n\nThe sum of the kernels is NOT 1!\n");
		printf("Beware that memory overruns can occur during computation! \n\n");
		// 간혹가다가 총합이 1이 안될때도 있다. 이는 부동소수점 때문에 발생한 것이고, 약간의 전체적으로 픽셀값이 어두워질 수도 있지만
		// 우리눈으로보기엔 별로 티 안난다.
	}

	/*------------------------------------*/
	// 7. 행렬이 y축과 x축에 대칭인지 확인후 드디어 블러링 연산
	/*------------------------------------*/


	int result = 0;

	if (check_symmetry(kernel, kernel_size) && is_seperatable(kernel,Sizeside)) 
	// K = Kx * Ky ^ (T) 의 형태로 쪼개지면 분리가능한 커널이다.
	// 대칭이며, rank = 1 인 행렬이어야 한다. 이를 검사하기 위한 조건
	{
		printf("\n\nThe kernel is seperatale about both x and y axes\n");
		printf("When calculating kernels, the time cost is reduced by 33\%%! Compared to regular calculation\n\n");

		// 행과 열로 쪼개서 연산 (어차피 컨볼루션이라 쪼개기가 가능하다)

		temp_buffer = (BYTE*)malloc(infoheader->ImageSize * sizeof(BYTE));

		if (temp_buffer == NULL)
		{
			printf("Memory allocation Error! at generatimg tempBuffer\n");
			*errcCode = 2;
			goto release;
		}

		vector_buffer = (double*)malloc(Sizeside * sizeof(double)); // 벡터 연산에 필요한 벡터버퍼 생성

		if (vector_buffer == NULL)
		{
			printf("Memory allocation Error! at generatimg vectorBuffer\n");
			*errcCode = 2;
			goto release;
		}

		/*------------------------------------*/
		// 8. 올드버퍼 ★ 행벡터(x축) = 임시버퍼
		/*------------------------------------*/

		//system("cls");

		int idx;
		printf("Row Vector of kernel!\n");

		for (idx = 0; idx < Sizeside; idx++) // 커널의 행벡터(x축) 나열
		{
			vector_buffer[idx] = kernel[idx * Sizeside];
			printf("%lf ", vector_buffer[idx]);
		}

		printf("\nNow row vec calculation is start!\n\n"); // 가로부터 행렬계산

		for (int Y = 0; Y < infoheader->height; Y++)
		{
			for (int X = 0; X < infoheader->width; X++)
			{
				result = row_cal(old_buffer, vector_buffer, X, Y, infoheader, Sizeside);
				temp_buffer[Y * infoheader->width + X] = (BYTE)result;
				//printf("%d ", (int)result);
			}
		}

		/*------------------------------------*/
		// 9. 임시버퍼 ★ 열벡터(y축) = 뉴버퍼
		/*------------------------------------*/

		//system("cls");

		printf("Column Vector of kernel!\n");
		double N = kernel[0];
		for (idx = 0; idx < Sizeside; idx++) // 커널의 행벡터(x축)
		{
			vector_buffer[idx] = kernel[idx * Sizeside] / N; 
			// 위의 분리가능한 커널을 쪼개서 다시 붙이면, N값이 제곱이 되서 나오기에 정규화
			printf("%lf ", vector_buffer[idx]);
		}

		printf("\nNow col vec calculation is start!\n\n\n"); // 그 후에 세로로 행렬계산

		for (int Y = 0; Y < infoheader->height; Y++)
		{
			for (int X = 0; X < infoheader->width; X++)
			{
				result = col_cal(temp_buffer, vector_buffer, X, Y, infoheader, Sizeside);
				new_buffer[Y * infoheader->width + X] = (BYTE)result;
				//printf("%d ", (int)result);
			}
		}
	}

	else // 분리가 불가능하면
	{
		printf("\n\nThe kernel is NOT seperatable about both x and y axes\n");
		printf("As a result, Calculate standard convolution!\n");

		flipping(kernel, Sizeside);
		// 컨볼루션이기에 -기호는 y축과 x축에 대칭일것이다.

		// 행벡터와 열벡터로 쪼갤수가 없기에, 일반연산
		for (int Y = 0; Y < infoheader->height; Y++)
			for (int X = 0; X < infoheader->width; X++)
				new_buffer[Y * infoheader->width + X] = regular_cal(old_buffer, kernel, X, Y, infoheader, Sizeside);
	}

	/*------------------------------------*/
	// 10. 정리하기
	/*------------------------------------*/

release:

	if (kernel != NULL) free(kernel);
	if (temp_buffer != NULL) free(temp_buffer);
	if (vector_buffer != NULL) free(vector_buffer);
	return 0;
}

void flipping(double* kernel, int Sizeside)
{
	int Size = (int)pow(Sizeside, 2);
	int Mid = Size / 2 + 1;
	int idx;

	for (idx = 0; idx <= Mid; idx++) // y축과 x축에 대해 flip 연산
		swap(double, kernel[idx], kernel[(Size - 1) - idx]);

	return;
}

double* gen_AVG_kernel(int size)
{
	// 여기서, 커널을 동적할당한다
	double* AVG_kernel = (double*)malloc(sizeof(double) * size);
	double sum = 0;
	int idx;

	for (idx = 0; idx < size; idx++)
	{
		AVG_kernel[idx] = 1;
		sum += AVG_kernel[idx];
	}


	for (idx = 0; idx < size; idx++)
		AVG_kernel[idx] = 1 / sum;


	// 평균필터 완성!

	return AVG_kernel;
}

double* gen_GAU_kernel(int size)
{
	/*------------------------------------*/
	// 4. 커널의 분포를 결정하는 시그마 값을 입력
	/*------------------------------------*/

	// 표준편차는 커널의 폭을 제어하여, 스무딩 강도를 조절한다.
	// 표준편차는 말그대로, 각 측정값과 해당 그룹의 평균과의 떨어진것의 평균이다
	// 첨도와 어느정도 관계가 있을 줄 알았지만, 전혀 관계가 없다.

	int side = (int)floor(sqrt(size));

	double* GAU_kernel = (double*)malloc(sizeof(double) * size);

	if (GAU_kernel == NULL)
		goto exit;

	printf("You selected Gaussian kernel!\n");
	printf("To create a custom Gaussian kernel, you need to enter an additional sigma value!\n");

	double sigma = 1;
	double Limit_sigma = ((double)side)/6.;
	// Z분포의 신뢰구간을 생각하면, 대부분의 y값이 표준편차와 근접한 곳에 몰려있다. 그래서, 오차를 줄이기 위해 99.7%의 법칙을 위반하면 안된다.

	while (1)
	{
		printf("\n\nThe default value for sigma is 1!\n");
		printf("If you raise the sigma value, the smoothing ability becomes stronger. And if you lower it, the smoothing becomes weaker\n");
		printf("The condition for entering the value\n");
		printf("1. Please enter a positive number\n ");
		printf("2. Because of the 99.7%% law of the normal distribution, the sigma must be less than or equal to \'sideSize(%d)/6\' = %lf : ",side,Limit_sigma);

		/*-----------------------------
		* 여기서 문제가 발생! 먼저 커널 크기를 입력받고, 법칙에 의해 '변의크기 >= 표준편차 * 3 * 2' 이어야 한다
		* 나는 커널의 크기를 먼저 입력받았기에, 표준편차를 조건에 맞춰서 받아야 한다.
		* 변의크기/6 >= 표준편차인데, 이런 조건에 맞춰서 받으면, 총합이 1이 안나올 뿐더러
		* 블러링 연산이 정상적으로 되지않고 이진화가 된것처럼 나온다 -> 이것은 커널과 밀접한 문제이다.
		-------------------------------*/

		if (scanf("%lf", &sigma) != 1 || (sigma <= 0)  || (sigma > Limit_sigma))
		{
			printf("Invalid sigma value. Please enter valid number.\n");
			while (getchar() != '\n'); // fflush(stdin) 으로 안되서 하나하나 비워줘야한다;;;;
			continue;
		}
		else
			break;
	}

	/*------------------------------------*/
	// 5. 2차원 가우시안 분포 필터 만들기
	/*------------------------------------*/

	// 왜도와 첨도는 상관안한다
	// 평행이동도 X
	int Radius = side / 2;
	double sum = 0;

	int Y, X;
	int idx;

	// Y와X값은 평균에 대칭임을 보이기 위해 일부러 -반경 ~ +반경 까지
	// 어차피 인덱스 계산할때, 반경을 더해주긴한다.
	for (Y = -Radius; Y <= Radius; Y++)
	{
		for (X = -Radius; X <= Radius; X++)
		{
			int idx = (Y + Radius) * side + (X + Radius);
			GAU_kernel[idx] = exp(-(X * X + Y * Y) / (2 * sigma * sigma)) / (2 * M_PI * sigma * sigma);
			sum += GAU_kernel[idx];
		}
	}

	/*------------------------------------*/
	// 6. 총합이 1이되게 정규화
	/*------------------------------------*/

	printf("Not normarlized sum of kernel : %lf\n", sum);

	/*for (idx = 0; idx < size; idx++)
		GAU_kernel[idx] /= sum;*/
	// C에서의 double, float 자료형의 곱셈과 나눗셈은 부동소수점이라서 너무 작은 값과 연산하면
	// 더더욱 부정확하다는것을 제발 인지하자

	double Diff = 1.0 - sum;
	sum = 0.0;

	if (compareDouble(Diff, 0.))
	{
		printf("\nkernel sum is NOT 1\n");
		for (idx = 0; idx < size; idx++)
		{
			GAU_kernel[idx] += Diff / size;
			sum += GAU_kernel[idx];
		}

		printf("kernel sum = %lf", sum);
	}

	// 가우시안 필터 완성!

exit:
	return GAU_kernel;
}

int check_symmetry(double* kernel, int size)
{
	int side = (int)floor(sqrt(size));
	int Symmetric = 1;
	// 딱, y축&x축에 대칭인지를 파악하기 위해선 그냥, 맨 처음과 맨 끝부터 1씩 증가시킨게 똑같으면 된다.

	for (int i = 0; i < side; i++)
	{
		for (int j = 0; j < side; j++)
		{
			if (compareDouble(kernel[i * side + j],kernel[(side-1 - i) * side + (side-1 - j)]))
				// 부동소수점이기에 오차허용범위를 두고 연산을한다.
			{
				Symmetric = 0;
				break;
			}
		}
		if (!Symmetric)
			break;
	}
	return Symmetric;
}

BYTE row_cal(BYTE* old_buffer, double* row_vec, int curX, int curY, BITMAPINFOHEADER* infoheader, int Sizeside)
{
	double sum = 0;
	int wrapped_i = 0;

	int Height = infoheader->height;
	int Width = infoheader->width;
	int Radius = Sizeside / 2;
	int vector_idx, buffer_idx;

	for (int i = curY - Radius; i < curY + Radius+1; i++) // i -> 행(y축)
	{
		wrapped_i = circular_wrapping(i, Height);
		buffer_idx = wrapped_i * Width + curX;
		vector_idx = i - curY + Radius;
		sum += (double)old_buffer[buffer_idx] * row_vec[vector_idx];
		//printf("buffer idx = %d, vector idx = %d\t", buffer_idx, vector_idx);
	}

	sum = clipping((int)round(sum));

	return (BYTE)sum;
}

BYTE col_cal(BYTE* temp_buffer, double* col_vec, int curX, int curY, BITMAPINFOHEADER* infoheader, int Sizeside)
{
	double sum = 0;
	int wrapped_j = 0;

	int Height = infoheader->height;
	int Width = infoheader->width;
	int Radius = Sizeside / 2;
	int vector_idx, buffer_idx;

	for (int j = curX - Radius; j < curX + Radius+1; j++) // j -> 열(x축)
	{
		wrapped_j = circular_wrapping(j, Width);
		buffer_idx = curY * Width + wrapped_j;
		vector_idx = j - curX + Radius;
		sum += (double)temp_buffer[buffer_idx] * col_vec[vector_idx];
		//printf("buffer idx = %d, vector idx = %d\t", buffer_idx, vector_idx);
	}
	sum = clipping((int)round(sum));

	return (BYTE)sum;
}

BYTE regular_cal(BYTE* old_buffer, double* kernel, int curX, int curY, BITMAPINFOHEADER* infoheader, int Sizeside)
{
	double sum = 0.0;

	int wrapped_j = 0; int wrapped_i = 0;

	int Height = infoheader->height;
	int Width = infoheader->width;
	int Radius = Sizeside / 2;
	int kernel_idx;

	for (int i = curY - Radius; i<= curY + Radius; i++)
	{
		wrapped_i = circular_wrapping(i, Height);

		for (int j = curX - Radius; j <= curX + Radius; j++)
		{
			wrapped_j = circular_wrapping(j, Width);
			kernel_idx = (i - curY + Radius) * Sizeside + (j - curX + Radius);
			sum += (double)old_buffer[wrapped_i * Width + wrapped_j] * kernel[kernel_idx];
		}
	}

	sum = clipping((int)round(sum));

	return (BYTE)sum;
	/*--------------------------------
	 순서가 매우 중요하다.

	1. 먼저 반올림 처리하고 byte 형태로 만들어서 리턴 후, 콜러가 클리핑을 하게한다
	VS
	2. 먼저 반올림 처리하고, 콜리가 클리핑을 한후, byte 형태로 만들어서 리턴한다.


	전자는 메모리 오버런의 영향이 있을 수도 있기에 안쓰는게 좋다.
	-----------------------------------*/
}

int is_seperatable(double* kernel, int Sizeside)

{
	double threshold = 1e-8; // 문턱값 (0.00000001)
	int X, Y;

	/*-------------------------------------*/
	// 조건 1 : 첫번째 행, 열 벡터에 0이 있으면 안됨
	// 커널의 첫번째 원소는 0이 아닐것. 밑의 연산에서 0으로 나눌수도 있음
	/*-------------------------------------*/

	if (kernel[0] == 0) 
		return 0;

	// 선형종속적이라면, 모든 벡터의 원소들이 첫 벡터의 원소들과 똑같은 비율을 유지

	int first_RowVec_zero = 0;
	int first_ColVec_zero = 0;

	for ( X = 1; X < Sizeside; X++)
	{
		if (fabs(kernel[X]) < threshold) // 너무 작은값이면 나눗셈 연산에 영향 
		{
			first_RowVec_zero++;
			break;
		}
	}

	for (Y = 1; Y < Sizeside; Y++) 
	{
		if (fabs(kernel[Y * Sizeside]) < threshold)
		{
			first_ColVec_zero ++;
			break;
		}
	}

	if (first_RowVec_zero || first_ColVec_zero) // 하나라도 0있는 벡터면 아웃
		return 0;

	/*-------------------------------------*/
	// 조건 2 : rank=1 임을 가정하기에, 첫행과 열이 서로 같아야 한다 (둘중에 하나는 선형종속)
	/*-------------------------------------*/

	for (int i = 0; i < Sizeside; i++)
		if (fabs(kernel[i] - kernel[i * Sizeside]) > threshold)
			return 0;


	/*-------------------------------------*/
	// 조건 3 : 행렬의 rank 값이 1인것으로 가정 (행렬의 선형독립적인 행,열 벡터의 갯수)
	// 이는 첫번째를 제외한 모든 행과 열이 첫번째에 선형 종속적이어야 함
	/*-------------------------------------*/

	// 첫 번째 행을 기준으로 행렬의 모든 행이 배수 관계인지 확인
	for (int Y = 1; Y < Sizeside; Y++) 
	{
		double row_ratio = kernel[Y * Sizeside] / kernel[0]; // 첫행과 배수관계에 있다고 가정

		for (int X = 0; X < Sizeside; X++)

			// kernel[X]는 첫번째 행의 원소를 의미
			// 대상 행의 원소들과 첫 행의 원소를 하나씩 빼가며, 차이를 확인

			if (fabs(kernel[Y * Sizeside + X] - row_ratio * kernel[X]) > threshold) // 둘의 차이가 크면 안된다
				return 0;
	}

	// 첫 번째 열을 기준으로 행렬의 모든 열이 배수 관계인지 확인
	for (int X = 1; X < Sizeside; X++) 
	{
		double col_ratio = kernel[X] / kernel[0]; // 첫열과 배수관계에 있다고 가정

		for (int Y = 0; Y < Sizeside; Y++) 

			// kernel[Y * Sizeside]는 첫번째 열의 원소를 의미

			if (fabs(kernel[Y * Sizeside + X] - col_ratio * kernel[Y * Sizeside]) > threshold) 
				return 0;
	}

	return 1; // 모두 통과시, 분리가능한 커널
}