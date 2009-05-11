//=======================================
// DIB utilities
//=======================================
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4) 
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
  return ::DIB_NumColors(lpbi) * sizeof(RGBQUAD);
}
//---------------------------------------
inline BYTE* DIB_FindBits( LPBITMAPINFOHEADER lpbi ) 
{ 
  return reinterpret_cast<BYTE*>(lpbi) + lpbi->biSize + DIB_PaletteSize( lpbi ); 
}
//---------------------------------------
inline DWORD DIB_BytesPerLine( LPBITMAPINFOHEADER lpbi ) 
{
  return WIDTHBYTES(lpbi->biWidth * lpbi->biPlanes * lpbi->biBitCount); 
}
//---------------------------------------
inline DWORD DIB_BitmapSize( LPBITMAPINFOHEADER lpbi ) 
{
  return WIDTHBYTES(lpbi->biWidth * lpbi->biPlanes * lpbi->biBitCount) * lpbi->biHeight; 
}
//---------------------------------------
inline EPixelFormat DIB_Format(LPBITMAPINFOHEADER lpbi)
{
  /* return number of colors based on bits per pixel */
  switch(lpbi->biBitCount)
  {
    case 8  : return dmPixelFormat8bppIndexed;
    case 24 : return dmPixelFormat24bppRGB;
  }

  return dmPixelFormatUndefined;
}
//---------------------------------------
