#include "Convert.h"

int Extern_App_STAT = 0;
char CMD[1024];
BYTE WIN_STAT = 1;

int convert_TXT(char* oldName, char* newName)
{
	/*---------------------------------------*/
	/* 1. ���Ͽ��� + ����б� + ���ϰ˻� */
	/*---------------------------------------*/

	int errorcode = 0;

	FILE* fileBMP;
	FILE* fileTXT;
	BITMAPFILEHEADER fileheader;
	BITMAPINFOHEADER infoheader;
	BITMAPColorPalette RGB[256];

	fileBMP = fopen(oldName, "rb");
	fileTXT = fopen(newName, "wb");

	if (fileBMP == NULL)
	{
		printf("Can not open file!\n\n");
		errorcode = 1;
		goto close;
	}

	fread((char*)&fileheader, sizeof(BITMAPFILEHEADER), 1, fileBMP);
	fread((char*)&infoheader, sizeof(BITMAPINFOHEADER), 1, fileBMP);
	fread((char*)RGB, sizeof(RGB), 1, fileBMP);

	if (infoheader.BitPerPxl != 8)
		printf("Caution : Image file is not black & white!\n\n");

	/*---------------------------------------*/
	/* 2. �������� + �̹��� �б� + txt ���Ͽ� ���� */
	/*---------------------------------------*/

	fseek(fileBMP, fileheader.offset, SEEK_SET); // raw �����Ͱ� �ִ� ��ġ�� ��Ŀ �̵�

	int size = infoheader.ImageSize;

	if (infoheader.ImageSize == 0) // ����� �߸� �ۼ��Ǿ��ִ°�� ����
		size = infoheader.width * infoheader.height;

	BYTE* RAW = (BYTE*)malloc(size * sizeof(BYTE));

	if (RAW == NULL)
	{
		printf("Cannot allocate memory section!\n\n");
		errorcode = 2;
		goto close;
	}

	size_t totoal_bytes = fread(RAW, size, 1, fileBMP);

	if (totoal_bytes != size)
	{
		printf("Error reading RAW data. Expected %u bytes, but read only %zu bytes.\n", size, totoal_bytes);
		errorcode = 6;	
	}

	fprintf(fileTXT, "The information of %s\n\n", newName);
	write_header(&fileheader,&infoheader,RGB,fileTXT);
	write_raw(&infoheader,RAW,fileTXT,&errorcode);

	/*---------------------------------------*/
	/* 3. �����ϱ� + �ؽ�Ʈ���� �����쿡 ���� */
	/*---------------------------------------*/

	if (fileTXT != NULL)
		Extern_App_STAT++;

clean_up:
	free(RAW);

close:
	fclose(fileBMP);
	fclose(fileTXT);


	if (Extern_App_STAT) // ������ ������ ��������, �ؽ�Ʈ ������ ����
	{
		fflush(stdin);
		sprintf(CMD,"start notepad.exe %s",newName); 
		// �񵿱������� �� ���μ����� �����Ű�� ����, �Է¹��ۿ� ��ɾ �ű��
		WIN_STAT = system(CMD); 
		// WIN_STAT = system(newName); �̰͸� �����Ű��, �޸���� ���μ����� ���������� ����Ǿ� 
		// �޸����� ����������, ���μ����� ����Ͽ��� �Ѵ�.
	}
		

		
	if(~WIN_STAT) // ������ 0�� ��ȯ��
		printf("\nThe NotePad app has been executed!\n");

	else
	{
		printf("Failed to open the text editor!!\n");
		errorcode = 7;
	}

	return errorcode;
}

void write_header(BITMAPFILEHEADER* fileheader, BITMAPINFOHEADER* infoheader, BITMAPColorPalette* rgb, FILE* TXT)
{
	fprintf(TXT, "Information of Fileheader\n\n");

	char* char_add = (char*)&fileheader->type;
	fprintf(TXT, "Type : %c %c\n", char_add[0], char_add[1]);
	/*fprintf(TXT, "Type : %c%c\n", (fileheader->type & 0xFF), (fileheader->type >> 8) & 0xFF);*/
	// �̷��� ���ĵ� ��ȿ�ϴ�, ����ȿ� ������� ��� ����
	fprintf(TXT, "Size : %u\n", fileheader->size );
	fprintf(TXT, "Reserved1 : %u, Reserved2 : %u\n", fileheader->reserved1, fileheader->reserved2);
	fprintf(TXT, "Location of RAW data : %u\n\n", fileheader->offset);

	fprintf(TXT, "Information of Infoheader\n\n" );

	fprintf(TXT, "Header size : %d\n", infoheader->structSize);
	fprintf(TXT, "Row size : %u\n", infoheader->height);
	fprintf(TXT, "Column size : %u\n", infoheader->width);
	fprintf(TXT, "Planes : %u\n",infoheader->planes );
	fprintf(TXT, "BitPerPixel : %u\n",infoheader->BitPerPxl);
	fprintf(TXT, "Compressed : %u\n",infoheader->compression );
	fprintf(TXT, "Size of Image : %u\n", infoheader->ImageSize);
	fprintf(TXT, "X Pels Per Meter : %d\n", infoheader->xPelsPerMeter);
	fprintf(TXT, "Y Pels Per Meter : %d\n", infoheader->yPelsPerMeter);
	fprintf(TXT, "A Number of used color : %u\n",infoheader->clrUsed );
	fprintf(TXT, "The important color : %u\n\n", infoheader->clrImportant);

	fprintf(TXT, "Array of color Palletet\n\n");

	for (int i = 0; i < 256; i++)
		fprintf(TXT,"Number(%d) Blue: %d, Green: %d, Red: %d, Reserved: %d\n",
			i, (rgb+i)->blue, (rgb+i)->green, (rgb + i)->red, (rgb + i)->reserved);
}


void write_raw(BITMAPINFOHEADER* infoheader, BYTE * RAW,FILE* TXT,int* errCode )
{
	fprintf(TXT, "\n\nThis is RAW data \n\n"); // RAW ������ ������ �˸�

	long count = 0;
	int size = infoheader->width * infoheader->height; // ����� �߸� �ۼ��Ǿ��ִ°�� ����

	while (count < size)
	{
		fprintf(TXT,"%X ", *(RAW+count));

		if ( !(count % infoheader->width)) // �� �̹����� �� ũ�⸸ŭ�� txt�ۼ�
			fprintf(TXT, "\n");

		count++;
	}

	fprintf(TXT,"\n\nThe compiled size of RAW data is : %d\n", count); //raw ������ ũ�⸦ �����ٰ�

	if (count == infoheader->width * infoheader->height) // RAW ���Ͽ� �е��� �ʿ������� �Ǵ�
		fprintf(TXT,"The calculated size and compiled size are same!\n");
	else
	{
		fprintf(TXT, "The calculated size and compiled size are different!!!\n");
		fprintf(TXT, "before Image Processing, You should add some pedding byte!\n");
		*errCode = 6;
	}
		
}