#include "Convert.h"

int revising(char* oldName, char* newName)
// ��������� ���� + RAW�������� ��Ʈ�е�
{
	/*---------------------------------------*/
	// 1. ���Ͽ��� + ����б� + ���ϰ˻�
	/*---------------------------------------*/

	int errorcode = 0;
	char answer = 0;

	FILE* fileBMP;
	FILE* revisedBMP;
	BITMAPFILEHEADER fileheader;
	BITMAPINFOHEADER infoheader;
	BITMAPColorPalette RGB[256];

	fileBMP = fopen(oldName, "rb");
	revisedBMP = fopen(newName, "wb");

	if (fileBMP == NULL)
	{
		printf("Can not open file!\n\n");
		errorcode = 1;
		goto close;
	}

	fread((char*)&fileheader, sizeof(BITMAPFILEHEADER), 1, fileBMP);
	fread((char*)&infoheader, sizeof(BITMAPINFOHEADER), 1, fileBMP);
	fread((char*)RGB, sizeof(RGB), 1, fileBMP);

	/*---------------------------------------*/
	// 2. �������� + �̹��� �б�
	/*---------------------------------------*/

	int size = infoheader.ImageSize;

	if (infoheader.ImageSize != infoheader.width * infoheader.height) // ����� �߸� �ۼ��Ǿ��ִ°�� ����
		size = infoheader.width * infoheader.height;

	BYTE* RAW = (BYTE*)malloc(size * sizeof(BYTE));

	if (RAW == NULL)
	{
		printf("Cannot allocate memory section!\n\n");
		errorcode = 2;
		goto close;
	}

	/*---------------------------------------*/
	// 3. RAW ������ �б� + ũ��˻� + ��Ʈ�е� ���� Ȯ��
	/*---------------------------------------*/

	size_t byte; 
	int read_bytes = 0;
	int end_of_file = 0;

	while ((byte = fgetc(fileBMP)) != EOF)
	{
		if (read_bytes < size)
			RAW[read_bytes++] = (BYTE)byte; 
		// ��¦ �ʰ��ϴ� ��쵵 �ֱ� �ϴ�. �׷��� �Ҵ���� ũ������� �����Ѵ�.
		else
			read_bytes++;
		// �׷��� ī��Ʈ�� �����ؾ� �Ѵ�.
	}

	printf("Read_bytes : %d, Size : %d, is End Of File : %s\n\n", read_bytes, size, end_of_file?"True":"False");

	// �̷��� �Ǹ�, ���� RAW ������ ũ��� ��������� ũ�Ⱑ ���ų� (ó�� ����)
	// RAW ������ ũ�Ⱑ ����������� �۰� �Ǵ� �ΰ��� ��츸 ���´�. (�е� �ʿ�)

	if (feof(fileBMP) || read_bytes == size) 
		printf("Reached the end of file\n\n");

	if (read_bytes < size) // ��Ʈ �е��� �Ұ��� ����� ����.
	{
		printf("Little problem at reading RAW data. \nNoted size is %u bytes, but read only %u bytes.\n", size, read_bytes);
		printf("This can be a big problem in image processing\n");
		printf("Would you like to access zero padding service?\n\n");
		
		while (1)
		{
			printf("If you want : y, if not : n  -> ");
			answer = getchar();
			fflush(stdin);
			puts("\n\n\n");

			if (answer == 'Y' || answer == 'N' || answer == 'y' || answer == 'n')
				break;
		}
	}

	/*---------------------------------------*/
	// 4. ������� �����ϱ�  
	/*---------------------------------------*/

	errorcode = revise_header(&fileheader, &infoheader, RGB);

	if (errorcode == 8) // ��������� �߸��ۼ��ϸ�
		goto clean_up;

	/*---------------------------------------*/
	// 5. answer = y || = Y �̸� ����� �°�, RAW ������ ���� 
	/*---------------------------------------*/

	if (answer == 'Y' || answer == 'y')
	{
		int pedding_width; int pedding_height; int temp;
		pedding_height = infoheader.height-(infoheader.ImageSize / infoheader.width); // �е��� ����
		pedding_width = infoheader.width - (infoheader.ImageSize % infoheader.width); // �׿��� ������

		temp = pedding_height * infoheader.width + pedding_width;

		errorcode = zero_padding(RAW, temp, read_bytes);
	}

	/*---------------------------------------*/
	// 6. bmp ���Ͽ� ����
	/*---------------------------------------*/

	fwrite((char*)&fileheader, sizeof(BITMAPFILEHEADER), 1, revisedBMP);
	fwrite((char*)&infoheader, sizeof(BITMAPINFOHEADER), 1, revisedBMP);
	fwrite((char*)RGB, sizeof(RGB), 1, revisedBMP);
	fwrite(RAW, infoheader.ImageSize, 1, revisedBMP);

	/*---------------------------------------*/
	// 5. �����ϱ�
	/*---------------------------------------*/

	printf("The Image revising process has been completed!!\n\n");

clean_up:
	free(RAW);

close:
	fclose(fileBMP);
	fclose(revisedBMP);

	return errorcode;
}

int revise_header(BITMAPFILEHEADER* fileheader, BITMAPINFOHEADER* infoheader, BITMAPColorPalette* rgb)
{
	// ������� ����

	/*
	 ���� : ���ͳݿ��� �ǽ��� bmp ��������� �ܾ���� �Ǹ�, ��ǻ�ͷ� �� ���� ������,
	 ������ ���� ��������� �߸��Ǿ�, �̹��� ó���� �� �� ���� ���� �ִ�.
	 ���� �پ��� �ǽ��� �ϱ� ���ؼ�, ����� ������ �� �ʿ��ϴٰ� �����ؼ�, �߰��� �� ����� �������
	*/

	/*
	* ���� Ʋ���� ���� ����! (�������)
	* 1. type : type�� Ʋ������ �Ǹ�, bmp ���Ϸ� �ν��� �Ұ����ϴ�
	* 2. offset : ���� RAW ������ ����ִ� ��ġ�̴�. �̰� Ʋ����, ���� �ν��� �Ұ����ϴ�
	*/

	fileheader->type = ('M' << 8) | 'B'; // M�� ��������Ʈ��, B�� ��������Ʈ�� (�н���)
	// �̸� ��ǻ�Ͱ� char������ �аԵǸ�, ��Ʋ����� ������� �����ּҿ������� �б⶧����, MB�� �аԵȴ�
	fileheader->size = infoheader->width * infoheader->height + fileheader->offset;
	fileheader->reserved1 = 0; fileheader->reserved2 = 0; // ����μ� �ǹ̾��� ������

	/*
	* ���� Ʋ�� �� ���� ����! (�������)
	* 1. structsize : ��������� ũ�⸦ ��Ÿ����. Ʋ������ ������ �ν��� �� ����
	* 2. width, height : �̰� Ʋ������ ������ ��µ�����, �˾ƺ� �� ���Եȴ�
	*/

	infoheader->structSize = 40; // ��������� ũ��
	infoheader->planes = 0; //2 ���� �����Ͷ�� �̾߱�
	infoheader->ImageSize = infoheader->width * infoheader->height; 
	// Ʋ������ ��ü ����� ���� �а�, offset���� �����ؼ� RAW �����͸� ������, ������ ��µǱ�� �Ѵ�
	infoheader->BitPerPxl = 8;
	// �ȼ��� ��Ʈ���̱⿡, 8 �̸�, �Ϲ����� ����� ��Ÿ���� �ȴ�. Ʈ���÷��� 24
	infoheader->compression = 0;
	infoheader->clrUsed = 256;
	// �ȷ�Ʈ ����� �̹������� ���ȴ�. ���� bpp = 8�̱⿡, 256���� ���� ����� �����ϴ� (0�̸�, �ڵ����� ���� ����)
	// ���⼭ clrused = 100�̸�, ���߿��� 100���� ���� ����Ѵٰ� �̸� �˷��شٴ� �� 
	infoheader->clrImportant = 256;
	// �̹����� ǰ���� ���Ͻ�Ű�� �����鼭 ���� ���� ���̴� ����ȭ �������� ���ȴ�.
	// �̹����� ���������� ǥ���Ϸ��� ��� ���� �ʿ������� �˷���


	/*
	* bpp = 24��, �ȷ�Ʈ�� ���� �ʿ䰡 ����. 
	* �׷���, ����� ����̱⿡, �ȷ�Ʈ ������ �ݵ�� �ʿ��� ��Ȳ�̴�.
	* ����� �� �ȼ����� r=g=b �̴�.
	*/
	for (int i = 0; i < 256; i++)
	{
		rgb[i].blue = i;
		rgb[i].red = i;
		rgb[i].green = i;
		rgb[i].reserved = 0;
	}
	// �Ϲ����� bpp=8�� bmp������ �� ũ��� 1078 (=offset) �̴�.

	if (infoheader->ImageSize + fileheader->offset != fileheader->size)
		return 8;

	return 0;
}

int zero_padding(BYTE* raw, int pedding_bytes, int pedding_loc)
{
	// �⺻������ raw ������ �Ųٷ� �������ֱ⿡, ���� �߰��Ǵ� �ȼ����� ȭ�鿡���� ���� �� �����ڸ� �κ��̴�.
	
	for (int i = 0; i < pedding_bytes; i++)
	{
		raw[i+pedding_loc] = 0;
	}

	// �������� ��� 0���� �������

	return 0;
}