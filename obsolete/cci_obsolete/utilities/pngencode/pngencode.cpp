#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <io.h> 
#include "png.h"


/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2005 David Marteau
 *
 * This file is part of the DAIM Image Processing Library (DAIM library)
 *
 * The DAIM library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License (LGPL)
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * The DAIM library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the DAIM library; see the file LICENSE.  If
 * not, write to the Free Software Foundation, Inc., 59 Temple Place -
 * Suite 330, Boston, MA 02111-1307, USA.
 *
 *  ::: END LICENSE BLOCK::: */


#define __XXXBreakPoint__ _asm { int 3h }

// Must be defined for pre 1.2.7 release
#if (PNG_LIBPNG_VER<10207)
 #define PNG_UINT_32_MAX (~static_cast<png_uint_32>(0))
 #define png_sizeof(x)  sizeof(x)
#endif

int gError = 0;

#define ROW_WIDTH 1024

//--------------------------------------------------------------------
static void PNGLoader_Error_fn(png_structp png_ptr, png_const_charp error_msg)
{
  if(error_msg) 
    fprintf(stderr,"ERROR: %s\n",error_msg);

  gError = 1;
}
//--------------------------------------------------------------------
static void PNGLoader_Warning_fn(png_structp png_ptr,png_const_charp warning_msg)
{
  if(warning_msg) 
    fprintf(stderr,"WARNING: %s\n",warning_msg);
}
//--------------------------------------------------------------------

  
png_infop   info_ptr;
png_structp png_ptr;

//--------------------------------------------------------------------

int main( int argc, char ** argv )
{
  //__XXXBreakPoint__
  
  static char* gFileNameKey = "oh le beau bebe";
  static char* gFileSizeKey = "oh le gros bebe";
  
  FILE* fp;
  FILE* outf;

  char* file_name = NULL;
  long  base_size = 0;

  png_uint_32  width;
  png_uint_32  height;

  int bit_depth   = 8;
  int color_type  = PNG_COLOR_TYPE_GRAY;
  
  if(argc > 1) 
    file_name = argv[1];
     
  /* Open the prospective PNG file. */
  if((fp = fopen(file_name, "rb")) == NULL) {
     fprintf(stderr,"Cannot open file: %s",file_name);
     return 10;
  }

  base_size = filelength(fileno(fp));
  
  fprintf(stderr,"Encoding %s (size:%d)\n",file_name,base_size);

  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                   NULL, 
                                   PNGLoader_Error_fn, 
                                   PNGLoader_Warning_fn);

  if(gError || png_ptr == NULL)
    return 10;

  info_ptr = png_create_info_struct(png_ptr);
  
  if(info_ptr == NULL)
     return 10;

  char png_name[MAX_PATH];

  sprintf(png_name,"%s" ".png",file_name);

  /* Open the prospective PNG file. */
  if((outf = fopen(png_name, "wb")) == NULL) {
     fprintf(stderr,"Cannot open output file\n");
     return 10;
  }

  width  = ROW_WIDTH;
  height = (base_size / ROW_WIDTH) + 1;

  /* Set up the input control if you are using standard C streams */
  png_init_io(png_ptr,outf);
  
  png_set_IHDR(png_ptr, 
               info_ptr, 
               width,
               height,
               bit_depth, 
               color_type,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE, 
               PNG_FILTER_TYPE_BASE
  );

  /*  Create text data */

  #define NUM_TEXT 2
  png_textp text_ptr = new png_text[NUM_TEXT];

  text_ptr[0].key         = gFileNameKey;
  text_ptr[0].text        = file_name;
  text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;

  char buff[256];
  sprintf(buff,"%d",base_size);

  text_ptr[1].key         = gFileSizeKey;
  text_ptr[1].text        = buff;
  text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
 
  png_set_text(png_ptr,info_ptr,text_ptr,NUM_TEXT);


  /* Write the file header information.  REQUIRED */

  png_write_info(png_ptr,info_ptr);

  // Check that stride is large enough

  size_t rowbytes = png_get_rowbytes(png_ptr,info_ptr);
  size_t rowsize  = ROW_WIDTH*sizeof(png_byte);
 
  if( height > PNG_UINT_32_MAX/png_sizeof(png_bytep) ) {
     fprintf(stderr,"Image is too tall to process in memory \n");
     return 12;
  }

  png_bytep  row_pointer = new png_byte[rowsize];
  
  size_t byteread;
  for (int y = 0; y < height; ++y) 
  {
    memset(row_pointer,0,rowsize);
    byteread   = fread(row_pointer,1,rowsize,fp);
    base_size -= byteread;
    if(byteread < 0) {
      fprintf(stderr,"ERROR: Inconsistent data\n");
      return 12;      
    }
    
    png_write_rows(png_ptr,&row_pointer,1);
  }

  /* It is REQUIRED to call this to finish writing the rest of the file */
  png_write_end(png_ptr,info_ptr);

  delete [] text_ptr;
  delete [] row_pointer;
 
  return 0;
}
