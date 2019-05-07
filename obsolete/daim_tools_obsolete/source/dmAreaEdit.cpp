
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

#define dmUseAreaEdit
#include "daim_tools.h"
//-------------------------------------------------------------------
// Area tool
//-------------------------------------------------------------------
#define dmAreaEdit_SCHEMA_VERSION 1
dmImplementClassInfo( dmAreaEdit, dmTool,dmAreaEdit_SCHEMA_VERSION );
//---------------------------------------------------------------
dmAreaEdit::dmAreaEdit( dmTool* _parent )
: dmTool(_parent)
{}	  
//---------------------------------------------------------------
dmAreaEdit::dmAreaEdit( const dmRect& r, dmTool* _parent )
: dmTool(r,_parent)
{} 
//---------------------------------------------------------------
dmAreaEdit::dmAreaEdit( const dmAreaEdit& _toCopy, dmTool* _parent )
: dmTool(_toCopy,_parent),
  mMouseDown(_toCopy.mMouseDown),
  mStates(_toCopy.mStates)
{
}
//---------------------------------------------------------------
dmAreaEdit::~dmAreaEdit()
{}
//----------------------------------------------------------------
// Initializer
//----------------------------------------------------------------
bool dmAreaEdit::InitialUpdate()
{
  // On rend l'objet visible si en est en train d'editer l'objet 
  if(IsEditing()) Show(true); 
  return dmTool::InitialUpdate();
}
//----------------------------------------------------------------
dmTool* dmAreaEdit::Clone( dmTool* _parent  ) const
{
  dmAreaEdit* tool = new dmAreaEdit(*this,_parent);
  return tool;
}
//-------------------------------------------------------
bool dmAreaEdit::ProcessDefaultEvent( dmEvent& evt )
{
  // Handle events only in capture phase
  // and if the event is looking for target
  if(evt.IsCapturing())
  {
    evt.mTarget=this;    // Assign the target to this object

    switch(evt.mStructType) 
    {
      case evStruct_MouseEvent : 
      {
        dmMouseEvent& _evt = static_cast<dmMouseEvent&>(evt);
        switch(_evt.mEventType)
        {
          case evMouseMove   : return OnMouseMove  (_evt.mCursor,_evt.mButton,_evt.mKeyQual); 
          case evMouseDown   : return OnMouseDown  (_evt.mCursor,_evt.mButton,_evt.mKeyQual); 
          case evMouseUp     : return OnMouseUp    (_evt.mCursor,_evt.mButton,_evt.mKeyQual); 
          case evMouseWheel  : return OnMouseWheel (_evt.mCursor,_evt.mButton,_evt.mKeyQual); 
          case evClick       : return OnClick      (_evt.mCursor,_evt.mButton,_evt.mKeyQual); 
          case evDblClick    : return OnDblClick   (_evt.mCursor,_evt.mButton,_evt.mKeyQual); 
          case evContextMenu : return OnContextMenu(_evt.mCursor,_evt.mButton,_evt.mKeyQual); 
          default: break;
        }
      } break;

      case evStruct_KeyEvent : 
      {
        dmKeyEvent& _evt = static_cast<dmKeyEvent&>(evt);
        switch(_evt.mEventType)
        {
          case evKeyDown     : return OnKeyDown    (_evt.mKeyCode,_evt.mKeyQual); 
          case evKeyUp       : return OnKeyUp      (_evt.mKeyCode,_evt.mKeyQual); 
          case evKeyPress    : return OnKeyPress   (_evt.mKeyCode,_evt.mKeyQual);
          default: break;
        }
      } break;

      default: break;
    } 
  }
  return false; // Event is not handled
}
//-------------------------------------------------
bool dmAreaEdit::OnClick      (const dmPoint&,dm_uint ,dm_uint ) { return false; }
bool dmAreaEdit::OnDblClick   (const dmPoint&,dm_uint ,dm_uint ) { return false; } 
bool dmAreaEdit::OnMouseWheel (const dmPoint&,dm_uint ,dm_uint ) { return false; }
bool dmAreaEdit::OnContextMenu(const dmPoint&,dm_uint ,dm_uint ) { return false; }
//-------------------------------------------------
bool dmAreaEdit::OnKeyDown    (dm_uint ,dm_uint ) { return false; }
bool dmAreaEdit::OnKeyUp      (dm_uint ,dm_uint ) { return false; }
bool dmAreaEdit::OnKeyPress   (dm_uint ,dm_uint ) { return false; }
//-------------------------------------------------
// Fonctions de manipulations
//-------------------------------------------------
dmPoint dmAreaEdit::DoMouseEvent( const dmPoint& _pt,dm_uint _button,dm_uint _keyQual )
{
   MoveBy(DeltaXY(_pt),!(_button & buttonLeft));
   return _pt;
}
//---------------------------------------------------
bool dmAreaEdit::OnMouseDown( const dmPoint& _pt,dm_uint _button,dm_uint _keyQual ) 
{
  bool _hasCarrets = (Carrets() !=NULL);

  mStates.Set(stObjectMove,IsNullObject());
  mMouseDown = _pt;

  if(IsInitialized())  //
  {
    SetEditMode( GetObject(_pt,true)==this );
    if(IsEditing()) 
    {
      if(_hasCarrets && CarretIndex()>=0) 
       mMouseDown = Carrets()->Carret( CarretIndex() );

      return true; // Handled
    }
  } 
  else  // Edit a new object
  {                                 
    if(IsCarretsEnabled()) 
      SetCarretIndex( GetCarrets()->Initialize( mMouseDown ) );
    
    SetEditMode(true);  // On attend d'autres évènements
    return true;        // Handled
  }
  return false;
}
//---------------------------------------------------
bool dmAreaEdit::OnMouseMove( const dmPoint& _pt,dm_uint _button,dm_uint _keyQual ) 
{ 
  if(IsEditing())
  {
    if(!mStates.Contains(stObjectMove) && IsPositionnable()) 
    {
      mStates.Set(stObjectMove, 
       (CarretIndex() >= 0)                  || 
       (daim::abs(DeltaX(_pt)) > nMoveTestMax) || 
       (daim::abs(DeltaY(_pt)) > nMoveTestMax));
    }   
    if(!(_button & buttonLeft)) 
        return OnMouseUp(_pt,_button,_keyQual); 

    if(mStates.Contains(stObjectMove))
      mMouseDown = DoMouseEvent(_pt,_button,_keyQual); 

    return true; // Handled
  }
  return false;
}
//---------------------------------------------------
// Par défaut la sélection se termine lorsque
// le bouton de la souris est relevée
//-------------------------------------------------
bool dmAreaEdit::OnMouseUp( const dmPoint& _pt,dm_uint _button,dm_uint _keyQual ) 
{
  if(IsEditing()) {
    if(mStates.Contains(stObjectMove))  
        DoMouseEvent(_pt,_button,_keyQual);

    SetEditMode(false);     
    return true;
  } 
  return false; // not handled
}
//---------------------------------------------------------------
// Edit functions for Rectangle
//---------------------------------------------------------------
#define dmRectEdit_SCHEMA_VERSION 1
dmImplementClassInfo( dmRectEdit, dmAreaEdit, dmRectEdit_SCHEMA_VERSION );
//---------------------------------------------------------------
dmRectEdit::dmRectEdit( dmTool* _parent  )
: dmAreaEdit(_parent)
{
  SetCarrets( &mRect );
}
//---------------------------------------------------------------	 
dmRectEdit::dmRectEdit( const dmRect& r, dmTool* _parent )
: dmAreaEdit(r,_parent ) 
 ,mRect(r)
{ 
  SetCarrets( &mRect );
}
//---------------------------------------------------------------		
dmRectEdit::dmRectEdit( const dmRectEdit& _toCopy, dmTool* _parent )
: dmAreaEdit( _toCopy,_parent )
 ,mRect(_toCopy.mRect)
{ 
  SetCarrets( &mRect,_toCopy.IsCarretsEnabled() ); 
}
//---------------------------------------------------------------
dmRectEdit::~dmRectEdit() {}
//---------------------------------------------------------------
dmTool* dmRectEdit::Clone( dmTool* _parent ) const
{
   return new dmRectEdit(*this,_parent);
}
//---------------------------------------------------------------
void dmRectEdit::OnUpdateRegion( dmRegion& _rgn )
{ 
  mRect.BuildRegion(_rgn);
}
//-------------------------------------------------------
void  dmRectEdit::SetCoordinates(const dmRect& r)
{
  mRect = r;
  UpdateObject();
}
////////////////////////////////-------------------------
static inline dmPoint __align_diag( dmPoint p )
{
  int wx = daim::abs(p.x);
  int wy = daim::abs(p.y);
  if( wx < wy ) p.y = daim::sgn(p.y)*wx; else p.x = daim::sgn(p.x)*wy;
  return p;
}
//-------------------------------------------------------
dmPoint dmRectEdit::DoMouseEvent( const dmPoint& _pt ,dm_uint _button,dm_uint _keyQual )
{ 
  if(CarretIndex()>=0 && CarretIndex()<4 && (_keyQual & keyShift) )
  {
    dmPoint p =_pt;
    p  =  mRect[(CarretIndex() + 2) % 4]; 
    p += __align_diag(_pt - p);
    MoveBy(p - mRect[CarretIndex()],!(_button & buttonLeft));
    return p;
  } 
  else 
    return dmAreaEdit::DoMouseEvent( _pt,_button,_keyQual );
}
//----------------------------------------------------------------
void dmRectEdit::OnDisplay( dmGraphics& gr ) const
{
  gr.DrawRectangle( mRect); 
}
//----------------------------------------------------------------
// Edit functions for Line
//----------------------------------------------------------------
#define dmLineEdit_SCHEMA_VERSION 1
dmImplementClassInfo( dmLineEdit, dmAreaEdit, dmLineEdit_SCHEMA_VERSION );
//---------------------------------------------------------------
dmLineEdit::dmLineEdit( dmTool* _parent )
: dmAreaEdit(_parent )
 ,mLineConstraint(LineConstraintDelta)
{
   SetCarrets( &mLine ); 
}
//---------------------------------------------------------------
dmLineEdit::dmLineEdit( const dmLine& _l, dmTool* _parent )
: dmAreaEdit(_parent )
 ,mLine(_l)
 ,mLineConstraint(LineConstraintDelta)
{ 
  SetCarrets( &mLine );
  UpdateRegion();
}
//---------------------------------------------------------------	 
dmLineEdit::dmLineEdit( const dmLineEdit& _toCopy, dmTool* _parent )
: dmAreaEdit( _toCopy,_parent)
 ,mLine(_toCopy.mLine)
 ,mLineConstraint(_toCopy.mLineConstraint)
{ 
  SetCarrets( &mLine,_toCopy.IsCarretsEnabled() ); 
}
//---------------------------------------------------------------
dmLineEdit::~dmLineEdit() {}
//-------------------------------------------------------
void dmLineEdit::SetCoordinates(const dmLine& l)
{
  mLine = l;
  UpdateObject();
}
//---------------------------------------------------------------
dmTool* dmLineEdit::Clone( dmTool* _parent ) const
{
   return new dmLineEdit(*this,_parent);
}
//---------------------------------------------------------------
void dmLineEdit::OnUpdateRegion( dmRegion& _rgn )
{ 
  mLine.BuildRegion(_rgn); 
}
//----------------------------------------------------------------
void dmLineEdit::OnDisplay( dmGraphics& gr ) const
{
  gr.DrawLine( mLine );
}
//-------------------------------------------------------
dmPoint dmLineEdit::DoMouseEvent( const dmPoint& _pt,dm_uint _button,dm_uint _keyQual )
{
  dmPoint _m = _pt;
  if(CarretIndex()>=0 && (_keyQual & keyShift) )
  {
    const dm_point& _start = mLine[(CarretIndex()?0:1)];
    switch(mLineConstraint)  
    {
      case LineConstraintX    : _m.y = _start.y; break;
      case LineConstraintY    : _m.x = _start.x; break;
      case LineConstraintDelta: {
        if(daim::abs(_m.y - _start.y) > daim::abs(_m.x-_start.x ))
          _m.x = _start.x; 
        else _m.y = _start.y;
      } break;
    }
  }
  return dmAreaEdit::DoMouseEvent(_m,_button,_keyQual);
}
//---------------------------------------------------------------
// Edit functions for Ellipse
//---------------------------------------------------------------
#define dmEllipseEdit_SCHEMA_VERSION 1
dmImplementClassInfo( dmEllipseEdit, dmAreaEdit,1 );
//---------------------------------------------------------------
dmEllipseEdit::dmEllipseEdit( dmTool* _parent )
: dmAreaEdit(_parent )
{  
  SetCarrets( &mEllipse ); 
}
//---------------------------------------------------------------
dmEllipseEdit::dmEllipseEdit( const dmEllipse& e, dmTool* _parent )
: dmAreaEdit( _parent )
 ,mEllipse(e)
{
  SetCarrets( &mEllipse );
  UpdateRegion();
}
//---------------------------------------------------------------
dmEllipseEdit::dmEllipseEdit( const dmEllipseEdit& _toCopy, dmTool* _parent )
: dmAreaEdit( _toCopy,_parent )
 ,mEllipse(_toCopy.mEllipse)
{ 
  SetCarrets( &mEllipse,_toCopy.IsCarretsEnabled() ); 
}
//---------------------------------------------------------------
dmEllipseEdit::~dmEllipseEdit() {}
//-------------------------------------------------------
void  dmEllipseEdit::SetCoordinates(const dmEllipse& e)
{
  mEllipse = e;
  UpdateObject();
}
//---------------------------------------------------------------
dmTool* dmEllipseEdit::Clone( dmTool* _parent ) const
{
   return new dmEllipseEdit(*this,_parent);
}
//---------------------------------------------------------------
void dmEllipseEdit::OnUpdateRegion( dmRegion& _rgn )
{ 
  mEllipse.BuildRegion(_rgn);
}
//----------------------------------------------------------------
void dmEllipseEdit::OnDisplay( dmGraphics& gr ) const
{
  // We Let the style defining filling and border modes
  gr.DrawEllipse(mEllipse);
}
//-------------------------------------------------------
dmPoint dmEllipseEdit::DoMouseEvent( const dmPoint& _pt,dm_uint _button,dm_uint _keyQual )
{ 
  if(IsNullObject() && CarretIndex()>=0 && (_keyQual & keyCtrl))
    SetCarretIndex(6);    
  else if(IsNullObject() || ( CarretIndex()>=0 && (_keyQual & keyShift)) )
    SetCarretIndex(999);
  
  if(CarretIndex()==999)
  {
    if(!(_button & buttonLeft)) 
       mEllipse.SetMoveState( true );

    dmPoint _size = GetAxis(mEllipse.Center(),_pt);

    if(_keyQual & keyShift) 
       _size.y = _size.x = GetRadius(mEllipse.Center(),_pt); 
    
    MoveBy( _size.x - (int)mEllipse.Rx(),
            _size.y - (int)mEllipse.Ry(),
            !(_button & buttonLeft) ); 
    
    return _pt;
  } else 
    return dmAreaEdit::DoMouseEvent(_pt,_button,_keyQual);
}
//----------------------------------------------------------------
dmPoint dmEllipseEdit::GetAxis(const dmPoint& _start,const dmPoint& _end) const
{
  dmPoint delta = _end-_start;
  int Xmax = dm_abs(delta.x); 
  int Ymax = dm_abs(delta.y);
  return dmPoint(Xmax,Ymax);
}
//----------------------------------------------------------------
int dmEllipseEdit::GetRadius(const dmPoint& _start,const dmPoint& _end)  const
{
  dmPoint   delta   = _end - _start;
  dm_double radius2 = delta.x*delta.x + delta.y*delta.y;

  int rad = static_cast<int>(sqrt( radius2 )+0.5);
  return rad;
}
//----------------------------------------------------------------
// Line Brise 
//----------------------------------------------------------------
#define dmPolyEdit_SCHEMA_VERSION 1
dmImplementClassInfo( dmPolyEdit, dmAreaEdit, dmPolyEdit_SCHEMA_VERSION );
//---------------------------------------------------------------
dmPolyEdit::dmPolyEdit(  dmTool* _parent )
: dmAreaEdit(_parent) 
{  
  SetCarrets( &mPoly ); 
}
//---------------------------------------------------------------
dmPolyEdit::dmPolyEdit( const dmPolyEdit& _toCopy, dmTool* _parent  )
: dmAreaEdit(_toCopy,_parent)
 ,mPoly(_toCopy.mPoly  )
{  
  SetCarrets( &mPoly,_toCopy.IsCarretsEnabled()); 
}
//---------------------------------------------------------------
dmPolyEdit::dmPolyEdit( const dmPoly& p, dmTool* _parent  )
: dmAreaEdit(_parent)
 ,mPoly(p)
{  
  SetCarrets( &mPoly ); 
  UpdateRegion();
}
//-------------------------------------------------------
void dmPolyEdit::SetCoordinates(const dmPoly& p)
{
  mPoly = p;
  UpdateObject();
}
//---------------------------------------------------------------
dmPolyEdit::~dmPolyEdit()
{}
//---------------------------------------------------------------
void dmPolyEdit::OnUpdateRegion( dmRegion& _rgn )
{
  mPoly.BuildRegion(_rgn); 
}
//---------------------------------------------------------------
void dmPolyEdit::OnDisplay( dmGraphics& gr ) const
{
  // We Let the style defining filling and border modes
  gr.DrawPoly(mPoly);
}
//---------------------------------------------------------------
dmTool* dmPolyEdit::Clone( dmTool* _parent ) const
{ 
  return new dmPolyEdit(*this,_parent);
}
//---------------------------------------------------------------
bool dmPolyEdit::OnMouseDown( const dmPoint& _pt,dm_uint _button,dm_uint _keyQual  )
{
  if(IsNullObject()) 
  {
    if(mPoly.Size()==0) 
    {
      mPoly.Clear();
      mPoly.Push_Back(_pt);             // On insere le premier point
      dmAreaEdit::OnMouseDown(_pt,_button,_keyQual);
      mPoly.Push_Back( _pt );          // On insere le point suivant
      SetCarretIndex( mPoly.Size()-1 );
    }
    return true;
  } 
  else 
   return dmAreaEdit::OnMouseDown(_pt,_button,_keyQual); 
}
//---------------------------------------------------------------
bool dmPolyEdit::OnMouseMove( const dmPoint& _pt,dm_uint _button,dm_uint _keyQual )
{
  if(IsEditing()) {
    if(IsNullObject()) 
       dmAreaEdit::OnMouseMove(_pt,_button||buttonLeft,_keyQual);
    else 
       dmAreaEdit::OnMouseMove(_pt,_button,_keyQual);

    return true; // Handled
  } 
  return false;
}
//---------------------------------------------------------------
bool dmPolyEdit::OnMouseUp( const dmPoint& _pt,dm_uint _button,dm_uint _keyQual )
{
  if(IsEditing()) 
  {
    if(IsNullObject()) 
    {
      mPoly.SetMoveState(false);
      dmSurface gr = ObtainGraphics();
      if(gr) 
      {
        dmSavedDrawStyle(gr,GetDrawStyle());

        gr->DrawLine(*(mPoly.End()-2),*(mPoly.End()-1)); 
        mPoly.Push_Back( _pt );    // on insere le point suivant
        SetCarretIndex( mPoly.Size()-1 );
      }
      return true;
    } 
    else 
      return dmAreaEdit::OnMouseUp(_pt,_button,_keyQual);
  }
  return false; 
}
//---------------------------------------------------------------
bool dmPolyEdit::OnDblClick( const dmPoint& _pt,dm_uint _button,dm_uint _keyQual )
{
  if(IsEditing() && IsNullObject())
  {
    mPoly.Pop_Back(); // Cause deux OnMouseUp appelé avant OnDblClick
    if((_keyQual & keyShift) && mPoly.Size()>2) 
     mPoly.Push_Back(mPoly[0]);

    MoveBy(0,0,true);
    SetEditMode(false);
  }
  // Apparemment on reçoit un OnMouseUp après le OnDblClick;
  return true;
}
//---------------------------------------------------------------
// Line Brisée Fermée
//---------------------------------------------------------------
#define dmClosedPolyEdit_SCHEMA_VERSION 1
dmImplementClassInfo( dmClosedPolyEdit, dmPolyEdit,dmClosedPolyEdit_SCHEMA_VERSION );
//---------------------------------------------------------------
dmClosedPolyEdit::dmClosedPolyEdit( dmTool* _parent )
: dmPolyEdit(_parent) 
{}
//---------------------------------------------------------------
dmClosedPolyEdit::dmClosedPolyEdit( const dmPolyEdit& _toCopy, dmTool* _parent )
: dmPolyEdit(_toCopy,_parent) 
{
  if(!mPoly.Closed() && mPoly.Size() > 1 ) 
    mPoly.Push_Back(mPoly[0]); 
}
//---------------------------------------------------------------
dmTool* dmClosedPolyEdit::Clone( dmTool* _parent ) const
{
  return new dmClosedPolyEdit(*this,_parent);
}
//---------------------------------------------------------------
// Contour main levé 
//---------------------------------------------------------------
#define dmFreeHandEdit_SCHEMA_VERSION 1
dmImplementClassInfo( dmFreeHandEdit, dmAreaEdit, dmFreeHandEdit_SCHEMA_VERSION );
//---------------------------------------------------------------
dmFreeHandEdit::dmFreeHandEdit(  dmTool* _parent ) 
: dmAreaEdit(_parent) 
{ 
  SetCarrets( &mPoly );
  LockCarrets( true );
}
//---------------------------------------------------------------
dmFreeHandEdit::dmFreeHandEdit( const dmPoly& p, dmTool* _parent  )
: dmAreaEdit(_parent)
 ,mPoly(p) 
{ 
  SetCarrets( &mPoly); 
  LockCarrets( true );
  UpdateRegion();
}
//---------------------------------------------------------------
dmFreeHandEdit::dmFreeHandEdit( const dmFreeHandEdit& _toCopy, dmTool* _parent  )
: dmAreaEdit(_toCopy,_parent )
 ,mPoly(_toCopy.mPoly  ) 
{ 
  SetCarrets( &mPoly,_toCopy.IsCarretsEnabled()); 
}
//---------------------------------------------------------------
dmFreeHandEdit::~dmFreeHandEdit() {}
//---------------------------------------------------------------
void dmFreeHandEdit::OnUpdateRegion( dmRegion& _rgn )
{
  mPoly.BuildRegion(_rgn); 
}
//---------------------------------------------------------------
void dmFreeHandEdit::OnDisplay( dmGraphics& gr ) const
{
  // We Let the style defining filling and border modes
  gr.DrawPoly(mPoly);
}
//---------------------------------------------------------------
dmTool* dmFreeHandEdit::Clone( dmTool* _parent ) const
{ 
  return new dmFreeHandEdit(*this,_parent);
}
//-------------------------------------------------------
void dmFreeHandEdit::SetCoordinates(const dmPoly& p)
{
  mPoly = p;
  UpdateObject();
}
//---------------------------------------------------------------
bool dmFreeHandEdit::OnMouseDown( const dmPoint& _pt,dm_uint _button,dm_uint _keyQual  )
{
  if(IsNullObject()) 
  {
    // Editing a new object
    // Set the default style from carrets
    dmGraphics* gr = GetGraphics();
    if(gr)
       gr->SetPenAttrs ( *mPoly.GetLineAttrs() );
 
    mPoly.Clear();
    mPoly.Push_Back(_pt);
  }
  return dmAreaEdit::OnMouseDown(_pt,_button,_keyQual);
}
//---------------------------------------------------------------
dmPoint dmFreeHandEdit::DoMouseEvent( const dmPoint& _pt,dm_uint _button,dm_uint _keyQual )
{
  if(IsNullObject()) 
  { 
    int k = mPoly.Size()-1;
    if(k<0 || _pt!=mPoly[k]) 
      mPoly.Push_Back( _pt );
    
    dmSurface gr = ObtainGraphics();
    if(gr) {
      DisplayObject(*gr); 
    }

    // Button Up
    if(!(_button  & buttonLeft) && (_keyQual & keyShift) && mPoly.Size()>1 ) 
         mPoly.Push_Back( mPoly[0] );   

    // TODO : Need to notify a move 

    if(!(_button & buttonLeft)) 
        RecalcObject();

    return _pt;
  }
  else 
   return dmAreaEdit::DoMouseEvent(_pt,_button,_keyQual); 
}
//---------------------------------------------------------------
// Contour main levé fermé
//---------------------------------------------------------------
#define dmClosedFreeHandEdit_SCHEMA_VERSION 1
dmImplementClassInfo( dmClosedFreeHandEdit, dmFreeHandEdit,1 );
//---------------------------------------------------------------
dmClosedFreeHandEdit::dmClosedFreeHandEdit( dmTool* _parent )
: dmFreeHandEdit(_parent) 
{}
//---------------------------------------------------------------
dmClosedFreeHandEdit::dmClosedFreeHandEdit( const dmPoly& p, dmTool* _parent )
: dmFreeHandEdit(p,_parent) 
{}	
//---------------------------------------------------------------
dmClosedFreeHandEdit::dmClosedFreeHandEdit( const dmFreeHandEdit& _toCopy, dmTool* _parent )
: dmFreeHandEdit(_toCopy,_parent) 
{
  if(!mPoly.Closed() && mPoly.Size() > 1 )  
    mPoly.Push_Back(mPoly[0]); 

}
//---------------------------------------------------------------
dmTool* dmClosedFreeHandEdit::Clone( dmTool* _parent ) const
{
  return new dmClosedFreeHandEdit(*this,_parent);
}
//---------------------------------------------------------------
bool dmClosedFreeHandEdit::OnMouseUp(const dmPoint& _pt,dm_uint _button,dm_uint _keyQual ) 
{
  return dmFreeHandEdit::OnMouseUp(_pt,_button,_keyQual | keyShift);
}
//---------------------------------------------------------------
