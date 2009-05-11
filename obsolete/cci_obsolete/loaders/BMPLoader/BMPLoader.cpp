
#define SIZEOF_BMPFILEHEADER_14 14
#define SIZEOF_BMPINFOHEADER_64 64
#define SIZEOF_BMPINFOHEADER_40 40

/* $ChangeLog  v1.3, 13 sept. 2006 08:37:57 , David
 *
 * Fixed bug when writing bmpinfoheader ! Always returned false !! 
 */ 

inline bool FREAD_OK( dmIInputStream& input,void* buffer,size_t sizeofbuf) {
  return input.Read(buffer,sizeofbuf) == sizeofbuf;
}

inline bool FWRITE_OK(dmIOutputStream& output,void* buffer,size_t sizeofbuf) {
  return output.Write(buffer,sizeofbuf) == sizeofbuf;
}

#define GET_INT16(array,offset) (\
     static_cast<unsigned int>( array[offset]) + \
    (static_cast<unsigned int>( array[offset+1]) << 8))

#define GET_INT32(array,offset) (\
     static_cast<dm_int32>( array[offset]   ) +        \
    (static_cast<dm_int32>( array[offset+1] ) << 8)  + \
    (static_cast<dm_int32>( array[offset+2] ) << 16) + \
    (static_cast<dm_int32>( array[offset+3] ) << 24))

#define PUT_INT16(array,offset,value) (\
     array[offset]   = static_cast<dm_byte>(  (value) & 0xFF ), \
     array[offset+1] = static_cast<dm_byte>( ((value) >> 8) & 0xFF) )

#define PUT_INT32(array,offset,value) (\
	 array[offset]   = static_cast<dm_byte>(  (value) & 0xFF), \
	 array[offset+1] = static_cast<dm_byte>( ((value) >> 8 ) & 0xFF), \
	 array[offset+2] = static_cast<dm_byte>( ((value) >> 16) & 0xFF), \
	 array[offset+3] = static_cast<dm_byte>( ((value) >> 24) & 0xFF) )

#define RETURN_BMP_ERROR( _txt ) {\
    dmLOG( _TXT("ERROR:BMP:") _txt _TXT(" !\n") );\
    return (_This->Status = CCI_ERR_GENERIC);     \
  }

#define BMP_ERR_INVALID_HEADER() \
     RETURN_BMP_ERROR( _TXT("Invalid bmp header") )

#define BMP_ERR_BAD_FILE() \
     RETURN_BMP_ERROR( _TXT("Not a valid bmp file") )

#define BMP_ERR_EOF() \
     RETURN_BMP_ERROR( _TXT("Not a valid bmp file (truncated file)") )

#define BMP_ERR_WRITE() \
     RETURN_BMP_ERROR( _TXT("Error writing file !") )

#define BMP_ERR_COMPRESSED() \
     RETURN_BMP_ERROR( _TXT("Compressed data are not supported !") )

//--------------------------------------------------------------------
// Swap RGB to BGR and vice-versa
//--------------------------------------------------------------------
#ifdef __RGB_DATA__

static void BMPLoader_BGR( _Instance_Ptr _This, void* _Scan0, int _Stride )
{ 
   /* flip the RGB pixels to BGR */
   if(_This->PixelFormat == dmPixelFormat24bppRGB) 
   {
     size_t  height = _This->Height;
     size_t  width  = _This->Width;

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

#endif
//--------------------------------------------------------------------
void BMPLoader_Clear( _Instance_Ptr _This  )
{
  _This->Status = CCI_ERR_OK;
  _This->Mode   = 0;
}
//--------------------------------------------------------------------
CCI_METHOD BMPLoader_InitReadStruct( _Instance_Ptr _This )
{
   _This->Mode        = CCI_ME_GET;
   _This->PixelFormat = dmPixelFormatUndefined;
  
   CCI_RETURN_OK()
}
//--------------------------------------------------------------------
// Read JPEG header
//--------------------------------------------------------------------
CCI_METHOD BMPLoader_ReadHDR( _Instance_Ptr _This )
{
  dmIInputStream& _Input = _This->InputStream;

  dm_byte bmpfileheader[SIZEOF_BMPFILEHEADER_14];
  dm_byte bmpinfoheader[SIZEOF_BMPINFOHEADER_64];

  dm_int32 bfOffBits;
  dm_int32 headerSize;

  unsigned int biPlanes;

  dm_int32 biCompression;
  dm_int32 biClrUsed = 0;

  int mapentrysize = 0;		// 0 indicates no colormap

  dm_int32 bPad;

  // Read and verify the bitmap file header 
  if(! FREAD_OK(_Input,bmpfileheader,SIZEOF_BMPFILEHEADER_14) ) 
     BMP_ERR_EOF();

  if(GET_INT16(bmpfileheader,0) != 0x4D42) // 'BM' 
     BMP_ERR_BAD_FILE();

  bfOffBits = GET_INT32(bmpfileheader,10);
  // We ignore the remaining fileheader fields

  // The infoheader might be 12 bytes (OS/2 1.x), 40 bytes (Windows),
  // or 64 bytes (OS/2 2.x).  Check the first 4 bytes to find out which.

  if (!FREAD_OK(_Input, bmpinfoheader, 4))
    BMP_ERR_EOF()

  headerSize = GET_INT32(bmpinfoheader,0);

  if (headerSize < 12 || headerSize > 64)
    BMP_ERR_INVALID_HEADER()

  if (! FREAD_OK(_Input, bmpinfoheader+4,static_cast<size_t>(headerSize-4) ))
    BMP_ERR_EOF()

  switch (headerSize) 
  {
    case 12:
      // Decode OS/2 1.x header (Microsoft calls this a BITMAPCOREHEADER)
      _This->Width  = GET_INT16(bmpinfoheader,4);
      _This->Height = GET_INT16(bmpinfoheader,6);

      biPlanes = GET_INT16(bmpinfoheader,8);
      if (biPlanes == 1)
        dmLOG ("WARNING:BMP: found biPlanes != 1\n");

      _This->BitsPerPixels = GET_INT16(bmpinfoheader,10);

      if(_This->BitsPerPixels == 8) 
        mapentrysize = 3;

      break;

    case 40:
    case 64:
      // Decode Windows 3.x header (Microsoft calls this a BITMAPINFOHEADER)
      // or OS/2 2.x header, which has additional fields that we ignore
      _This->Width  = GET_INT32(bmpinfoheader,4);
      _This->Height = GET_INT32(bmpinfoheader,8);

      biPlanes = GET_INT16(bmpinfoheader,12);
      if (biPlanes != 1)
        dmLOG ("WARNING:BMP: found biPlanes != 1\n");

      _This->BitsPerPixels = GET_INT16(bmpinfoheader,14);

      biCompression   = GET_INT32(bmpinfoheader,16);
      if (biCompression != 0)
        BMP_ERR_COMPRESSED();

      _This->XResolution = GET_INT32(bmpinfoheader,24);
      _This->YResolution = GET_INT32(bmpinfoheader,28);

      // Used for colormap
      biClrUsed  = GET_INT32(bmpinfoheader,32);

      if(_This->BitsPerPixels == 8) 
        mapentrysize = 4;
 
      break;

    default:
      BMP_ERR_INVALID_HEADER()
      break;
  }

  // Compute distance to bitmap data --- will adjust for colormap below
  bPad = bfOffBits - (headerSize + 14);

  // Skip the colormap, if any
  if (mapentrysize > 0) 
  {
    if(biClrUsed <= 0)
       biClrUsed = 256;	      // Assume it's 256
    else if(biClrUsed > 256)
       BMP_ERR_INVALID_HEADER()

    if(! FREAD_OK(_Input,NULL,biClrUsed * mapentrysize))
       BMP_ERR_EOF()

    // account for size of colormap
    bPad -= biClrUsed * mapentrysize;
  }

  // Skip any remaining pad bytes
  if (bPad < 0)			// incorrect bfOffBits value ?
     BMP_ERR_INVALID_HEADER()

  if(! FREAD_OK(_Input,NULL,bPad))
       BMP_ERR_EOF()

/*
  while (--bPad >= 0) {
    if(getc(fp) == EOF)
      BMP_ERR_EOF()
  }
*/
  // Now the file pointer should be positionned to the beginnning
  // of image data 

  CCI_RETURN_OK()
}
//--------------------------------------------------------------------
// Create correct DAIM Pixel format
//--------------------------------------------------------------------
CCI_METHOD BMPLoader_GetFormat( _Instance_Ptr _This )
{
  switch(_This->BitsPerPixels)
  {
    case 8:
      _This->PixelFormat =  dmPixelFormat8bppIndexed;
      CCI_RETURN_OK()

    case 24:
      _This->PixelFormat =  dmPixelFormat24bppRGB;
      CCI_RETURN_OK()

    default :
      dmLOG(_TXT("ERROR:JPEG Unsupported pixel format !\n"));
      _This->Status = CCI_ERR_NOT_IMPLEMENTED;
   } 
   CCI_RETURN_FAIL()
}
//--------------------------------------------------------------------
dm_uint BMPLoader_ScanLineSize( _Instance_Ptr _This, int _Stride )
{
  dm_uint  bmp_stride;

  // Compute row width in file,
  // including padding to 4-byte boundary

  if (_This->BitsPerPixels == 24)
    bmp_stride = (_This->Width * 3);
  else
    bmp_stride = _This->Width;

  while ((bmp_stride & 3) != 0) bmp_stride++;

  return bmp_stride;
}
//--------------------------------------------------------------------
// Read BMP data
//--------------------------------------------------------------------
CCI_METHOD BMPLoader_ReadData( _Instance_Ptr _This , void* _Scan0, int _Stride )
{
  dmIInputStream& _Input = _This->InputStream;

  dm_uint bmp_stride = BMPLoader_ScanLineSize(_This,_Stride);

  // Check size
  // size_t ustride  = dm_abs(_Stride);
  size_t height   = _This->Height;
  size_t row_size = _This->Width * (_This->BitsPerPixels >> 3);

  if( row_size > bmp_stride ) {
    dmLOG("ERROR:BMP: Invalid stride in image data ! \n");
    return (_This->Status = CCI_ERR_INVALID_PARAM);
  }

  size_t bmp_offs = bmp_stride - row_size;

  dm_byte* buffer = static_cast<dm_byte*>(_Scan0);

  // Get row from bottom
  buffer += (height - 1) * _Stride;

  // Read scan lines
  for(int row = height; --row >=0; buffer -= _Stride) 
  {
    if(!FREAD_OK(_Input,buffer,row_size)) 
     BMP_ERR_EOF()

    // skip line offset
    if(!FREAD_OK(_Input,NULL,bmp_offs)) 
       BMP_ERR_EOF()
  }

#ifdef __RGB_DATA__
  // We are working in RGB mode
  // and bmp are written in BGR
  BMPLoader_BGR(_This,_Scan0,_Stride);
#endif

  CCI_RETURN_OK()  
}
//--------------------------------------------------------------------
// Writing
//--------------------------------------------------------------------
static CCI_METHOD BMPLoader_SetFormat( _Instance_Ptr _This )
{
   switch(_This->PixelFormat)
   {
     case dmPixelFormat8bppIndexed    : 
       _This->BitsPerPixels = 8;
       break;

     case dmPixelFormat24bppRGB :
       _This->BitsPerPixels = 24;
       break;

     default:
        _This->Status = CCI_ERR_NOT_IMPLEMENTED;
        return _This->Status;    
   }

   CCI_RETURN_OK()
}
//--------------------------------------------------------------------
static void BMPLoader_LoadProperties( _Instance_Ptr _This, dm_param me )
{
  // Nothing to do
}
//--------------------------------------------------------------------
int BMPLoader_CreateText( _Instance_Ptr _This )
{
  // BMP does not accept text
  return 0;
}
//--------------------------------------------------------------------
static CCI_METHOD BMPLoader_InitWriteStruct( _Instance_Ptr _This )
{
  _This->Mode = CCI_ME_SET;

  CCI_RETURN_OK()
}
//--------------------------------------------------------------------
// Write BMP header 
//--------------------------------------------------------------------
static CCI_METHOD BMPLoader_WriteHDR( _Instance_Ptr _This, int _Stride )
{
  dmIOutputStream& _Output = _This->OutputStream;

  dm_byte bmpfileheader[SIZEOF_BMPFILEHEADER_14];
  dm_byte bmpinfoheader[SIZEOF_BMPINFOHEADER_40];

  dm_int32 headersize, bfSize;
  int      map_entries = 0;

  if(_This->BitsPerPixels == 8)  // Grayscale output.  
       map_entries = 256;        // We need to fake a 256-entry colormap.

  dm_uint height  = _This->Height;
  dm_uint ustride = dm_abs(_Stride);

  // File size
  headersize = SIZEOF_BMPFILEHEADER_14 + SIZEOF_BMPINFOHEADER_40 + map_entries * 4;
  bfSize     = headersize + ustride * height;
  
  // Set unused fields of header to 0
  memset(bmpfileheader, 0, sizeof(bmpfileheader));
  memset(bmpinfoheader, 0, sizeof(bmpinfoheader));

  // Fill the file header
  bmpfileheader[0] = 0x42;	// first 2 bytes are ASCII 'B', 'M'
  bmpfileheader[1] = 0x4D;
  PUT_INT32(bmpfileheader,2, bfSize);       // bfSize

  // we leave bfReserved1 & bfReserved2 = 0
  PUT_INT32(bmpfileheader, 10, headersize); // bfOffBits

  // Fill the info header (Microsoft calls this a BITMAPINFOHEADER)
  PUT_INT16(bmpinfoheader, 0, 40);                   // biSize
  PUT_INT32(bmpinfoheader, 4, _This->Width);         // biWidth
  PUT_INT32(bmpinfoheader, 8, _This->Height);        // biHeight
  PUT_INT16(bmpinfoheader, 12, 1);                   // biPlanes - must be 1
  PUT_INT16(bmpinfoheader, 14,_This->BitsPerPixels); // biBitCount

  // we leave biCompression = 0, for none
  // we leave biSizeImage = 0; this is correct for uncompressed data

  PUT_INT32(bmpinfoheader, 24,_This->XResolution); // XPels/M
  PUT_INT32(bmpinfoheader, 28,_This->XResolution); // XPels/M

  PUT_INT16(bmpinfoheader, 32, map_entries); // biClrUsed
  // we leave biClrImportant = 0

  if(!FWRITE_OK(_Output,bmpfileheader,SIZEOF_BMPFILEHEADER_14))
    BMP_ERR_WRITE();

  if(!FWRITE_OK(_Output,bmpinfoheader,SIZEOF_BMPINFOHEADER_40))
    BMP_ERR_WRITE();

  // Generate linear map entry (RGB0)
  if (map_entries > 0) 
  {
    dm_byte clr[4] = {0};
    for (int i = 0; i < 256; ++i) {
      clr[1] = clr[2] = clr[3] = static_cast<dm_byte>(i);
      _Output.Write(clr,4);
    }
  }
  
  CCI_RETURN_OK()
}
//--------------------------------------------------------------------
// Write BMP image data
//--------------------------------------------------------------------
static CCI_METHOD BMPLoader_WriteData( _Instance_Ptr _This, void* _Scan0, int _Stride )
{
  if(!CCI_SUCCEEDED(BMPLoader_WriteHDR(_This,_Stride)))
    return _This->Status;

  dmIOutputStream& _Output = _This->OutputStream;

  dm_byte* buffer = static_cast<dm_byte*>(_Scan0);

  dm_uint bmp_stride = BMPLoader_ScanLineSize(_This,_Stride);
  dm_uint height     = _This->Height;

  size_t row_size = _This->Width * (_This->BitsPerPixels >> 3);

  if( row_size > bmp_stride ) {
    dmLOG("ERROR:BMP: Invalid stride in image data ! \n");
    return (_This->Status = CCI_ERR_INVALID_PARAM);
  }

  size_t bmp_offs = bmp_stride - row_size;

  // Get row from bottom
  buffer += (height - 1) * _Stride;

#ifndef __RGB_DATA__

  // BMP are written in BGR, so just write the data
  for(int row = height; --row >=0; buffer -= _Stride) 
  {
    if(!FWRITE_OK(_Output,buffer,row_size)) 
      BMP_ERR_WRITE();

    // skip line offset
    _Output.Write(NULL,bmp_offs);
  }

#else

  if(_This->PixelFormat == dmPixelFormat24bppRGB) 
  {
    // we need to allocate a temporary buffer
    dm_byte* row_ptr = new dm_byte[dm_abs(_Stride)];

    int     width  = _This->Width;
    dm_byte *inbuf,*outbuf;

    for (int row = height; --row >= 0;  buf -= _Stride )
    {
      // swap bgr to rgb
      inbuf  = buf;
      outbuf = row_ptr;
      for(int x = width; --x >=0; inbuf+=3,outbuf+=3 ) {
        *inbuf     = *(outbuf+2);
        *(inbuf+2) = *outbuf;
      } 

      if(!FWRITE_OK(_Output,row_ptr,row_size)) {
        delete [] row_ptr;
        BMP_ERR_WRITE();
      }

      FWRITE_OK(_Output,row_ptr,row_size)
      
      // skip line offset
       _Output.Write(NULL,bmp_offs);
    }

    delete [] row_ptr;
  }
  else 
  {
    for(int row = height; --row >=0; buffer -= _Stride) 
    {
      if(!FWRITE_OK(_Output,buffer,row_size)) 
        BMP_ERR_WRITE();

      // skip line offset
      for(int i=0;i<bmp_offs;++i) 
        putc(0, fp);
    }
  }
 

#endif

  return _This->Status; 
}
//--------------------------------------------------------------------
