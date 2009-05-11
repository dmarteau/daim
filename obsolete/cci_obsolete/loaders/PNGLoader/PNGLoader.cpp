

#define PNG_BYTES_TO_CHECK 4

// Must be defined for pre 1.2.7 release
#if (PNG_LIBPNG_VER<10207)
 #define PNG_UINT_32_MAX (~static_cast<png_uint_32>(0))
 #define png_sizeof(x)  sizeof(x)
#endif

//--------------------------------------------------------------------
static void  PNGLoader_Read_fn( png_structp png_ptr, png_bytep buffer, png_size_t length ) 
{
  _Instance_Ptr _This = (_Instance_Ptr)png_ptr->io_ptr;
  if(_This->InputStream.Read( buffer, length ) != length) {
     png_error(png_ptr, "Buffer read Error!");
  }
}
//--------------------------------------------------------------------
static void  PNGLoader_Write_fn( png_structp png_ptr, png_bytep buffer, png_size_t length ) 
{
  _Instance_Ptr _This = (_Instance_Ptr)png_ptr->io_ptr;
  if(_This->OutputStream.Write( buffer, length ) != length) {
     png_error(png_ptr, "Buffer write Error!");
  }
}
//--------------------------------------------------------------------
static void  PNGLoader_Output_Flush_fn( png_structp png_ptr ) 
{
  _Instance_Ptr _This = (_Instance_Ptr)png_ptr->io_ptr;
  if(!_This->OutputStream.Flush()) {
     png_error(png_ptr, "Buffer flush Error!");
  }
}
//--------------------------------------------------------------------
static void PNGLoader_Error_fn(png_structp png_ptr, png_const_charp error_msg)
{
  if(png_ptr) {
    _Instance_Ptr _This = reinterpret_cast<_Instance_Ptr>(png_get_error_ptr(png_ptr));
    _This->Status   = CCI_ERR_IOERROR;
  }

  dmTHROW_ERROR(error_msg); // This should not return
}
//--------------------------------------------------------------------
static void PNGLoader_Warning_fn(png_structp png_ptr,png_const_charp warning_msg)
{
  if(warning_msg) 
    dmLOG("WARNING:PNG: %s\n",warning_msg);
}
//--------------------------------------------------------------------
// Reading
//--------------------------------------------------------------------
static CCI_METHOD PNGLoader_CheckIfPNG(_Instance_Ptr _This )
{
   unsigned char buf[PNG_BYTES_TO_CHECK];

   /* Read in some of the signature bytes */
   if(_This->InputStream.Read(buf,PNG_BYTES_TO_CHECK) != PNG_BYTES_TO_CHECK)
      return (_This->Status = CCI_ERR_INVALID_OBJECT);

   /* Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
      Return nonzero (true) if they match */

   if(!png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK)) 
      CCI_RETURN_OK()

   return (_This->Status = CCI_ERR_GENERIC);
}
//--------------------------------------------------------------------
static void PNGLoader_Clear( _Instance_Ptr _This )
{
  if(_This->png_ptr) {
    png_infopp info_pptr = ( _This->info_ptr ? &_This->info_ptr : png_infopp_NULL);

    if(_This->Mode==CCI_ME_GET)
      png_destroy_read_struct(&_This->png_ptr,info_pptr,png_infopp_NULL);
    else
      png_destroy_write_struct(&_This->png_ptr,info_pptr);

    _This->png_ptr  = NULL;
    _This->info_ptr = NULL;
  } 

  _This->Status = CCI_ERR_OK;
  _This->Mode   = 0;
}
//--------------------------------------------------------------------
static CCI_METHOD PNGLoader_InitReadStruct( _Instance_Ptr _This, unsigned int sig_read )
{
   /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also supply the
    * the compiler header file version, so that we know if the application
    * was compiled with a compatible version of the library.  REQUIRED
    */
    _This->Mode = CCI_ME_GET;
    _This->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                     _This, 
                                     PNGLoader_Error_fn, 
                                     PNGLoader_Warning_fn
                                   );

   if(_This->png_ptr == NULL)
     return (_This->Status = CCI_ERR_GENERIC);

   /* Allocate/initialize the memory for image information.  REQUIRED. */
   _This->info_ptr = png_create_info_struct(_This->png_ptr);

   if (_This->info_ptr == NULL) 
     return (_This->Status = CCI_ERR_GENERIC);

   /* Set up the input control if you are using standard C streams */
   //png_init_io(_This->png_ptr, _This->fp);
   
   png_set_read_fn(_This->png_ptr, (void *)_This, PNGLoader_Read_fn);

   /* If we have already read some of the signature */
   png_set_sig_bytes(_This->png_ptr, sig_read);

   CCI_RETURN_OK()
}
//--------------------------------------------------------------------
// Read PNG header
//--------------------------------------------------------------------
static CCI_METHOD PNGLoader_ReadHDR( _Instance_Ptr _This )
{
   dmDEBUG_ASSERT( _This->png_ptr  != NULL )
   dmDEBUG_ASSERT( _This->info_ptr != NULL )

   /* The call to png_read_info() gives us all of the information from the
    * PNG file before the first IDAT (image data chunk).  REQUIRED
    */

   png_read_info(_This->png_ptr,_This->info_ptr);
   png_get_IHDR (_This->png_ptr,_This->info_ptr,
                &_This->width, 
                &_This->height,
                &_This->bit_depth,
                &_This->color_type,
                &_This->interlace_type,
                int_p_NULL, int_p_NULL);

   if(_This->StringList!=NULL) {
     dmIStringList _Strings;
     if(_Strings.QueryInterface(_This->StringList)) 
     {
       _Strings.Clear();

       int num_text        = 0;
       png_textp text_ptr  = NULL;
       if(png_get_text(_This->png_ptr,_This->info_ptr,&text_ptr, &num_text)>0) 
         for(int i= 0;i<num_text;++i) {
           _Strings.AddTail(text_ptr[i].key);
           _Strings.AddTail(text_ptr[i].text);
         } 
     }
   }


   CCI_RETURN_OK()
}
//--------------------------------------------------------------------
// Read PNG data
//--------------------------------------------------------------------
static CCI_METHOD PNGLoader_ReadData( _Instance_Ptr _This, void* _Scan0, int _Stride )
{
   dmDEBUG_ASSERT( _This->png_ptr  != NULL )
   dmDEBUG_ASSERT( _This->info_ptr != NULL )

   int iBitDepth;
   int iColorType;
   
   png_uint_32 ulWidth,ulHeight;
   png_uint_32 ulChannels;

   double dGamma;
    
    
   png_byte bkgRed   =  _This->bckgnd_color.red;
   png_byte bkgGreen =  _This->bckgnd_color.green;
   png_byte bkgBlue  =  _This->bckgnd_color.blue;

   /* Set up the data transformations you want.  Note that these are all
    * optional.  Only call them if you want/need them.  Many of the
    * transformations only work on specific types of images, and many
    * are mutually exclusive.
    */

   /* tell libpng to strip 16 bit/color files down to 8 bits/color */
   //XXXDavid You need to comment this if you don't want to have 16 bits
   // greyscale very badely mapped to 8 bits  
   //png_set_strip_16(_This->png_ptr);

   /* Strip alpha bytes from the input data without combining with the
    * background (not recommended).
    */
   //png_set_strip_alpha(_This->png_ptr);

   /* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
    * byte into separate bytes (useful for paletted and grayscale images).
    */
   png_set_packing(_This->png_ptr);

   // Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel
   if(_This->color_type == PNG_COLOR_TYPE_GRAY && _This->bit_depth < 8)
      png_set_gray_1_2_4_to_8(_This->png_ptr);

   // XXX ALPHA HACK ! 
   // Convert Alpha grey to rgb image
   if(_This->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
      png_set_gray_to_rgb(_This->png_ptr);

   if(png_get_valid(_This->png_ptr, _This->info_ptr, PNG_INFO_tRNS))
      png_set_expand(_This->png_ptr);

   png_color_16  *pBackground;
    
   // set the background color to draw transparent and alpha images over.
   if (png_get_bKGD(_This->png_ptr, _This->info_ptr, &pBackground))
   {
      png_set_background(_This->png_ptr, pBackground, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
      bkgRed   = (png_byte) pBackground->red;
      bkgGreen = (png_byte) pBackground->green;
      bkgBlue  = (png_byte) pBackground->blue;
   }
        
   // if required set gamma conversion
   if (png_get_gAMA(_This->png_ptr, _This->info_ptr, &dGamma))
       png_set_gamma(_This->png_ptr, (double) 2.2, dGamma);
        
   // swap bytes of 16 bit files to least significant byte first
   if(_This->decodeSwap16b)
      png_set_swap(_This->png_ptr);

#ifndef __RGB_DATA__
 
    // flip the RGB pixels to BGR (or RGBA to BGRA)
    if (_This->color_type & PNG_COLOR_MASK_COLOR)
       png_set_bgr(_This->png_ptr);

    // swap the RGBA or GA data to ARGB or AG (or BGRA to ABGR)
    // png_set_swap_alpha(_This->png_ptr);
 
#endif

   // after the transformations have been registered update info_ptr data
   png_read_update_info(_This->png_ptr, _This->info_ptr);
        
   // get again width, height and the new bit-depth and color-type    
   png_get_IHDR(_This->png_ptr, _This->info_ptr, &ulWidth, &ulHeight, &iBitDepth,
                &iColorType, NULL, NULL, NULL);

   // Check consistency
   if(_This->bit_depth != iBitDepth ||
      _This->height    != ulHeight   || 
      _This->width     != ulWidth ) 
   {
     dmLOG("ERROR:PNG: Inconsistent data ! \n");
     return (_This->Status = CCI_ERR_INVALID_PARAM);         
   }

   size_t rowbytes = png_get_rowbytes(_This->png_ptr,_This->info_ptr);
   size_t ustride  = dm_abs(_Stride);

   // Handle RGB ALPHA images
   if(iColorType == PNG_COLOR_TYPE_RGB_ALPHA) 
   {
     ulChannels = png_get_channels(_This->png_ptr, _This->info_ptr);
     if(ulChannels != 4) 
     {
       dmLOG("ERROR:PNG: Inconsistent data in rgb alpha image ! \n");
       return (_This->Status = CCI_ERR_INVALID_PARAM);         
     }
     
     png_bytep rowp  = new png_byte[rowbytes];

     png_uint_32 i,k,image_height,width = _This->width;
     int pass, j;

     #ifdef PNG_READ_INTERLACING_SUPPORTED
        pass = png_set_interlace_handling(_This->png_ptr);
     #else
        if (_This->png_ptr->interlaced)
            png_error(_This->png_ptr,"Cannot read interlaced image -- interlace handler disabled.");
            pass = 1; 
     #endif

     image_height=_This->png_ptr->height;
     _This->png_ptr->num_rows = image_height; // Make sure this is set correctly //

     png_bytep row_dest_p = static_cast<png_bytep>(_Scan0);

     for(j=0;j<pass;j++)
     {
        for (i = 0; i < image_height; i++)
        {
           png_read_row(_This->png_ptr,rowp,png_bytep_NULL);
           
           png_bytep bsrc = rowp;
           png_bytep bdst = row_dest_p; 
           
           float alpha,nalph;
            
           // Copy back rgb data to buffer
           for(k=0;k < width; ++k,bsrc+=4,bdst+=3) 
           {
             alpha = (float)bsrc[3] / 255.0f;
             nalph = 1.0f - alpha;

             #ifndef __RGB_DATA__

               bdst[2] = static_cast<png_byte>(bsrc[2] * alpha + nalph * bkgRed  );
               bdst[1] = static_cast<png_byte>(bsrc[1] * alpha + nalph * bkgGreen);
               bdst[0] = static_cast<png_byte>(bsrc[0] * alpha + nalph * bkgBlue );

             #else // __RGB_DATA__

               bdst[0] = static_cast<png_byte>(bsrc[0] * alpha + nalph * bkgRed  );
               bdst[1] = static_cast<png_byte>(bsrc[1] * alpha + nalph * bkgGreen);
               bdst[2] = static_cast<png_byte>(bsrc[2] * alpha + nalph * bkgBlue );

             #endif

           }
           row_dest_p += _Stride;
        }
     } 
     
     delete [] rowp;         
   } 
   else
   {
     // Check that stride is large enough
     if( rowbytes > ustride ) {
         dmLOG("ERROR:PNG: Invalid stride in image data ! \n");
        return (_This->Status = CCI_ERR_INVALID_PARAM);
     }

     size_t height = _This->height;
     png_bytep* row_pointers = new png_bytep[height];

     png_bytep row_p = static_cast<png_bytep>(_Scan0);

     // Assign rows to the imagedata rows
     for(size_t row = 0; row < height; ++row,row_p += _Stride)
        row_pointers[row] = row_p;
 
     // Read the entire image in one go
     png_read_image(_This->png_ptr, row_pointers);
 
     delete [] row_pointers;

   }

   // read rest of file, and get additional chunks in info_ptr - REQUIRED */
   png_read_end(_This->png_ptr, _This->info_ptr);
   
   // At this point you have read the entire image
   CCI_RETURN_OK()  
}
//--------------------------------------------------------------------
static CCI_METHOD PNGLoader_GetFormat( _Instance_Ptr _This )
{
   switch(_This->color_type)
    {
      case PNG_COLOR_TYPE_GRAY      :
      case PNG_COLOR_TYPE_PALETTE   :
        if(_This->bit_depth <=8) { 
          _This->PixelFormat = dmPixelFormat8bppIndexed;
          CCI_RETURN_OK()
        }
        if(_This->bit_depth == 16) { 
           _This->PixelFormat = dmPixelFormat16bppGrayScale;
           CCI_RETURN_OK()
        }
        break;

      case PNG_COLOR_TYPE_RGB:
      case PNG_COLOR_TYPE_RGB_ALPHA:
      case PNG_COLOR_TYPE_GRAY_ALPHA:
        _This->PixelFormat = dmPixelFormat24bppRGB;
        CCI_RETURN_OK()

      default :
        _This->PixelFormat = dmPixelFormatUndefined;
        _This->Status      = CCI_ERR_GENERIC;
        dmLOG("ERROR:PNG: Invalid pixel format !\n");
        break;
   } 

   CCI_RETURN_FAIL()   
}
//--------------------------------------------------------------------
static CCI_METHOD PNGLoader_SetFormat( _Instance_Ptr _This )
{
   switch(_This->PixelFormat)
   {
     case dmPixelFormat8bppIndexed    : 
     case dmPixelFormat16bppGrayScale :
         _This->color_type = PNG_COLOR_TYPE_GRAY;
         _This->bit_depth  = dmGetPixelFormatBits(_This->PixelFormat);
         break;

     case dmPixelFormat24bppRGB :
         _This->color_type  = PNG_COLOR_TYPE_RGB;
         _This->bit_depth   = 8; 
         break;
 
     default:
        _This->Status     = CCI_ERR_NOT_IMPLEMENTED;
        return _This->Status;    
   }

   CCI_RETURN_OK()
}
//--------------------------------------------------------------------
// Writing
//--------------------------------------------------------------------
#define PNG_FILTER_DEFAULT -1

static void PNGLoader_LoadProperties( _Instance_Ptr _This, dm_uint ioflags, dmIProperties& _IProps  )
{
   if(_IProps.IsValid()) 
   {
     if((ioflags & CCIV_Loader_Write)!=0)
     {
       const char* _Filter   = _IProps.GetData(NULL,_TXT("encode.compression"),_TXT("default"));
       if(EQUAL_NOCASE_STRING(_Filter,_TXT("none") )) _This->encodeFilter = PNG_FILTER_NONE;  else
       if(EQUAL_NOCASE_STRING(_Filter,_TXT("sub")  )) _This->encodeFilter = PNG_FILTER_SUB;   else
       if(EQUAL_NOCASE_STRING(_Filter,_TXT("up")   )) _This->encodeFilter = PNG_FILTER_UP;    else
       if(EQUAL_NOCASE_STRING(_Filter,_TXT("avg")  )) _This->encodeFilter = PNG_FILTER_AVG;   else
       if(EQUAL_NOCASE_STRING(_Filter,_TXT("paeth"))) _This->encodeFilter = PNG_FILTER_PAETH; else
       if(EQUAL_NOCASE_STRING(_Filter,_TXT("all")  )) _This->encodeFilter = PNG_ALL_FILTERS;  else
       _This->encodeFilter = static_cast<dm_uint>(PNG_FILTER_DEFAULT);

       const char* _encodeSwap = _IProps.GetData(NULL,_TXT("encode.swap16bits"),_TXT("no"));
       if(EQUAL_NOCASE_STRING(_encodeSwap,"y")||EQUAL_NOCASE_STRING(_encodeSwap,"yes"))
         _This->encodeSwap16b = dm_true;
       else      
        _This->encodeSwap16b = dm_false;
     }

     if((ioflags & CCIV_Loader_Read)!=0)
     {
       const char* _decodeSwap = _IProps.GetData(NULL,_TXT("decode.swap16bits"),_TXT("no"));
       if(EQUAL_NOCASE_STRING(_decodeSwap,"y")||EQUAL_NOCASE_STRING(_decodeSwap,"yes"))
         _This->decodeSwap16b = dm_true;
       else      
         _This->decodeSwap16b = dm_false;

       const char* _bckgndColor = _IProps.GetData(NULL,_TXT("decode.background"),_TXT("white"));
       if(EQUAL_NOCASE_STRING(_bckgndColor,"white")) {
          _This->bckgnd_color.red   = 255;
          _This->bckgnd_color.green = 255;
          _This->bckgnd_color.blue  = 255;
       } else {
          _This->bckgnd_color.red   = 0;
          _This->bckgnd_color.green = 0;
          _This->bckgnd_color.blue  = 0;       
       }

       const char* _expandPalette = _IProps.GetData(NULL,_TXT("decode.expandPaletteToRGB"),_TXT("no"));
       if(EQUAL_NOCASE_STRING(_expandPalette,"y")||EQUAL_NOCASE_STRING(_decodeSwap,"yes"))
         _This->expandPalette = dm_true;
       else      
         _This->expandPalette = dm_false;
     }
  }
}
//--------------------------------------------------------------------
png_textp PNGLoader_CreateText( _Instance_Ptr _This,
                                int& num_text )
{
  if(_This->StringList!=NULL)
  {

    dmIStringList _Strings;
    if(_Strings.QueryInterface(_This->StringList))
    {
      num_text = _Strings.Count() >> 1;
      if(num_text > 0) 
      {
        png_textp text_ptr = new png_text[num_text];

        num_text  = 0;
        dm_param pos = dm_null;
        while( (pos=_Strings.GetNext(pos)) )
        {
          const char_t* _key = _Strings.GetString(pos);

          pos = _Strings.GetNext(pos);
          if(pos) 
          {
            const char_t* _txt = _Strings.GetString(pos);

            text_ptr[num_text].key         = const_cast<char*>(_key);
            text_ptr[num_text].text        = const_cast<char*>(_txt);
            text_ptr[num_text].compression = PNG_TEXT_COMPRESSION_NONE;

            #ifdef PNG_iTXt_SUPPORTED
             text_ptr[num_text].lang = NULL;
            #endif
            ++num_text;
          } 
          else
            break;
        } 
        return text_ptr;
      }
    }
  }
  return NULL;
}
//--------------------------------------------------------------------
static CCI_METHOD PNGLoader_InitWriteStruct( _Instance_Ptr _This )
{
//dmDEBUG_ASSERT( _This->fp == NULL );
  dmDEBUG_ASSERT( _This->width  > 0 );
  dmDEBUG_ASSERT( _This->height > 0 );

  _This->Mode = CCI_ME_SET;
  _This->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                   _This, 
                                   PNGLoader_Error_fn, 
                                   PNGLoader_Warning_fn);

  if(_This->png_ptr == NULL)
    return (_This->Status = CCI_ERR_GENERIC);

  _This->info_ptr = png_create_info_struct(_This->png_ptr);

  if(_This->info_ptr == NULL)
    return (_This->Status = CCI_ERR_GENERIC);

  /* Set up the input control if you are using standard C streams */
  //png_init_io(_This->png_ptr, _This->fp);

  png_set_write_fn(_This->png_ptr, (void *)_This,
                   PNGLoader_Write_fn,PNGLoader_Output_Flush_fn);
  
  if(_This->Flags & CCIV_Loader_NOCOMPRESSION) 
    png_set_filter(_This->png_ptr,0,PNG_FILTER_NONE);
  else
  if(_This->encodeFilter != static_cast<dm_uint>(PNG_FILTER_DEFAULT)) {
    png_set_filter(_This->png_ptr,0,_This->encodeFilter);
  }

  png_set_IHDR(_This->png_ptr, 
               _This->info_ptr, 
               _This->width,
               _This->height,
               _This->bit_depth, 
               _This->color_type,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, 
        PNG_FILTER_TYPE_BASE
  );

  CCI_RETURN_OK()
}
//--------------------------------------------------------------------
// Read PNG data
//--------------------------------------------------------------------
static CCI_METHOD PNGLoader_WriteData( _Instance_Ptr _This, 
                                       void* _Scan0, int _Stride,
                                       png_textp text_ptr, int num_text )
{
   dmDEBUG_ASSERT( _This->png_ptr  != NULL )
   dmDEBUG_ASSERT( _This->info_ptr != NULL )

   // Write text data if any
   if(text_ptr!=NULL)
     png_set_text(_This->png_ptr,_This->info_ptr, text_ptr, num_text);

   /* Write the file header information.  REQUIRED */
   /* We write the header at this place because text can be added */
   /* between PNGLoader_InitWriteStruct && PNGLoader_WriteData    */

   png_write_info(_This->png_ptr,_This->info_ptr);

#ifndef __RGB_DATA__
      png_set_bgr(_This->png_ptr);
#endif

   /* swap bytes of 16 bit files to least significant byte first */
   if(_This->encodeSwap16b)
     png_set_swap(_This->png_ptr);

   // Check that stride is large enough

   size_t rowbytes = png_get_rowbytes(_This->png_ptr,_This->info_ptr);
   size_t ustride  = dm_abs(_Stride);
   if( rowbytes > ustride ) {
     dmLOG(_TXT("ERROR:PNG: Invalid stride in image data ! \n"));
     return (_This->Status = CCI_ERR_GENERIC);
   }

   size_t height = _This->height;

   if( height > PNG_UINT_32_MAX/png_sizeof(png_bytep) ) {
     dmLOG(_TXT("Image is too tall to process in memory"));
     return (_This->Status = CCI_ERR_GENERIC);
   }

   png_bytep* row_pointers = new png_bytep[height];
   png_bytep  row_p = static_cast<png_bytep>(_Scan0);

   // Assign rows to the imagedata rows
   for(size_t row = 0; row < height; ++row,row_p += _Stride)
      row_pointers[row] = row_p;
 
   png_write_image(_This->png_ptr,row_pointers);

   if(CCI_SUCCEEDED(_This->Status))
     /* It is REQUIRED to call this to finish writing the rest of the file */
     png_write_end(_This->png_ptr,_This->info_ptr);

   delete [] row_pointers;

   CCI_RETURN_OK()  
}
//--------------------------------------------------------------------
