#define dmUseExtendedTools
#include "daim_tools.h"
#include "advtools/dmCircleEdit.h"

#define dmCircleEdit_SCHEMA_VERSION 1
dmImplementClassInfo( dmCircleEdit, dmAreaEdit,dmCircleEdit_SCHEMA_VERSION );
//---------------------------------------------------------------
dmCircleEdit::dmCircleEdit( dmTool* _parent ) 
: dmAreaEdit( _parent )
 ,mbShowCenter(true)
 ,mbShowDiameter(true)
{  
  SetCarrets( &mCircle ); 
}
//---------------------------------------------------------------
dmCircleEdit::dmCircleEdit( const dmCircle& c, dmTool* _parent )
: dmAreaEdit( _parent )
 ,mCircle(c)
 ,mbShowCenter(true)
 ,mbShowDiameter(true)
{
  SetCarrets( &mCircle );
}
//---------------------------------------------------------------
dmCircleEdit::dmCircleEdit( const dmCircleEdit& _toCopy, dmTool* _parent )
: dmAreaEdit( _toCopy,_parent ) 
 ,mCircle(_toCopy.mCircle)
 ,mbShowCenter(_toCopy.mbShowCenter)
 ,mbShowDiameter(_toCopy.mbShowDiameter)
{ 
  SetCarrets( &mCircle,_toCopy.IsCarretsEnabled() );  
}
//---------------------------------------------------------------
dmCircleEdit::~dmCircleEdit()
{}
//-------------------------------------------------------
void  dmCircleEdit::SetCoordinates(const dmCircle& c)
{
  mCircle = c;
  UpdateObject();
}
//---------------------------------------------------------------
dmTool* dmCircleEdit::Clone( dmTool* _parent ) const
{
   return new dmCircleEdit(*this,_parent);
}
//---------------------------------------------------------------
void dmCircleEdit::OnUpdateRegion( dmRegion& _rgn )
{ 
  mCircle.BuildRegion(_rgn);
}
//----------------------------------------------------------------
void dmCircleEdit::ShowCenter( bool _st )
{
  O_INVALIDATE(this)
  mbShowCenter = _st;
}
//----------------------------------------------------------------
void dmCircleEdit::ShowDiameter( bool _st )
{
  O_INVALIDATE(this)
  mbShowDiameter = _st;
}
//----------------------------------------------------------------
void dmCircleEdit::OnDisplay( dmGraphics& gr ) const
{
  gr.DrawCircle(mCircle);
  if(mbShowDiameter) gr.DrawLine(mCircle[0],mCircle[1] );
  if(mbShowCenter) {
    dmPoint p = mCircle.Center();
    gr.DrawLine( p.x-8,p.y,p.x+9,p.y );
    gr.DrawLine( p.x,p.y-8,p.x,p.y+9 );
  }
}
//-------------------------------------------------------
