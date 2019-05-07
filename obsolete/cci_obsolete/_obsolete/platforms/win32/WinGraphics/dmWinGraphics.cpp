#define dmUseKernelImage
#include "daim_tools.h"

#include "dmWinGraphics.h"
#include "dmWinNativeImage.h"

#include "dmResources.h"

#define EMPTY_STRING(lpsz) ((lpsz)==NULL || (lpsz)[0]=='\0')

//--------------------------------------------------------------
// Helper for handling drawing in XOR mode
//--------------------------------------------------------------
struct GDISELECT
{
  HGDIOBJ _old;
  HDC     _hdc;
  GDISELECT( HDC hdc,  HGDIOBJ obj ) : _hdc(hdc) { _old = ::SelectObject(_hdc,obj); }
 ~GDISELECT() { ::SelectObject(_hdc,_old);       }
};
//--------------------------------------------------------------
HINSTANCE dmWinGraphics::mhModuleDll = NULL;
//--------------------------------------------------------------
dmWinGraphics::dmWinGraphics(HWND hWnd)
: mhWnd    (hWnd)
 ,mGraphics(NULL)
{
  GDI_Initialize();
  SetDisplay(0,0,1,1);
}
//--------------------------------------------------------------
void dmWinGraphics::GDI_Initialize()
{
  mFromWindow   = false;
  mPtsBuffer    = NULL;
  mPtsCount     = 0;
  mLocks        = 0;
  
  UIEnabled     = false;
 
  mForeColor = RGB(0,0,0);
  mBackColor = RGB(255,255,255);

  mPen          = ::CreatePen(PS_SOLID,1,mForeColor);
  mSolidBrush   = ::CreateSolidBrush(mBackColor);
  mTextColor    = mForeColor;  
  mFont         = NULL;        
  mHatchBrush   = NULL; 
  mPageScale    = 100;

  mNullPen      = (HPEN)::GetStockObject(NULL_PEN);
}
//--------------------------------------------------------------
#define VERIFY_DELETE( p ) if(p) ::DeleteObject(p);
//--------------------------------------------------------------
dmWinGraphics::~dmWinGraphics()
{
  if(mGraphics && mFromWindow)
    ::ReleaseDC(mhWnd,mGraphics);

  if(mPtsBuffer) 
    delete [] mPtsBuffer; 

  VERIFY_DELETE(mPen)          
  VERIFY_DELETE(mSolidBrush)
  VERIFY_DELETE(mHatchBrush)
  VERIFY_DELETE(mFont)
}
//--------------------------------------------------------------
// Allocate points buffer for operation like "drawpoly"
//--------------------------------------------------------------
bool dmWinGraphics::GDI_GetPtsBuffer( const dm_point* points, size_t count )
{
  if(count > mPtsCount) 
  {
    if(mPtsBuffer) delete [] mPtsBuffer; 
    mPtsBuffer = new POINT[count];
    mPtsCount  = count;
  }

  for(size_t i=0;i<count;++i,++points) {
     mPtsBuffer[i].x = points->x;
     mPtsBuffer[i].y = points->y;
  }

  return count > 0;  
}
//--------------------------------------------------------------
// Set Gdiplus Stringformat 
//--------------------------------------------------------------
void dmWinGraphics::GDI_SetStringFormatAlignement( int& aStringFormat,
                                                   int  alignStyle )
{
  aStringFormat = TA_NOUPDATECP;
  switch(alignStyle & eHAlignMask) {
    case eAlignStyle_right  : aStringFormat |= TA_RIGHT;   break;
    case eAlignStyle_center : aStringFormat |= TA_CENTER;  break;
    default: aStringFormat |= TA_LEFT;
  }
  switch(alignStyle & eVAlignMask) {
    case eAlignStyle_bottom  : aStringFormat |= TA_BOTTOM;  break;
    default: aStringFormat |= TA_TOP;
  }
}
//--------------------------------------------------------------
// Set Brush style and create the corresponding brush
//--------------------------------------------------------------
void dmWinGraphics::GDI_SetBrush( EBrushStyle )
{
  ::DeleteObject(mSolidBrush);	
  mSolidBrush = ::CreateSolidBrush(mBackColor);

  if(mBrushStyle > eBrushStyle_solid) 
  {
    int _HatchStyle;
    switch(mBrushStyle) 
    {
      case eBrushStyle_horizontal   : _HatchStyle = HS_HORIZONTAL; break;
      case eBrushStyle_vertical     : _HatchStyle = HS_VERTICAL;   break;
      case eBrushStyle_cross        : _HatchStyle = HS_CROSS;      break;
      case eBrushStyle_diagBackward : _HatchStyle = HS_BDIAGONAL;  break;
      case eBrushStyle_diagCross    : _HatchStyle = HS_DIAGCROSS;  break; 
      default:
        _HatchStyle = HS_FDIAGONAL;
    }

    if(mHatchBrush!=NULL)
    {
      ::DeleteObject(mHatchBrush);
      mHatchBrush = NULL;
    }
    
    mHatchBrush = ::CreateHatchBrush(_HatchStyle,mForeColor);
  }
}
//--------------------------------------------------------------
// Set Gdi font
//--------------------------------------------------------------
void dmWinGraphics::GDI_SetFont(const char_t* _Family, 
                                EFontStyle  _FontStyle ,
                                EFontWeight _FontWeight, 
                                int _PtSize ) 
{
  // Delete previous font
  if(mFont!=NULL) {
    ::DeleteObject(mFont);
    mFont = NULL;
  }

  if(EMPTY_STRING(_Family)) 
    _Family = "Arial";

  LONG yPerInch = ::GetDeviceCaps(mGraphics,LOGPIXELSY);

  int fnHeight = -(int)((_PtSize * yPerInch) / 72.0 + 0.5);
  int fnWeight = FW_NORMAL;

  DWORD fdwItalic = FALSE; 

  switch(_FontStyle) {
    case eFontStyle_italic  : 
    case eFontStyle_oblique : fdwItalic = TRUE; break;
    default: break;
  } 
    
  switch(_FontWeight) {
    case eFontWeight_normal : fnWeight = FW_NORMAL;    break;
    case eFontWeight_bold   : fnWeight = FW_BOLD;      break;
    case eFontWeight_bolder : fnWeight = FW_EXTRABOLD; break;
    case eFontWeight_lighter: fnWeight = FW_LIGHT;     break;
    default: 
      fnWeight = _FontWeight;
      break;
  }

  if(mFont) ::DeleteObject(mFont);
  mFont = ::CreateFont(
    fnHeight,           // logical height of font
    0,                  // logical average character width
    0,                  // angle of escapement
    0,                  // base-line orientation angle
    _FontWeight,        // font weight
    fdwItalic,          // italic attribute flag
    FALSE,              // underline attribute flag
    FALSE,              // strikeout attribute flag
    DEFAULT_CHARSET,    // character set identifier
    OUT_DEFAULT_PRECIS, // output precision
    CLIP_DEFAULT_PRECIS,// clipping precision
    DEFAULT_QUALITY,    // output quality
    DEFAULT_PITCH,      // pitch and family
    _Family             // pointer to typeface name string
  );
}
//--------------------------------------------------------------
void dmWinGraphics::SetDisplay(
     dm_int  xoffset,
     dm_int  yoffset,
     dm_uint width,
     dm_uint height)
{
   mLogOffset.x = xoffset;
   mLogOffset.y = yoffset;
   mLogSize.cx  = width;
   mLogSize.cy  = height;  

   // Setup default Viewport extent to be conversion of Window extent
   // into device units.

   HDC hdc = ::GetWindowDC(NULL);
   if(hdc!=NULL) 
   {
     ::SetMapMode(hdc,MM_ANISOTROPIC); // Need for arbitrary scaling
      
     mDevSize   = mLogSize;
     mDevOffset = mLogOffset;

    ::LPtoDP(hdc,(LPPOINT)&mDevOffset,1);
    ::LPtoDP(hdc,(LPPOINT)&mDevSize  ,1);

    ::ReleaseDC(NULL,hdc);
   }
}
//--------------------------------------------------------------
HFONT dmWinGraphics::GetFont()
{
  if(mFont==NULL)
   GDI_SetFont(NULL,eFontStyle_normal,eFontWeight_normal,12);

  return mFont;
}
//--------------------------------------------------------------
HBRUSH dmWinGraphics::GetBrush()
{
  if(mBrushStyle > eBrushStyle_solid && mHatchBrush!=NULL) 
   return mHatchBrush;

  return mSolidBrush;
}
//--------------------------------------------------------------
bool dmWinGraphics::InitGraphics( HDC _Graphics )
{
  // Apply the current options

  // Set PageScale
  if(mPageScale!=100 &&
     mLogSize.cx>1   &&
     mLogSize.cy>1 ) 
  {
    SIZE vp_Size;

    ::SetMapMode(_Graphics,MM_ANISOTROPIC);
	  ::SetWindowExtEx(_Graphics,mLogSize.cx,mLogSize.cy,&vp_Size);
    ::SetViewportExtEx(_Graphics,
           static_cast<int>((mDevSize.cx * mPageScale)/100 + 0.5),
           static_cast<int>((mDevSize.cy * mPageScale)/100 + 0.5),
           &vp_Size);
  }

  return true;
}
//--------------------------------------------------------------
HDC dmWinGraphics::SetGraphics( HDC _Graphics )
{
  HDC OldGraphics = mGraphics;
  mGraphics = _Graphics;

  if(mGraphics!=NULL) 
    InitGraphics(mGraphics);

  return OldGraphics;
}
//--------------------------------------------------------------
void dmWinGraphics::Restore( HDC _Graphics ) 
{
  mGraphics = _Graphics;
}
//--------------------------------------------------------------
bool dmWinGraphics::ObtainGraphics( dm_uint _Flags )  
{ 
  // No graphics available
  // Create one based on the current window
  if(mGraphics == NULL) 
  {
    dmDEBUG_ASSERT(mLocks==0)

    if(mhWnd) {
      mGraphics = ::GetDC(mhWnd);
      mFromWindow = true;
      SetGraphics(mGraphics);
    }
    else
      return false;
  }

  ++mLocks;   // Increment the lock counter
  return true;
}
//--------------------------------------------------------------
void dmWinGraphics::ReleaseGraphics() 
{
  dmDEBUG_ASSERT(mLocks>0)

  if(--mLocks<=0) 
  {  
    if(mFromWindow)
    {
      dmDEBUG_ASSERT( mGraphics != NULL );
      ::ReleaseDC( mhWnd, mGraphics );
      mGraphics   = NULL;
      mFromWindow = false;
    }
    mLocks = 0;
  }
}
//--------------------------------------------------------------
dm_uint dmWinGraphics::GetPageScale() 
{
  return mPageScale; 
}
//--------------------------------------------------------------
void dmWinGraphics::SetPenAttrs ( const dmPenAttributs attrs )
{
  dmWinGraphics::SetPen(attrs.LineType(),attrs.PenWidth(),attrs.PenColor());
  dmWinGraphics::SetBrush(attrs.BackColor(),attrs.BrushStyle());

  mForeColor = attrs.ForeColor();
}
//--------------------------------------------------------------
void dmWinGraphics::SetPen( EPenStyle aLineType , int aWidth , EColorType aColor )
{
  mLineStyle = aLineType;
  int penStyle = 0;

  switch(aLineType) {
    case ePenStyle_dashed : penStyle = PS_DASH ; break;
    case ePenStyle_dotted : penStyle = PS_DOT  ; break;
    case ePenStyle_solid  : penStyle = PS_SOLID; break;
    default:
      penStyle =  ePenStyle_none;
      break;
  }

  if(aColor==eClr_NONE)
    aLineType = ePenStyle_none;

  if(mPen)
    ::DeleteObject(mPen);

  mPen = ::CreatePen(penStyle,aWidth,aColor);
}	
//--------------------------------------------------------------
void dmWinGraphics::SetBrush( EColorType aColor, EBrushStyle aBrushStyle )
{
  if(aColor==eClr_NONE)
   aBrushStyle = eBrushStyle_null;

  mBrushStyle = aBrushStyle;
  mBackColor  = aColor;

  GDI_SetBrush(aBrushStyle);
}
//--------------------------------------------------------------
void dmWinGraphics::SetPenColor ( EColorType aColor )
{
  LOGPEN logpen;
  if(mPen) {
    ::GetObject(mPen,sizeof(LOGPEN),&logpen);
    ::DeleteObject(mPen); 
  }
  mPen = ::CreatePen( logpen.lopnStyle,logpen.lopnWidth.x, aColor );
}
//----------------------------------------
// Drawing functions
//----------------------------------------
#define PENSTYLE()   ((flags & eDrawBorder) && mLineStyle !=ePenStyle_none)
#define BRUSHSTYLE() ((flags & eDrawFill  ) && mBrushStyle!=eBrushStyle_null)
#define LAYER_OP()   ((flags & eDrawLayer ) ? R2_XORPEN : R2_COPYPEN)
//----------------------------------------
void dmWinGraphics::DrawLine( int x1,int y1,int x2, int y2,  int flags  )
{
  if(mGraphics) {
    GDISELECT Pe(mGraphics,mPen);
    ::SetROP2 (mGraphics,LAYER_OP());
    ::MoveToEx(mGraphics,x1,y1,NULL);
    ::LineTo  (mGraphics,x2,y2);
  }
}
//----------------------------------------
void dmWinGraphics::DrawRectangle( int x1,int y1,int x2,int y2,int flags )
{
  if(mGraphics)
  {
    ::SetROP2(mGraphics,LAYER_OP());

    if(BRUSHSTYLE())
    {
      GDISELECT Pe(mGraphics,PENSTYLE() ? mPen : mNullPen);
      GDISELECT Br(mGraphics,GetBrush());
      ::Rectangle(mGraphics,x1,y1,x2,y2);
    }
    else
    if(PENSTYLE())  
    {
      GDISELECT Pe(mGraphics,mPen);
      ::MoveToEx(mGraphics,x1,y1,NULL);
      ::LineTo(mGraphics,x2,y1);
      ::LineTo(mGraphics,x2,y2);
      ::LineTo(mGraphics,x1,y2); 
      ::LineTo(mGraphics,x1,y1);
    }
  }
}  
//----------------------------------------
void dmWinGraphics::DrawEllipse( int x0,int y0,int rx, int ry, int flags )
{
  if(mGraphics) 
  {
    ::SetROP2(mGraphics,LAYER_OP());

    if(BRUSHSTYLE())
    {
      GDISELECT Pe(mGraphics,PENSTYLE() ? mPen : mNullPen);
      GDISELECT Br(mGraphics,GetBrush());
      ::Ellipse(mGraphics,x0-rx,y0-ry,x0+rx,y0+ry);
    } 
    else
    if(PENSTYLE()) 
    {
      GDISELECT Pe(mGraphics,mPen);
      ::Arc(mGraphics,x0 - rx,y0 - ry,
                      x0 + rx,y0 + ry,
                      x0 - rx,y0,
                      x0 - rx,y0 );
    }
  }
}
//----------------------------------------
void dmWinGraphics::DrawArc( int x0,int y0,int rx,int ry, 
                             dm_real startAngle, dm_real endAngle, 
                             int flags )
{
  if(mGraphics) 
  {
    int sx, sy, ex, ey;

    dm_double distance;

    // figure out the the coordinates of the arc from the angle
    distance = sqrt(static_cast<dm_double>(rx * rx + ry * ry));

    sx = static_cast<int>(distance * cos(startAngle) + x0);
    sy = static_cast<int>(y0 - distance * sin(startAngle));

    ex = static_cast<int>(distance * cos(endAngle) + x0);
    ey = static_cast<int>(y0 - distance * sin(endAngle));

    ::SetROP2(mGraphics,LAYER_OP());

    // Compare angles to get the drawing dir

    if(endAngle > startAngle) 
       ::SetArcDirection(mGraphics, AD_COUNTERCLOCKWISE);
    else
       ::SetArcDirection(mGraphics, AD_CLOCKWISE);
       
    if(BRUSHSTYLE())
    {
      GDISELECT Pe(mGraphics,PENSTYLE() ? mPen : mNullPen);
      GDISELECT Br(mGraphics,GetBrush());

      ::Pie(mGraphics,x0-rx,y0-ry,x0+rx,y0+ry,sx,sy,ex,ey);
    } 
    else
    if(PENSTYLE()) 
    {
      GDISELECT Pe(mGraphics,mPen);

      ::Arc(mGraphics,x0-rx,y0-ry,x0+rx,y0+ry,sx,sy,ex,ey);
    }
  }  
}
//----------------------------------------
void dmWinGraphics::DrawPoly( const dm_point* points, size_t count, int flags )
{
  if(mGraphics && count>0) 
  {
    ::SetROP2(mGraphics,LAYER_OP());

    if(BRUSHSTYLE())
    {
      if(GDI_GetPtsBuffer(points,count)) {
        GDISELECT Pe(mGraphics,PENSTYLE() ? mPen : mNullPen);
        GDISELECT Br(mGraphics,GetBrush());
        ::Polygon(mGraphics,mPtsBuffer,count);
      }
    }
    else
    if(PENSTYLE()) 
    {
      GDISELECT Pe(mGraphics,mPen);
      ::MoveToEx(mGraphics,points->x,points->y,NULL);
      ++points;
      for(size_t i=1;i<count;++i,++points)
      ::LineTo(mGraphics,points->x,points->y);
    }
  }
}
//--------------------------------------------------------------
// Text drawing	 
//--------------------------------------------------------------
void dmWinGraphics::SetTextAttrs( const dmTextAttributs& attrs )
{
  SetTextColor(attrs.Color());
  GDI_SetFont(attrs.Family(),attrs.Style(),attrs.Weight(),attrs.Size());
}
//--------------------------------------------------------------
// Here is a problem, because windows api function DrawText()
// does not work with all devices context (may be corrected in 2000/XP ?)
//--------------------------------------------------------------
void dmWinGraphics::DrawText( const dmString::E* text, const dmRect& rect, int alignStyle )
{
  if(mGraphics && !EMPTY_STRING(text) ) 
  {
    GetFont();

    int aStringFormat = DT_EXPANDTABS;
    switch(alignStyle & eHAlignMask) {
      case eAlignStyle_right  : aStringFormat |= DT_RIGHT;   break;
      case eAlignStyle_center : aStringFormat |= DT_CENTER;  break;
      default: aStringFormat |= DT_LEFT;
    }

    switch(alignStyle & eVAlignMask) {
      case eAlignStyle_bottom  : aStringFormat |= DT_BOTTOM;  break;
      case eAlignStyle_vcenter : aStringFormat |= DT_VCENTER; break;
      default: aStringFormat |= DT_TOP;
    }

    GDISELECT Fnt(mGraphics,mFont);
    RECT rct;
    ::SetRect(&rct,rect.Left(),rect.Top(),rect.Right(),rect.Bottom());
    ::DrawText(mGraphics,text,-1,&rct,aStringFormat);
  }
}
//--------------------------------------------------------------
void dmWinGraphics::DrawText( const dmString::E* text, const dmPoint& p, int alignStyle )
{
  if(mGraphics && !EMPTY_STRING(text) )
  {
    GetFont();

    COLORREF oldFgColor = ::SetTextColor(mGraphics,mTextColor);
    int      oldBkMode  = ::SetBkMode   (mGraphics,TRANSPARENT);

    int      textAlign;
    GDI_SetStringFormatAlignement(textAlign,alignStyle);
    textAlign = ::SetTextAlign(mGraphics,textAlign);

    GDISELECT Fnt(mGraphics,mFont);

    ::TextOut(mGraphics,p.x,p.y,text,_tcslen(text));

    ::SetTextAlign(mGraphics, textAlign);
    ::SetTextColor(mGraphics, oldFgColor);
    ::SetBkMode   (mGraphics, oldBkMode);
  }
}
//--------------------------------------------------------------
void dmWinGraphics::GetTextRect ( const dmString::E* text, dmRect& rect, int alignStyle )
{
  if(EMPTY_STRING(text)) {
    rect.Clear();
    return;
  }

  if(ObtainGraphics()) 
  {
    GetFont();

    int aStringFormat = DT_CALCRECT|DT_EXPANDTABS;

    switch(alignStyle & eHAlignMask) {
      case eAlignStyle_right  : aStringFormat |= DT_RIGHT;   break;
      case eAlignStyle_center : aStringFormat |= DT_CENTER;  break;
      default: aStringFormat |= DT_LEFT;
    }

    switch(alignStyle & eVAlignMask) {
      case eAlignStyle_bottom  : aStringFormat |= DT_BOTTOM;  break;
      case eAlignStyle_vcenter : aStringFormat |= DT_VCENTER; break;
      default: aStringFormat |= DT_TOP;
    }

    GDISELECT Fnt(mGraphics,mFont);
    RECT rct;

    if(::DrawText(mGraphics,text,-1,&rct,aStringFormat)>0) {
       rect = dmRect(rct.top,rct.left,
                     rct.right  - rct.left + 1,
                     rct.bottom - rct.top  + 1 );
    }

    ReleaseGraphics();
  }
}
//--------------------------------------------------------------
void dmWinGraphics::SetTextColor( EColorType aColor )
{
  // May be something to do with clr_None
  mTextColor = aColor;
}
//--------------------------------------------------------------
// Image drawing	 
//--------------------------------------------------------------
#define WIN_GETNATIVE( _Img ) const_cast<dmWinNativeImage*>(\
                                  static_cast<const dmWinNativeImage*>(_Img))
//--------------------------------------------------------------
void dmWinGraphics::Display( const dmNativeImage* _Image, 
                             const dmPoint& _Dest )
{
  if(mGraphics && _Image!=NULL)
  {
    dmWinNativeImage*  _WinImage = WIN_GETNATIVE(_Image);
    LPBITMAPINFO bmi = _WinImage->BitmapHeader();

    ::SetStretchBltMode( mGraphics, STRETCH_DELETESCANS ) ;
    ::StretchDIBits(mGraphics,
         _Dest.x,_Dest.y,
         bmi->bmiHeader.biWidth,
         bmi->bmiHeader.biHeight,
         0 ,0,
         bmi->bmiHeader.biWidth,
         bmi->bmiHeader.biHeight,
         _WinImage->BitmapBits(),
         bmi,DIB_RGB_COLORS,SRCCOPY
    );
  }
}
//--------------------------------------------------------------
void dmWinGraphics::Display( const dmNativeImage* _Image, 
                             const dmPoint& _Dest, 
                             const dmRect&  _Src ) 
{
  if(mGraphics && _Image!=NULL)
  {
    dmWinNativeImage*  _WinImage = WIN_GETNATIVE(_Image);
    LPBITMAPINFO bmi = _WinImage->BitmapHeader();

    ::SetStretchBltMode( mGraphics, STRETCH_DELETESCANS ) ;
    ::StretchDIBits(mGraphics,
         _Dest.x,_Dest.y,_Src.Width(),_Src.Height(),
         _Src.Left() ,bmi->bmiHeader.biHeight - (_Src.Top() + _Src.Height()),
         _Src.Width(),_Src.Height(),
         _WinImage->BitmapBits(),
         bmi,DIB_RGB_COLORS,SRCCOPY
    );
  }
}
//--------------------------------------------------------------
void dmWinGraphics::Display( const dmNativeImage* _Image, 
                             const dmRect& _Dest )
{
  if(mGraphics && _Image!=NULL)
  {
    dmWinNativeImage*  _WinImage = WIN_GETNATIVE(_Image);
    LPBITMAPINFO bmi = _WinImage->BitmapHeader();

    ::SetStretchBltMode( mGraphics, STRETCH_DELETESCANS ) ;
    ::StretchDIBits(mGraphics,
         _Dest.Left(),_Dest.Top(),_Dest.Width(),_Dest.Height(),
         0 ,0,
         bmi->bmiHeader.biWidth,
         bmi->bmiHeader.biHeight,
         _WinImage->BitmapBits(),
         bmi,DIB_RGB_COLORS,SRCCOPY
    );
  } 
}
//--------------------------------------------------------------
// Display _Src part of the image into _Dest rect
//--------------------------------------------------------------
void dmWinGraphics::Display( const dmNativeImage* _Image, 
                             const dmRect& _Dest, 
                             const dmRect& _Src )  
{
  if(mGraphics && _Image!=NULL)
  {
    dmWinNativeImage*  _WinImage = WIN_GETNATIVE(_Image);
    LPBITMAPINFO bmi = _WinImage->BitmapHeader();

    ::SetStretchBltMode( mGraphics, STRETCH_DELETESCANS ) ;
    ::StretchDIBits(mGraphics,
         _Dest.Left(),_Dest.Top(),_Dest.Width(),_Dest.Height(),
         _Src.Left() ,bmi->bmiHeader.biHeight - (_Src.Top() + _Src.Height()),
         _Src.Width(),_Src.Height(),
         _WinImage->BitmapBits(),
         bmi,DIB_RGB_COLORS,SRCCOPY
    );
  } 
}
//----------------------------------------
// Redessine la region rect
//----------------------------------------
void dmWinGraphics::Refresh() 
{
  if(mhWnd)
    ::RedrawWindow(mhWnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE);
}
//----------------------------------------
void dmWinGraphics::Refresh( const dmRect& rect )
{
 if(mhWnd) 
 {
   RECT r = { rect.Left()-1, rect.Top()-1, rect.Right()+1, rect.Bottom()+1 };

   if(mPageScale!=100 &&
      mLogSize.cx>1   &&
      mLogSize.cy>1 ) 
   {
     HDC hdc = ::GetWindowDC(mhWnd);
     if(hdc) {
       InitGraphics(hdc);
       ::LPtoDP(hdc,(LPPOINT)&r,2);
       ::ReleaseDC(mhWnd,hdc);
     }
   }

   ::RedrawWindow(mhWnd,&r,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE); 
 }
}
//------------------------------------------------------
// Define IDC_HAND as defined in winuser.h
//------------------------------------------------------
#ifndef IDC_HAND
#define IDC_HAND  MAKEINTRESOURCE(32649)
#endif
//---------------------------------------------------
bool dmWinGraphics::UpdateCursor()
{ 
    HCURSOR newCursor = NULL;
    switch(mCursor) 
    {
      case eCursor_select:
        newCursor = ::LoadCursor(NULL, IDC_IBEAM);
        break;
        
      case eCursor_wait:
        newCursor = ::LoadCursor(NULL, IDC_WAIT);
        break;

      case eCursor_hyperlink: {
        newCursor = ::LoadCursor(NULL, IDC_HAND);
        if (!newCursor) {
          newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_SELECTANCHOR));
        }
        break;
      }

      case eCursor_standard:
        newCursor = ::LoadCursor(NULL, IDC_ARROW);
        break;

      case eCursor_sizeWE:
        newCursor = ::LoadCursor(NULL, IDC_SIZEWE);
        break;

      case eCursor_sizeNS:
        newCursor = ::LoadCursor(NULL, IDC_SIZENS);
        break;

      case eCursor_sizeNW:
      case eCursor_sizeSE:
        newCursor = ::LoadCursor(NULL, IDC_SIZENWSE);
        break;

      case eCursor_sizeNE:
      case eCursor_sizeSW:
        newCursor = ::LoadCursor(NULL, IDC_SIZENESW);
        break;

      case eCursor_arrow_north:
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_ARROWNORTH));
        break;

      case eCursor_arrow_north_plus:
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_ARROWNORTHPLUS));
        break;

      case eCursor_arrow_south:
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_ARROWSOUTH));
        break;

      case eCursor_arrow_south_plus:
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_ARROWSOUTHPLUS));
        break;

      case eCursor_arrow_east:
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_ARROWEAST));
        break;

      case eCursor_arrow_east_plus:
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_ARROWEASTPLUS));
        break;

      case eCursor_arrow_west:
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_ARROWWEST));
        break;

      case eCursor_arrow_west_plus:
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_ARROWWESTPLUS));
        break;

      case eCursor_crosshair:
        newCursor = ::LoadCursor(NULL, IDC_CROSS);
        break;
                 
      case eCursor_move:
        newCursor = ::LoadCursor(NULL, IDC_SIZEALL);
        break;

      case eCursor_help:
        newCursor = ::LoadCursor(NULL, IDC_HELP);
        break;

      case eCursor_copy:
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_COPY));
        break;

      case eCursor_alias:
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_ALIAS));
        break;

      case eCursor_cell:
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_CELL));
        break;

      case eCursor_grab:
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_GRAB));
        break;

      case eCursor_grabbing:
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_GRABBING));
        break;

      case eCursor_spinning:
        newCursor = ::LoadCursor(NULL, IDC_APPSTARTING);
        break;

      case eCursor_context_menu:
      case eCursor_count_up:
      case eCursor_count_down:
      case eCursor_count_up_down:
        break;

      case eCursor_zoom_in:
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_ZOOMIN));
        break;

      case eCursor_zoom_out:
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_ZOOMOUT));
        break;

      // non standard cursors

      case eDMCursor_roi:     
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_DMROI));
        break;

      case eDMCursor_roi_plus:     
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_DMROIPLUS));
        break;

      case eDMCursor_roi_minus:     
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_DMROIMINUS));
        break;

      case eDMCursor_masks:     
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_DMMASKS));
        break;

      case eDMCursor_cut:     
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_DMCUT));
        break;

      case eDMCursor_colorpicker:     
        newCursor = ::LoadCursor(mhModuleDll, MAKEINTRESOURCE(IDC_DMCOLORPICKER));
        break;

      // Use arrow as default cursor
      default:
        newCursor = ::LoadCursor(NULL, IDC_ARROW);
        break;

    } // switch(aCursor) 

    if(NULL != newCursor) {
      //HCURSOR oldCursor = ::SetCursor(newCursor);
      ::SetCursor(newCursor);
      return true;
    }

    return false;
}
//---------------------------------------------------
// Image Factory
//---------------------------------------------------
dmNativeImageFactory* dmWinGraphics::GetImageFactory() const 
{ 
  static dmWinNativeImageFactory __NativeFactory;
  return &__NativeFactory;
}
//------------------------------------------------------
bool dmWinGraphics::IsUI() const
{
  return UIEnabled; 
} 
//---------------------------------------------------
