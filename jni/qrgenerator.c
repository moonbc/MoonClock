#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qrencoder.h"
#include "SimpleImage.h"
#include "qrmemory.h"
#include "qrInterface.h"

#define MAX_STRINGSIZE		400
#define ESC			(char)27
#define QRCODE_SIZE		30	// 20mm
#define PCL5CMD_SIZE	100

#define QRSIZE_1CM		123	// Gap 123
#define QRSIZE_1_5CM	186
#define QRSIZE_2CM		246
#define QRSIZE_2_5CM	309
#define QRSIZE_3CM		369
#define QRSIZE_3_5CM	432

static int casesensitive = 1;
static int eightbit = 0;
static int version = 0;
static U32 size;
static int QRSIZE;
static int structured = 0;
static QRecLevel level = QR_ECLEVEL_L;
static QRencodeMode hint = QR_MODE_8;

extern U32 FileOpen(U8 * pubFileName);
extern void Command_Write(U8 * pubPCL5Command, U32 ulSize);
extern U8 *Malloc(U32 ulByteLen, enum Enum_HeapUse en_use);
extern void Free(U8 *ptr, U32 ul_byte_len, enum Enum_HeapUse en_use);
extern void FileClose(void);
extern void Show_Usaged(void);
extern void InitBlock(void);
extern void MakeBitmapHeader(U32 ulWidth, U32 ulStride, U32 ulsize);

static QRcode *encode(const char *intext)
{
	QRcode *code = NULL;

	if(eightbit) {
		code = QRcode_encodeString8bit(intext, version, level);
	} else {
		code = QRcode_encodeString(intext, version, level, hint, casesensitive);
	}

	return code;
}
U8 GetPixel(U8 *pubSrcData, U32 uloffset, U32 ulx, U32 uly)
{
	U8 ubColor;
	U32 uli = 0;
	U32 ulValue = 0;

	ulValue = size / 3;
	uli = (uly * uloffset) + (ulx * 3);
	uli+= ulValue;
	
	ubColor = pubSrcData[uli];
	
	return ubColor;

}
void GetLine(U8 *publine, U8 *pubGetData, U32 ulrow, U32 ulImgWidth, U32 uloffset)
{
	U8 *srcData;
	U8 ubValue = 0;
	U8 ubRed;
	U32 ulx = 1;
	U32 idx = 0;
	U32 uli = 0;
	U32 count;

	count = 0;	
	srcData = pubGetData;

	ubRed = GetPixel(pubGetData, uloffset, 0, ulrow);
		
	if(ubRed == 0)
		ubValue += (U8)ulx;
		
	for(uli = 0; uli< ulImgWidth; uli++)
	{
		if((uli % 8) == 0)
		{
			publine[idx++] = ubValue;
			ubValue = 0;
			ulx = 128;
		}
		else
		{
			publine[idx] = ubValue;
			ulx /= 2;
		}

		ubRed = GetPixel(pubGetData, uloffset, uli, ulrow);
		
		if(ubRed == 0)
			ubValue += (U8)ulx;
	}	
}

void MakeBitmapData(U8 *pBitmapData, U8* pSrcData, U32 ulWidth, U32 ulStride)
{
	U32 uly, ulx, uli, ulj;
	U8 clr;
	U8 *pbuf;
	
#ifdef SUPPORT_OUTPUT_IMAGE_BMP
	
	if(ulImgFlag == TRUE)
	{
		if(FileOpen(pubImgOutFile) == FALSE)
			return;

		MakeBitmapHeader(ulWidth, ulStride, size); /* BMP Header Setting*/
	}
#endif /* SUPPORT_OUTPUT_IMAGE_BMP */

	pbuf = &pBitmapData[ulWidth * size * ulStride];

	for (uly = 0; uly < ulWidth; uly++) 
	{ 
		pbuf -= ulStride; 
		for (ulx = 0; ulx < ulWidth; ulx++) 
		{ 			
			if ((U8)0x01 & (*pSrcData)) 
				clr = (U8)0x00;
			else 
				clr = (U8)0xff; 

			for (uli = 0; uli < 3 * size; uli++) 
			{ 
				*(pbuf + uli + ulx * size * 3) = clr; 				
			} 
			pSrcData++; 
		} 
		for (ulj = 0; ulj < size-1; ulj++) 
		{ 
			memcpy(pbuf-ulStride, pbuf, ulStride); 
			pbuf -= ulStride; 
		} 
	} 
#ifdef SUPPORT_OUTPUT_IMAGE_BMP
	if(ulImgFlag == TRUE)
	{
		Command_Write(pBitmapData, ulWidth * size * ulStride);
		FileClose();
	}
#endif /* SUPPORT_OUTPUT_IMAGE_BMP*/
}
void Convert_Bitmap2PCL5(U8 *pBitmapData, U32 ulWidth, U32 ulOffset, U8* pubPRNOutFile, U32 ulPrnFlag)
{
	
	U8* pLineData;
	U8 ubpcl5Cmd[PCL5CMD_SIZE];
	U32 ulWidthSize;
	U32 ulHeightSize;
	U32 ulMod8 = 0;
	S32 sli;
	

	if(ulPrnFlag == TRUE)
	{
		if(FileOpen(pubPRNOutFile) == FALSE)
			return;
	}
	else
		return;

	/* PCL5 Command */
	memset(ubpcl5Cmd, 0x00, PCL5CMD_SIZE);
	sprintf((char*)ubpcl5Cmd, "%c*t0300R%c*r0F%c*r1A", ESC, ESC, ESC, ESC);

	Command_Write(ubpcl5Cmd, strlen((char*)ubpcl5Cmd));

	ulWidthSize = ulWidth * size;	// 2cm : 246, 3cm : 369
	ulHeightSize =ulWidth * size;


	ulMod8 = (ulWidthSize % 8);

	if(ulMod8 > 0)
		ulWidthSize += 8 - ulMod8;
	
	ulWidthSize = (ulWidthSize / 8 ) + 1;
	pLineData = (U8*)Malloc(ulWidthSize, eHeapTemp);
	
	for(sli = ulHeightSize -1 ; sli >=  0; sli--)
	{		
		memset(pLineData, 0xff, ulWidthSize);
		GetLine(pLineData, pBitmapData, sli,ulWidth * size, ulOffset);

		/* PCL5 Command : Line Length  */
		memset(ubpcl5Cmd, 0x00, PCL5CMD_SIZE);
		sprintf((char*)ubpcl5Cmd, "%c*b%dW", ESC, (int)ulWidthSize);
		Command_Write(ubpcl5Cmd, strlen((char*)ubpcl5Cmd));
		Command_Write(pLineData, ulWidthSize);
		
	}	
	/* PCL5 Command */
	memset(ubpcl5Cmd, 0x00, PCL5CMD_SIZE);
	sprintf((char*)ubpcl5Cmd, "%c*rB", ESC);
	Command_Write(ubpcl5Cmd, strlen((char*)ubpcl5Cmd));
	
	if(ulPrnFlag == TRUE)
		FileClose();

	Free(pLineData, ulWidthSize, eHeapTemp);
}
void writePCL(QRcode *qrcode, U8 *pubImgoutfile, U32 ulImgFlag, U8* pubprnOutFIle, U32 ulPrnFlag)
{
	extern int test_one_file(const char *outname, unsigned long ulwidth, unsigned long ulheight, unsigned char *pub);
	U32 ulDataSize = 0;
	U32 nStride;
	U8 *pBitmapData;		 

	if(qrcode->width == 0)
		return;

	size = (int)((QRSIZE / qrcode->width) + 0.5);
	// pcl5 header....
	nStride = ((((qrcode->width*size * 24) + 31) & ~31) >> 3);
		
	ulDataSize = nStride * (qrcode->width * size);
	
	pBitmapData = (U8*)Malloc(ulDataSize, eHeapTemp);
	memset(pBitmapData, 0, ulDataSize);
			
	MakeBitmapData(pBitmapData, qrcode->data, qrcode->width, nStride);

	if(ulImgFlag == TRUE)
	{
		if(test_one_file((const char*)pubImgoutfile, qrcode->width * size , qrcode->width * size, &pBitmapData[ulDataSize]) == FALSE)
		{
			Free(pBitmapData, ulDataSize, eHeapTemp);	
			return;
		}
	}
	
	Convert_Bitmap2PCL5(pBitmapData, qrcode->width, nStride, pubprnOutFIle, ulPrnFlag);
	
	Free(pBitmapData, ulDataSize, eHeapTemp);	

}
void qrencode(char *intext, U8 *pubImgoutfile, U32 ulImgFlag, U8* ubprnOutFIle, U32 ulPrnFlag)
{
	QRcode *qrcode;

	qrcode = encode((const char*)intext);
	
	if(qrcode == NULL)
	{
		Print_Core("Failed to encode the input data\n");
		return;
	}
	writePCL(qrcode, pubImgoutfile, ulImgFlag, ubprnOutFIle,ulPrnFlag);


	QRcode_free(qrcode);
}
void QRCode_Size_Setting(U8* pubValue)
{
	switch(pubValue[0])
	{
	case '1':
		if(strlen((char*)pubValue) > 1)
			QRSIZE = QRSIZE_1_5CM;
		else 
			QRSIZE = QRSIZE_1CM;
		break;
	case '2':
		if(strlen((char*)pubValue) > 1)
			QRSIZE = QRSIZE_2_5CM;
		else
			QRSIZE = QRSIZE_2CM;
		break;
	case '3':
		if(strlen((char*)pubValue) > 1)
			QRSIZE = QRSIZE_3_5CM;
		else 
			QRSIZE = QRSIZE_3CM;
		break;
	default:
		QRSIZE = QRSIZE_3CM;
	}
}
void Help_Used(void)
{
	Print_Core("PDL Team : SoYoung.Kim (sy1124.kim@samsung.com)\n");
	Print_Core("Usage : qrgenerator.exe [OPTION] [STRING]\n");
	Print_Core("Encode Input dat in a QR Code and Save Bitmap. \n");
	Print_Core("========================================================================\n");
	Print_Core("	-h : display this message\n");
	Print_Core("	-B FILENAME : write png image to FILENAME. ex) -B test.png\n");
	Print_Core("	-P FILENAME : write prn to FILENAME. ex) -P test.prn\n");
	Print_Core("	-S size : QR code size. value : 1, 1.5, 2, 2.5, 3, 3.5 (default : 3) ex) -S 2.5");
	Print_Core("	-T string : QR code input text. ex) -T \"www.naver.com\"\n");
	Print_Core("========================================================================\n");

	return ;
}

// input : qrgenerator -M string -P string -A string -S string -O a.bmp
int main(int argc, char **argv)
{
	int i = 0;
	char *pszTemp = NULL;
	char stInputText[MAX_STRINGSIZE+1]= {0,};

	char BMPoutfile[MAX_STRINGSIZE+1] = {0,};
	char prnOutFile[MAX_STRINGSIZE+1] = {0,};
	U32 bmpFlag, PRNFlag;
	
	bmpFlag = FALSE;
	PRNFlag = FALSE;

	if(argc == 1)
	{
		Help_Used();
		return 0;
	}
	
	QRSIZE = QRSIZE_3CM;

	for(i = 1; i< argc; i++)
	{
		if(*argv[i] == '-')
		{
			pszTemp = argv[i];
		}
		if(pszTemp == NULL)
			return 0;

		if(!strcmp(pszTemp, "-T") || !strcmp(pszTemp, "-t"))
		{
			i++;
			if(strlen(argv[i]) > MAX_STRINGSIZE)
			{
				return 0;
			}
			strncpy(stInputText, argv[i], strlen(argv[i]));
		}
		else if(!strcmp(pszTemp, "-B") || !strcmp(pszTemp, "-b"))
		{
			i++;
			if(strlen(argv[i]) > MAX_STRINGSIZE)
			{
				return 0;
			}
			strncpy(BMPoutfile, argv[i], strlen(argv[i]));
			bmpFlag = TRUE;
		}
		else if(!strcmp(pszTemp, "-P") || !strcmp(pszTemp, "-p"))
		{
			i++;
			if(strlen(argv[i]) > MAX_STRINGSIZE)
			{
				return 0;
			}
			strncpy(prnOutFile, argv[i], strlen(argv[i]));
			PRNFlag = TRUE;
		}
		else if(!strcmp(pszTemp, "-S") || !strcmp(pszTemp, "-s"))
		{
			i++;
			QRCode_Size_Setting((U8*)argv[i]);
		}
		else if(!strcmp(pszTemp, "-h") || !strcmp(pszTemp, "-H"))
		{
			Help_Used();
			return 0;
		}		

	}
	InitBlock();
	
	if(strlen(stInputText) > 0 && strlen(stInputText) < MAX_STRINGSIZE)
		qrencode(stInputText, (U8*)BMPoutfile, bmpFlag, (U8*)prnOutFile, PRNFlag);

	Show_Usaged();

	return 0;
}