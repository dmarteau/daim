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

#define PNG_BYTES_TO_CHECK 4

// Must be defined for pre 1.2.7 release
#if (PNG_LIBPNG_VER<10207)
 #define PNG_UINT_32_MAX (~static_cast<png_uint_32>(0))
 #define png_sizeof(x)  sizeof(x)
#endif

int gError = 0;

//--------------------------------------------------------------------
static void PNGLoader_Error_fn(png_structp png_ptr, png_const_charp error_msg)
{
  if(error_msg) 
    fprintf(stderr,"%s",error_msg);

  gError = 1;
}
//--------------------------------------------------------------------
static void PNGLoader_Warning_fn(png_structp png_ptr,png_const_charp warning_msg)
{
  if(warning_msg) 
    fprintf(stderr,"%s",warning_msg);
}
//--------------------------------------------------------------------

  
png_infop   info_ptr;
png_structp png_ptr;

int check_if_png(FILE *fp)
{
   unsigned  char buf[PNG_BYTES_TO_CHECK];

   /* Read in some of the signature bytes */
   if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK)
      return 0;

   /* Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
      Return nonzero (true) if they match */

   return(!png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK));
}
//--------------------------------------------------------------------

int main( int argc, char ** argv )
{
  //__XXXBreakPoint__
  
  static char* gFileNameKey = "oh le beau bebe";
  static char* gFileSizeKey = "oh le gros bebe";
  
  FILE* fp;

  png_uint_32  width;
  png_uint_32  height;

  int bit_depth;
  int color_type;
  int interlace_type;

  char* file_name = NULL;
  char* base_name = NULL;
  int   base_size = 0;
  
  if(argc > 1) 
    file_name = argv[1];
     
  /* Open the prospective PNG file. */
  if((fp = fopen(file_name, "rb")) == NULL) {
     fprintf(stderr,"Cannot open file: %s",file_name);
     return 10;
  }

  /*  Check if that file is a png file */
  if(check_if_png(fp) == 0) {
     fprintf(stderr,"Not a PNG file !\n");
     return 10;    
  }

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                   NULL, 
                                   PNGLoader_Error_fn, 
                                   PNGLoader_Warning_fn);
                                   
  if(gError || png_ptr == NULL)
     return 10;

   /* Allocate/initialize the memory for image information.  REQUIRED. */
  info_ptr = png_create_info_struct(png_ptr);

   if(info_ptr == NULL) 
      return 10;

   /* Set up the input control if you are using standard C streams */
   png_init_io(png_ptr,fp);

   /* If we have already read some of the signature */
   png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);
  
   /* The call to png_read_info() gives us all of the information from the
    * PNG file before the first IDAT (image data chunk).  REQUIRED
    */
   png_read_info(png_ptr,info_ptr);
   png_get_IHDR (png_ptr,info_ptr,
                &width, 
                &height,
                &bit_depth,
                &color_type,
                &interlace_type,
                int_p_NULL, int_p_NULL);

   if(gError)
     return 10;

   // Accept only 8 bits grey scale
   
   if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth  == 8)
   {
     // Look for basename in text
     int num_text        = 0;
     png_textp text_ptr  = NULL;
     if(png_get_text(png_ptr,info_ptr,&text_ptr, &num_text)>0) 
     for(int i= 0;i<num_text;++i) 
     {
       if(strcmp(text_ptr[i].key,gFileNameKey)==0 && text_ptr[i].text) {
         base_name = strdup(text_ptr[i].text);
         continue;
       }
         
       if(strcmp(text_ptr[i].key,gFileSizeKey)==0 && text_ptr[i].text) {
         base_size = atol(text_ptr[i].text);
         continue;
       }
       
     }
   } else
   
   if(!base_name || base_size == 0) {
     fprintf(stderr,"Not a valid file !\n");
     return 12;
   }

   fprintf(stderr,"Decoding to file: %s (size: %d)\n",base_name,base_size);

   int iBitDepth;
   int iColorType;
   
   png_uint_32 ulWidth,ulHeight;
 
/*
   png_set_packing(png_ptr);

   if(png_get_valid(png_ptr,info_ptr,PNG_INFO_tRNS))
      png_set_expand(png_ptr);
*/
   // after the transformations have been registered update info_ptr data
   png_read_update_info(png_ptr,info_ptr);
        
   // get again width, height and the new bit-depth and color-type    
   png_get_IHDR(png_ptr,info_ptr,&ulWidth,&ulHeight,&iBitDepth,
                &iColorType, NULL, NULL, NULL);

   if(gError)
     return 10;

   // Check consistency
   if(bit_depth != iBitDepth ||
      height    != ulHeight   || 
      width     != ulWidth ) 
   {
     fprintf(stderr,"ERROR Inconsistent data !\n");
     return 12;         
   }

   /* Open output file */
   FILE* outf;
   if((outf = fopen(base_name, "wb")) == NULL) {
      fprintf(stderr,"Cannot open file: %s",base_name);
      return 10;
   }

   size_t rowbytes = png_get_rowbytes(png_ptr,info_ptr);
   
   png_bytep row_pointer = new png_byte[rowbytes];

   int rowsize = width * sizeof(png_byte);
 
   for (png_uint_32 y = 0; y < height; ++y) 
   {
     png_read_rows(png_ptr,&row_pointer,png_bytepp_NULL,1);
     if(gError==0) 
     {
        if(base_size < rowsize) 
        {
          if(base_size < 0) {
            fprintf(stderr,"ERROR Inconsistent data !\n");
            return 13;                
          }
          if(base_size > 0)
             fwrite(row_pointer,1,base_size,outf);
        }
        else   
         fwrite(row_pointer,1,rowsize,outf);
         
       base_size -= rowsize; 
     }
   }

   fclose(outf);

   delete [] row_pointer;

   // read rest of file, and get additional chunks in info_ptr - REQUIRED */
   png_read_end(png_ptr, info_ptr);

   // At this point we have read the entire image

   png_destroy_read_struct(&png_ptr,&info_ptr,png_infopp_NULL);
    
   if(base_name)
      free( base_name );
    
   return 0;   
}

