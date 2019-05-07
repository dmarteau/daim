#define dmUseKernelImageTemplates
#include "daim_kernel.h"

#define DM_USE_DIB_COMPATIBILIY
#include "dibUtils.h"

//----------------------------------------------------------------------
EPixelFormat DIB_IsCompatibleImage( LPBITMAPINFOHEADER lpbi )
{
  if(lpbi!=NULL)
  {
    switch(lpbi->biBitCount)
    {
      case 8  : return dmPixelFormat8bppIndexed;
      case 16 : return dmPixelFormat16bppGrayScale;
      case 24 : return dmPixelFormat24bppRGB;
    }
  }
  return dmPixelFormatUndefined;
}
//----------------------------------------------------------------------
BOOL DIB_GetCompatibleImageData( LPBITMAPINFOHEADER lpbi, dmImageData& _Data )
{
  EPixelFormat _Format = DIB_IsCompatibleImage(lpbi);
  if(_Format != dmPixelFormatUndefined)
  {
     size_t _RowBytes  = DIB_BytesPerLine(lpbi);
     BYTE*  _ImageBits = DIB_FindBits(lpbi);
    
    _Data.Width       = lpbi->biWidth;      
    _Data.Height      = lpbi->biHeight;
    _Data.Stride      = -1 * static_cast<int>(_RowBytes); 
    _Data.Scan0       = _ImageBits + _RowBytes * (lpbi->biHeight-1);
    _Data.PixelFormat = _Format; 

    return TRUE;
  }
  return FALSE;
}
//----------------------------------------------------------------------
dmLink<dmImage> DIB_CreateCompatibleImage( LPBITMAPINFOHEADER lpbi )
{
  if(lpbi!=NULL)
  {
    dmLink<dmImage> _img;
    
    switch(lpbi->biBitCount)
    {
      case 8  : _img = dmCreateImage<dmPixelFormat8bppIndexed>   (lpbi->biWidth,lpbi->biHeight); break;
      case 16 : _img = dmCreateImage<dmPixelFormat16bppGrayScale>(lpbi->biWidth,lpbi->biHeight); break;
      case 24 : _img = dmCreateImage<dmPixelFormat24bppRGB>      (lpbi->biWidth,lpbi->biHeight); break;
      case 32 : // This is a 32 bit RGB image
         switch(lpbi->biCompression) 
         {
           case BI_BITFIELDS:
             _img = dmCreateImage<dmPixelFormat24bppRGB>(lpbi->biWidth,lpbi->biHeight); break;
             break;
         } break;

      default : break;
    }

    //=======================================
    // Copy bitmap data
    //=======================================
    if(!_img.IsNull()) 
    {
      dmImageData _Data;
      _img->GetImageData(_Data);
  
      size_t _NumBytesPixel = dmGetPixelFormatBits(_Data.PixelFormat) >> 3;
      size_t _RowBytes      = DIB_BytesPerLine(lpbi);

      BYTE* _ImageBits = DIB_FindBits(lpbi);

      // Copy bitmap data into the user buffer in _Data
      BYTE* dstScan0 = static_cast<BYTE*>(_Data.Scan0);
      BYTE* bmpScan0 = _ImageBits + _RowBytes * (lpbi->biHeight-1);

      size_t height = dm_min(_Data.Height,static_cast<dm_uint>(lpbi->biHeight));
      size_t width  = dm_min(_Data.Width ,static_cast<dm_uint>(lpbi->biWidth ));

      if(lpbi->biCompression == BI_BITFIELDS && _Data.PixelFormat == dmPixelFormat24bppRGB)
      {
         // find masks 
         DWORD rMask,gMask,bMask;

         rMask = BITMAPREDMASK(lpbi);
         gMask = BITMAPGREENMASK(lpbi); 
         bMask = BITMAPBLUEMASK(lpbi);

         for(int j=height;--j>=0;
             bmpScan0 -= _RowBytes, 
             dstScan0 += _Data.Stride)
         {
           dmRGBColor* pDst = (dmRGBColor*)dstScan0;
           DWORD*      pSrc = (DWORD*)bmpScan0;

           for(int i=width;--i>=0;++pDst,++pSrc) 
           {
             pDst->red   = static_cast<BYTE>((rMask & *pSrc) >> 16);
             pDst->green = static_cast<BYTE>((gMask & *pSrc) >> 8); 
             pDst->blue  = static_cast<BYTE>((bMask & *pSrc));                
           }
         }          
      }
      else
      {
        size_t bytesToCopy = width * _NumBytesPixel;

        for(int j=height;--j>=0; 
              bmpScan0 -= _RowBytes, 
              dstScan0 += _Data.Stride )
        {
          dmMemory::Memcpy(dstScan0,bmpScan0,bytesToCopy);
        }
      }
      
      return _img;
    }
  }
  
  return dmNewLink<dmImage>(NULL);
}
