#define dmUseExtendedTools
#include "daim_tools.h"
#include "advtools/dmPtsCircleEdit.h"
//---------------------------------------------------------------
#define dmPtsCircleEdit_SCHEMA_VERSION 1
dmImplementClassInfo( dmPtsCircleEdit, dmAreaEdit, dmPtsCircleEdit_SCHEMA_VERSION );
//---------------------------------------------------------------
dmPtsCircleEdit::dmPtsCircleEdit(  dmTool* _parent )
: dmAreaEdit(_parent)
 ,mbShowCenter(true)
{  
  SetCarrets( &mCircle ); 
}
//---------------------------------------------------------------
dmPtsCircleEdit::dmPtsCircleEdit( const dmPtsCircleEdit& _toCopy, dmTool* _parent  )
: dmAreaEdit  (_toCopy,_parent)
 ,mbShowCenter(_toCopy.mbShowCenter)
 ,mCircle     (_toCopy.mCircle)
{  
  SetCarrets( &mCircle,_toCopy.IsCarretsEnabled()); 
}
//---------------------------------------------------------------
dmPtsCircleEdit::dmPtsCircleEdit( const dmPoint& p1, const dmPoint& p2, const dmPoint& p3, dmTool* _parent )
: dmAreaEdit(_parent)
 ,mbShowCenter(true)
{
  SetCarrets( &mCircle );
  mCircle.SetPoints(p1,p2,p3);
}
//---------------------------------------------------------------
dmPtsCircleEdit::~dmPtsCircleEdit()
{}
//---------------------------------------------------------------
void dmPtsCircleEdit::SetCoordinates( const dmPoly& p )
{
  if(p.Size()>=3) {
    mCircle.SetPoints(p[0],p[1],p[2]);
    UpdateObject();
  }
}
//---------------------------------------------------------------
void dmPtsCircleEdit::ShowCenter( bool _bShow )
{
  O_INVALIDATE(this)
  mbShowCenter  = _bShow;
}
//---------------------------------------------------------------
void dmPtsCircleEdit::OnUpdateRegion( dmRegion& _rgn )
{
  mCircle.BuildRegion(_rgn); 
}
//---------------------------------------------------------------
void dmPtsCircleEdit::OnDisplay( dmGraphics& gr ) const
{
  if(!mCircle.IsValid()) 
    gr.DrawPoly(mCircle.Carrets());

  dmCircle c = mCircle;
  gr.DrawCircle( c );
  if(mbShowCenter) {
    gr.DrawLine( c.center.x-8,c.center.y,c.center.x+9,c.center.y );
    gr.DrawLine( c.center.x,c.center.y-8,c.center.x,c.center.y+9 );    
  }
}
//---------------------------------------------------------------
dmTool* dmPtsCircleEdit::Clone( dmTool* _parent ) const
{ 
  return new dmPtsCircleEdit(*this,_parent);
}
//---------------------------------------------------------------
bool dmPtsCircleEdit::OnMouseDown(const dmPoint& cursor,dm_uint _button,dm_uint _keyQual)
{
  if(IsNullObject() && IsEditing()) 
    return true; // handled
  
  return dmAreaEdit::OnMouseDown(cursor,_button,_keyQual);
}
//---------------------------------------------------------------
bool dmPtsCircleEdit::OnMouseMove(const dmPoint& cursor,dm_uint _button,dm_uint _keyQual)
{
  if(IsNullObject() && IsEditing()) 
    return true;  // handled
 
  return dmAreaEdit::OnMouseMove(cursor,_button,_keyQual);
}
//---------------------------------------------------------------
bool dmPtsCircleEdit::OnMouseUp(const dmPoint& cursor,dm_uint _button,dm_uint _keyQual)
{
  if(IsNullObject() && IsEditing()) {
    switch(CarretIndex()) 
    {
      case 0: mCircle.SetPoints(cursor,cursor,cursor);break;
      case 1: mCircle.SetPoints(mCircle[0],cursor,cursor);break;
      case 2: mCircle.SetPoints(mCircle[0],mCircle[1],cursor);
              RecalcObject();
              SetEditMode(false);
              return false;
    }

    dmGraphics* gr = GetGraphics();    
    if(gr) 
      mCircle.DisplayCarrets(*gr);

    SetCarretIndex( CarretIndex()+1 );
    return true; // handled
  }
  return dmAreaEdit::OnMouseUp(cursor,_button,_keyQual);
}
//---------------------------------------------------------------
