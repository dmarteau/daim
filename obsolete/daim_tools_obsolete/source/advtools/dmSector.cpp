#define dmUseExtendedTools
#include "daim_tools.h"
#include "advtools/dmSector.h"
#include "advtools/dmDrawUtils.h"

//---------------------------------------------------------------
#define dmSector_SCHEMA_VERSION 1
dmImplementClassInfo( dmSector, dmPolyEdit, dmSector_SCHEMA_VERSION );
//---------------------------------------------------------------
dmSector::dmSector( dmTool* _parent )
: dmPolyEdit(_parent) 
{}
//---------------------------------------------------------------
dmSector::dmSector( const dmSector& _toCopy, dmTool* _parent )
: dmPolyEdit(_toCopy,_parent) 
{}
//---------------------------------------------------------------
void dmSector::OnDisplay( dmGraphics& gr ) const
{
  dmPolyEdit::OnDisplay(gr);
  dmTk::DrawTick( gr, mPoly[1], mPoly[0], 8 );
  dmTk::DrawTick( gr, mPoly[1], mPoly[2], 8 );
}
//---------------------------------------------------------------
dmTool* dmSector::Clone( dmTool* _parent ) const
{
  return new dmSector(*this,_parent);
}
//---------------------------------------------------------------
bool dmSector::OnMouseUp(const dmPoint& p,dm_uint _button,dm_uint _keyQual)
{
  if(IsNullObject()) {
    if(mPoly.Size()==3) 
    {
       MoveBy(0,0,true);
       SetEditMode(false);
       return true; // handled
    }
  }
  return dmPolyEdit::OnMouseUp(p,_button,_keyQual);
}
//---------------------------------------------------------------
