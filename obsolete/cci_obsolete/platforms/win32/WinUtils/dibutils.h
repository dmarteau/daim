#ifndef dibUtilsH
#define dibUtilsH


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
//---------------------------------------
inline DWORD DIB_GetImageData( LPBITMAPINFOHEADER lpbi ) 
{
  return WIDTHBYTES(lpbi->biWidth * lpbi->biPlanes * lpbi->biBitCount); 
}
//---------------------------------------
#ifdef DM_USE_DIB_COMPATIBILIY

EPixelFormat    DIB_IsCompatibleImage     ( LPBITMAPINFOHEADER lpbi );
dmLink<dmImage> DIB_CreateCompatibleImage ( LPBITMAPINFOHEADER lpbi );
BOOL            DIB_GetCompatibleImageData( LPBITMAPINFOHEADER lpbi, 
                                            dmImageData& _Data );

#endif
//---------------------------------------

#endif // dibUtilsH
