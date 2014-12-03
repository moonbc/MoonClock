
/* pngtest.c - a simple test program to test libpng
 *
 * Last changed in libpng 1.2.51 [February 6, 2014]
 * Copyright (c) 1998-2014 Glenn Randers-Pehrson
 * (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
 * (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 *
 * This code is released under the libpng license.
 * For conditions of distribution and use, see the disclaimer
 * and license in png.h
 *
 * This program reads in a PNG image, writes it out again, and then
 * compares the two files.  If the files are identical, this shows that
 * the basic chunk handling, filtering, and (de)compression code is working
 * properly.  It does not currently test all of the transforms, although
 * it probably should.
 *
 * The program will report "FAIL" in certain legitimate cases:
 * 1) when the compression level or filter selection method is changed.
 * 2) when the maximum IDAT size (PNG_ZBUF_SIZE in pngconf.h) is not 8192.
 * 3) unknown unsafe-to-copy ancillary chunks or unknown critical chunks
 *    exist in the input file.
 * 4) others not listed here...
 * In these cases, it is best to check with another tool such as "pngcheck"
 * to see what the differences between the two files are.
 *
 * If a filename is given on the command-line, then this file is used
 * for the input, rather than the default "pngtest.png".  This allows
 * testing a wide variety of files easily.  You can also test a number
 * of files at once by typing "pngtest -m file1.png file2.png ..."
 */

#define PNG_PEDANTIC_WARNINGS
#include "png.h"

#ifdef _WIN32_WCE
#  if _WIN32_WCE < 211
     __error__ (f|w)printf functions are not supported on old WindowsCE.;
#  endif
#  include <windows.h>
#  include <stdlib.h>
#  define READFILE(file, data, length, check) \
     if (ReadFile(file, data, length, &check, NULL)) check = 0
#  define WRITEFILE(file, data, length, check)) \
     if (WriteFile(file, data, length, &check, NULL)) check = 0
#  define FCLOSE(file) CloseHandle(file)
#else
#  include <stdio.h>
#  include <stdlib.h>
#  define READFILE(file, data, length, check) \
     check=(png_size_t)fread(data, (png_size_t)1, length, file)
#  define WRITEFILE(file, data, length, check) \
     check=(png_size_t)fwrite(data, (png_size_t)1, length, file)

#endif

#ifndef PNG_STDIO_SUPPORTED
#  ifdef _WIN32_WCE
     typedef HANDLE                png_FILE_p;
#  else
     typedef FILE                * png_FILE_p;
#  endif
#endif

/* Makes pngtest verbose so we can find problems (needs to be before png.h) */
#ifndef PNG_DEBUG
#  define PNG_DEBUG 0
#endif

#if !PNG_DEBUG
#  define SINGLE_ROWBUF_ALLOC  /* Makes buffer overruns easier to nail */
#endif

/* Turn on CPU timing
#define PNGTEST_TIMING
*/

#ifndef PNG_FLOATING_POINT_SUPPORTED
#undef PNGTEST_TIMING
#endif

#ifdef PNGTEST_TIMING
static float t_start, t_stop, t_decode, t_encode, t_misc;
#include <time.h>
#endif

#ifdef PNG_TIME_RFC1123_SUPPORTED
#define PNG_tIME_STRING_LENGTH 29
static int tIME_chunk_present = 0;
static char tIME_string[PNG_tIME_STRING_LENGTH] = "tIME chunk is not present";
#endif

static int verbose = 0;

//int test_one_file PNGARG(((PNG_CONST char *outname, unsigned long ulwidth, unsigned long ulheight, unsigned char *pubImage)));

#ifdef __TURBOC__
#include <mem.h>
#endif

/* Defined so I can write to a file on gui/windowing platforms */
/*  #define STDERR stderr  */
#define STDERR stdout   /* For DOS */

/* In case a system header (e.g., on AIX) defined jmpbuf */
#ifdef jmpbuf
#  undef jmpbuf
#endif

/* Define png_jmpbuf() in case we are using a pre-1.0.6 version of libpng */
#ifndef png_jmpbuf
#  define png_jmpbuf(png_ptr) png_ptr->jmpbuf
#endif

/* Example of using row callbacks to make a simple progress meter */
static int status_pass = 1;
static int status_dots_requested = 0;
static int status_dots = 1;

void
#ifdef PNG_1_0_X
PNGAPI
#endif
read_row_callback(png_structp png_ptr, png_uint_32 row_number, int pass);
void
#ifdef PNG_1_0_X
PNGAPI
#endif
read_row_callback(png_structp png_ptr, png_uint_32 row_number, int pass)
{
   if (png_ptr == NULL || row_number > PNG_UINT_31_MAX)
      return;
   if (status_pass != pass)
   {
      
      status_pass = pass;
      status_dots = 31;
   }
   status_dots--;
   if (status_dots == 0)
   {
      
      status_dots=30;
   }
   
}

void
#ifdef PNG_1_0_X
PNGAPI
#endif
write_row_callback(png_structp png_ptr, png_uint_32 row_number, int pass);
void
#ifdef PNG_1_0_X
PNGAPI
#endif
write_row_callback(png_structp png_ptr, png_uint_32 row_number, int pass)
{
   if (png_ptr == NULL || row_number > PNG_UINT_31_MAX || pass > 7)
      return;
   
}


#ifdef PNG_READ_USER_TRANSFORM_SUPPORTED
/* Example of using user transform callback (we don't transform anything,
 * but merely examine the row filters.  We set this to 256 rather than
 * 5 in case illegal filter values are present.)
 */
static png_uint_32 filters_used[256];
void
#ifdef PNG_1_0_X
PNGAPI
#endif
count_filters(png_structp png_ptr, png_row_infop row_info, png_bytep data);
void
#ifdef PNG_1_0_X
PNGAPI
#endif
count_filters(png_structp png_ptr, png_row_infop row_info, png_bytep data)
{
   if (png_ptr != NULL && row_info != NULL)
      ++filters_used[*(data - 1)];
}
#endif

#ifdef PNG_WRITE_USER_TRANSFORM_SUPPORTED
/* Example of using user transform callback (we don't transform anything,
 * but merely count the zero samples)
 */

static png_uint_32 zero_samples;

void
#ifdef PNG_1_0_X
PNGAPI
#endif
count_zero_samples(png_structp png_ptr, png_row_infop row_info, png_bytep data);
void
#ifdef PNG_1_0_X
PNGAPI
#endif
count_zero_samples(png_structp png_ptr, png_row_infop row_info, png_bytep data)
{
   png_bytep dp = data;
   if (png_ptr == NULL)return;

   /* Contents of row_info:
    *  png_uint_32 width      width of row
    *  png_uint_32 rowbytes   number of bytes in row
    *  png_byte color_type    color type of pixels
    *  png_byte bit_depth     bit depth of samples
    *  png_byte channels      number of channels (1-4)
    *  png_byte pixel_depth   bits per pixel (depth*channels)
    */

    /* Counts the number of zero samples (or zero pixels if color_type is 3 */

    if (row_info->color_type == 0 || row_info->color_type == 3)
    {
       int pos = 0;
       png_uint_32 n, nstop;
       for (n = 0, nstop=row_info->width; n<nstop; n++)
       {
          if (row_info->bit_depth == 1)
          {
             if (((*dp << pos++ ) & 0x80) == 0)
                zero_samples++;
             if (pos == 8)
             {
                pos = 0;
                dp++;
             }
          }
          if (row_info->bit_depth == 2)
          {
             if (((*dp << (pos+=2)) & 0xc0) == 0)
                zero_samples++;
             if (pos == 8)
             {
                pos = 0;
                dp++;
             }
          }
          if (row_info->bit_depth == 4)
          {
             if (((*dp << (pos+=4)) & 0xf0) == 0)
                zero_samples++;
             if (pos == 8)
             {
                pos = 0;
                dp++;
             }
          }
          if (row_info->bit_depth == 8)
             if (*dp++ == 0)
                zero_samples++;
          if (row_info->bit_depth == 16)
          {
             if ((*dp | *(dp+1)) == 0)
                zero_samples++;
             dp+=2;
          }
       }
    }
    else /* Other color types */
    {
       png_uint_32 n, nstop;
       int channel;
       int color_channels = row_info->channels;
       if (row_info->color_type > 3)color_channels--;

       for (n = 0, nstop=row_info->width; n<nstop; n++)
       {
          for (channel = 0; channel < color_channels; channel++)
          {
             if (row_info->bit_depth == 8)
                if (*dp++ == 0)
                   zero_samples++;
             if (row_info->bit_depth == 16)
             {
                if ((*dp | *(dp+1)) == 0)
                   zero_samples++;
                dp+=2;
             }
          }
          if (row_info->color_type > 3)
          {
             dp++;
             if (row_info->bit_depth == 16)
                dp++;
          }
       }
    }
}
#endif /* PNG_WRITE_USER_TRANSFORM_SUPPORTED */

static int wrote_question = 0;

#ifndef PNG_STDIO_SUPPORTED
/* START of code to validate stdio-free compilation */
/* These copies of the default read/write functions come from pngrio.c and
 * pngwio.c.  They allow "don't include stdio" testing of the library.
 * This is the function that does the actual reading of data.  If you are
 * not reading from a standard C stream, you should create a replacement
 * read_data function and use it at run time with png_set_read_fn(), rather
 * than changing the library.
 */

#ifndef USE_FAR_KEYWORD
static void
pngtest_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
   png_size_t check = 0;
   png_voidp io_ptr;

   /* fread() returns 0 on error, so it is OK to store this in a png_size_t
    * instead of an int, which is what fread() actually returns.
    */
   io_ptr = png_get_io_ptr(png_ptr);
   if (io_ptr != NULL)
   {
      READFILE((png_FILE_p)io_ptr, data, length, check);
   }

   if (check != length)
   {
      png_error(png_ptr, "Read Error!");
   }
}
#else
/* This is the model-independent version. Since the standard I/O library
   can't handle far buffers in the medium and small models, we have to copy
   the data.
*/

#define NEAR_BUF_SIZE 1024
#define MIN(a,b) (a <= b ? a : b)

static void
pngtest_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
   int check;
   png_byte *n_data;
   png_FILE_p io_ptr;

   /* Check if data really is near. If so, use usual code. */
   n_data = (png_byte *)CVT_PTR_NOCHECK(data);
   io_ptr = (png_FILE_p)CVT_PTR(png_ptr->io_ptr);
   if ((png_bytep)n_data == data)
   {
      READFILE(io_ptr, n_data, length, check);
   }
   else
   {
      png_byte buf[NEAR_BUF_SIZE];
      png_size_t read, remaining, err;
      check = 0;
      remaining = length;
      do
      {
         read = MIN(NEAR_BUF_SIZE, remaining);
         READFILE(io_ptr, buf, 1, err);
         png_memcpy(data, buf, read); /* Copy far buffer to near buffer */
         if (err != read)
            break;
         else
            check += err;
         data += read;
         remaining -= read;
      }
      while (remaining != 0);
   }
   if (check != length)
      png_error(png_ptr, "read Error");
}
#endif /* USE_FAR_KEYWORD */

#ifdef PNG_WRITE_FLUSH_SUPPORTED
static void
pngtest_flush(png_structp png_ptr)
{
   /* Do nothing; fflush() is said to be just a waste of energy. */
   PNG_UNUSED(png_ptr)  /* Stifle compiler warning */
}
#endif

/* This is the function that does the actual writing of data.  If you are
 * not writing to a standard C stream, you should create a replacement
 * write_data function and use it at run time with png_set_write_fn(), rather
 * than changing the library.
 */
#ifndef USE_FAR_KEYWORD
static void
pngtest_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
   png_uint_32 check;

   WRITEFILE((png_FILE_p)png_ptr->io_ptr,  data, length, check);
   if (check != length)
   {
      png_error(png_ptr, "Write Error");
   }
}
#else
/* This is the model-independent version. Since the standard I/O library
   can't handle far buffers in the medium and small models, we have to copy
   the data.
*/

#define NEAR_BUF_SIZE 1024
#define MIN(a,b) (a <= b ? a : b)

static void
pngtest_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
   png_uint_32 check;
   png_byte *near_data;  /* Needs to be "png_byte *" instead of "png_bytep" */
   png_FILE_p io_ptr;

   /* Check if data really is near. If so, use usual code. */
   near_data = (png_byte *)CVT_PTR_NOCHECK(data);
   io_ptr = (png_FILE_p)CVT_PTR(png_ptr->io_ptr);
   if ((png_bytep)near_data == data)
   {
      WRITEFILE(io_ptr, near_data, length, check);
   }
   else
   {
      png_byte buf[NEAR_BUF_SIZE];
      png_size_t written, remaining, err;
      check = 0;
      remaining = length;
      do
      {
         written = MIN(NEAR_BUF_SIZE, remaining);
         png_memcpy(buf, data, written); /* Copy far buffer to near buffer */
         WRITEFILE(io_ptr, buf, written, err);
         if (err != written)
            break;
         else
            check += err;
         data += written;
         remaining -= written;
      }
      while (remaining != 0);
   }
   if (check != length)
   {
      png_error(png_ptr, "Write Error");
   }
}
#endif /* USE_FAR_KEYWORD */

/* This function is called when there is a warning, but the library thinks
 * it can continue anyway.  Replacement functions don't have to do anything
 * here if you don't want to.  In the default configuration, png_ptr is
 * not used, but it is passed in case it may be useful.
 */
static void
pngtest_warning(png_structp png_ptr, png_const_charp message)
{
   PNG_CONST char *name = "UNKNOWN (ERROR!)";
   char *test;
   test = png_get_error_ptr(png_ptr);
   
}

/* This is the default error handling function.  Note that replacements for
 * this function MUST NOT RETURN, or the program will likely crash.  This
 * function is used by default, or if the program supplies NULL for the
 * error function pointer in png_set_error_fn().
 */
static void
pngtest_error(png_structp png_ptr, png_const_charp message)
{
   pngtest_warning(png_ptr, message);
   /* We can return because png_error calls the default handler, which is
    * actually OK in this case.
    */
}
#endif /* !PNG_STDIO_SUPPORTED */
/* END of code to validate stdio-free compilation */

/* START of code to validate memory allocation and deallocation */
#if defined(PNG_USER_MEM_SUPPORTED) && PNG_DEBUG

/* Allocate memory.  For reasonable files, size should never exceed
 * 64K.  However, zlib may allocate more then 64K if you don't tell
 * it not to.  See zconf.h and png.h for more information.  zlib does
 * need to allocate exactly 64K, so whatever you call here must
 * have the ability to do that.
 *
 * This piece of code can be compiled to validate max 64K allocations
 * by setting MAXSEG_64K in zlib zconf.h *or* PNG_MAX_MALLOC_64K.
 */
typedef struct memory_information
{
   png_uint_32               size;
   png_voidp                 pointer;
   struct memory_information FAR *next;
} memory_information;
typedef memory_information FAR *memory_infop;

static memory_infop pinformation = NULL;
static int current_allocation = 0;
static int maximum_allocation = 0;
static int total_allocation = 0;
static int num_allocations = 0;

png_voidp png_debug_malloc PNGARG((png_structp png_ptr, png_uint_32 size));
void png_debug_free PNGARG((png_structp png_ptr, png_voidp ptr));

png_voidp
png_debug_malloc(png_structp png_ptr, png_uint_32 size)
{

   /* png_malloc has already tested for NULL; png_create_struct calls
    * png_debug_malloc directly, with png_ptr == NULL which is OK
    */

   if (size == 0)
      return (NULL);

   /* This calls the library allocator twice, once to get the requested
      buffer and once to get a new free list entry. */
   {
      /* Disable malloc_fn and free_fn */
      memory_infop pinfo;
      png_set_mem_fn(png_ptr, NULL, NULL, NULL);
      pinfo = (memory_infop)png_malloc(png_ptr,
         (png_uint_32)png_sizeof(*pinfo));
      pinfo->size = size;
      current_allocation += size;
      total_allocation += size;
      num_allocations ++;
      if (current_allocation > maximum_allocation)
         maximum_allocation = current_allocation;
      pinfo->pointer = (png_voidp)png_malloc(png_ptr, size);
      /* Restore malloc_fn and free_fn */
      png_set_mem_fn(png_ptr,
          png_voidp_NULL, (png_malloc_ptr)png_debug_malloc,
          (png_free_ptr)png_debug_free);
      if (size != 0 && pinfo->pointer == NULL)
      {
         current_allocation -= size;
         total_allocation -= size;
         png_error(png_ptr,
           "out of memory in pngtest->png_debug_malloc.");
      }
      pinfo->next = pinformation;
      pinformation = pinfo;
      /* Make sure the caller isn't assuming zeroed memory. */
      png_memset(pinfo->pointer, 0xdd, pinfo->size);
      if (verbose)
         printf("png_malloc %lu bytes at %x\n", (unsigned long)size,
            pinfo->pointer);
      return (png_voidp)(pinfo->pointer);
   }
}

/* Free a pointer.  It is removed from the list at the same time. */
void
png_debug_free(png_structp png_ptr, png_voidp ptr)
{
   
   if (ptr == 0)
   {

      return;
   }

   /* Unlink the element from the list. */
   {
      memory_infop FAR *ppinfo = &pinformation;
      for (;;)
      {
         memory_infop pinfo = *ppinfo;
         if (pinfo->pointer == ptr)
         {
            *ppinfo = pinfo->next;
            current_allocation -= pinfo->size;
            
            png_memset(ptr, 0x55, pinfo->size);
            png_free_default(png_ptr, pinfo);
            pinfo = NULL;
            break;
         }
         if (pinfo->next == NULL)
         {
            
            break;
         }
         ppinfo = &pinfo->next;
      }
   }

   /* Finally free the data. */
   if (verbose)
      printf("Freeing %x\n", ptr);
   png_free_default(png_ptr, ptr);
   ptr = NULL;
}
#endif /* PNG_USER_MEM_SUPPORTED && PNG_DEBUG */
/* END of code to test memory allocation/deallocation */


/* Demonstration of user chunk support of the sTER and vpAg chunks */
#ifdef PNG_UNKNOWN_CHUNKS_SUPPORTED

/* (sTER is a public chunk not yet known by libpng.  vpAg is a private
chunk used in ImageMagick to store "virtual page" size).  */

static png_uint_32 user_chunk_data[4];

    /* 0: sTER mode + 1
     * 1: vpAg width
     * 2: vpAg height
     * 3: vpAg units
     */

static int read_user_chunk_callback(png_struct *png_ptr,
   png_unknown_chunkp chunk)
{
   png_uint_32
     *my_user_chunk_data;

   /* Return one of the following:
    *    return (-n);  chunk had an error
    *    return (0);  did not recognize
    *    return (n);  success
    *
    * The unknown chunk structure contains the chunk data:
    * png_byte name[5];
    * png_byte *data;
    * png_size_t size;
    *
    * Note that libpng has already taken care of the CRC handling.
    */

   if (chunk->name[0] == 115 && chunk->name[1] ==  84 &&     /* s  T */
       chunk->name[2] ==  69 && chunk->name[3] ==  82)       /* E  R */
      {
         /* Found sTER chunk */
         if (chunk->size != 1)
            return (-1); /* Error return */
         if (chunk->data[0] != 0 && chunk->data[0] != 1)
            return (-1);  /* Invalid mode */
         my_user_chunk_data=(png_uint_32 *) png_get_user_chunk_ptr(png_ptr);
         my_user_chunk_data[0]=chunk->data[0]+1;
         return (1);
      }

   if (chunk->name[0] != 118 || chunk->name[1] != 112 ||    /* v  p */
       chunk->name[2] !=  65 || chunk->name[3] != 103)      /* A  g */
      return (0); /* Did not recognize */

   /* Found ImageMagick vpAg chunk */

   if (chunk->size != 9)
      return (-1); /* Error return */

   my_user_chunk_data=(png_uint_32 *) png_get_user_chunk_ptr(png_ptr);

   my_user_chunk_data[1]=png_get_uint_31(png_ptr, chunk->data);
   my_user_chunk_data[2]=png_get_uint_31(png_ptr, chunk->data + 4);
   my_user_chunk_data[3]=(png_uint_32)chunk->data[8];

   return (1);

}
#endif
/* END of code to demonstrate user chunk support */

/* Test one file */
extern unsigned long FileOpen(unsigned char * pubFileName);
extern void FileClose(void);
int
test_one_file(unsigned char *outname, unsigned long ulwidth, unsigned long ulheight, unsigned char *pubImage)
{
    //static png_FILE_p fpout;  /* "static" prevents setjmp corruption */
   
#ifdef PNG_WRITE_SUPPORTED
   png_structp write_ptr;
   png_infop write_info_ptr;
   png_infop write_end_info_ptr;
#else
   png_structp write_ptr = NULL;
   png_infop write_info_ptr = NULL;
   png_infop write_end_info_ptr = NULL;
#endif
   png_bytep row_buf;
   png_uint_32 y;
   png_uint_32 width, height;
   int num_pass, pass;
   int bit_depth, color_type;
   png_uint_32 nStride;
#ifdef PNG_SETJMP_SUPPORTED
#ifdef USE_FAR_KEYWORD
   jmp_buf jmpbuf;
#endif
#endif

#ifdef _WIN32_WCE
   TCHAR path[MAX_PATH];
#endif

   width = ulwidth;
   height = ulheight;
   nStride = ((((width * 24) + 31) & ~31) >> 3);
   row_buf = NULL;
   color_type = PNG_COLOR_MASK_COLOR;
   bit_depth = 8;

   if(FileOpen(outname) == 0)
	   return 0;
      
#ifdef PNG_WRITE_SUPPORTED
#if defined(PNG_USER_MEM_SUPPORTED) && PNG_DEBUG
   write_ptr =
      png_create_write_struct_2(PNG_LIBPNG_VER_STRING, png_voidp_NULL,
      png_error_ptr_NULL, png_error_ptr_NULL, png_voidp_NULL,
      (png_malloc_ptr)png_debug_malloc, (png_free_ptr)png_debug_free);
#else
   write_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, png_voidp_NULL,
      png_error_ptr_NULL, png_error_ptr_NULL);
#endif

#ifndef PNG_STDIO_SUPPORTED
   png_set_error_fn(write_ptr, (png_voidp)inname, pngtest_error,
       pngtest_warning);
#endif
#endif

#ifdef PNG_WRITE_SUPPORTED
   write_info_ptr = png_create_info_struct(write_ptr);
   write_end_info_ptr = png_create_info_struct(write_ptr);
#endif

#ifdef PNG_SETJMP_SUPPORTED
   

#ifdef USE_FAR_KEYWORD
   png_memcpy(png_jmpbuf(read_ptr), jmpbuf, png_sizeof(jmp_buf));
#endif

#ifdef PNG_WRITE_SUPPORTED
   
#ifdef USE_FAR_KEYWORD
   if (setjmp(jmpbuf))
#else
   if (setjmp(png_jmpbuf(write_ptr)))
#endif
   {
#ifdef PNG_WRITE_SUPPORTED
      png_destroy_write_struct(&write_ptr, &write_info_ptr);
#endif

	  FileClose();
      return (1);
   }
#ifdef USE_FAR_KEYWORD
   png_memcpy(png_jmpbuf(write_ptr), jmpbuf, png_sizeof(jmp_buf));
#endif
#endif
#endif

   
#ifdef PNG_STDIO_SUPPORTED

#  ifdef PNG_WRITE_SUPPORTED
   png_init_io(write_ptr, NULL);
#  endif
#else
   png_set_read_fn(read_ptr, (png_voidp)fpin, pngtest_read_data);
#  ifdef PNG_WRITE_SUPPORTED
   png_set_write_fn(write_ptr, (png_voidp)fpout,  pngtest_write_data,
#    ifdef PNG_WRITE_FLUSH_SUPPORTED
      pngtest_flush);
#    else
      NULL);
#    endif
#  endif
#endif
   if (status_dots_requested == 1)
   {
#ifdef PNG_WRITE_SUPPORTED
      png_set_write_status_fn(write_ptr, write_row_callback);
#endif
   }
   else
   {
#ifdef PNG_WRITE_SUPPORTED
      png_set_write_status_fn(write_ptr, png_write_status_ptr_NULL);
#endif

   }

#ifdef PNG_WRITE_USER_TRANSFORM_SUPPORTED
   zero_samples = 0;
   png_set_write_user_transform_fn(write_ptr, count_zero_samples);
#endif

#ifdef PNG_READ_UNKNOWN_CHUNKS_SUPPORTED
#  ifndef PNG_HANDLE_CHUNK_ALWAYS
#    define PNG_HANDLE_CHUNK_ALWAYS       3
#  endif

#endif
#ifdef PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED
#  ifndef PNG_HANDLE_CHUNK_IF_SAFE
#    define PNG_HANDLE_CHUNK_IF_SAFE      2
#  endif
   png_set_keep_unknown_chunks(write_ptr, PNG_HANDLE_CHUNK_IF_SAFE,
      png_bytep_NULL, 0);
#endif

   {
      int interlace_type = 0, compression_type = 0, filter_type = 0;

      {
         png_set_IHDR(write_ptr, write_info_ptr, width, height, bit_depth,
#ifdef PNG_WRITE_INTERLACING_SUPPORTED
            color_type, interlace_type, compression_type, filter_type);
#else
            color_type, PNG_INTERLACE_NONE, compression_type, filter_type);
#endif
      }
   }

#ifdef PNG_sRGB_SUPPORTED
   {
      int intent = 0;
         png_set_sRGB(write_ptr, write_info_ptr, intent);
   }
#endif

#ifdef PNG_WRITE_SUPPORTED
   

/* If we wanted, we could write info in two steps:
 * png_write_info_before_PLTE(write_ptr, write_info_ptr);
 */
   png_write_info(write_ptr, write_info_ptr);

#ifdef PNG_UNKNOWN_CHUNKS_SUPPORTED
   if (user_chunk_data[0] != 0)
   {
      png_byte png_sTER[5] = {115,  84,  69,  82, '\0'};

      unsigned char
        ster_chunk_data[1];

      ster_chunk_data[0]=(unsigned char)(user_chunk_data[0] - 1);
      png_write_chunk(write_ptr, png_sTER, ster_chunk_data, 1);
   }
   if (user_chunk_data[1] != 0 || user_chunk_data[2] != 0)
   {
      png_byte png_vpAg[5] = {118, 112,  65, 103, '\0'};

      unsigned char
        vpag_chunk_data[9];

      png_save_uint_32(vpag_chunk_data, user_chunk_data[1]);
      png_save_uint_32(vpag_chunk_data + 4, user_chunk_data[2]);
      vpag_chunk_data[8] = (unsigned char)(user_chunk_data[3] & 0xff);
      png_write_chunk(write_ptr, png_vpAg, vpag_chunk_data, 9);
   }

#endif
#endif

   row_buf = (png_bytep)png_malloc(write_ptr, nStride);    

#if defined(PNG_READ_INTERLACING_SUPPORTED) || \
  defined(PNG_WRITE_INTERLACING_SUPPORTED)

#  ifdef PNG_WRITE_SUPPORTED
   png_set_interlace_handling(write_ptr);
#  endif
#else
   num_pass = 1;
#endif

   num_pass = 1;
   
   for (pass = 0; pass < num_pass; pass++)
   {      
      for (y = 0; y < height; y++)
      {
		 pubImage -= nStride;
		 memcpy(row_buf, pubImage, nStride);
		 png_write_rows(write_ptr, (png_bytepp)&row_buf, 1);
      }
   }

#ifdef PNG_WRITE_SUPPORTED
   png_write_end(write_ptr, write_end_info_ptr);
#endif
   png_free(write_ptr, row_buf);

   png_write_destroy(write_ptr);

   FileClose();

   return (0);
}

/* Input and output filenames */
#ifdef RISCOS
static PNG_CONST char *inname = "pngtest/png";
static PNG_CONST char *outname = "pngout/png";
#else
static PNG_CONST char *inname = "pngtest.png";
static PNG_CONST char *outname = "pngout.png";
#endif


/* Generate a compiler error if there is an old png.h in the search path. */
typedef version_1_2_51 your_png_h_is_not_version_1_2_51;
