#define dmUseKernelImageTemplates
#include "daim_kernel.h"

#include "documents/dmDocFactory.h"
#include "documents/dmDocHandler.h"

#define DAIM_MEAN_AND_LEAN
#include "daim/daim_modules.h"

#define IS_BITMAPCOREHEADER(lpbi) (sizeof(BITMAPCOREHEADER) == *(LPDWORD)lpbi) 
#define IS_BITMAPINFOHEADER(lpbi) (sizeof(BITMAPINFOHEADER) == *(LPDWORD)lpbi) 
#define IS_BITMAPV4HEADER(lpbi)   (sizeof(BITMAPV4HEADER)   == *(LPDWORD)lpbi) 
#define IS_BITMAPV5HEADER(lpbi)   (sizeof(BITMAPV5HEADER)   == *(LPDWORD)lpbi)  


#define BITMAPREDMASK(lpbi)    (((LPBITMAPV4HEADER)lpbi)->bV4RedMask)
#define BITMAPGREENMASK(lpbi)  (((LPBITMAPV4HEADER)lpbi)->bV4GreenMask) 
#define BITMAPBLUEMASK(lpbi)   (((LPBITMAPV4HEADER)lpbi)->bV4BlueMask) 

                                     
//=======================================
// DIB utilities
//=======================================
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4) 

#define DIB_SIZE(lpbi) lpbi->biSize
#define DIB_V4 sizeof(BITMAPV4HEADER)
#define DIB_V5 sizeof(BITMAPV5HEADER)

//---------------------------------------
inline DWORD DIB_NumColors(LPBITMAPINFOHEADER lpbi)
{
  if(lpbi->biClrUsed != 0)
    return lpbi->biClrUsed;

  /* return number of colors based on bits per pixel */
  switch(lpbi->biBitCount)
  {
    case 1 : return 2;
    case 4 : return 16;
    case 8 : return 256;
    default: return 0;
  }
}  
//---------------------------------------
inline DWORD DIB_PaletteSize( LPBITMAPINFOHEADER lpbi )
{
  if(lpbi->biCompression == BI_BITFIELDS && (DIB_SIZE(lpbi) < DIB_V4)) 
     return 3 * sizeof(DWORD);
  else
    return ::DIB_NumColors(lpbi) * sizeof(RGBQUAD);
}
//---------------------------------------
inline BYTE* DIB_FindBits( LPBITMAPINFOHEADER lpbi ) 
{ 
  return reinterpret_cast<BYTE*>(lpbi) + lpbi->biSize  +  DIB_PaletteSize( lpbi );
}
//---------------------------------------
inline DWORD DIB_BytesPerLine( LPBITMAPINFOHEADER lpbi ) 
{
  return WIDTHBYTES(lpbi->biWidth * lpbi->biPlanes * lpbi->biBitCount); 
}
//=======================================
// Clipboard handler
//=======================================

#define DM_CLIPBOARD_PROTOCOL _TXT("clipboard")
//---------------------------------------
class daim_clipboardHandler : public dmDocHandler
{
  public:
    daim_clipboardHandler() {}

    virtual ~daim_clipboardHandler() {}
    virtual int GetAttributs( const dmString::E *_prot, const dmString::E* _args, const dmString::E* _path );
    virtual int OpenDocument( const dmString::E *_prot, const dmString::E* _args, const dmString::E* _path, dmLink<dmImage>& , int& _flags );
    virtual int SaveDocument( const dmString::E *_prot, const dmString::E* _args, const dmString::E* _path, dmImage* );
};
//---------------------------------------
int daim_clipboardHandler::GetAttributs( 
       const dmString::E *_prot, 
       const dmString::E* _args, 
       const dmString::E* _path )
{
  if(_tcscmp(_prot,DM_CLIPBOARD_PROTOCOL)==0) return 0;
  return FDOC_UNKNOWN;
}
//---------------------------------------
int daim_clipboardHandler::OpenDocument( 
                    const dmString::E* _prot, 
                    const dmString::E* _args, 
                    const dmString::E* _path, 
                    dmLink<dmImage>& _img, int& _flags )
{
   int Err = ERR_DOC_NOERROR;

   DWORD dwError = 0; 
   if(::OpenClipboard(NULL))
   {
     HGLOBAL hGlobal = ::GetClipboardData(CF_DIB);
     if(hGlobal!= 0)
     {
       LPBITMAPINFOHEADER lpbi = static_cast<LPBITMAPINFOHEADER>(::GlobalLock(hGlobal));
       if(lpbi!=NULL)
       {
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

               default: Err = ERR_DOC_INVALIDFORMAT;
             } break;

           default : Err = ERR_DOC_INVALIDFORMAT;
         }

         //=======================================
         // Copy bitmap data
         //=======================================
         if(Err==ERR_DOC_NOERROR && !_img.IsNull()) 
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
         } else
           Err = ERR_DOC_IOERROR;
       }
       ::GlobalUnlock(hGlobal);
     } 
     else {
        dwError = ::GetLastError();
        Err = ERR_DOC_IOERROR;
     }
     ::CloseClipboard();
   }
   else { 
     dwError = ::GetLastError();
     Err = ERR_DOC_IOERROR;
   }

   if(dwError)
     dmLOG(_TXT("ERROR:Clipboard:Read error:%d\n"),dwError);             

   return Err;
}
//---------------------------------------
int daim_clipboardHandler::SaveDocument( 
     const dmString::E* _prot, 
     const dmString::E* _args, 
     const dmString::E* _path, dmImage* _img )
{
   if(_img==NULL) 
     return ERR_DOC_BADARGS;

   int Err = ERR_DOC_NOERROR;

   DWORD dwError = 0; 

   LPBITMAPINFOHEADER _BmpHeader; // BITMAPINFOHEADER
   RGBQUAD*           _RGBQuad;   // Palettes entries

   size_t  _NumBytesPixel;     // number of bytes per pixel
   size_t  _NumPaletteColors;  // Number of colors in the palette 256 
   size_t  _RowBytes;          // Number of bytes per row
   size_t  _SizeImage;         // number of bytes

   INT _Depth;

   //=======================================
   // Create the DIB image 
   //=======================================
   dmLink<dmImage> thisImage = dmNewLink<dmImage>(_img);

   EPixelFormat _PixelFormat = thisImage->PixelFormat();

   // Try to convert this image to a displyable image format
   if(!dmIsPixelFormatDisplayable(_PixelFormat))
   {
      if( dmIsPixelFormatScalar(thisImage->PixelFormat()) )
        thisImage = dmCreateCopy(thisImage,dmPixelFormat8bppIndexed);
      else
        thisImage = dmCreateCopy(thisImage,dmPixelFormat24bppRGB);
   }

   dmImageData _Data;
   thisImage->GetImageData(_Data);

   switch( _Data.PixelFormat )
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

     default :
       return ERR_DOC_INVALIDFORMAT;
   }
 

   // Calculate the buffer width in bytes: make sure the buffer width is
   // a multiple of 4
   _RowBytes  = dm_align32(_Data.Width * (LONG)_NumBytesPixel);
   _SizeImage = _RowBytes * (LONG)_Data.Height;   // Calculate the size of the image data

   // Calculate the amount of memory needed to hold the header,
   // palette, and image data. Calc the size of the palette in bytes

   ULONG palbyts = _NumPaletteColors * sizeof(RGBQUAD);
   ULONG bufsize = sizeof(BITMAPINFOHEADER) + palbyts + _SizeImage;

   // Generate handle
   HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,bufsize);

   if (hMem != NULL) 
   {
     _BmpHeader = static_cast<LPBITMAPINFOHEADER>(::GlobalLock(hMem));
     if(_BmpHeader!=NULL) 
     {
       memset(_BmpHeader,0,sizeof(BITMAPINFOHEADER));

       _BmpHeader->biSize          = sizeof(BITMAPINFOHEADER);
       _BmpHeader->biWidth         = _Data.Width;
       _BmpHeader->biHeight        = _Data.Height;
       _BmpHeader->biPlanes        = 1;
       _BmpHeader->biBitCount      = (WORD)_Depth;
       _BmpHeader->biCompression   = BI_RGB;       // No compression
       _BmpHeader->biSizeImage     = _SizeImage;
       _BmpHeader->biXPelsPerMeter = 0;
       _BmpHeader->biYPelsPerMeter = 0;
       _BmpHeader->biClrUsed       = _NumPaletteColors;
       _BmpHeader->biClrImportant  = _NumPaletteColors;
     
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

       // Copy data to DIB
       size_t _RowBytes  = DIB_BytesPerLine(_BmpHeader);
       BYTE*  _ImageBits = DIB_FindBits(_BmpHeader);

       // Copy bitmap data into the the user buffer in _Data
       BYTE* srcScan0 = static_cast<BYTE*>(_Data.Scan0);
       BYTE* bmpScan0 = _ImageBits + _RowBytes * (_BmpHeader->biHeight-1);

       size_t bytesToCopy = dm_min(_Data.Width ,static_cast<dm_uint>(_BmpHeader->biWidth)) * _NumBytesPixel;
       size_t height      = dm_min(_Data.Height,static_cast<dm_uint>(_BmpHeader->biHeight));

       for(int j=height;--j>=0; 
             bmpScan0 -= _RowBytes, 
             srcScan0 += _Data.Stride )
       {
          dmMemory::Memcpy(bmpScan0,srcScan0,bytesToCopy);
       }

       // Unlock
       ::GlobalUnlock(hMem);

       //=======================================
       // Save in windows clipboard 
       //=======================================
       if(::OpenClipboard(NULL))
       {
         ::EmptyClipboard(); // Clear the clipboard first

         if(!::SetClipboardData( CF_DIB, hMem )) {
            dwError = ::GetLastError();
            Err = ERR_DOC_IOERROR;
         }
         ::CloseClipboard();
       }
     } else {
       dwError = ::GetLastError();
       Err = ERR_DOC_IOERROR;
     }
   } 
   else {
     dwError = ::GetLastError();
     Err = ERR_DOC_IOERROR;
   }

   if(dwError)
     dmLOG(_TXT("ERROR:Clipboard:Write error:%d\n"),dwError);

   return Err;
} 
//----------------------------------------------
// Initialisation du gestionnaire du "clipboard"
//----------------------------------------------
void dmRegisterClipboardHandler()
{
  daim_clipboardHandler* pHandler = new daim_clipboardHandler;
  dmSetDocHandler(DM_CLIPBOARD_PROTOCOL,pHandler);
  pHandler->Release();
}
//---------------------------------------
