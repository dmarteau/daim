//--------------------------------------------------------
// Copyright (c) 2004 David Marteau
// 
// File         : dmWinNativeImage.h
// Date         : 7/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------
#ifndef dmWinNativeImage_h
#define dmWinNativeImage_h

#include "daim_kernel/images/dmNativeImage.h"
//----------------------------------
class dmLUT;

class dmWinNativeImage : public dmNativeImage,
                         public dmMemoryObject
{
  friend class dmWinNativeImageFactory;
  protected:
   HBITMAP            _HBitmap;   // the GDI bitmaps
   LPBITMAPINFOHEADER _BmpHeader; // BITMAPINFOHEADER
   RGBQUAD*           _RGBQuad;   // Palettes entries

   EPixelFormat _PixelFormat;       // Pixel formats
   BYTE*        _ImageBits;         // Starting address of DIB bits
   size_t       _NumBytesPixel;     // number of bytes per pixel
   size_t       _NumPaletteColors;  // Number of colors in the palette 256 
   size_t       _RowBytes;          // Number of bytes per row
   size_t       _SizeImage;         // number of bytes

   int          _Lock;
   dmRect       _LockRect;

  public:
   virtual ~dmWinNativeImage();

   dmWinNativeImage();

   bool CreateBitmap( INT _Width, INT _Height, EPixelFormat _Format );

   virtual bool LockBits  ( const dmRect&,EPixelFormat, dmImageData&, int dmNativesLockModes );
   virtual bool LockBits  ( EPixelFormat, dmImageData&, int dmNativesLockModes );
   virtual bool UnlockBits( dmImageData& );
   virtual bool ApplyLUT  ( const dmLUT& aLUT );

   HBITMAP      Bitmap()        { return _HBitmap;   }
   BYTE*        BitmapBits()    { return _ImageBits; }
   LPBITMAPINFO BitmapHeader()  { return reinterpret_cast<LPBITMAPINFO>(_BmpHeader); }
};
//----------------------------------
// Native image factory 
//----------------------------------
class dmWinNativeImageFactory : public dmNativeImageFactory
{
  public:
   virtual ~dmWinNativeImageFactory();
   virtual  dmNativeImage* Create( size_t _Width, size_t _Height, EPixelFormat );
   virtual  dmNativeImage* Create( const dmImageData& );
};
//----------------------------------
#endif // dmWinNativeImage_h
