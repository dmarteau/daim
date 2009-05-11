#ifndef dmAreaEdit_h
#define dmAreaEdit_h

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
// File         : dmAreaEdit.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

//---------------------------------------------------------------------------
// Generic Editable Tool
//---------------------------------------------------------------------------
class __dmTools dmAreaEdit : public dmTool 
{
  private:
    typedef enum {
      stEditing,      // specifie si on est en cours d'édition
      stObjectMove,
      stNoStates,
    } _EditStates;
 
    // distance maximum en pixel pour laquelle
    // un move est efectivement déclenché
    enum { nMoveTestMax = 5 };

    friend class dmSetOf<_EditStates>;

    dmPoint              mMouseDown;
    dmSetOf<_EditStates> mStates;
  
  protected:     
    bool IsEditing() const     { return mStates.Contains(stEditing); }
    void SetEditMode(bool _st) { mStates.Set(stEditing,_st);         }

    //-------------------------------------------------------------
    // Relative mouse moves
    //-------------------------------------------------------------
    int     DeltaX  (const dmPoint& _cursor) const { return _cursor.x - mMouseDown.x; }
    int     DeltaY  (const dmPoint& _cursor) const { return _cursor.y - mMouseDown.y; }
    dmPoint DeltaXY (const dmPoint& _cursor) const { return _cursor   - mMouseDown;   }

    const dmPoint& MouseDown() const { return mMouseDown; }

    virtual dmPoint DoMouseEvent(const dmPoint&,dm_uint _button,dm_uint _keyQual);
    virtual bool    InitialUpdate();

    // Protected constructor
    dmAreaEdit( const dmRect&, dmTool* _parent=NULL );
    
  public:
    dmAreaEdit( dmTool* _parent=NULL );	 
    dmAreaEdit( const dmAreaEdit& _toCopy, dmTool* _parent=NULL);

    virtual ~dmAreaEdit();

  public:
    // Events management

    virtual bool ProcessDefaultEvent( dmEvent& );

    virtual bool OnClick      (const dmPoint&,dm_uint _button,dm_uint _keyQual); 
    virtual bool OnDblClick   (const dmPoint&,dm_uint _button,dm_uint _keyQual); 
    virtual bool OnMouseDown  (const dmPoint&,dm_uint _button,dm_uint _keyQual); 
    virtual bool OnMouseUp    (const dmPoint&,dm_uint _button,dm_uint _keyQual); 
    virtual bool OnMouseMove  (const dmPoint&,dm_uint _button,dm_uint _keyQual); 
    virtual bool OnMouseWheel (const dmPoint&,dm_uint _button,dm_uint _keyQual); 
    virtual bool OnContextMenu(const dmPoint&,dm_uint _button,dm_uint _keyQual); 

    virtual bool OnKeyDown    (dm_uint _keyCode,dm_uint _keyQual); 
    virtual bool OnKeyUp      (dm_uint _keyCode,dm_uint _keyQual); 
    virtual bool OnKeyPress   (dm_uint _keyCode,dm_uint _keyQual); 

    virtual dmTool* Clone( dmTool* _parent = NULL ) const;

    dmDeclareClassInfo( dmAreaEdit );
};

//---------------------------------------------------------------------------
// Editor for Rectangular area
//---------------------------------------------------------------------------
class __dmTools dmRectEdit : public dmAreaEdit 
{
  protected:
    dmRectShape mRect;

  public:
    dmRectEdit( dmTool* _parent = NULL );
    dmRectEdit( const dmRect& , dmTool* _parent = NULL );		 
    dmRectEdit( const dmRectEdit& _toCopy, dmTool* _parent = NULL );
   ~dmRectEdit();
  
  protected:
    virtual void OnUpdateRegion( dmRegion& _rgn );
    virtual dmPoint DoMouseEvent(const dmPoint&,dm_uint _button,dm_uint _keyQual);

  public:
    virtual void OnDisplay( dmGraphics& ) const;
    virtual dmTool* Clone( dmTool* _parent = NULL ) const;

    void SetCoordinates( const dmRect& );  

    dmRectEdit& operator=(const dmRect& r) { 
       SetCoordinates(r); 
       return *this; 
    }

    operator dmRect() const { return mRect; }
        
  public:
    dmDeclareClassInfo( dmRectEdit );
};
//---------------------------------------------------------------------------
// Line Editor
//---------------------------------------------------------------------------
class __dmTools dmLineEdit : public dmAreaEdit
{
  friend class dmPolyEdit;
  public:
    enum {
      startIndex = 0,
      endIndex   = 1
    };

    typedef enum {
      LineConstraintX,
      LineConstraintY,
      LineConstraintDelta
    } LineConstraintType;

  protected:
    dmLineShape        mLine;
    LineConstraintType mLineConstraint;

  public:
    dmLineEdit( dmTool* _parent = NULL );	 
    dmLineEdit( const dmLine&, dmTool* _parent = NULL );
    dmLineEdit( const dmLineEdit& _toCopy, dmTool* _parent = NULL );
   ~dmLineEdit();

  protected:
    virtual void    OnUpdateRegion( dmRegion& _rgn );
    virtual dmPoint DoMouseEvent(const dmPoint&,dm_uint _button,dm_uint _keyQual );

  public:
    virtual void OnDisplay( dmGraphics& ) const;
    virtual dmTool* Clone( dmTool* _parent = NULL ) const;

    void SetCoordinates( const dmLine& );

    dmLineEdit& operator=( const dmLine& l) { 
        SetCoordinates(l); 
        return *this; 
    }

    operator  dmLine() const { return mLine; }

    void SetConstraint( LineConstraintType lc ) { mLineConstraint = lc; }

    const dm_point& Start() const { return mLine.Start(); }
    const dm_point& End()   const { return mLine.End();   }

  public:
    dmDeclareClassInfo( dmLineEdit );
};
//---------------------------------------------------------------------------
// Ellipse area
//---------------------------------------------------------------------------
class __dmTools dmEllipseEdit : public dmAreaEdit 
{
  protected:
    dmEllipseShape mEllipse;

  public:
    dmEllipseEdit( dmTool* _parent = NULL );	 
    dmEllipseEdit( const dmEllipse&, dmTool* _parent = NULL );	 
    dmEllipseEdit( const dmEllipseEdit& _toCopy, dmTool* _parent = NULL );
   ~dmEllipseEdit();
  
  protected:
    virtual void    OnUpdateRegion( dmRegion& _rgn );
    virtual dmPoint DoMouseEvent(const dmPoint&,dm_uint _button,dm_uint _keyQual);

    dmPoint GetAxis  (const dmPoint& _start,const dmPoint& _end) const;
    int     GetRadius(const dmPoint& _start,const dmPoint& _end) const;

  public:
    void SetCoordinates( const dmEllipse& e );
 
    dmEllipseEdit& operator=( const dmEllipse& e) { 
       SetCoordinates(e); return *this; 
    }

    operator dmEllipse() const { return  mEllipse; }         
 
    virtual void OnDisplay( dmGraphics& ) const;
    virtual dmTool* Clone( dmTool* _parent = NULL ) const;

  public:
    dmDeclareClassInfo( dmEllipseEdit );
};
//---------------------------------------------------------------------------
// Edition d'un polygone
//---------------------------------------------------------------------------
class __dmTools dmPolyEdit : public dmAreaEdit
{
  friend class dmClosedPolyEdit;
  protected:
    dmPolyShape  mPoly;

  public:
    dmPolyEdit( dmTool* _parent = NULL );
    dmPolyEdit( const dmPoly&, dmTool* _parent = NULL );	 
    dmPolyEdit( const dmPolyEdit& _toCopy, dmTool* _parent = NULL );
    virtual ~dmPolyEdit();

  protected:
    virtual void OnUpdateRegion( dmRegion& _rgn );

  public:
    virtual void OnDisplay( dmGraphics& ) const;

    virtual dmTool* Clone( dmTool* _parent = NULL ) const;
    
    virtual bool OnMouseDown (const dmPoint&,dm_uint _button,dm_uint _keyQual );
    virtual bool OnMouseUp   (const dmPoint&,dm_uint _button,dm_uint _keyQual );
    virtual bool OnMouseMove (const dmPoint&,dm_uint _button,dm_uint _keyQual ); 
    virtual bool OnDblClick  (const dmPoint&,dm_uint _button,dm_uint _keyQual );

    const dm_point& operator[](int i) const { return  mPoly[i]; }   

    void SetCoordinates( const dmPoly& p );

  public:
    dmDeclareClassInfo( dmPolyEdit );
};
//---------------------------------------------------------------------------
// Edition d'un polygone Fermé
//---------------------------------------------------------------------------
class __dmTools dmClosedPolyEdit : public dmPolyEdit
{
  public:
    dmClosedPolyEdit( dmTool* _parent = NULL );
    dmClosedPolyEdit( const dmPoly&, dmTool* _parent = NULL );
    dmClosedPolyEdit( const dmPolyEdit& _toCopy, dmTool* _parent = NULL );
   ~dmClosedPolyEdit() {}
 
  public:
    virtual dmTool* Clone( dmTool* _parent = NULL ) const;
    
    virtual bool OnDblClick (const dmPoint& _pt,dm_uint _button,dm_uint _keyQual ) {
       return dmPolyEdit::OnDblClick(_pt,_button,_keyQual || keyShift);
    }

  public:
    dmDeclareClassInfo( dmClosedPolyEdit );
};
//---------------------------------------------------------------------------
// Edition d'un contour main levé
//---------------------------------------------------------------------------
class __dmTools dmFreeHandEdit : public dmAreaEdit
{
  protected:
    dmPolyShape mPoly;

  public:
    dmFreeHandEdit( dmTool* _parent = NULL );
    dmFreeHandEdit( const dmPoly&, dmTool* _parent = NULL );	 	 
    dmFreeHandEdit( const dmFreeHandEdit& _toCopy, dmTool* _parent = NULL );
    virtual ~dmFreeHandEdit();

  protected:
    virtual void    OnUpdateRegion( dmRegion& _rgn ); 
    virtual dmPoint DoMouseEvent(const dmPoint&,dm_uint _button,dm_uint _keyQual);
     
  public:
    void SetCoordinates( const dmPoly& p );

    virtual void    OnDisplay( dmGraphics& ) const;
    virtual dmTool* Clone( dmTool* _parent = NULL ) const;

    virtual bool OnMouseDown(const dmPoint&,dm_uint _button,dm_uint _keyQual );
 
    const dm_point& operator[](int i) const { return  mPoly[i]; }   

  public:
    dmDeclareClassInfo( dmFreeHandEdit );
};
//---------------------------------------------------------------------------
// Edition d'un contour fermé main levé
//---------------------------------------------------------------------------
class __dmTools dmClosedFreeHandEdit : public dmFreeHandEdit
{
  public:
    dmClosedFreeHandEdit( dmTool* _parent = NULL );	 
    dmClosedFreeHandEdit( const dmPoly&, dmTool* _parent = NULL );	 
    dmClosedFreeHandEdit( const dmFreeHandEdit& _toCopy, dmTool* _parent = NULL );
   ~dmClosedFreeHandEdit() {}

  public:
    virtual dmTool* Clone( dmTool* _parent = NULL ) const;  
    virtual bool OnMouseUp(const dmPoint& _pt,dm_uint _button,dm_uint _keyQual );

  public:
    dmDeclareClassInfo( dmClosedFreeHandEdit );
};
//-----------------------------------------------------------
#endif // dmAreaEdit_h

