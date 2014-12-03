
#include <stdio.h>
#include <stdlib.h>
#include "enum.h"


typedef struct tagRGBQUAD {
        U8    rgbBlue;
        U8    rgbGreen;
        U8    rgbRed;
        U8    rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFOHEADER{
        U32      biSize;
        LONG       biWidth;
        LONG       biHeight;
        U16       biPlanes;
        U16       biBitCount;
        U32      biCompression;
        U32      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        U32      biClrUsed;
        U32      biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPINFO {
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
} BITMAPINFO;

typedef struct tagBITMAPFILEHEADER {
        U16   bfType;
        U32   bfSize;
        U16   bfReserved1;
        U16   bfReserved2;
        U32   bfOffBits;
} BITMAPFILEHEADER;
