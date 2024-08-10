#ifndef CONVERT_H
#define CONVERT_H

#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define QUIT 17

#pragma pack(push,2) 
// 윈도우가 bmp 헤더를 올바르게 읽어야 하기때문에, 구조체의 멤버를 기존의 8바이트가 아닌 2바이트로 맞춤

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef int LONG;

typedef struct
{
	WORD type;
	DWORD size;
	WORD reserved1;
	WORD reserved2;
	DWORD offset;
}BITMAPFILEHEADER;

typedef struct
{
	DWORD structSize;
	LONG width;
	LONG height;
	WORD planes;
	WORD BitPerPxl;
	DWORD compression;
	DWORD ImageSize;
	LONG xPelsPerMeter;
	LONG yPelsPerMeter;
	DWORD clrUsed;
	DWORD clrImportant;
}BITMAPINFOHEADER;

typedef struct
{
	BYTE blue;
	BYTE green;
	BYTE red;
	BYTE reserved;
}BITMAPColorPalette;

#pragma pack(pop)

extern int Extern_App_STAT;
extern BYTE WIN_STAT; //플래그들
extern char CMD[1024]; // 입력버퍼에 문자열을 쓰기위한 배열선언
// 전역변수는 BMP2TXT 소스에 선언되어 있다

int isBMP(char*);
int addName(char*, char**, unsigned int);

int convert_BMP(char*, char*, unsigned int);
void print_inform(BITMAPFILEHEADER*, BITMAPINFOHEADER*, BITMAPColorPalette*);
int print_data(BYTE*, DWORD, DWORD);
int mode_select(BYTE**, BYTE**, BITMAPINFOHEADER*, BITMAPFILEHEADER*, unsigned int, int*);

int convert_TXT(char*, char*);
void write_header(BITMAPFILEHEADER*, BITMAPINFOHEADER*, BITMAPColorPalette*, FILE*);
void write_raw(BITMAPINFOHEADER*, BYTE*, FILE*, int*);

int revising(char*, char*);
int revise_header(BITMAPFILEHEADER*, BITMAPINFOHEADER*, BITMAPColorPalette*);
int zero_padding(BYTE*, int, int);


#endif