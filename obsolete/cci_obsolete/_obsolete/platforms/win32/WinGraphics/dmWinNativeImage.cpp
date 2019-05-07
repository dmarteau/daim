#define dmUseKernelImage
#include "daim_tools.h"

#include "dmWinNativeImage.h"

//----------------------------------
// Native image
//----------------------------------
bool dmWinNativeImage::CreateBitmap( INT _Width, INT _Height, EPixelFormat _Format ) 
{
  if(_BmpHeader!=NULL)
    return false;

  if(_Width <= 0 || _Height<= 0) {
    dmLOG("ERROR: NativeImage: Invalid size !\n");
    return false;
  }

  INT  _Depth;

  switch( _Format )
  {
    case dmPixelFormat8bppIndexed:
       _NumPaletteColors = 256;
       _NumBytesPixel    = 1;
       _Depth            = 8;
       break;

    case dmPixelFormat24bppRGB:
      _NumPaletteColors = 0;
      _NumBytesPixel    = 3;
      _Depth            = 24;
      break;

    case dmPixelFormat16bppGrayScale:
      _NumPaletteColors = 0;
      _NumBytesPixel    = 2;
      _Depth            = 16;
      break;

    default :
      dmLOG("ERROR: NativeImage: Unexpected format !\n");
      return false;
  }
 
  _PixelFormat = _Format;

  // Calculate the buffer width in bytes: make sure the buffer width is
  // a multiple of 4
  _RowBytes  = dm_align32(_Width * (LONG)_NumBytesPixel);
  _SizeImage = _RowBytes * (long)_Height;   // Calculate the size of just the image

  // Calculate the amount of memory needed to hold the header,
  // palette, and image data. Calc the size of the palette in bytes

  ULONG palbyts = _NumPaletteColors * sizeof(RGBQUAD);
  ULONG bufsize = sizeof(BITMAPINFOHEADER) + palbyts;

  _BmpHeader = static_cast<LPBITMAPINFOHEADER>(dmMemory::Malloc(bufsize));
  memset(_BmpHeader,0,sizeof(BITMAPINFOHEADER));

  _BmpHeader->biSize          = sizeof(BITMAPINFOHEADER);
  _BmpHeader->biWidth         = _Width;
  _BmpHeader->biHeight        = _Height;
  _BmpHeader->biPlanes        = 1;
  _BmpHeader->biBitCount      = (WORD)_Depth;
  _BmpHeader->biCompression   = BI_RGB;       // No compression
  _BmpHeader->biSizeImage     = _SizeImage;
  _BmpHeader->biXPelsPerMeter = 0;
  _BmpHeader->biYPelsPerMeter = 0;
  _BmpHeader->biClrUsed       = _NumPaletteColors;
  _BmpHeader->biClrImportant  = _NumPaletteColors;


  //_ImageBits will be allocated width <CreateDIBSection>
    
  if(palbyts) 
  {
    _RGBQuad = (RGBQUAD*)(_BmpHeader + 1); //sizeof(BITMAPINFOHEADER));

    RGBQUAD* pQuad = _RGBQuad;
    for(size_t i=0;i<_NumPaletteColors;++i,++pQuad) 
    {
      pQuad->rgbBlue     = i; 
      pQuad->rgbGreen    = i; 
      pQuad->rgbRed      = i; 
      pQuad->rgbReserved = 0; 
    } 
  } else
    _RGBQuad = NULL;

  // Create the DIB section so that we can have a HBITMAP;

  HDC hDC  = ::GetDC(NULL); // Get the screen DC;
  _HBitmap = ::CreateDIBSection(hDC,(const BITMAPINFO*)_BmpHeader,DIB_RGB_COLORS,(void**)&_ImageBits,NULL,0);

  if(_HBitmap==NULL)
    dmLOG("Error in system function <CreateDIBSection> ( error %d )\n",::GetLastError());

  if(hDC) 
   ::ReleaseDC(NULL,hDC);

  return _HBitmap != NULL;
} 
//----------------------------------
dmWinNativeImage::dmWinNativeImage()
:_HBitmap(NULL)
,_BmpHeader(NULL)    
,_RGBQuad(NULL)           
,_PixelFormat(dmPixelFormatUndefined)  
,_ImageBits(NULL)       
,_NumBytesPixel(0)
,_NumPaletteColors(0)  
,_RowBytes(0)       
,_SizeImage(0)
,_Lock(0)       
{
}
//----------------------------------
dmWinNativeImage::~dmWinNativeImage()
{
  if (_HBitmap != NULL)
   ::DeleteObject(_HBitmap);

  if (_BmpHeader != NULL)
    dmMemory::Free(_BmpHeader);
}
//----------------------------------
// LockBits : pixel format must corresponds
// to the format of the image data
//----------------------------------
bool dmWinNativeImage::LockBits( const dmRect& _Rect,EPixelFormat _Format,
                                 dmImageData& _Data, int _LockModes )
{
  if(_ImageBits != NULL && _Lock==0 && _Format==_PixelFormat) 
  {
    if(_Format != _PixelFormat)
      return false;

    if((_LockModes & ELockUserBuffer)!=0) 
    {
      _LockRect = _Rect;
      if(_LockRect.Clip(dmRect(0,0,_BmpHeader->biWidth,_BmpHeader->biHeight)))
      {  
        size_t width       = dm_min(_Data.Width ,static_cast<dm_uint>(_LockRect.Width() ));
        size_t height      = dm_min(_Data.Height,static_cast<dm_uint>(_LockRect.Height()));

        if((_LockModes & ELockRead)!= 0)
        {
          // Copy bitmap data into the the user buffer in _Data
          BYTE* dstScan0     = static_cast<BYTE*>(_Data.Scan0);
          BYTE* bmpScan0     = _ImageBits + _RowBytes * (_BmpHeader->biHeight-1 - _LockRect.Top());
          size_t byteStart   = _LockRect.Left() * _NumBytesPixel;
          size_t bytesToCopy = width * _NumBytesPixel;

          for(int j=height;--j>=0; 
              bmpScan0 -= _RowBytes, 
              dstScan0 += _Data.Stride ) 
          {
            dmMemory::Memcpy(dstScan0,bmpScan0+byteStart,bytesToCopy);
          }
        }
        _Data.PixelFormat = _Format;
        _Lock = _LockModes;
        return true;
      }
    }
    else
    {
      dmRect srcRect(_Rect);
      if(srcRect.Clip(dmRect(0,0,_BmpHeader->biWidth,_BmpHeader->biHeight)))
      {
        size_t byteStart   = _LockRect.Left() * _NumBytesPixel;
        
        _Data.Width       =  srcRect.Width();      
        _Data.Height      =  srcRect.Height();
        _Data.Stride      =  -1 * static_cast<int>(_RowBytes); 
        _Data.Scan0       =  _ImageBits + _RowBytes * (_BmpHeader->biHeight-1 - srcRect.Top()) + byteStart;
        _Data.PixelFormat =  _Format; 

        _Lock = _LockModes;
        return true;
      }
    }
  }
  return false;
}
//----------------------------------
bool dmWinNativeImage::LockBits( EPixelFormat _Format, dmImageData& _Data, 
                                 int _LockModes )
{
  if(_ImageBits != NULL && _Lock==0 && _Format==_PixelFormat) 
  {
    if((_LockModes & ELockUserBuffer)!=0) 
    {
      size_t height = dm_min(_Data.Height,static_cast<dm_uint>(_BmpHeader->biHeight));
      size_t width  = dm_min(_Data.Width ,static_cast<dm_uint>(_BmpHeader->biWidth ));

      if((_LockModes & ELockRead)!= 0)
      {
        // Copy bitmap data into the the user buffer in _Data
        BYTE* dstScan0     = static_cast<BYTE*>(_Data.Scan0);
        BYTE* bmpScan0     = _ImageBits + _RowBytes * (_BmpHeader->biHeight-1);
        size_t bytesToCopy = width * _NumBytesPixel;

        for(int j=height;--j>=0; 
            bmpScan0 -= _RowBytes, 
            dstScan0 += _Data.Stride ) 
        {
          dmMemory::Memcpy(dstScan0,bmpScan0,bytesToCopy);
        }
      }

      _Data.PixelFormat = _Format;
      _LockRect = dmRect(0,0,width,height);
      _Lock     = _LockModes;
      return true;
    }
    else
    {
      _Data.Width       = _BmpHeader->biWidth;      
      _Data.Height      = _BmpHeader->biHeight;
      _Data.Stride      = -1 * static_cast<int>(_RowBytes); 
      _Data.Scan0       = _ImageBits + _RowBytes * (_BmpHeader->biHeight-1);
      _Data.PixelFormat = _Format; 

      _LockRect = dmRect(0,0,_Data.Width,_Data.Height);
      _Lock     = _LockModes;
      return true;
    }
  }
  return false;
}
//----------------------------------
bool dmWinNativeImage::UnlockBits( dmImageData& _Data )
{
  if(_ImageBits != NULL && _Lock!=0 && _PixelFormat==_Data.PixelFormat) 
  {
    // Buffer is enabled && Write is enabled, copy back data to
    // bitmap
    if((_Lock & ELockUserBuffer)!=0 && (_Lock & ELockWrite)!=0) 
    {
      BYTE* srcScan0    = static_cast<BYTE*>(_Data.Scan0);
      BYTE* bmpScan0    = _ImageBits + _RowBytes * (_BmpHeader->biHeight-1 - _LockRect.Top());
      size_t bytesToCopy = dm_min(_Data.Width ,static_cast<dm_uint>(_LockRect.Width()) ) * _NumBytesPixel;
      size_t height      = dm_min(_Data.Height,static_cast<dm_uint>(_LockRect.Height()));
      size_t byteStart   = _LockRect.Left() * _NumBytesPixel;
      for(int j=height;--j>=0; 
          bmpScan0 -= _RowBytes, 
          srcScan0 += _Data.Stride ) 
      {
        dmMemory::Memcpy(bmpScan0+byteStart,srcScan0,bytesToCopy);
      }

      _Lock  = 0;
      _LockRect.Clear();
      return true;
    } 
    else
    {
      // Nothing to do
      _Lock  = 0;
      _LockRect.Clear();
      return true;
    }
  }
  return false;
}
//----------------------------------
// Apply a palette transformation
// according to the daim LUT specifications 
// and color mapping table
//----------------------------------
bool dmWinNativeImage::ApplyLUT( const dmLUT& aLUT )
{
  if(_RGBQuad==NULL) 
    return false;

  RGBQUAD*  pQRGB = _RGBQuad;
  ULONG nColors   = dm_min(_NumPaletteColors,static_cast<size_t>(dmLUT8_MAX_COLORS));

  for(size_t i=0;i<nColors;++i,++pQRGB)
  {
    const dmRGBColor& rgbv = aLUT[i]; 
    pQRGB->rgbBlue  =  rgbv.blue;
    pQRGB->rgbRed   =  rgbv.red;    
    pQRGB->rgbGreen =  rgbv.green;
  }
  return true;
}
//---------------------------------------------------
// Factory for dmNativeImage, used by dmWinGraphics
//---------------------------------------------------
dmWinNativeImageFactory::~dmWinNativeImageFactory()
{}
//----------------------------------
dmNativeImage* dmWinNativeImageFactory::Create( size_t _Width, size_t _Height, 
                                                EPixelFormat _Format )
{
  dmWinNativeImage* _Image = new dmWinNativeImage();  
  if(_Image->CreateBitmap(_Width,_Height,_Format))
    return _Image;

  if(_Image != NULL)
    delete _Image;

  return NULL; 
}
//----------------------------------
dmNativeImage* dmWinNativeImageFactory::Create( const dmImageData& _Data )
{
  dmWinNativeImage* _Image = new dmWinNativeImage();  
  if(_Image->CreateBitmap(_Data.Width,_Data.Height,_Data.PixelFormat)) 
  {
    // Force unlock bits
    _Image->_Lock     = ELockWrite|ELockUserBuffer;
    _Image->_LockRect = dmRect(0,0,_Data.Width,_Data.Height);
    if(_Image->UnlockBits(const_cast<dmImageData&>(_Data)))
      return _Image;
  }

  if(_Image != NULL)
    delete _Image;

  return NULL; 
}
//----------------------------------
