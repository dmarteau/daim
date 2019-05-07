
#define MAX_STRBUFF_SIZE 512
//--------------------------------------------------------------------
// Error handlers
//--------------------------------------------------------------------
static void TIFFLoader_Error_fn(const char* module, const char* fmt, va_list ap)
{
  if(fmt) {
    char _buffer[MAX_STRBUFF_SIZE] = "\0"; 
    _vsntprintf(_buffer,MAX_STRBUFF_SIZE,fmt,ap);
    dmLOG("ERROR:TIFF: %s: %s\n",module,_buffer);
  }
}
//--------------------------------------------------------------------
static void TIFFLoader_Warning_fn(const char* module, const char* fmt, va_list ap)
{
  if(fmt) {
    char _buffer[MAX_STRBUFF_SIZE] = "\0"; 
    _vsntprintf(_buffer,MAX_STRBUFF_SIZE,fmt,ap);
    dmLOG("WARNING:TIFF: %s: %s\n",module,_buffer);
  }
}
//--------------------------------------------------------------------
static tsize_t TIFFLoader_ReadProc (thandle_t fd, tdata_t buf, tsize_t size)
{
  _Instance_Ptr _This = reinterpret_cast<_Instance_Ptr>(fd);
  return _This->InputStream.Read(buf,size);
}
//--------------------------------------------------------------------
static tsize_t TIFFLoader_WriteProc (thandle_t fd, tdata_t buf, tsize_t size)
{
  _Instance_Ptr _This = reinterpret_cast<_Instance_Ptr>(fd);
  return _This->OutputStream.Write(buf,size);
}
//--------------------------------------------------------------------
static toff_t TIFFLoader_SeekProc (thandle_t fd, toff_t off, int i)
{
  // This appears to return the location that it went to
  return off;
}
//--------------------------------------------------------------------
static int TIFFLoader_CloseProc (thandle_t fd)
{
  /*
  if(_This->OutputStream.IsValid())
     _This->OutputStream.Close();

  if(_This->IntputStream.IsValid())
     _This->IntputStream.Close();
  */   
  // Return a zero meaning all is well
  return 0;
}
//--------------------------------------------------------------------
// Reading
//--------------------------------------------------------------------
static void TIFFLoader_Clear( _Instance_Ptr _This )
{
  if(_This->tif_ptr)
  {
    TIFFClose(_This->tif_ptr);
    _This->tif_ptr = NULL;
  } 

  _This->Status = CCI_ERR_OK;
  _This->Mode   = 0;
}
//--------------------------------------------------------------------
static CCI_METHOD TIFFLoader_InitReadStruct( _Instance_Ptr _This )
{
  _This->Mode    = CCI_ME_GET;
  _This->tif_ptr = TIFFOpen(_This->InputStream.Spec(),"r");
 
/*  
  // Open the dummy document (which actually only exists in memory)
  _This->tif_ptr = TIFFClientOpen ("input", "r",(thandle_t)_This, 
            TIFFLoader_ReadProc,
            TIFFLoader_WriteProc,
            TIFFLoader_SeekProc,
            TIFFLoader_CloseProc, 
            NULL, NULL, NULL);
*/
  
  _This->xresolution = 1.0f;
  _This->yresolution = 1.0f;

 if(_This->tif_ptr == NULL)
   return (_This->Status = CCI_ERR_GENERIC);

 // Count the number of tiff directories
 _This->nrecords = 0; 
 do { ++_This->nrecords; }
  while (TIFFReadDirectory(_This->tif_ptr));

  
 CCI_RETURN_OK()
}
//--------------------------------------------------------------------
// Read TIFF header
//--------------------------------------------------------------------
static CCI_METHOD TIFFLoader_ReadHDR( _Instance_Ptr _This )
{
   dmDEBUG_ASSERT( _This->tif_ptr  != NULL )

   // Restore base directory
   if(_This->record < _This->nrecords) 
     TIFFSetDirectory(_This->tif_ptr,_This->record);
  
   _This->photometric    = 0;
   _This->xresolution    = 1.0f;
   _This->yresolution    = 1.0f;
   _This->resolutionunit = RESUNIT_NONE;

   TIFF* in = _This->tif_ptr; 

   if(TIFFGetField(in, TIFFTAG_IMAGEWIDTH     , &_This->width)           && 
      TIFFGetField(in, TIFFTAG_IMAGELENGTH    , &_This->height)          &&
      TIFFGetField(in, TIFFTAG_BITSPERSAMPLE  , &_This->bitspersample)   &&
      TIFFGetField(in, TIFFTAG_SAMPLESPERPIXEL, &_This->samplesperpixel))
    {
      // Get image resolution
      TIFFGetField(in, TIFFTAG_PHOTOMETRIC    , &_This->photometric);
      TIFFGetField(in, TIFFTAG_XRESOLUTION    , &_This->xresolution); 
      TIFFGetField(in, TIFFTAG_YRESOLUTION    , &_This->yresolution); 
      TIFFGetField(in, TIFFTAG_RESOLUTIONUNIT , &_This->resolutionunit);

      TIFFGetField(in, TIFFTAG_SAMPLEFORMAT   , &_This->sampleformat);
 
      // Assume that format is uint
      if(_This->sampleformat==0)
         _This->sampleformat = SAMPLEFORMAT_UINT;
/*
           char* info;
           // Get standarts TIFF tags
		       TIFFGetField(in, TIFFTAG_SOFTWARE, &info);
           TIFFGetField(in, TIFFTAG_ARTIST, &info);
           TIFFGetField(in, TIFFTAG_COPYRIGHT, &info);
           TIFFGetField(in, TIFFTAG_DOCUMENTNAME, &info);
           TIFFGetField(in, TIFFTAG_IMAGEDESCRIPTION, &info);
*/ 
      // Read specific infos
      if(_This->StringList!=NULL) 
      {
        dmIStringList _Strings;
        if(_Strings.QueryInterface(_This->StringList)) 
        {
           _Strings.Clear();
           char* info;

           // Get standarts TIFF tags

		   if( TIFFGetField(in, TIFFTAG_SOFTWARE, &info) != 0) {
              _Strings.AddTail("Software");
              _Strings.AddTail(info);
           }
           if( TIFFGetField(in, TIFFTAG_ARTIST, &info) != 0) {
              _Strings.AddTail("Author");
              _Strings.AddTail(info);
           }
           if( TIFFGetField(in, TIFFTAG_COPYRIGHT, &info) != 0) {
              _Strings.AddTail("Copyright");
              _Strings.AddTail(info);
           }
           if( TIFFGetField(in, TIFFTAG_DOCUMENTNAME, &info) != 0) {
              _Strings.AddTail("DocumentName");
              _Strings.AddTail(info);
           }
           if( TIFFGetField(in, TIFFTAG_IMAGEDESCRIPTION, &info) != 0) {
              _Strings.AddTail("Description");
              _Strings.AddTail(info);
           }
         }
       }

       CCI_RETURN_OK()
   }

   CCI_RETURN_FAIL()
}
//--------------------------------------------------------------------
// Read TIFF image data in separate chunks
//--------------------------------------------------------------------
static CCI_METHOD TIFFLoader_ReadSeparateData( _Instance_Ptr _This, void* _Scan0, int _Stride )
{ 
  dmDEBUG_ASSERT( _This->tif_ptr  != NULL )

  TIFF* in = _This->tif_ptr;

  size_t ustride  = dm_abs(_Stride);
  size_t rowsize  = TIFFScanlineSize(in);

  uint32 height   = _This->height;
  uint16 nsamples = _This->samplesperpixel;

  if( rowsize*nsamples > ustride ) {
    dmLOG("ERROR:TIFF: Invalid stride in image data ! \n");
    return (_This->Status = CCI_ERR_INVALID_PARAM);
  }

  // we need to allocate a specific buffer
  tdata_t buf            = _TIFFmalloc(rowsize);
  size_t  width          = _This->width; 
  size_t  bytespersample = _This->bitspersample >> 3;
  size_t  pixelbytes     = bytespersample * nsamples;

  dm_byte *_r,*_b;

  for(uint16 s=0; s<nsamples; ++s) 
  { 
    dm_byte* inbuf = static_cast<dm_byte*>(_Scan0) + s * bytespersample;

    for(size_t row=0;row<height;++row,inbuf += _Stride) {
      if(TIFFReadScanline(in, buf, row, s)>0) 
      {
        _r = inbuf;
        _b = static_cast<dm_byte*>(buf);
        for(size_t x = 0;x < width; ++x, _r+= pixelbytes,++_b )
          *_r = *_b;
      } else {
         _This->Status = CCI_ERR_IOERROR;
         break;
      }
    }
  }

  _TIFFfree(buf);

  return _This->Status;
} 
//--------------------------------------------------------------------
// Read TIFF contiguous image data
//--------------------------------------------------------------------
static CCI_METHOD TIFFLoader_ReadContiguousData( _Instance_Ptr _This, void* _Scan0, int _Stride )
{ 
   dmDEBUG_ASSERT( _This->tif_ptr  != NULL )

   TIFF* in = _This->tif_ptr;

   size_t ustride  = dm_abs(_Stride);
   size_t rowsize  = TIFFScanlineSize(in);

   uint32 height = _This->height;

   // Check that stride is large enough
   if( rowsize > ustride ) {
     dmLOG("ERROR:TIFF: Invalid stride in image data ! \n");
     return (_This->Status = CCI_ERR_INVALID_PARAM);
   }

   // Default: copy raw data format
   dm_byte* buf = static_cast<dm_byte*>(_Scan0);
   for (size_t row = 0; row < height; ++row, buf += _Stride)
     if(TIFFReadScanline(in, buf, row) < 0) {
       _This->Status = CCI_ERR_IOERROR;
       break;
     }

   return _This->Status;

}
//--------------------------------------------------------------------
// Swap RGB to BGR and vice-versa
//--------------------------------------------------------------------
static void TIFFLoader_BGR( _Instance_Ptr _This, void* _Scan0, int _Stride )
{ 
   /* flip the RGB pixels to BGR */
   if(_This->PixelFormat == dmPixelFormat24bppRGB) 
   {
     uint32 height = _This->height;
     uint32 width  = _This->width;

     dm_byte byte;
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
// Read TIFF image data
//--------------------------------------------------------------------
static CCI_METHOD TIFFLoader_ReadData( _Instance_Ptr _This, void* _Scan0, int _Stride )
{
   dmDEBUG_ASSERT( _This->tif_ptr  != NULL );
   uint16 config;

   TIFFGetField(_This->tif_ptr, TIFFTAG_PLANARCONFIG, &config);

   // Pixels is contiguous
   if (config == PLANARCONFIG_CONTIG)
     TIFFLoader_ReadContiguousData(_This,_Scan0,_Stride);

   // Pixels are in separate chunk of data
   else if (config == PLANARCONFIG_SEPARATE) 
     TIFFLoader_ReadSeparateData(_This,_Scan0,_Stride);

#ifndef __RGB_DATA__
     TIFFLoader_BGR(_This,_Scan0,_Stride);
#endif

   return _This->Status;
}
//--------------------------------------------------------------------
static CCI_METHOD TIFFLoader_GetFormat( _Instance_Ptr _This )
{
   size_t pixelbytes = _This->bitspersample >> 3;

   if(_This->samplesperpixel==1) 
   {
     switch(_This->sampleformat)
     {
       case SAMPLEFORMAT_UINT:
         if(pixelbytes == 1) {
           _This->PixelFormat = dmPixelFormat8bppIndexed; 
           CCI_RETURN_OK() 
         } 
         if(pixelbytes == 2) { 
           _This->PixelFormat = dmPixelFormat16bppGrayScale; 
           CCI_RETURN_OK() 
         }
         if(pixelbytes == 4) { 
           _This->PixelFormat = dmPixelFormat32bppGrayScale; 
           CCI_RETURN_OK()
         }

         break;

       case SAMPLEFORMAT_INT: 
         if(pixelbytes == 1) {
           _This->PixelFormat = dmPixelFormat8bppIndexed; 
           CCI_RETURN_OK() 
         } 
         if(pixelbytes == 2) { 
           _This->PixelFormat = dmPixelFormat16bppGrayScale; 
           CCI_RETURN_OK() 
         }
         if(pixelbytes == 4) { 
           _This->PixelFormat = dmPixelFormat32bppGrayScale; 
           CCI_RETURN_OK()
         }

       case SAMPLEFORMAT_IEEEFP:
         if(pixelbytes == 4) { 
           _This->PixelFormat = dmPixelFormat32bppFloat;
           CCI_RETURN_OK()
         }
     }  
   } 
   else if(_This->samplesperpixel==3)
   {
     if(pixelbytes == 1) {   
       _This->PixelFormat = dmPixelFormat24bppRGB;
       CCI_RETURN_OK()
     }
   }

   _This->PixelFormat = dmPixelFormatUndefined;
   _This->Status      = CCI_ERR_GENERIC;
   dmLOG("ERROR:TIFF: Invalid pixel format !\n");

   CCI_RETURN_FAIL()   
}
//--------------------------------------------------------------------
static CCI_METHOD TIFFLoader_SetFormat( _Instance_Ptr _This )
{
   switch(_This->PixelFormat)
   {
     case dmPixelFormat8bppIndexed    : 
     case dmPixelFormat16bppGrayScale :
     case dmPixelFormat32bppGrayScale :
       _This->photometric     = PHOTOMETRIC_MINISBLACK;
       _This->sampleformat    = SAMPLEFORMAT_UINT;
       _This->bitspersample   = dmGetPixelFormatBits(_This->PixelFormat);
       _This->samplesperpixel = 1; 
       break;
     
     case dmPixelFormat24bppRGB :
       _This->photometric     = PHOTOMETRIC_RGB;
       _This->sampleformat    = SAMPLEFORMAT_UINT;
       _This->samplesperpixel = 3; 
       _This->bitspersample   = 8;
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
static void TIFFLoader_LoadProperties( _Instance_Ptr _This, dm_param me )
{
    dmIInterface<dmIProperties> _IProps;
    if(_IProps.Open(_TXT("daim://loaders/tiffldr"),_TXT("image/tiff"))) 
    {
      if(me == CCI_ME_SET) {
        const char* _Filter   = _IProps.GetData(NULL,_TXT("encode.compression"),_TXT("default"));
        if(EQUAL_NOCASE_STRING(_Filter,_TXT("none")     )) _This->compression = COMPRESSION_NONE;     else
        if(EQUAL_NOCASE_STRING(_Filter,_TXT("lzw")      )) _This->compression = COMPRESSION_LZW;      else
        if(EQUAL_NOCASE_STRING(_Filter,_TXT("jpeg")     )) _This->compression = COMPRESSION_JPEG;     else
        if(EQUAL_NOCASE_STRING(_Filter,_TXT("packbits") )) _This->compression = COMPRESSION_PACKBITS; else
     // Need to compile with zlib in order to enable this experimental compression mode
     // if(EQUAL_NOCASE_STRING(_Filter,_TXT("deflate")  )) _This->compression = COMPRESSION_DEFLATE;  else
        _This->compression = COMPRESSION_NONE;

        _This->jpegquality = atoi(_IProps.GetData(NULL,_TXT("encode.jpeg.quality" ),_TXT("75")));
        _This->predictor   = atoi(_IProps.GetData(NULL,_TXT("encode.lzw.predictor"),_TXT("0")));

        const char* _ColorMode = _IProps.GetData(NULL,_TXT("encode.jpeg.colormode"),_TXT("raw"));
        if(EQUAL_NOCASE_STRING(_ColorMode,_TXT("raw") )) _This->jpegcolormode = JPEGCOLORMODE_RAW; else
        if(EQUAL_NOCASE_STRING(_ColorMode,_TXT("rgb") )) _This->jpegcolormode = JPEGCOLORMODE_RGB;
      } 
      else {
      }
   }
}
//--------------------------------------------------------------------
#define TIFF_SET_TEXT( _key, _str, _tag, _value ) \
  if(EQUAL_NOCASE_STRING(_key,_str)) \
      TIFFSetField(_This->tif_ptr,_tag,_value)   
//--------------------------------------------------------------------
int TIFFLoader_CreateText( _Instance_Ptr _This )
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

          // Set std TIFF tags

          TIFF_SET_TEXT( _key, _TXT("Software")    ,TIFFTAG_SOFTWARE    , _txt);  else 
          TIFF_SET_TEXT( _key, _TXT("Author")      ,TIFFTAG_ARTIST      , _txt);  else 
          TIFF_SET_TEXT( _key, _TXT("Copyright")   ,TIFFTAG_COPYRIGHT   , _txt);  else 
          TIFF_SET_TEXT( _key, _TXT("DocumentName"),TIFFTAG_DOCUMENTNAME, _txt);  else 
          {
            // Add pseudo XML tags
            unparsedtag.Append( _TXT("<"));
            unparsedtag.Append( _key );
            unparsedtag.Append( _TXT(">\n"));
            unparsedtag.Append( _txt );
            unparsedtag.Append( _TXT("</"));
            unparsedtag.Append( _key );
            unparsedtag.Append( _TXT(">\n") );
          }            
          ++num_text;
        } 
        else
          break;
      } 

      if(num_text>0)
         TIFFSetField(_This->tif_ptr,TIFFTAG_IMAGEDESCRIPTION,unparsedtag.CStr());

      return num_text;
    }
  }
  return 0;
}
//--------------------------------------------------------------------
static CCI_METHOD TIFFLoader_InitWriteStruct( _Instance_Ptr _This )
{
  dmDEBUG_ASSERT( _This->tif_ptr == NULL );
  dmDEBUG_ASSERT( _This->width  > 0 );
  dmDEBUG_ASSERT( _This->height > 0 );

  _This->Mode    = CCI_ME_SET;
  _This->tif_ptr = TIFFOpen(_This->OutputStream.Spec(),"w");
  
/*
  _This->tif_ptr = TIFFClientOpen ("output", "w",(thandle_t)_This, 
            TIFFLoader_ReadProc,
            TIFFLoader_WriteProc,
            TIFFLoader_SeekProc,
            TIFFLoader_CloseProc, 
            NULL, NULL, NULL);
*/            
  if(_This->tif_ptr!=NULL)
  {
    TIFF* out = _This->tif_ptr; 

    uint16 photometric = _This->photometric;

    TIFFSetField(out, TIFFTAG_IMAGEWIDTH     , _This->width);
    TIFFSetField(out, TIFFTAG_IMAGELENGTH    , _This->height);
    TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, _This->samplesperpixel);
    TIFFSetField(out, TIFFTAG_BITSPERSAMPLE  , _This->bitspersample  );
    TIFFSetField(out, TIFFTAG_SAMPLEFORMAT   , _This->sampleformat   );

    TIFFSetField(out, TIFFTAG_PLANARCONFIG   , PLANARCONFIG_CONTIG);
    TIFFSetField(out, TIFFTAG_ORIENTATION    , ORIENTATION_TOPLEFT);

    TIFFSetField(out, TIFFTAG_COMPRESSION, _This->compression);
    switch (_This->compression) 
    {
      case COMPRESSION_JPEG:
       if (photometric  == PHOTOMETRIC_RGB && _This->jpegcolormode == JPEGCOLORMODE_RGB)
           photometric = PHOTOMETRIC_YCBCR;

        TIFFSetField(out, TIFFTAG_JPEGQUALITY  , _This->jpegquality  );
        TIFFSetField(out, TIFFTAG_JPEGCOLORMODE, _This->jpegcolormode);
        break;
      case COMPRESSION_LZW:
      case COMPRESSION_DEFLATE:
        if (_This->predictor != 0)
          TIFFSetField(out, TIFFTAG_PREDICTOR, _This->predictor);
        break;
    }

    TIFFSetField(out, TIFFTAG_PHOTOMETRIC , photometric );

    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//--------------------------------------------------------------------
// Write TIFF image data
//--------------------------------------------------------------------
static CCI_METHOD TIFFLoader_WriteData( _Instance_Ptr _This, void* _Scan0, int _Stride )
{
   dmDEBUG_ASSERT( _This->tif_ptr  != NULL )

   TIFF* out = _This->tif_ptr; 

   TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(out, (uint32) -1 ));

   // Check that stride is large enough
   size_t ustride  = dm_abs(_Stride);
   size_t rowsize  = TIFFScanlineSize(out);

   if( rowsize < ustride ) {
     dmLOG("ERROR:TIFF: Invalid stride in image data ! \n");
     return (_This->Status = CCI_ERR_INVALID_PARAM);
   }
 
   dm_byte* buf    = static_cast<dm_byte*>(_Scan0);
   uint32  height = _This->height;

#ifdef __RGB_DATA__

   for (size_t row = 0; row < height; ++row, buf += _Stride ) {
      if(TIFFWriteScanline(out,buf,row,0) < 0) {
        _This->Status = CCI_ERR_GENERIC;
        break;
      }
   }

#else

  if(_This->PixelFormat == dmPixelFormat24bppRGB) 
  {
    // we need to allocate a specific buffer
    dm_byte* row_ptr = static_cast<dm_byte*>(_TIFFmalloc(rowsize));

    uint32  width  = _This->width;
    dm_byte *inbuf,*outbuf;

    for (size_t row = 0; row < height; ++row, buf += _Stride )
    {
      // swap bgr to rgb
      inbuf  = buf;
      outbuf = row_ptr;
      for(int x = width; --x >=0; inbuf+=3,outbuf+=3 ) {
        *inbuf     = *(outbuf+2);
        *(inbuf+2) = *outbuf;
      } 

      if(TIFFWriteScanline(out,row_ptr,row,0) < 0) {
        _This->Status = CCI_ERR_GENERIC;
        break;
      }
    }
    _TIFFfree(row_ptr);
  }
  else 
  {
    for (size_t row = 0; row < height; ++row, buf += _Stride ) {
       if(TIFFWriteScanline(out,buf,row,0) < 0) {
         _This->Status = CCI_ERR_GENERIC;
         break;
       }
    }
  }
 

#endif

   return _This->Status; 
}
//--------------------------------------------------------------------
