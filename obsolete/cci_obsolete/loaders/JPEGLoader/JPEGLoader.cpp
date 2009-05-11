
//#define HAVE_BOOLEAN
//typedef unsigned char boolean;

//--------------------------------------------------------------------
static void JPEGLoader_Error_fn(j_common_ptr cinfo)
{
  jpeg_err_struct* err_ptr = reinterpret_cast<jpeg_err_struct*>(cinfo->err);

  _Instance_Ptr _This = err_ptr->instance;
  _This->Status = CCI_ERR_IOERROR;   // set error status

  (*err_ptr->jpeg_mngr.output_message)(cinfo);       // output message
}
//--------------------------------------------------------------------
static void JPEGLoader_Output_fn(j_common_ptr cinfo)
{
  char buffer[JMSG_LENGTH_MAX];

  /* Create the message */
  (*cinfo->err->format_message) (cinfo, buffer);

  jpeg_err_struct* err_ptr = reinterpret_cast<jpeg_err_struct*>(cinfo->err);

  _Instance_Ptr _This = err_ptr->instance;
  if(!CCI_SUCCEEDED(_This->Status)) {
    dmLOG(_TXT("ERROR:JPEG: %s\n"),buffer);
    dmTHROW_ERROR(buffer);                // should not return
  } else
    dmLOG(_TXT("WARNING:JPEG: %s\n"),buffer); 
}
//--------------------------------------------------------------------
// Swap RGB to BGR and vice-versa
//--------------------------------------------------------------------
static void JPEGLoader_BGR( _Instance_Ptr _This, void* _Scan0, int _Stride )
{ 
   /* flip the RGB pixels to BGR */
   if(_This->PixelFormat == dmPixelFormat24bppRGB) 
   {
     size_t  height = DJPEG_PTR(_This)->image_height;
     size_t  width  = DJPEG_PTR(_This)->image_width;

     dm_byte  byte;
     dm_byte *rbuf,*bbuf,*buf = static_cast<dm_byte*>(_Scan0);

     for (int row = height; --row >=0; buf += _Stride) {
        rbuf = buf;
        bbuf = buf+2;
        for(int x = width; --x >=0; rbuf+=3,bbuf+=3 ) {
          byte  = *rbuf;
          *rbuf = *bbuf;
          *bbuf = byte;  
        } 
     }
   }
}
//--------------------------------------------------------------------
void JPEGLoader_Clear( _Instance_Ptr _This  )
{
  if(_This->Mode==CCI_ME_GET)
     jpeg_destroy_decompress(DJPEG_PTR(_This));

  else  

  if(_This->Mode==CCI_ME_SET)
     jpeg_destroy_compress(CJPEG_PTR(_This));

  _This->Status = CCI_ERR_OK;
  _This->Mode   = 0;
}
//--------------------------------------------------------------------
CCI_METHOD JPEGLoader_InitReadStruct( _Instance_Ptr _This )
{
   _This->Mode  = CCI_ME_GET;

   // Step 1: allocate and initialize JPEG decompression object

   // We set up the normal JPEG error routines, then override error_exit.
   DJPEG_PTR(_This)->err = jpeg_std_error(&_This->jpeg_err.jpeg_mngr);

   _This->jpeg_err.jpeg_mngr.error_exit     = JPEGLoader_Error_fn;
   _This->jpeg_err.jpeg_mngr.output_message = JPEGLoader_Output_fn;

   // Now we can initialize the JPEG decompression object.
   jpeg_create_decompress(DJPEG_PTR(_This));

   // Step 2: specify data source
   jpeg_input_src(DJPEG_PTR(_This),&_This->InputStream);
     
   CCI_RETURN_OK()
}
//--------------------------------------------------------------------
// Read JPEG header
//--------------------------------------------------------------------
CCI_METHOD JPEGLoader_ReadHDR( _Instance_Ptr _This )
{
  // Step 3: read file parameters with jpeg_read_header()

  jpeg_read_header(DJPEG_PTR(_This), 1);

   CCI_RETURN_OK()
}
//--------------------------------------------------------------------
// Create correct DAIM Pixel format
//--------------------------------------------------------------------
CCI_METHOD JPEGLoader_GetFormat( _Instance_Ptr _This )
{
  switch(DJPEG_PTR(_This)->jpeg_color_space)
  {
    case JCS_GRAYSCALE :
       _This->PixelFormat =  dmPixelFormat8bppIndexed;
       CCI_RETURN_OK()

      case JCS_YCbCr:
      case JCS_YCCK :
      case JCS_CMYK :
        // force output to RGB format
        DJPEG_PTR(_This)->out_color_space = JCS_RGB;

      case JCS_RGB  :
         _This->PixelFormat =  dmPixelFormat24bppRGB;
         CCI_RETURN_OK()

     default :
        dmLOG(_TXT("ERROR:JPEG Unsupported pixel format !\n"));
        _This->Status = CCI_ERR_NOT_IMPLEMENTED;

   } 

   CCI_RETURN_FAIL()
}
//--------------------------------------------------------------------
// Read JPEG data
//--------------------------------------------------------------------
int JPEGLoader_ReadData( _Instance_Ptr _This , void* _Scan0, int _Stride )
{
  jpeg_decompress_struct* jpg_ptr = DJPEG_PTR(_This);
  
  // Step 4: set parameters for decompression


  // Step 5: Start decompressor

  jpeg_start_decompress(jpg_ptr);

  // We may need to do some setup of our own at this point before reading
  // the data.  After jpeg_start_decompress() we have the correct scaled
  // output image dimensions available, as well as the output colormap
  // if we asked for color quantization.
  // In this example, we need to make an output work buffer of the right size.

  // We do not process rescaling, so check that scaled image is the
  // the same as jpeg size

  dmASSERT( jpg_ptr->output_width  == jpg_ptr->image_width  );
  dmASSERT( jpg_ptr->output_height == jpg_ptr->image_height );

  size_t jpeg_stride = jpg_ptr->output_width * jpg_ptr->output_components;

  // Check size
  size_t ustride  = dm_abs(_Stride);
  size_t height   = jpg_ptr->output_height;

  if( jpeg_stride > ustride ) {
    dmLOG("ERROR:JPEG: Invalid stride in image data ! \n");
    return (_This->Status = CCI_ERR_INVALID_PARAM);

  }

  unsigned char* buffer = static_cast<unsigned char*>(_Scan0);

  // Step 6: read scan lines

  for(size_t row = 0; row < height; ++row, buffer += _Stride)
     (void) jpeg_read_scanlines(jpg_ptr,&buffer,1);

  // Step 7: Finish decompression

  jpeg_finish_decompress(jpg_ptr);

#ifndef __RGB_DATA__
  JPEGLoader_BGR(_This,_Scan0,_Stride);
#endif

  CCI_RETURN_OK()  
}
//--------------------------------------------------------------------
// Writing
//--------------------------------------------------------------------
static CCI_METHOD JPEGLoader_SetFormat( _Instance_Ptr _This )
{
   switch(_This->PixelFormat)
   {
     case dmPixelFormat8bppIndexed    : 
       _This->jpegoutputformat = JCS_GRAYSCALE;
       break;

     case dmPixelFormat24bppRGB :
       _This->jpegoutputformat = JCS_RGB;
       break;

     default:
        _This->Status = CCI_ERR_NOT_IMPLEMENTED;
        return _This->Status;    
   }

   CCI_RETURN_OK()
}
//--------------------------------------------------------------------
static void JPEGLoader_LoadProperties( _Instance_Ptr _This, dm_param me )
{
    dmIInterface<dmIProperties> _IProps;
    if(_IProps.Open(_TXT("daim://loaders/jpegldr"),_TXT("image/jpeg"))) 
    {
      if(me == CCI_ME_SET) {
        const char_t* _Quality = _IProps.GetData(NULL,_TXT("encode.jpeg.quality"),_TXT("default"));
        if(EQUAL_NOCASE_STRING(_Quality,_TXT("default") )) _This->jpegquality = 75 ; else
        if(EQUAL_NOCASE_STRING(_Quality,_TXT("full")    )) _This->jpegquality = 100; else 
        {
          _This->jpegquality = atoi(_Quality);
          if(_This->jpegquality > 100)
            _This->jpegquality = 100;
        }
      } 
      else {
      }
   }
}
//--------------------------------------------------------------------
int JPEGLoader_CreateText( _Instance_Ptr _This )
{
  if(_This->StringList!=NULL)
  {
    dmIStringList _Strings;
    if(_Strings.QueryInterface(_This->StringList))
    {
      dmString unparsedtag = "<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>\n";
      int   num_text  = 0;

      dm_param pos = dm_null;
      while( (pos = _Strings.GetNext(pos)) )
      {
        const char_t* _key  = _Strings.GetString(pos);
        pos = _Strings.GetNext(pos);
        if(pos) {
          const char_t* _txt = _Strings.GetString(pos);

          // Set JPEG Comments

          // Add pseudo XML tags
          unparsedtag.Append( _TXT("<"));
          unparsedtag.Append( _key );
          unparsedtag.Append( _TXT(">\n"));
          unparsedtag.Append( _txt );
          unparsedtag.Append( _TXT("</"));
          unparsedtag.Append( _key );
          unparsedtag.Append( _TXT(">\n") );

          ++num_text;
        } 
        else
          break;
      }
 
      if(num_text>0) {
      }
 
      return num_text;
    }
  }
  return 0;
}
//--------------------------------------------------------------------
static CCI_METHOD JPEGLoader_InitWriteStruct( _Instance_Ptr _This, size_t width, size_t height )
{
 // dmDEBUG_ASSERT( !_This->File.Empty() );

   _This->Mode  = CCI_ME_SET;

  jpeg_compress_struct* jpg_ptr = CJPEG_PTR(_This);

  // Step 1: allocate and initialize JPEG compression object

  // We set up the normal JPEG error routines, then override error_exit.
  jpg_ptr->err = jpeg_std_error(&_This->jpeg_err.jpeg_mngr);

  _This->jpeg_err.jpeg_mngr.error_exit     = JPEGLoader_Error_fn;
  _This->jpeg_err.jpeg_mngr.output_message = JPEGLoader_Output_fn;

  // Now we can initialize the JPEG decompression object.
  jpeg_create_compress(jpg_ptr);

  // Step 2: specify data destination
  jpeg_output_dest(jpg_ptr,&_This->OutputStream);

  // Step 3: set parameters for compression

  // First we supply a description of the input image.
  // Four fields of the cinfo struct must be filled in:
  //
  jpg_ptr->image_width    = width; 	    // image width and height, in pixels */
  jpg_ptr->image_height   = height;

  jpg_ptr->in_color_space = _This->jpegoutputformat;

  if(jpg_ptr->in_color_space == JCS_RGB)
    jpg_ptr->input_components = 3; 
  else 
    jpg_ptr->input_components = 1; 
     
  // Now use the library's routine to set default compression parameters.
  // (You must set at least cinfo.in_color_space before calling this,
  // since the defaults depend on the source color space.)

  jpeg_set_defaults(jpg_ptr);

  // Now you can set any non-default parameters you wish to.
  // Here we just illustrate the use of quality (quantization table) scaling:

  jpeg_set_quality(jpg_ptr,_This->jpegquality, TRUE ); //limit to baseline-JPEG values;

  CCI_RETURN_OK()
}
//--------------------------------------------------------------------
// Write JPEG image data
//--------------------------------------------------------------------
static CCI_METHOD JPEGLoader_WriteData( _Instance_Ptr _This, void* _Scan0, int _Stride )
{
  //dmDEBUG_ASSERT( _This->fp  != NULL )

  jpeg_compress_struct* jpg_ptr = CJPEG_PTR(_This);

  // Step 4: Start compressor

  // TRUE ensures that we will write a complete interchange-JPEG file.
  // Pass TRUE unless you are very sure of what you're doing.
  jpeg_start_compress(jpg_ptr,TRUE);

  // Step 5: write scan lines

  dm_byte* buf   = static_cast<dm_byte*>(_Scan0);
  int     height = jpg_ptr->image_height;

#ifdef __RGB_DATA__

  for(int row = height; --row >= 0; buf += _Stride)
     (void) jpeg_write_scanlines(jpg_ptr,&buf,1);

#else //__RGB_DATA__

  if(_This->PixelFormat == dmPixelFormat24bppRGB) 
  {
    // we need to allocate a specific buffer
    dm_byte* row_ptr = new dm_byte[dm_abs(_Stride)];

    int     width  = jpg_ptr->image_width;
    dm_byte *inbuf,*outbuf;

    for(int row = height; --row >= 0;  buf += _Stride )
    {
      // swap bgr to rgb
      inbuf  = buf;
      outbuf = row_ptr;
      for(int x = width; --x >=0; inbuf+=3,outbuf+=3 ) 
      {
         *outbuf    = *(inbuf+2);
        *(outbuf+1) = *(inbuf+1);
        *(outbuf+2) = *inbuf ;
      } 

      (void) jpeg_write_scanlines(jpg_ptr,&row_ptr,1);

    }

    delete [] row_ptr;
  }
  else 
  {
    for(int row = height; --row >= 0; buf += _Stride)
      (void) jpeg_write_scanlines(jpg_ptr,&buf,1);
  }
 
#endif //__RGB_DATA__

  // Step 6: Finish compression
  jpeg_finish_compress(jpg_ptr);

  return _This->Status; 
}
//--------------------------------------------------------------------
