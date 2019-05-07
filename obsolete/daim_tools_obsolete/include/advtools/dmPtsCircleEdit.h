//--------------------------------------------------------
// Copyright (c) 2004 David Marteau
// 
// File         : dmPtsCircleEdit.h
// Date         : 10/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
// THIS FILE IS SUBJECT TO CHANGE WITHOUT NOTICE !!
//--------------------------------------------------------
#ifndef dmPtsCircleEdit_h
#define dmPtsCircleEdit_h

#include "dmPtsCircleShape.h"

class dmPtsCircleEdit : public dmAreaEdit
{
  private:
   bool   mbShowCenter;
  
  protected:
    dmPtsCircleShape  mCircle;

  public:
    dmPtsCircleEdit( dmTool* _parent = NULL );
    dmPtsCircleEdit( const dmPtsCircleEdit& _toCopy, dmTool* _parent = NULL );
    dmPtsCircleEdit( const dmPoint& p1, const dmPoint& p2, const dmPoint& p3, dmTool* _parent = NULL );

    virtual ~dmPtsCircleEdit();

  protected:
    virtual void OnUpdateRegion( dmRegion& _rgn );

  public:
    void SetCoordinates( const dmPoly& p );
    void ShowCenter( bool );

    virtual void OnDisplay( dmGraphics& ) const;

    virtual dmTool* Clone( dmTool* _parent = NULL ) const;

    virtual bool OnMouseDown  (const dmPoint&,dm_uint _button,dm_uint _keyQual); 
    virtual bool OnMouseUp    (const dmPoint&,dm_uint _button,dm_uint _keyQual); 
    virtual bool OnMouseMove  (const dmPoint&,dm_uint _button,dm_uint _keyQual); 

  public:
    dmDeclareClassInfo( dmPtsCircleEdit );

};

#endif // dmPtsCircleEdit_h
