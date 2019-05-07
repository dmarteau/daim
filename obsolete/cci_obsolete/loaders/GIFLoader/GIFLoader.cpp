
#include "GIFError.cpp"

#define GIF_NOT_TRANSPARENT -1

const int InterlacedOffset[] = { 0, 4, 2, 1 }; // The way Interlaced image should.
const int InterlacedJumps[]  = { 8, 8, 4, 2 }; // be read - offsets and jumps... 

//--------------------------------------------------------------------
static dm_param GIFLoader_Error_fn(GifFileType* GifFile, const char* msg)
{
  _Instance_Ptr _This = (_Instance_Ptr)GifFile->UserData;
  _This->Status = CCI_ERR_IOERROR;   // set error status

  if(msg) 
     dmLOG("GIF:ERROR: %s\n",msg);

  return _This->Status;
}
//--------------------------------------------------------------------
// I/O Callbacks 
//--------------------------------------------------------------------
static int  GIFLoader_Read_fn( GifFileType* GifFile, GifByteType* buf, int count ) 
{
  _Instance_Ptr _This = (_Instance_Ptr)GifFile->UserData;
  if(_This->InputStream.Read( buf, count ) != (dm_uint)count) {
     GIFLoader_Error_fn(GifFile, "Buffer read Error!");
     return 0;
  }
  return count;
}
//--------------------------------------------------------------------
static int  GIFLoader_Write_fn(  GifFileType* GifFile, const GifByteType* buffer, int count ) 
{
  _Instance_Ptr _This = (_Instance_Ptr)GifFile->UserData;
  if(_This->OutputStream.Write( const_cast<GifByteType*>(buffer), count ) != (dm_uint)count) {
     GIFLoader_Error_fn(GifFile, "Buffer write Error!");
     return 0;
  }
  return count;
}
//--------------------------------------------------------------------
// Reading
//--------------------------------------------------------------------
static void GIFLoader_Clear( _Instance_Ptr _This )
{
  if(_This->GifFile) 
  {
    if(_This->Mode==CCI_ME_GET) 
    {
       if(DGifCloseFile(_This->GifFile) == GIF_ERROR)
          PrintGifError();
    }
    else 
    {
       if(EGifCloseFile(_This->GifFile) == GIF_ERROR)
          PrintGifError();
    }
    
    _This->GifFile  = NULL;
  } 

  _This->Status = CCI_ERR_OK;
  _This->Mode   = 0;
}
//--------------------------------------------------------------------
static CCI_METHOD GIFLoader_InitReadStruct( _Instance_Ptr _This )
{
   _This->Mode    = CCI_ME_GET;
   _This->GifFile = DGifOpen(_This, GIFLoader_Read_fn);
  
   if(_This->GifFile == NULL) {
      PrintGifError();
      return (_This->Status = CCI_ERR_GENERIC);
   }

   CCI_RETURN_OK()
}
//--------------------------------------------------------------------
// Read GIF header
//--------------------------------------------------------------------
static CCI_METHOD GIFLoader_ReadHDR( _Instance_Ptr _This )
{
   dmDEBUG_ASSERT( _This->GifFile  != NULL )

   GifFileType    *GifFile    =_This->GifFile;

   GifByteType    *Extension;
   GifRecordType   RecordType;

   int ExtCode;

   // XXX Limitation : we only return the first image in this file
 
    /* Scan the content of the GIF file and load the image(s) in: */
    do
    {
      if (DGifGetRecordType(GifFile,&RecordType) == GIF_ERROR) {
        PrintGifError();
        return (_This->Status = CCI_ERR_GENERIC);
      }
      
      switch (RecordType)  
      {
        case IMAGE_DESC_RECORD_TYPE:
          if(DGifGetImageDesc(GifFile) == GIF_ERROR) 
          {
            PrintGifError();
            return (_This->Status = CCI_ERR_GENERIC);
          }
       
          _This->width  = GifFile->Image.Width; 
          _This->height = GifFile->Image.Height;
                  
          // We are done : next input will be image data
          CCI_RETURN_OK()
          break;
       
       case EXTENSION_RECORD_TYPE:
         // Skip any extension blocks in file: don't know what to do with
         if(DGifGetExtension(GifFile, &ExtCode, &Extension) == GIF_ERROR) {
            PrintGifError();
            return (_This->Status = CCI_ERR_GENERIC);
         }
         while (Extension != NULL) 
         {
           if(DGifGetExtensionNext(GifFile, &Extension) == GIF_ERROR) {
              PrintGifError();
              return (_This->Status = CCI_ERR_GENERIC);
           }
         }
         break;
 
        default:        // Should be traps by DGifGetRecordType.
          break;
      }
    }
    while (RecordType != TERMINATE_RECORD_TYPE);

    // Hue we should not be here : no image data !!!
    return GIFLoader_Error_fn(GifFile, "No image data !");
}

//--------------------------------------------------------------------
// Read Helpers
//--------------------------------------------------------------------

// Copy bytes from source to destination  skipping transparent bytes
static void GIFLoader_CopyGIF(dm_byte* pDst, dm_byte* pSrc, int width, const int transparent, 
                             GifColorType* pColorTable,
                             GifColorType& backgroundColor )
{
  if (width)
  {
    GifColorType* pColor;
    do
    {
      GifPixelType b = *pSrc++;
      if(b != transparent)
         pColor = pColorTable + b;
      else   
         pColor = &backgroundColor;

     #ifndef __RGB_DATA__
      // Translate to 24-bit RGB value if not transparent
      *pDst++ = pColor->Blue;
      *pDst++ = pColor->Green;
      *pDst++ = pColor->Red;
     #else
      *pDst++ = pColor->Red;
      *pDst++ = pColor->Green;
      *pDst++ = pColor->Blue;      
     #endif     
    }
    while (--width);
  }
}
//--------------------------------------------------------------------
static CCI_METHOD GIFLoader_ReadExpand( _Instance_Ptr _This, void* _Scan0, int _Stride )
{
  GifFileType *GifFile =_This->GifFile;

  // Ensure that we have a RGB image container
  if(_This->PixelFormat != dmPixelFormat24bppRGB) 
     return GIFLoader_Error_fn(GifFile, "Invalid image buffer type !");
       
   // Get color Map
   ColorMapObject *ColorMap = (GifFile->Image.ColorMap
                               ? GifFile->Image.ColorMap
                               : GifFile->SColorMap);

   const int width  = GifFile->Image.Width;
   const int height = GifFile->Image.Height;
     
   dm_byte* row_p = static_cast<dm_byte*>(_Scan0);

   // Get a line buffer
   GifPixelType* pLine         = new GifPixelType[width];
   GifColorType* pColorTable   = ColorMap->Colors;
   GifColorType  rBckgndColor  = pColorTable[GifFile->SBackGroundColor];
     
   int transparent = GIF_NOT_TRANSPARENT;            
                 
   // Expand palette to rgb      
   if (GifFile->Image.Interlace)
   {
      // Need to perform 4 passes on the images:
      for (int pass=0; pass<4; pass++)
      {
        for (int i=InterlacedOffset[pass];i<height;i+=InterlacedJumps[pass])
        {
          if (DGifGetLine(GifFile,pLine,width) == GIF_ERROR)
          {
            PrintGifError();
            _This->Status = CCI_ERR_GENERIC;
            break;
          }
          GIFLoader_CopyGIF(row_p + i * _Stride,pLine,
                            width,transparent,pColorTable,
                            rBckgndColor);
        }

        if(!CCI_SUCCEEDED(_This->Status))
           break;
      }       
   }
   else
   {
     // Non-interlaced image
     for (int i=0;i<height;i++)
     {
       if (DGifGetLine(GifFile, pLine,width) == GIF_ERROR)
       {
         PrintGifError();
         _This->Status = CCI_ERR_GENERIC;
         break;
       }
       GIFLoader_CopyGIF(row_p,pLine,width,transparent,pColorTable,rBckgndColor);
            
       row_p += _Stride;
     }
   }   
     
   delete [] pLine;   

   return _This->Status;
}
//--------------------------------------------------------------------
static CCI_METHOD GIFLoader_ReadIndexed( _Instance_Ptr _This, void* _Scan0, int _Stride )
{
   GifFileType    *GifFile =_This->GifFile;

   // Image is not expanded to rgb , read 
   if(_This->PixelFormat != dmPixelFormat8bppIndexed) 
      return GIFLoader_Error_fn(GifFile, "Invalid image buffer type !");

   const int width  = GifFile->Image.Width;
   const int height = GifFile->Image.Height;
     
   dm_byte* row_p = static_cast<dm_byte*>(_Scan0);
   
   GifPixelType* pLine = row_p;
    
   if (GifFile->Image.Interlace)
   {
      // Need to perform 4 passes on the images:
      for (int pass=0; pass<4; pass++)
      {
        for (int i=InterlacedOffset[pass];i<height;i+=InterlacedJumps[pass])
        {
          if (DGifGetLine(GifFile,pLine + i * _Stride,width) == GIF_ERROR) 
          {
            PrintGifError();
            return (_This->Status = CCI_ERR_GENERIC);
          }
        }
      }
   }
   else
   {
     // Non-interlaced image
     for (int i=0;i<height;i++)
     {
       if (DGifGetLine(GifFile, pLine,width) == GIF_ERROR)
       {
         PrintGifError();
         _This->Status = CCI_ERR_GENERIC;
         break;
       }
       pLine += _Stride;
     }
   }       

   return _This->Status;
}
//--------------------------------------------------------------------
// Read GIF data
//--------------------------------------------------------------------
static CCI_METHOD GIFLoader_ReadData( _Instance_Ptr _This, void* _Scan0, int _Stride )
{
  dmDEBUG_ASSERT( _This->Status == CCI_ERR_OK );
  dmDEBUG_ASSERT( _This->GifFile != NULL )

  if(_This->expandPalette)
     return GIFLoader_ReadExpand(_This,_Scan0,_Stride);
  else
     return GIFLoader_ReadIndexed(_This,_Scan0,_Stride);
}
//--------------------------------------------------------------------
static CCI_METHOD GIFLoader_GetFormat( _Instance_Ptr _This )
{
  if(_This->expandPalette)
     _This->PixelFormat = dmPixelFormat24bppRGB; 
  else 
     _This->PixelFormat = dmPixelFormat8bppIndexed; 
  
  CCI_RETURN_OK()
}
//--------------------------------------------------------------------
static CCI_METHOD GIFLoader_SetFormat( _Instance_Ptr _This )
{
  if(_This->PixelFormat == dmPixelFormat8bppIndexed ||
     _This->PixelFormat == dmPixelFormat24bppRGB)
      CCI_RETURN_OK()
  
  return (_This->Status = CCI_ERR_NOT_IMPLEMENTED);
}
//--------------------------------------------------------------------
// Writing
//--------------------------------------------------------------------
static void GIFLoader_LoadProperties( _Instance_Ptr _This, dm_uint ioflags, dmIProperties& _IProps  )
{
  if(_IProps.IsValid()) 
  {
    if((ioflags & CCIV_Loader_Read)!=0)
    {
      const char* _expandPalette = _IProps.GetData(NULL,_TXT("decode.expandPaletteToRGB"),_TXT("yes"));
      if(EQUAL_NOCASE_STRING(_expandPalette,"y")||EQUAL_NOCASE_STRING(_expandPalette,"yes"))
        _This->expandPalette = dm_true;
      else      
        _This->expandPalette = dm_false;
    }
  }
}
//--------------------------------------------------------------------
static CCI_METHOD GIFLoader_InitWriteStruct( _Instance_Ptr _This )
{
  dmDEBUG_ASSERT( _This->width  > 0 );
  dmDEBUG_ASSERT( _This->height > 0 );

  _This->Mode = CCI_ME_SET;

  _This->GifFile = EGifOpen(_This, GIFLoader_Write_fn);
  
   if(_This->GifFile == NULL) {
      PrintGifError();
      return (_This->Status = CCI_ERR_GENERIC);
   }

   CCI_RETURN_OK()
}
//--------------------------------------------------------------------
// Read PNG data
//--------------------------------------------------------------------
static CCI_METHOD GIFLoader_WriteData( _Instance_Ptr _This, 
                                       void* _Scan0, int _Stride,
                                       textp text_ptr, int num_text )
{
  dmDEBUG_ASSERT( _This->Status == CCI_ERR_OK );
  dmDEBUG_ASSERT( _This->GifFile != NULL )
 
  GifFileType    *GifFile =_This->GifFile;
 
  int ColorMapSize = 256, ExpNumOfColors = 8;
 
  GifByteType    *OutputBuffer   = NULL;
  ColorMapObject *OutputColorMap = NULL;

  int BufferStride;

  size_t Width  = _This->width;
  size_t Height = _This->height;
 
  if((OutputColorMap = MakeMapObject(ColorMapSize, NULL)) == NULL)
      return  GIFLoader_Error_fn(GifFile,"Failed to allocate memory required");
    
  // Quantize RGB Colors
  if(_This->PixelFormat == dmPixelFormat24bppRGB)
  {
    GifByteType *RedBuffer    = NULL,
                *GreenBuffer  = NULL, 
                *BlueBuffer   = NULL;

    size_t Size = Width * Height;

    BufferStride = Width;

    if((OutputBuffer = new GifByteType[Size]) == NULL ||
       (RedBuffer    = new GifByteType[Size]) == NULL ||
       (GreenBuffer  = new GifByteType[Size]) == NULL || 
       (BlueBuffer   = new GifByteType[Size]) == NULL ) {
        
        GIFLoader_Error_fn(GifFile,"Failed to allocate memory required.");
    } else {
      
      GifByteType *pR = RedBuffer,*pG = GreenBuffer,*pB = BlueBuffer;                      

      GifByteType* row_p = static_cast<GifByteType*>(_Scan0), *pDst;
      
      for(size_t j=0;j<Height;++j) 
      {
        pDst = row_p;
        for(size_t i=0;i<Width;++i)
        {
          #ifndef __RGB_DATA__
            *pB++ = *pDst++;
            *pG++ = *pDst++;
            *pR++ = *pDst++;
          #else
            *pR++ = *pDst++;
            *pG++ = *pDst++;
            *pB++ = *pDst++;             
          #endif
        }
        row_p += _Stride;
      }
    
      if(QuantizeBuffer(Width, Height, &ColorMapSize,
                        RedBuffer, GreenBuffer, BlueBuffer,
                        OutputBuffer, OutputColorMap->Colors) == GIF_ERROR)
      {
        GIFLoader_Error_fn(GifFile,"Quantize error");
      } 
      
      if(RedBuffer)   delete [] RedBuffer;
      if(GreenBuffer) delete [] GreenBuffer;
      if(BlueBuffer)  delete [] BlueBuffer;
    }
  }
  // Use gray Scale palette
  else
  {
    OutputBuffer = static_cast<GifByteType*>(_Scan0);
    
    BufferStride = _Stride;
    
    GifColorType* pColorTable = OutputColorMap->Colors;
    for(int i=0;i<ColorMapSize;++i,++pColorTable) {
      pColorTable->Red   = i;
      pColorTable->Green = i;
      pColorTable->Blue  = i; 
    }      
  }
  
  // Save data
  if(EGifPutScreenDesc(GifFile,Width,Height,ExpNumOfColors,0,OutputColorMap)==GIF_ERROR) {
     PrintGifError();
     _This->Status = CCI_ERR_IOERROR;
  }
     
  if(EGifPutImageDesc(GifFile,0,0,Width,Height,FALSE,NULL)== GIF_ERROR) {
     PrintGifError();         
     _This->Status = CCI_ERR_IOERROR;
  }

  GifByteType *Ptr = OutputBuffer;
  
  for(size_t i=0;i<Height;++i)
  {
    if(EGifPutLine(GifFile,Ptr,Width) == GIF_ERROR) {
       PrintGifError();
       break;  
    }     
    Ptr += BufferStride;
  }

  if(_This->PixelFormat == dmPixelFormat24bppRGB) {
     if(OutputBuffer) delete [] OutputBuffer;
  }

  return _This->Status;
}
//--------------------------------------------------------------------
textp GIFLoader_CreateText( _Instance_Ptr _This, int& num_text )
{
  return NULL;
}
//--------------------------------------------------------------------
