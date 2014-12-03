#include <stdio.h>
#include "zlib.h"	//z_stream
#include "qrmemory.h"

typedef unsigned char   U8;
typedef signed char     S8;
typedef unsigned short  U16;
typedef signed short    S16;
typedef unsigned long   U32;
typedef signed long     S32;
typedef long			LONG;

#define FALSE	0
#define TRUE	1

extern unsigned char *Malloc( unsigned long ul_byte_len, enum Enum_HeapUse en_use );
extern void Free(unsigned char *ptr, unsigned long ul_byte_len, enum Enum_HeapUse en_use );

unsigned char * ZLib_Malloc(voidpf opaque, unsigned long ulItems, signed long sl_byte_len)
{
	U8* ptr_U8 = NULL;
	U8* ptr_Return = NULL;
	U32* ptr_U32 = NULL;
	U32 ulByteLen = (ulItems * (U32)sl_byte_len) + 4;

	if(ulByteLen)
	{
		ptr_U8 = Malloc( ulByteLen, (enum Enum_HeapUse)0);
		if( ptr_U8 != NULL)
		{
			ptr_U32 = (U32*)ptr_U8;
			(*ptr_U32) = ulByteLen;

			ptr_Return = (ptr_U8 + 4);
		}
	}

	return ptr_Return;
}

void ZLib_Free(voidpf opaque, U8* addr)
{
	U8* ptr_U8 = NULL;
	U32* ptr_U32 = NULL;
	U32 ulByteLen;

	if(addr)
	{
		ptr_U8 = (addr - 4);
		ptr_U32 = (U32*)ptr_U8;	
		ulByteLen = (*ptr_U32);

		if(ulByteLen)
			Free(ptr_U8, ulByteLen, (enum Enum_HeapUse)0 );
	}
}


///////////////////
/* from IPS ps/zlibsupp.c */
int uncompress_init(z_stream **ppZPtr)
{
    int err; 
    z_stream *ZPtr;

    ZPtr = (*ppZPtr) = (z_stream*)Malloc(sizeof(z_stream), 0);	
    //if (ZPtr == NULL)
    //    return err;

    ZPtr->zalloc = (alloc_func)ZLib_Malloc; 
    ZPtr->zfree = (free_func)ZLib_Free; 
    ZPtr->opaque = 0;//CSW 20130129 Not used //(voidp)pPDIstatePS->memSpace; 

    err = inflateInit(ZPtr); 

    return err;

}

int do_uncompress(z_stream *ZPtr,U8 *compr, U32 comprLen, U8 *uncompr, 
		  U32 *uncomprLen, U32 *newcomprLen)
{

    int status = Z_OK;

    ZPtr->next_out = uncompr; 
    ZPtr->avail_out = /*(U16)*/ (*uncomprLen); 
    ZPtr->next_in = compr;
    ZPtr->avail_in = /*(U16)*/ comprLen;
    status = inflate(ZPtr, Z_FULL_FLUSH); 

    *newcomprLen = comprLen - ((U32) ZPtr->avail_in);
    *uncomprLen = *uncomprLen - ((U32) ZPtr->avail_out);
    return status;
}

int uncompress_end(z_stream *ZPtr)
{

  int Stat;

  Stat = inflateEnd(ZPtr); 
  if (ZPtr)
  {
	  Free((U8*)ZPtr, sizeof(z_stream), 0 );	
      ZPtr = NULL;
  }

  return (Stat);
    
}
