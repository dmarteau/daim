//--------------------------------------------------------
// Copyright (c) 2004 David Marteau
// 
// File         : dmRectEditEx.h
// Date         : 10/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
// THIS FILE IS SUBJECT TO CHANGE WITHOUT NOTICE !!
//--------------------------------------------------------
#ifndef dmRectEditEx_h
#define dmRectEditEx_h

#include "dmRectShapeEx.h"

class dmRectEditEx : public dmAreaEdit 
{
  private:
    dm_double     mStore;
    dmRectShapeEx mRect;

  public:
    dmRectEditEx( dmTool* _parent = NULL );	 
    dmRectEditEx( const dmRect&, dm_real _angle, dmTool* _parent=NULL );
    dmRectEditEx( const dmRectEditEx&, dmTool* _parent=NULL );	 
   ~dmRectEditEx();
  
  protected:
    virtual void OnUpdateRegion( dmRegion& _rgn );

    virtual bool    OnMouseDown (const dmPoint&,dm_uint _button,dm_uint _keyQual);
    virtual dmPoint DoMouseEvent(const dmPoint&,dm_uint _button,dm_uint _keyQual);
       
 public:
    void SetOrientation( dm_double );
    void SetRectangle  ( const dmRect& );
    void SetCoordinates( const dmRect& r ) { SetRectangle(r); }
    
    dm_double     GetOrientation() const { return mRect.GetOrientation(); }
    const dmRect& GetRectangle()   const { return mRect.GetRect();        }

    virtual void OnDisplay( dmGraphics& ) const;
    virtual dmTool* Clone( dmTool* _parent = NULL ) const;

    dmDeclareClassInfo(dmRectEditEx);
    
};

#endif // dmRectEditEx_h

