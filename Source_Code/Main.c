/*------------------------------------------*/
// ������ : supergravityy (Github)
// ���۱Ⱓ : 2024.3.22 ~ 2024.5.12
// �� �ڵ�� '������ ����ó��' ����å�� ������ �����Ͽ� ��������ϴ�.
/*------------------------------------------*/

#include "Convert.h"
#define STR_LEN 100

// �� 3451���� �ڵ�

/****************************************************
* 
*  @ ���α׷� ���� 
*  
*	����, �ش� ������ ���ϴ� bmp ������ ����.
*	���ϴ� ��带 �Է��Ѵ�.
*	ó���� ������ ���Ͼȿ��� ��� bmp ������ Ž���Ѵ�.
* 
******************************************************/

int main()
{
	char* originalName = (char*)malloc(sizeof(char) * STR_LEN);
	char* neoName = (char*)malloc(sizeof(char) * STR_LEN);

	/*-------------------------------------------------*/
	// ��ȯ�� �̹��� ���� �̸� �Է¹ޱ�
	/*-------------------------------------------------*/
	
	printf("List of bmp files currently stored in the folder!\n");
	printf("----------------------------\n");
	printf("baboon.bmp\nbarbara.bmp\nboats.bmp\nlena.bmp\nmap.bmp\nnoise_city.bmp\nrafting.bmp\nvegetable.bmp\n");
	printf("cameraman.bmp\ntoys.bmp\nczp256.bmp\n");
	printf("----------------------------\n");

	do
	{
		printf("\ntype vaild BMP name! \n");
		printf("the file name must end with \".bmp\" \n");
		printf("typing \'q\' or \'Q\' is Quit!\n");

		printf("File name: ");
		fgets(originalName, STR_LEN, stdin); // stdin���� �ִ� 99���� + NULL�� �о��

		if (strcmp(originalName, "q\n") == 0 || strcmp(originalName, "Q\n") == 0)
			return 0;

		// fgets�� ���� ���ڱ��� �Է� �����Ƿ�, ���� ���ڸ� NULL�� �ٲ���
		size_t ln = strlen(originalName) - 1; // NULL ���� ������ NULL�� ü����

		if (originalName[ln] == '\n')
			originalName[ln] = '\0';
	} while (isBMP(originalName));

	unsigned int mode = 0;
	

	/*-------------------------------------------------*/
	// �̹��� ������ ��� ��ȯ�� ���ΰ�? ��� �Է¹ޱ�
	/*-------------------------------------------------*/
	while (mode < 1 || mode > QUIT)
	{
		printf("1. blurring\n");
		printf("2. sharpening\n");
		printf("3. midean filtering\n");
		printf("4. edge detection\n");
		printf("5. rotating\n");
		printf("6. minimizing\n");
		printf("7. magnifying\n");
		printf("8. histogram equalizing\n");
		printf("9. histogram stretching\n");
		printf("10. embossing\n");
		printf("11. binarization\n");
		printf("12. invert\n");
		printf("13. gamma correcting\n"); 
		printf("14. parabola processing\n");
		printf("15. bmp2txt\n");
		printf("16. revising\n"); 
		printf("17. Quit\n");

		printf("type the mode you want : ");
		scanf("%u", &mode);
		fflush(stdin); // �Է¹��� �Һ�
	}

	if (mode == QUIT)
	{
		printf("existing.. \n");
		goto termination;
	}

	/*-------------------------------------------------*/
	// ��忡 ���� ó����� �̹��� ������ �̸��� �ٲ��ֱ�
	/*-------------------------------------------------*/
	if (addName(originalName, &neoName, mode))
	{
		printf("Name changing progress has been terminated with Occuring Error! \n");
		goto termination;
	}

	/*printf("Name change progress has been completed! \n");*/
	printf("\n\n%s\n", neoName);

	/*-------------------------------------------------*/
	// ��忡 ���� �������� ��ȯ
	/*-------------------------------------------------*/

	int result;

	if (mode == 15) //txt ���Ϸ� ��ȯ�ϴ� ������ ���� ����ó���� ���̰� ���� ���⿡, ���� ����
		result = convert_TXT(originalName, neoName);

	else if (mode == 16)
		result = revising(originalName, neoName);

	else
		result = convert_BMP(originalName, neoName, mode);

	if (result == 6)
		printf("\nYour file looks like it needs editing~~\nIf you want, Please restart application!\n\n");

	if(result>0 && result != 6)
	{
		printf("Fatal error occured! => %d \n\n", result);
		goto termination;
		/* 
		1 : ���� ���� �Ұ���
		2 : �޸� �Ҵ� �Ұ�
		3 : �޸� ������, ����� 
		4 : ġ������ �޸� �̽�
		5 : 0���� ��������
		6 : �е��� �ʿ� (bmp2txt ��ɿ�����)
		7 : ������ �� �������
		8 : ���� ������ �߸��Ǿ���
		*/
	}

termination:

	free(originalName);
	free(neoName);

	return 0;
}

int isBMP(char* BMP_Name)
{
	int stat = 0;

	// ���ڿ��� �ϳ��� �˻��ϸ鼭 .bmp�� �ùٸ��� ����ִ��� �˻�
	for (int i = 0; i < strlen(BMP_Name); i++)
	{
		if (BMP_Name[i] == '.')
			stat++;
		else if (stat == 1 && BMP_Name[i] == 'b')
			stat++;
		else if (stat == 2 && BMP_Name[i] == 'm')
			stat++;
		else if (stat == 3 && BMP_Name[i] == 'p')
			stat++;
	}

	// ������ �������� 1�� ��ȯ -> if�������� true
	if (stat != 4) return 1;

	else return 0;
}

int addName(char* oldName, char** newName, unsigned int mode)
// �ʿ信 ���� ������� ���ڿ����̸� �ٲܰ��̱� ������, realloc ��� -> ���ο� �޸𸮰��� ���Ҵ�
{
	/*-------------------------------------------------*/
	// 1. newName ũ�⸦ �ٲٱ�
	/*-------------------------------------------------*/

	char* errCode = realloc(*newName, sizeof(char) * (STR_LEN + 30));

	if (errCode == NULL)
	{
		printf("realloc Error Occured! \n");
		return 1;
	}

	*newName = errCode;
	// realloc�� �ϸ� ���ο� �ּҰ��� ��ȯ�ϱ⿡, newName�� ���������ͷ� �޾Ҵ���

	/*-------------------------------------------------*/
	// 2. mode�� ���� �Ĺ��̸� ����
	/*-------------------------------------------------*/

	char additional_Name[30];

	switch (mode)
	{
	case 1:
		strcpy(additional_Name, "_blurred");
		break;
	case 2:
		strcpy(additional_Name, "_sharppend");
		break;
	case 3:
		strcpy(additional_Name, "_mid_filtered");
		break;
	case 4:
		strcpy(additional_Name, "_edge_detected");
		break;
	case 5:
		strcpy(additional_Name, "_rotated");
		break;
	case 6:
		strcpy(additional_Name, "_minimized");
		break;
	case 7:
		strcpy(additional_Name, "_magnified");
		break;
	case 8:
		strcpy(additional_Name, "_equalized");
		break;
	case 9:
		strcpy(additional_Name, "_stretched");
		break;
	case 10:
		strcpy(additional_Name, "_embossing");
		break;
	case 11:
		strcpy(additional_Name, "_binarizated");
		break;
	case 12:
		strcpy(additional_Name, "_inverted");
		break;
	case 13:
		strcpy(additional_Name, "_gammaCorrected");
		break;
	case 14:
		strcpy(additional_Name, "_parabolaProcessed");
		break;
	case 15:
		strcpy(additional_Name, "_4debug");
		break;
	case 16:
		strcpy(additional_Name, "_revised");
		break;

		/*
		* ��带 �߰��ϰ� ������ �������� �ܰ�
		* 1. Convert.h�� QUIT�� ���ϴ� ��ŭ +�Ѵ�
		* 2. �����Լ��� ��ȿ��� ��´ܰ迡, �߰��Ѵ�
		* 3. addName �Լ����� case�� �߰��Ѵ�, (break �����Ұ�!)
		* 4. Convert.c�� mode_select �Լ��� case�� �߰��Ѵ�. (�Լ��̸� �ۼ��ϰ� break ����!)
		* 5. ���� �ҽ������� ���� �Լ��� �ۼ��Ѵ�.
		* 6. Processing.h�� �ش� �Լ������� ������ �Լ������� �ۼ��Ѵ�. (�Լ� ���� �ùٸ��� �ۼ�����!)
		*/

	default:
		printf("mode is invaild! \n");
		return 1;
	}

	/*-------------------------------------------------*/
	// 3. oldName���� .�� �ε��� ��ġ�� ã�� �ڵ�
	/*-------------------------------------------------*/

	int dot_idx = strlen(oldName);

	for (int i = 0; i < strlen(oldName); i++)
	{
		if (oldName[i] == '.')
		{
			dot_idx = i;
			break;
		}
	}

	/*-------------------------------------------------*/
	// 4. newName�� �����
	/*-------------------------------------------------*/
		
	strncpy(*newName, oldName, dot_idx); // .bmp�������� ����
	(*newName)[dot_idx] = NULL; //strcpy�� �����Ṯ�ڸ� �߰� ����
	// ����, strcat�� ��ó�� NULL�� �ε����� ���ڿ��� �߰���

	strcat(*newName, additional_Name);

	if (mode != 15)
		strcat(*newName, ".bmp");
	else
		strcat(*newName, ".txt");

	return 0;
}