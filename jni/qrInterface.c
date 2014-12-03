#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "enum.h"
#include "qrmemory.h"
#include "SimpleImage.h"
#include "qrInterface.h"
FILE *fp;

#define BI_RGB        0L

#ifdef MEMORY_USAGE
S32 HEAP_BLOCK[500];
S32 HEAP_BLOCK_SIZE;
#endif

U32 FileOpen(U8 * pubFileName)
{	
	fp = fopen((char*)pubFileName, "wb");

	if(fp == NULL)
	{
		Print_Core("File Saved Error!\n");
		return FALSE;
	}	
	return TRUE;
}
void FileClose(void)
{
	int ret;
	ret = fclose(fp);
}
void Command_Write(U8 * pubPCL5Command, U32 ulSize)
{
	
	fwrite(pubPCL5Command, ulSize, 1, fp);

}

void MakeBitmapHeader(U32 ulWidth, U32 ulStride, U32 ulsize)
{
	BITMAPINFO Bi;
	BITMAPFILEHEADER Bfh;
	U32 ulBfnSize;

	memset(&Bi.bmiHeader, 0, sizeof(BITMAPINFOHEADER)); 
	memset(&Bi.bmiColors, 0, sizeof(RGBQUAD));

	Bi.bmiHeader.biSize                 = sizeof(BITMAPINFOHEADER);
	Bi.bmiHeader.biWidth                = ulWidth * ulsize;
	Bi.bmiHeader.biHeight               = ulWidth * ulsize; 
	Bi.bmiHeader.biPlanes               = 1;
	Bi.bmiHeader.biBitCount             = 24;
	Bi.bmiHeader.biCompression		    = BI_RGB;
	Bi.bmiHeader.biSizeImage            = ulStride * Bi.bmiHeader.biHeight;
		
	memset(&Bfh, 0x00, sizeof(BITMAPFILEHEADER));
	
	Bfh.bfType = 0x4d42;//bitmap 정의값. 항상 이 값이어야 한다
	ulBfnSize = sizeof(Bfh.bfType) + sizeof(Bfh.bfSize) + sizeof(Bfh.bfReserved1) + sizeof(Bfh.bfReserved2) + sizeof(Bfh.bfOffBits);
	Bfh.bfOffBits = ulBfnSize + sizeof(BITMAPINFOHEADER);
	Bfh.bfSize = ulBfnSize + sizeof(BITMAPINFOHEADER) + Bi.bmiHeader.biSizeImage ;

	fwrite(&Bfh.bfType, sizeof(Bfh.bfType), 1, fp);
	fwrite(&Bfh.bfSize, sizeof(Bfh.bfSize), 1, fp);
	fwrite(&Bfh.bfReserved1, sizeof(Bfh.bfReserved1), 1, fp);
	fwrite(&Bfh.bfReserved2, sizeof(Bfh.bfReserved2), 1, fp);
	fwrite(&Bfh.bfOffBits, sizeof(Bfh.bfOffBits), 1, fp);
	fwrite(&Bi, sizeof(BITMAPINFOHEADER), 1, fp);
	
}

void InitBlock(void)
{
#ifdef MEMORY_USAGE
	HEAP_BLOCK_SIZE = 0;
	
#endif
}
U8 *Malloc(U32 ulByteLen, enum Enum_HeapUse en_use)
{
	U8 *pub;	
	enum Enum_HeapUse enUse;
	U32 size;

	size = ulByteLen + sizeof(unsigned long);
	
	pub = (U8*)malloc(size);

	(*(unsigned long*)pub) = ulByteLen;
	pub += sizeof(unsigned long);
	
	enUse = en_use;
#ifdef MEMORY_USAGE	
	
	HEAP_BLOCK_SIZE += 1;//ulByteLen;
#endif
	return pub;	
}

void Free(U8 *ptr, U32 ul_byte_len, enum Enum_HeapUse en_use)
{
	enum Enum_HeapUse enUse;
	U32 ulSize = 0;
	
	enUse = en_use;
	
	if((ptr == NULL))
		return ;

	ptr -= sizeof(U32);
	ulSize =(*(U32*)ptr);	

	if(ulSize == 0)
		return ;

#ifdef MEMORY_USAGE
	HEAP_BLOCK_SIZE -= 1;
#endif
	free(ptr);
}

void Show_Usaged(void)
{	
#ifdef MEMORY_USAGE
	U32 i;
	for(i =0; i<500; i++)
	{
		if(HEAP_BLOCK[i] > 0)
			Print_Core("\nHEAP_BLOCK[%d] =  %d\n", i, HEAP_BLOCK[i]);
	}
	Print_Core("\nHEAP_BLOCK_SIZE =  %d\n", HEAP_BLOCK_SIZE);
#endif
}