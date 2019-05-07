//--------------------------------------------------------
// Copyright (c) 2004 David Marteau
// 
// File         : dmWinGraphics.h
// Date         : 7/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
// Graphic context based on gdi32 windows library
// This object is meant to be use by tools components
// in order to display  graphics such as line text and so on...
//--------------------------------------------------------
#ifndef dmWinGraphics_h
#define dmWinGraphics_h
//---------------------------------------------------------------------------
class dmWinGraphics : public dmGraphics
{   
  private:
    HWND  mhWnd;          // The default drawable window 
    bool  mFromWindow;    // Set When the graphics object has been allocated
                          // from mhWnd                  
    HDC      mGraphics;    // The Gdi graphic context
    POINT   *mPtsBuffer;   // Buffer to hold points for polygones
    dm_uint  mPtsCount;    // Number of points allocated in mPtsBuffer
    dm_uint  mLocks;       // Lock counter used in ObtainGraphics()/ReleaseGraphics()
    dm_uint  mPageScale;   // Page scale (ie zoom factor) in %
    
    HPEN     mPen;        // Current Pen
    HPEN     mNullPen;    // Null pen
    HFONT    mFont;       // Current Font
    HBRUSH   mSolidBrush; // Current solid brush
    HBRUSH   mHatchBrush; // Current hatch brush

    COLORREF mForeColor;  // Foreground color
    COLORREF mBackColor;  // Background color
    COLORREF mTextColor;  // Text color

    EPenStyle   mLineStyle;  // DAIM Line  Style
    EBrushStyle mBrushStyle; // DAIM Brush Style

    // Special properties applied when setting the graphics object
    // width SetGraphics(..)
    
    bool GDI_GetPtsBuffer( const dm_point*, size_t count );
    void GDI_SetStringFormatAlignement(int& aStringFormat, int alignStyle );
    void GDI_SetFont(const char_t* _Family, EFontStyle, EFontWeight, int _PtSize );
    void GDI_SetBrush( EBrushStyle );
    void GDI_Initialize();

    HFONT  GetFont();
    HBRUSH GetBrush();

    POINT  mLogOffset;
    SIZE   mLogSize;

    POINT  mDevOffset;
    SIZE   mDevSize;

  public:
    static HINSTANCE mhModuleDll;

    HWND GetWindow()    
    { 
       return mhWnd; 
    }

    HDC GetDC() 
    {
       return mGraphics; 
    }


    dm_uint SetPageScale( dm_uint _PageScale )
    { 
      dm_uint _OldPageScale = mPageScale;
      mPageScale = _PageScale;
      return _OldPageScale;
    }

    void SetDisplay(
     dm_int  xoffset,
     dm_int  yoffset,
     dm_uint width,
     dm_uint height);

     const SIZE& GetDisplaySize() const { return mLogSize; }

     bool  UIEnabled;
  
  public:	 
    dmWinGraphics(HWND);
   ~dmWinGraphics();

    bool InitGraphics( HDC );
    HDC  SetGraphics ( HDC );
    void Restore     ( HDC );

    virtual bool ObtainGraphics( dm_uint _Flags = 0 );
    virtual void ReleaseGraphics();

    virtual dm_uint GetPageScale();

    //--------------------------------------------------------------
    // Drawing state functions
    //--------------------------------------------------------------
    virtual void SetPenAttrs ( const dmPenAttributs attrs );
    virtual void SetPen      ( EPenStyle aLineType , int aWidth , EColorType aColor );
    virtual void SetBrush    ( EColorType, EBrushStyle style );
    virtual void SetPenColor ( EColorType aColor );

    //----------------------------------------
    // Drawing functions
    //----------------------------------------
    virtual void DrawLine     ( int x1,int y1,int x2,int y2, int flags );
    virtual void DrawRectangle( int x1,int y1,int x2,int y2, int flags );
    virtual void DrawEllipse  ( int x0,int y0,int rx,int ry, int flags );
    virtual void DrawArc      ( int x0,int y0,int rx,int ry, dm_real startAngle, dm_real endAngle, int flags );
    virtual void DrawPoly     ( const dm_point* , size_t, int flags );

    //--------------------------------------------------------------
    // Text drawing	 
    //--------------------------------------------------------------
    virtual void DrawText    ( const dmString::E*, const dmRect& ,int = eAlignStyle_normal );
    virtual void DrawText    ( const dmString::E*, const dmPoint&,int = eAlignStyle_normal );
    virtual void SetTextColor( EColorType );
    virtual void SetTextAttrs( const dmTextAttributs& );

    virtual void GetTextRect ( const dmString::E*, dmRect& , int = eAlignStyle_normal );

    //--------------------------------------------------------------
    // Image drawing	 
    //--------------------------------------------------------------
    virtual void Display( const dmNativeImage*,const dmPoint& _dest );
    virtual void Display( const dmNativeImage*,const dmRect&  _dest );
    virtual void Display( const dmNativeImage*,const dmPoint& _dest,const dmRect& _src );
    virtual void Display( const dmNativeImage*,const dmRect&  _dest,const dmRect& _src );

    //----------------------------------------
    // Refresh methods
    //----------------------------------------
    virtual void Refresh();
    virtual void Refresh( const dmRect& );

    virtual bool UpdateCursor();

    //----------------------------
    // Image Factory
    //----------------------------
    virtual dmNativeImageFactory* GetImageFactory() const;
    
    virtual bool IsUI() const;
};
//---------------------------------------------------

#endif // dmWinGraphics_h
