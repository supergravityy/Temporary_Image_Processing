#include "Processing.h"

int blurring(BYTE* old_buffer, BYTE* new_buffer, BITMAPINFOHEADER* infoheader, int* errcCode)
{
	// �������� ����þ� Ŀ���� �������� �ۼ��Ǿ����ϴ�! 

	/*------------------------------------*/
	// 1. ����ڿ��� ���ϴ� Ŀ���� ���ù޴´�
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
	// 2. Ŀ�� ũ�⸦ �Է¹޴´�
	// Ŀ��ũ��� Ư���� ����ó�� ������, Ȧ���� �������̱� ������, x�� �� ���� �Է¹޴´�
	/*------------------------------------*/

	while (1) // Ŀ��ũ�⸦ �����̶�� �ϸ�  �Ϲ����� (2x-1)^2 �̴�!
	{
		printf("\nThis is how the kernel size general term is configured : (2x-1)^2\n");
		printf("To suppress other possible errors, limit the kernel size to 100 or less\n");
		printf("And kernel size must bigger than 1\n");
		printf("Please input your kernel size to X! : ");

		if (scanf("%d", &formula_seed) != 1)
		{
			// �Ҽ����� ���� �Է��������, �Է� ���۸� ���� �ٽ� �õ�
			printf("Invalid input\n");
			while (getchar() != '\n'); // fflush(stdin) �� �ص� ���ѷ����� ��ӵǱ⿡ ��¿�� ���� ���� ����ش�.
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
	// 3. ���ϴ� Ŀ���� ������Ų��
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
	// 6. Ŀ���� ����� �ϼ��Ǿ��ִ��� Ȯ���Ѵ�.
	/*------------------------------------*/

	double checkSum = 0;
	putchar('\n');
	putchar('\n');

	// ������ 1���� Ȯ��

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
		// ��Ȥ���ٰ� ������ 1�� �ȵɶ��� �ִ�. �̴� �ε��Ҽ��� ������ �߻��� ���̰�, �ణ�� ��ü������ �ȼ����� ��ο��� ���� ������
		// �츮�����κ��⿣ ���� Ƽ �ȳ���.
	}

	/*------------------------------------*/
	// 7. ����� y��� x�࿡ ��Ī���� Ȯ���� ���� ���� ����
	/*------------------------------------*/


	int result = 0;

	if (check_symmetry(kernel, kernel_size) && is_seperatable(kernel,Sizeside)) 
	// K = Kx * Ky ^ (T) �� ���·� �ɰ����� �и������� Ŀ���̴�.
	// ��Ī�̸�, rank = 1 �� ����̾�� �Ѵ�. �̸� �˻��ϱ� ���� ����
	{
		printf("\n\nThe kernel is seperatale about both x and y axes\n");
		printf("When calculating kernels, the time cost is reduced by 33\%%! Compared to regular calculation\n\n");

		// ��� ���� �ɰ��� ���� (������ ��������̶� �ɰ��Ⱑ �����ϴ�)

		temp_buffer = (BYTE*)malloc(infoheader->ImageSize * sizeof(BYTE));

		if (temp_buffer == NULL)
		{
			printf("Memory allocation Error! at generatimg tempBuffer\n");
			*errcCode = 2;
			goto release;
		}

		vector_buffer = (double*)malloc(Sizeside * sizeof(double)); // ���� ���꿡 �ʿ��� ���͹��� ����

		if (vector_buffer == NULL)
		{
			printf("Memory allocation Error! at generatimg vectorBuffer\n");
			*errcCode = 2;
			goto release;
		}

		/*------------------------------------*/
		// 8. �õ���� �� �຤��(x��) = �ӽù���
		/*------------------------------------*/

		//system("cls");

		int idx;
		printf("Row Vector of kernel!\n");

		for (idx = 0; idx < Sizeside; idx++) // Ŀ���� �຤��(x��) ����
		{
			vector_buffer[idx] = kernel[idx * Sizeside];
			printf("%lf ", vector_buffer[idx]);
		}

		printf("\nNow row vec calculation is start!\n\n"); // ���κ��� ��İ��

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
		// 9. �ӽù��� �� ������(y��) = ������
		/*------------------------------------*/

		//system("cls");

		printf("Column Vector of kernel!\n");
		double N = kernel[0];
		for (idx = 0; idx < Sizeside; idx++) // Ŀ���� �຤��(x��)
		{
			vector_buffer[idx] = kernel[idx * Sizeside] / N; 
			// ���� �и������� Ŀ���� �ɰ��� �ٽ� ���̸�, N���� ������ �Ǽ� �����⿡ ����ȭ
			printf("%lf ", vector_buffer[idx]);
		}

		printf("\nNow col vec calculation is start!\n\n\n"); // �� �Ŀ� ���η� ��İ��

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

	else // �и��� �Ұ����ϸ�
	{
		printf("\n\nThe kernel is NOT seperatable about both x and y axes\n");
		printf("As a result, Calculate standard convolution!\n");

		flipping(kernel, Sizeside);
		// ��������̱⿡ -��ȣ�� y��� x�࿡ ��Ī�ϰ��̴�.

		// �຤�Ϳ� �����ͷ� �ɰ����� ���⿡, �Ϲݿ���
		for (int Y = 0; Y < infoheader->height; Y++)
			for (int X = 0; X < infoheader->width; X++)
				new_buffer[Y * infoheader->width + X] = regular_cal(old_buffer, kernel, X, Y, infoheader, Sizeside);
	}

	/*------------------------------------*/
	// 10. �����ϱ�
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

	for (idx = 0; idx <= Mid; idx++) // y��� x�࿡ ���� flip ����
		swap(double, kernel[idx], kernel[(Size - 1) - idx]);

	return;
}

double* gen_AVG_kernel(int size)
{
	// ���⼭, Ŀ���� �����Ҵ��Ѵ�
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


	// ������� �ϼ�!

	return AVG_kernel;
}

double* gen_GAU_kernel(int size)
{
	/*------------------------------------*/
	// 4. Ŀ���� ������ �����ϴ� �ñ׸� ���� �Է�
	/*------------------------------------*/

	// ǥ�������� Ŀ���� ���� �����Ͽ�, ������ ������ �����Ѵ�.
	// ǥ�������� ���״��, �� �������� �ش� �׷��� ��հ��� ���������� ����̴�
	// ÷���� ������� ���谡 ���� �� �˾�����, ���� ���谡 ����.

	int side = (int)floor(sqrt(size));

	double* GAU_kernel = (double*)malloc(sizeof(double) * size);

	if (GAU_kernel == NULL)
		goto exit;

	printf("You selected Gaussian kernel!\n");
	printf("To create a custom Gaussian kernel, you need to enter an additional sigma value!\n");

	double sigma = 1;
	double Limit_sigma = ((double)side)/6.;
	// Z������ �ŷڱ����� �����ϸ�, ��κ��� y���� ǥ�������� ������ ���� �����ִ�. �׷���, ������ ���̱� ���� 99.7%�� ��Ģ�� �����ϸ� �ȵȴ�.

	while (1)
	{
		printf("\n\nThe default value for sigma is 1!\n");
		printf("If you raise the sigma value, the smoothing ability becomes stronger. And if you lower it, the smoothing becomes weaker\n");
		printf("The condition for entering the value\n");
		printf("1. Please enter a positive number\n ");
		printf("2. Because of the 99.7%% law of the normal distribution, the sigma must be less than or equal to \'sideSize(%d)/6\' = %lf : ",side,Limit_sigma);

		/*-----------------------------
		* ���⼭ ������ �߻�! ���� Ŀ�� ũ�⸦ �Է¹ް�, ��Ģ�� ���� '����ũ�� >= ǥ������ * 3 * 2' �̾�� �Ѵ�
		* ���� Ŀ���� ũ�⸦ ���� �Է¹޾ұ⿡, ǥ�������� ���ǿ� ���缭 �޾ƾ� �Ѵ�.
		* ����ũ��/6 >= ǥ�������ε�, �̷� ���ǿ� ���缭 ������, ������ 1�� �ȳ��� �Ӵ���
		* ���� ������ ���������� �����ʰ� ����ȭ�� �Ȱ�ó�� ���´� -> �̰��� Ŀ�ΰ� ������ �����̴�.
		-------------------------------*/

		if (scanf("%lf", &sigma) != 1 || (sigma <= 0)  || (sigma > Limit_sigma))
		{
			printf("Invalid sigma value. Please enter valid number.\n");
			while (getchar() != '\n'); // fflush(stdin) ���� �ȵǼ� �ϳ��ϳ� �������Ѵ�;;;;
			continue;
		}
		else
			break;
	}

	/*------------------------------------*/
	// 5. 2���� ����þ� ���� ���� �����
	/*------------------------------------*/

	// �ֵ��� ÷���� ������Ѵ�
	// �����̵��� X
	int Radius = side / 2;
	double sum = 0;

	int Y, X;
	int idx;

	// Y��X���� ��տ� ��Ī���� ���̱� ���� �Ϻη� -�ݰ� ~ +�ݰ� ����
	// ������ �ε��� ����Ҷ�, �ݰ��� �����ֱ��Ѵ�.
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
	// 6. ������ 1�̵ǰ� ����ȭ
	/*------------------------------------*/

	printf("Not normarlized sum of kernel : %lf\n", sum);

	/*for (idx = 0; idx < size; idx++)
		GAU_kernel[idx] /= sum;*/
	// C������ double, float �ڷ����� ������ �������� �ε��Ҽ����̶� �ʹ� ���� ���� �����ϸ�
	// ������ ����Ȯ�ϴٴ°��� ���� ��������

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

	// ����þ� ���� �ϼ�!

exit:
	return GAU_kernel;
}

int check_symmetry(double* kernel, int size)
{
	int side = (int)floor(sqrt(size));
	int Symmetric = 1;
	// ��, y��&x�࿡ ��Ī������ �ľ��ϱ� ���ؼ� �׳�, �� ó���� �� ������ 1�� ������Ų�� �Ȱ����� �ȴ�.

	for (int i = 0; i < side; i++)
	{
		for (int j = 0; j < side; j++)
		{
			if (compareDouble(kernel[i * side + j],kernel[(side-1 - i) * side + (side-1 - j)]))
				// �ε��Ҽ����̱⿡ ������������ �ΰ� �������Ѵ�.
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

	for (int i = curY - Radius; i < curY + Radius+1; i++) // i -> ��(y��)
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

	for (int j = curX - Radius; j < curX + Radius+1; j++) // j -> ��(x��)
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
	 ������ �ſ� �߿��ϴ�.

	1. ���� �ݿø� ó���ϰ� byte ���·� ���� ���� ��, �ݷ��� Ŭ������ �ϰ��Ѵ�
	VS
	2. ���� �ݿø� ó���ϰ�, �ݸ��� Ŭ������ ����, byte ���·� ���� �����Ѵ�.


	���ڴ� �޸� �������� ������ ���� ���� �ֱ⿡ �Ⱦ��°� ����.
	-----------------------------------*/
}

int is_seperatable(double* kernel, int Sizeside)

{
	double threshold = 1e-8; // ���ΰ� (0.00000001)
	int X, Y;

	/*-------------------------------------*/
	// ���� 1 : ù��° ��, �� ���Ϳ� 0�� ������ �ȵ�
	// Ŀ���� ù��° ���Ҵ� 0�� �ƴҰ�. ���� ���꿡�� 0���� �������� ����
	/*-------------------------------------*/

	if (kernel[0] == 0) 
		return 0;

	// �����������̶��, ��� ������ ���ҵ��� ù ������ ���ҵ�� �Ȱ��� ������ ����

	int first_RowVec_zero = 0;
	int first_ColVec_zero = 0;

	for ( X = 1; X < Sizeside; X++)
	{
		if (fabs(kernel[X]) < threshold) // �ʹ� �������̸� ������ ���꿡 ���� 
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

	if (first_RowVec_zero || first_ColVec_zero) // �ϳ��� 0�ִ� ���͸� �ƿ�
		return 0;

	/*-------------------------------------*/
	// ���� 2 : rank=1 ���� �����ϱ⿡, ù��� ���� ���� ���ƾ� �Ѵ� (���߿� �ϳ��� ��������)
	/*-------------------------------------*/

	for (int i = 0; i < Sizeside; i++)
		if (fabs(kernel[i] - kernel[i * Sizeside]) > threshold)
			return 0;


	/*-------------------------------------*/
	// ���� 3 : ����� rank ���� 1�ΰ����� ���� (����� ������������ ��,�� ������ ����)
	// �̴� ù��°�� ������ ��� ��� ���� ù��°�� ���� �������̾�� ��
	/*-------------------------------------*/

	// ù ��° ���� �������� ����� ��� ���� ��� �������� Ȯ��
	for (int Y = 1; Y < Sizeside; Y++) 
	{
		double row_ratio = kernel[Y * Sizeside] / kernel[0]; // ù��� ������迡 �ִٰ� ����

		for (int X = 0; X < Sizeside; X++)

			// kernel[X]�� ù��° ���� ���Ҹ� �ǹ�
			// ��� ���� ���ҵ�� ù ���� ���Ҹ� �ϳ��� ������, ���̸� Ȯ��

			if (fabs(kernel[Y * Sizeside + X] - row_ratio * kernel[X]) > threshold) // ���� ���̰� ũ�� �ȵȴ�
				return 0;
	}

	// ù ��° ���� �������� ����� ��� ���� ��� �������� Ȯ��
	for (int X = 1; X < Sizeside; X++) 
	{
		double col_ratio = kernel[X] / kernel[0]; // ù���� ������迡 �ִٰ� ����

		for (int Y = 0; Y < Sizeside; Y++) 

			// kernel[Y * Sizeside]�� ù��° ���� ���Ҹ� �ǹ�

			if (fabs(kernel[Y * Sizeside + X] - col_ratio * kernel[Y * Sizeside]) > threshold) 
				return 0;
	}

	return 1; // ��� �����, �и������� Ŀ��
}