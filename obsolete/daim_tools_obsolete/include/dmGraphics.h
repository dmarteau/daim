#ifndef dmGraphics_h
#define dmGraphics_h

/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2005 David Marteau
 *
 * This file is part of the DAIM Image Processing Library (DAIM library)
 *
 * The DAIM library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License (LGPL)
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * The DAIM library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the DAIM library; see the file LICENSE.  If
 * not, write to the Free Software Foundation, Inc., 59 Temple Place -
 * Suite 330, Boston, MA 02111-1307, USA.
 *
 *  ::: END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : dmGraphics.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

namespace dmTk {

enum EDrawFlags {
  eDrawFill    = 0x0001,
  eDrawBorder  = 0x0002,

  eDrawLayer   = 0x0100, 
  eDrawErase   = 0x0200,

  eDrawDefault    = eDrawFill  | eDrawBorder,
  eDrawLayerErase = eDrawErase | eDrawLayer,
};

}; // namespace dmTk

//---------------------------------------------------------------------------
// Pen Attributs
//---------------------------------------------------------------------------
class __dmTools dmPenAttributs
{
  private:
    EColorType   mPenColor;
    EColorType   mForeColor;
    EColorType   mBackColor;
    EPenStyle    mLineType;
    EBrushStyle  mBrushStyle;
    int          mPenWidth;

  public:
    dmPenAttributs();
    dmPenAttributs( const dmPenAttributs& _attrs );
    dmPenAttributs( EColorType   _PenColor,
                    EColorType   _BackColor, int _Width,
                    EPenStyle    _PenStyle, 
                    EBrushStyle  _BrushStyle  = eBrushStyle_null
                  );

    dmPenAttributs& operator=(const dmPenAttributs& _attrs);

    EColorType   PenColor()    const { return mPenColor;    }
    EColorType   ForeColor()   const { return mForeColor;   }
    EColorType   BackColor()   const { return mBackColor;   }
    EPenStyle    LineType()    const { return mLineType;    }
    EBrushStyle  BrushStyle()  const { return mBrushStyle;  }
    int          PenWidth()    const { return mPenWidth;    }

    void SetPenWidth   (int aWidth)         { mPenWidth    = aWidth; }
    void SetPenColor   (EColorType  aColor) { mPenColor    = aColor; }
    void SetBackColor  (EColorType  aColor) { mBackColor   = aColor; }
    void SetForeColor  (EColorType  aColor) { mForeColor   = aColor; }
    void SetLineType   (EPenStyle   aStyle) { mLineType    = aStyle; }
    void SetBrushStyle (EBrushStyle aStyle) { mBrushStyle  = aStyle; }
};
//---------------------------------------------------------------------------
// Text Attributs
//---------------------------------------------------------------------------
class __dmTools dmTextAttributs
{
  private:
    EColorType   mTextColor;
    EFontWeight  mFontWeight;
    EFontStyle   mFontStyle;
    int          mFontSizePt;   
    char_t       mFontFamily[DM_FONTNAME_MAX];   
    
  public:
    dmTextAttributs();
    dmTextAttributs( const dmTextAttributs& );
    
    dmTextAttributs& operator=( const dmTextAttributs& );

    bool Empty() const { return mFontFamily[0]==_TXT('\0'); }

    void SetColor      ( EColorType  aColor        ) { mTextColor  = aColor;         }
    void SetStyle      ( EFontStyle  aFontStyle    ) { mFontStyle  = aFontStyle;     }
    void SetWeight     ( EFontWeight aFontWeight   ) { mFontWeight = aFontWeight;    }

    const dmString::E*  Family() const { return mFontFamily; }    
       
    EColorType  Color()       const { return mTextColor;  }
    EFontStyle  Style()       const { return mFontStyle;  }
    EFontWeight Weight()      const { return mFontWeight; }
    int         Size()        const { return mFontSizePt; }
};
//---------------------------------------------------------------------------
// Style
//---------------------------------------------------------------------------
class dmDrawStyle
{
  public:
    dmString        mClass;
    dmPenAttributs  mPenAttributs;
    dmTextAttributs mTextAttributs;

    dmDrawStyle() {}
    dmDrawStyle( const dmString::E* _class, 
                 const dmPenAttributs&  _pen,
                 const dmTextAttributs& _text ) 
   :  mClass(_class)
     ,mPenAttributs(_pen)
     ,mTextAttributs(_text)
     {}
};

//-------------------------------------------
// For use in dmGraphics::SetDrawStyle
//-------------------------------------------
enum dmDrawStyleFlags {
  dmDrawStyleSave    = 1,
  dmDrawStyleRestore = 2
};

//---------------------------------------------------------------------------
// dmGraphics
//---------------------------------------------------------------------------
class dmGraphics : public dmReferenced
{   
  protected: 
    dmTList<dmRect> mInvalidRect;
    int             mInvalidCount;
    bool            mFastRefresh;
    ECursorType     mCursor;

    const dmDrawStyle* mSavedStyle; 
   
    __dmTools void  Construct();
    __dmTools void  Destroy();

  public:	 
    dmGraphics()          { Construct(); }
    virtual ~dmGraphics() { Destroy();   }

    virtual bool    ObtainGraphics( dm_uint _Flags = 0 )  { return false; }
    virtual void    ReleaseGraphics() {}

    virtual dm_uint GetPageScale() { return 100; }

    //--------------------------------------------------------------
    // Drawing state functions
    //--------------------------------------------------------------

    virtual void SetPenAttrs ( const dmPenAttributs attrs ) {}
    virtual void SetPen      ( EPenStyle aLineType , int aWidth , EColorType aColor ) {}	
    virtual void SetBrush    ( EColorType, EBrushStyle style = eBrushStyle_solid ) {}
    virtual void SetPenColor ( EColorType aColor ) {}

    //----------------------------------------
    // Drawing functions
    //----------------------------------------

    virtual void DrawLine     ( int x1,int y1,int x2, int y2, int flags = dmTk::eDrawDefault )  {}
    virtual void DrawRectangle( int x1,int y1,int x2, int y2, int flags = dmTk::eDrawDefault ) {}
    virtual void DrawEllipse  ( int x0,int y0,int rx, int ry, int flags = dmTk::eDrawDefault ) {}
    virtual void DrawArc      ( int x0,int y0,int rx, int ry, dm_real startAngle , dm_real endAngle,  int flags = dmTk::eDrawDefault ) {}
    virtual void DrawPoly     ( const dm_point* , size_t , int flags = dmTk::eDrawDefault ) {}
    virtual void DrawCircle   ( int x0,int y0,int r,  int flags = dmTk::eDrawDefault    ) { DrawEllipse(x0,y0,r,r,flags); }

  	void DrawLine     ( const dm_point& p1, const dm_point& p2, int flags = dmTk::eDrawDefault ) { DrawLine(p1.x,p1.y,p2.x,p2.y,flags);  }
  	void DrawRectangle( const dm_point& p1, const dm_point& p2, int flags = dmTk::eDrawDefault ) { DrawRectangle(p1.x,p1.y,p2.x,p2.y,flags); }
  	void DrawEllipse  ( const dm_point& p1, int rx, int ry    , int flags = dmTk::eDrawDefault ) { DrawEllipse(p1.x,p1.y,rx,ry,flags); }
    void DrawArc      ( const dm_point& p1, int rx, int ry, dm_real startAngle, dm_real endAngle, int flags = dmTk::eDrawDefault ) { DrawArc(p1.x,p1.y,rx,ry,startAngle,endAngle,flags); }
    void DrawCircle	  ( const dm_point& p , int r             , int flags = dmTk::eDrawDefault ) { DrawEllipse(p,r,r,flags); }
    void DrawPoly     ( const dmPoly&  p                      , int flags = dmTk::eDrawDefault ) { DrawPoly(&p[0],p.Size(),flags); }

    void DrawLine     ( const dmLine&    _l, int flags = dmTk::eDrawDefault ) { DrawLine( _l.start,_l.end,flags); }
    void DrawRectangle( const dmRect&    _r, int flags = dmTk::eDrawDefault ) { DrawRectangle( _r.TopLeft(),_r.BottomRight(),flags); }
    void DrawEllipse  ( const dmEllipse& _e, int flags = dmTk::eDrawDefault ) { DrawEllipse  ( _e.center,_e.rx,_e.ry,flags); }
    void DrawEllipse  ( const dmRect&    _r, int flags = dmTk::eDrawDefault ) { DrawEllipse  ( _r.Center(),_r.Width()/2,_r.Height()/2,flags); }
    void DrawCircle   ( const dmCircle&  _c, int flags = dmTk::eDrawDefault ) { DrawCircle   ( _c.center,_c.r,flags); }
    void DrawArc      ( const dmEllipse& _e, dm_real startAngle, dm_real endAngle, int flags = dmTk::eDrawDefault ) { DrawArc(_e.center,_e.rx,_e.ry,startAngle,endAngle,flags); }
    void DrawArc      ( const dmRect&    _r, dm_real startAngle, dm_real endAngle, int flags = dmTk::eDrawDefault ) { DrawArc(_r.Center(),_r.Width()/2,_r.Height()/2,startAngle,endAngle,flags); }

    void Draw( const dmRect&    _r, int flags = dmTk::eDrawDefault ) { DrawRectangle( _r,flags); }
    void Draw( const dmCircle&  _c, int flags = dmTk::eDrawDefault ) { DrawCircle   ( _c,flags); }
    void Draw( const dmEllipse& _e, int flags = dmTk::eDrawDefault ) { DrawEllipse  ( _e,flags); }
    void Draw( const dmPoly&    _p, int flags = dmTk::eDrawDefault ) { DrawPoly     ( _p,flags); }
    void Draw( const dmLine&    _l ) { DrawLine(_l);  }
    void Draw( const dmString::E* s, const dmRect& r ) { DrawText(s,r); }

    //--------------------------------------------------------------
    // Text drawing	 
    //--------------------------------------------------------------
    virtual void DrawText    ( const dmString::E*, const dmRect& ,int = eAlignStyle_normal ) {}
    virtual void DrawText    ( const dmString::E*, const dmPoint&,int = eAlignStyle_normal ) {}
    virtual void SetTextColor( EColorType ) {}
    virtual void SetTextAttrs( const dmTextAttributs& ) {}

    virtual void GetTextRect ( const dmString::E*, dmRect& , int = eAlignStyle_normal ) {}

    //--------------------------------------------------------------
    // Image drawing	 
    //--------------------------------------------------------------
    virtual void Display( const dmNativeImage*,const dmPoint& _dest ) {}
    virtual void Display( const dmNativeImage*,const dmRect&  _dest ) {}
    virtual void Display( const dmNativeImage*,const dmPoint& _dest,const dmRect& _src ) {}
    virtual void Display( const dmNativeImage*,const dmRect&  _dest,const dmRect& _src ) {}

    //----------------------------------------
    // Redessine la region rect
    //----------------------------------------
    virtual void Refresh() {}
    virtual void Refresh( const dmRect& ) {}

    __dmTools void OptimizeRefresh( bool );
    __dmTools void AddInvalidRect ( const dmRect& );
    __dmTools int  BeginRefresh   ( const dmRect& );
    __dmTools int  EndRefresh     ( const dmRect& );
    __dmTools int  BeginRefresh   ();
    __dmTools int  EndRefresh( bool _bRedraw = true );

    __dmTools const dmDrawStyle* SetDrawStyle( const dmDrawStyle* _style, int _Flags );

    virtual bool UpdateCursor() { return false; }
    
    ECursorType GetCursorType() const {  return mCursor; }
    ECursorType SetCursor(ECursorType newCursor) {
      ECursorType oldCursor = mCursor;
      mCursor   = newCursor;
      return oldCursor; 
    }


    //----------------------------
    // Image Factory
    //----------------------------
    virtual dmNativeImageFactory* GetImageFactory() const { return NULL; }
    
    //----------------------------
    // Return true if this graphics 
    // is a user interface display 
    //----------------------------
    virtual bool IsUI() const { return false; }
};
//---------------------------------------------------
// Wrappers for Graphic context
//---------------------------------------------------
class dmSurface 
{
   private:
     dmGraphics* gr;    
   public:
     dmSurface( dmGraphics* _gr,dm_uint _Flags = 0) : gr(_gr) { 
       if(gr) gr->ObtainGraphics(_Flags); 
     }

     dmGraphics* operator->()  { return  gr; }
     dmGraphics& operator*()   { return *gr; } 
 
     operator dmGraphics*()    { return gr;  }

     void Refresh( const dmRect& r ) { 
       if(gr) { 
         gr->BeginRefresh(r); 
         gr->EndRefresh();
       }
     }

     operator bool() { return gr!=NULL; }

    ~dmSurface() { if(gr) gr->ReleaseGraphics(); }
};
//---------------------------------------------------
class dmSavedDrawStyle
{
  private: 
    dmGraphics*        gr;   
    const dmDrawStyle* saved;
   public:
     dmSavedDrawStyle( dmGraphics* _gr, const dmDrawStyle* _NewStyle  ) : gr(_gr) { 
       saved = gr->SetDrawStyle(_NewStyle,dmDrawStyleSave); 
     }
 
    ~dmSavedDrawStyle() { gr->SetDrawStyle(saved,dmDrawStyleRestore); }
};
//---------------------------------------------------

#endif // dmGraphics_h

