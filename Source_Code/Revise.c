#include "Convert.h"

int revising(char* oldName, char* newName)
// 헤더파일의 정보 + RAW데이터의 비트패딩
{
	/*---------------------------------------*/
	// 1. 파일열기 + 헤더읽기 + 파일검사
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
	// 2. 변수선언 + 이미지 읽기
	/*---------------------------------------*/

	int size = infoheader.ImageSize;

	if (infoheader.ImageSize != infoheader.width * infoheader.height) // 헤더에 잘못 작성되어있는경우 감안
		size = infoheader.width * infoheader.height;

	BYTE* RAW = (BYTE*)malloc(size * sizeof(BYTE));

	if (RAW == NULL)
	{
		printf("Cannot allocate memory section!\n\n");
		errorcode = 2;
		goto close;
	}

	/*---------------------------------------*/
	// 3. RAW 데이터 읽기 + 크기검사 + 비트패딩 여부 확인
	/*---------------------------------------*/

	size_t byte; 
	int read_bytes = 0;
	int end_of_file = 0;

	while ((byte = fgetc(fileBMP)) != EOF)
	{
		if (read_bytes < size)
			RAW[read_bytes++] = (BYTE)byte; 
		// 살짝 초과하는 경우도 있긴 하다. 그래도 할당받은 크기까지만 저장한다.
		else
			read_bytes++;
		// 그래도 카운트는 증가해야 한다.
	}

	printf("Read_bytes : %d, Size : %d, is End Of File : %s\n\n", read_bytes, size, end_of_file?"True":"False");

	// 이렇게 되면, 현재 RAW 파일의 크기와 헤더정보의 크기가 같거나 (처리 가능)
	// RAW 파일의 크기가 헤더정보보다 작게 되는 두가지 경우만 남는다. (패딩 필요)

	if (feof(fileBMP) || read_bytes == size) 
		printf("Reached the end of file\n\n");

	if (read_bytes < size) // 비트 패딩을 할건지 물어보기 위함.
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
	// 4. 헤더정보 수정하기  
	/*---------------------------------------*/

	errorcode = revise_header(&fileheader, &infoheader, RGB);

	if (errorcode == 8) // 헤더정보를 잘못작성하면
		goto clean_up;

	/*---------------------------------------*/
	// 5. answer = y || = Y 이면 사이즈에 맞게, RAW 데이터 수정 
	/*---------------------------------------*/

	if (answer == 'Y' || answer == 'y')
	{
		int pedding_width; int pedding_height; int temp;
		pedding_height = infoheader.height-(infoheader.ImageSize / infoheader.width); // 패딩할 높이
		pedding_width = infoheader.width - (infoheader.ImageSize % infoheader.width); // 그외의 나머지

		temp = pedding_height * infoheader.width + pedding_width;

		errorcode = zero_padding(RAW, temp, read_bytes);
	}

	/*---------------------------------------*/
	// 6. bmp 파일에 쓰기
	/*---------------------------------------*/

	fwrite((char*)&fileheader, sizeof(BITMAPFILEHEADER), 1, revisedBMP);
	fwrite((char*)&infoheader, sizeof(BITMAPINFOHEADER), 1, revisedBMP);
	fwrite((char*)RGB, sizeof(RGB), 1, revisedBMP);
	fwrite(RAW, infoheader.ImageSize, 1, revisedBMP);

	/*---------------------------------------*/
	// 5. 정리하기
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
	// 헤더정보 수정

	/*
	 목적 : 인터넷에서 실습용 bmp 흑백파일을 긁어오게 되면, 컴퓨터로 볼 수는 있지만,
	 실제로 몇몇개의 헤더정보가 잘못되어, 이미지 처리를 할 수 없을 때도 있다.
	 좀더 다양한 실습을 하기 위해서, 헤더의 수정은 꼭 필요하다고 생각해서, 중간에 이 기능을 만들었다
	*/

	/*
	* 절대 틀릴수 없는 정보! (파일헤더)
	* 1. type : type이 틀어지게 되면, bmp 파일로 인식이 불가능하다
	* 2. offset : 실제 RAW 파일이 들어있는 위치이다. 이게 틀리면, 사진 인식이 불가능하다
	*/

	fileheader->type = ('M' << 8) | 'B'; // M을 상위바이트로, B을 하위바이트로 (학습용)
	// 이를 컴퓨터가 char형으로 읽게되면, 리틀엔디언 방식으로 낮은주소에서부터 읽기때문에, MB로 읽게된다
	fileheader->size = infoheader->width * infoheader->height + fileheader->offset;
	fileheader->reserved1 = 0; fileheader->reserved2 = 0; // 현재로선 의미없는 데이터

	/*
	* 절대 틀릴 수 없는 정보! (정보헤더)
	* 1. structsize : 정보헤더의 크기를 나타낸다. 틀어지면 파일을 인식할 수 없다
	* 2. width, height : 이게 틀어져도 파일이 출력되지만, 알아볼 수 없게된다
	*/

	infoheader->structSize = 40; // 정보헤더의 크기
	infoheader->planes = 0; //2 차원 데이터라는 이야기
	infoheader->ImageSize = infoheader->width * infoheader->height; 
	// 틀어져도 전체 헤더를 전부 읽고, offset으로 점프해서 RAW 데이터를 읽으면, 영상이 출력되기는 한다
	infoheader->BitPerPxl = 8;
	// 픽셀당 비트수이기에, 8 이면, 일반적인 흑백을 나타내게 된다. 트루컬러는 24
	infoheader->compression = 0;
	infoheader->clrUsed = 256;
	// 팔레트 기반의 이미지에서 사용된다. 현재 bpp = 8이기에, 256개의 색을 사용이 가능하다 (0이면, 자동으로 전부 사용됨)
	// 여기서 clrused = 100이면, 그중에서 100개의 색을 사용한다고 미리 알려준다는 것 
	infoheader->clrImportant = 256;
	// 이미지의 품질을 저하시키지 않으면서 색상 수를 줄이는 최적화 과정에서 사용된다.
	// 이미지를 정상적으로 표현하려면 몇개의 수가 필요한지를 알려줌


	/*
	* bpp = 24면, 팔레트를 읽을 필요가 없다. 
	* 그러나, 현재는 흑백이기에, 팔레트 정보가 반드시 필요한 상황이다.
	* 흑백은 각 픽셀마다 r=g=b 이다.
	*/
	for (int i = 0; i < 256; i++)
	{
		rgb[i].blue = i;
		rgb[i].red = i;
		rgb[i].green = i;
		rgb[i].reserved = 0;
	}
	// 일반적인 bpp=8인 bmp에서의 총 크기는 1078 (=offset) 이다.

	if (infoheader->ImageSize + fileheader->offset != fileheader->size)
		return 8;

	return 0;
}

int zero_padding(BYTE* raw, int pedding_bytes, int pedding_loc)
{
	// 기본적으로 raw 파일은 거꾸로 뒤집혀있기에, 현재 추가되는 픽셀들은 화면에서의 왼쪽 위 가장자리 부분이다.
	
	for (int i = 0; i < pedding_bytes; i++)
	{
		raw[i+pedding_loc] = 0;
	}

	// 나머지를 모두 0으로 써버린다

	return 0;
}