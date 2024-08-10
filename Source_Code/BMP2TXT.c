#include "Convert.h"

int Extern_App_STAT = 0;
char CMD[1024];
BYTE WIN_STAT = 1;

int convert_TXT(char* oldName, char* newName)
{
	/*---------------------------------------*/
	/* 1. 파일열기 + 헤더읽기 + 파일검사 */
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
	/* 2. 변수선언 + 이미지 읽기 + txt 파일에 쓰기 */
	/*---------------------------------------*/

	fseek(fileBMP, fileheader.offset, SEEK_SET); // raw 데이터가 있는 위치로 시커 이동

	int size = infoheader.ImageSize;

	if (infoheader.ImageSize == 0) // 헤더에 잘못 작성되어있는경우 감안
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
	/* 3. 정리하기 + 텍스트파일 윈도우에 띄우기 */
	/*---------------------------------------*/

	if (fileTXT != NULL)
		Extern_App_STAT++;

clean_up:
	free(RAW);

close:
	fclose(fileBMP);
	fclose(fileTXT);


	if (Extern_App_STAT) // 파일이 정상적 닫혔으니, 텍스트 편집기 실행
	{
		fflush(stdin);
		sprintf(CMD,"start notepad.exe %s",newName); 
		// 비동기적으로 두 프로세스를 실행시키기 위해, 입력버퍼에 명령어를 옮긴다
		WIN_STAT = system(CMD); 
		// WIN_STAT = system(newName); 이것만 실행시키면, 메모장과 프로세스가 동기적으로 실행되어 
		// 메모장이 닫힐때까지, 프로세스는 대기하여야 한다.
	}
		

		
	if(~WIN_STAT) // 성공시 0을 반환함
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
	// 이러한 형식도 유효하다, 엔디안에 상관없이 출력 가능
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
	fprintf(TXT, "\n\nThis is RAW data \n\n"); // RAW 파일의 시작점 알림

	long count = 0;
	int size = infoheader->width * infoheader->height; // 헤더에 잘못 작성되어있는경우 감안

	while (count < size)
	{
		fprintf(TXT,"%X ", *(RAW+count));

		if ( !(count % infoheader->width)) // 딱 이미지의 열 크기만큼만 txt작성
			fprintf(TXT, "\n");

		count++;
	}

	fprintf(TXT,"\n\nThe compiled size of RAW data is : %d\n", count); //raw 파일의 크기를 적어줄것

	if (count == infoheader->width * infoheader->height) // RAW 파일에 패딩이 필요한지를 판단
		fprintf(TXT,"The calculated size and compiled size are same!\n");
	else
	{
		fprintf(TXT, "The calculated size and compiled size are different!!!\n");
		fprintf(TXT, "before Image Processing, You should add some pedding byte!\n");
		*errCode = 6;
	}
		
}