//--------------------------------------------------------
// Copyright (c) 2004 David Marteau
// 
// File         : dmPolyEditEx.h
// Date         : 10/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
// THIS FILE IS SUBJECT TO CHANGE WITHOUT NOTICE !!
//--------------------------------------------------------
#ifndef dmPolyEditEx_h
#define dmPolyEditEx_h


class dmPolyEditEx : public dmAreaEdit
{
  protected:
    dmPolyShape  mPoly;
    bool         mbEditClose;
    bool         mbEditPoints;

  public:
    dmPolyEditEx( dmTool* _parent = NULL );
    dmPolyEditEx( bool _filled, bool _bEditClose, dmTool* _parent = NULL );	 
    dmPolyEditEx( const dmPolyEditEx& _toCopy, dmTool* _parent = NULL );
    virtual ~dmPolyEditEx();

  protected:
    virtual void OnUpdateRegion( dmRegion& _rgn );

    const dmPolyShape& GetPoints() const { return mPoly; }
    void  SetPoints( const dmPoly& p )   { mPoly = p; }

  public:
    virtual void OnDisplay( dmGraphics& ) const;

    virtual dmTool* Clone( dmTool* _parent = NULL ) const;

    virtual bool OnMouseDown  (const dmPoint&,dm_uint _button,dm_uint _keyQual); 
    virtual bool OnMouseUp    (const dmPoint&,dm_uint _button,dm_uint _keyQual); 
    virtual bool OnMouseMove  (const dmPoint&,dm_uint _button,dm_uint _keyQual); 
    virtual bool OnDblClick   (const dmPoint&,dm_uint _button,dm_uint _keyQual); 

    const dm_point& operator[](int i) const { return  mPoly[i]; }   

    void SetCoordinates( const dmPoly& );
    void EditPoints( bool _Enable ) { mbEditPoints = _Enable; }

    // Retourne l'index de la carret de départ du segment
    // ou l'on a trouvé le point, -1 sinon
    int GetPoint( const dmPoint& pwhere, dmPoint& pt );

  public:
    dmDeclareClassInfo( dmPolyEditEx );

};
//---------------------------------------------------------------------------
#endif
