#define dmUseExtendedTools
#include "daim_tools.h"
#include "advtools/dmRectEditEx.h"
#include "common/dmMetrics.h"

//---------------------------------------------------------------
#define dmRectEditEx_SCHEMA_VERSION 1
dmImplementClassInfo( dmRectEditEx,dmAreaEdit,dmRectEditEx_SCHEMA_VERSION );
//---------------------------------------------------------------

dmRectEditEx::dmRectEditEx( dmTool* _parent ) 
: dmAreaEdit(_parent)
{  
  SetCarrets( &mRect ); 
}
//---------------------------------------------------------------
dmRectEditEx:: dmRectEditEx( const dmRect& r, dm_real angle,  dmTool* _parent )
: dmAreaEdit(_parent)
 ,mRect(r,angle)
{
  SetCarrets( &mRect );
}
//---------------------------------------------------------------
dmRectEditEx:: dmRectEditEx( const dmRectEditEx& _toCopy, dmTool* _parent )
: dmAreaEdit(_toCopy,_parent)
 ,mRect(_toCopy.mRect)
{
  SetCarrets( &mRect );
} 
//---------------------------------------------------------------
dmRectEditEx::~dmRectEditEx() 
{}
//-------------------------------------------------------
bool dmRectEditEx::OnMouseDown(const dmPoint& cursor,dm_uint _button,dm_uint _keyQual)
{
  mStore = mRect.GetOrientation();
  return dmAreaEdit::OnMouseDown(cursor,_button,_keyQual);
}
//-------------------------------------------------------
dmPoint dmRectEditEx::DoMouseEvent(const dmPoint& cursor,dm_uint _button,dm_uint _keyQual)
{ 
  if(CarretIndex()>=0)
  {
    if(_keyQual & keyCtrl)
    {
      dmPoint p = mRect.Center();
      mRect.SetOrientation( mStore + dmMetrics::Angle(MouseDown()-p,cursor-p), false);
      mRect.SetFixedSize(true);
      MoveBy(DeltaXY(cursor),(_button & buttonLeft) == 0);
      return MouseDown();
    }
    else if((_keyQual & keyShift) && mRect.GetOrientation()==0) 
    {
      mRect.SetFixedSize(false);

      dmPoint p  =  mRect[(CarretIndex() + 2) % 4]; 
      dmPoint pd = cursor - p;

      int wx = daim::abs(pd.x);
      int wy = daim::abs(pd.y);

      if( wx < wy ) pd.y = daim::sgn(pd.y)*wx; 
      else          pd.x = daim::sgn(pd.x)*wy;

      p += pd;
      MoveBy(p - mRect[CarretIndex()],(_button & buttonLeft)==0);
      return p;
    } 
    else
    {
      mRect.SetFixedSize(false);
      return dmAreaEdit::DoMouseEvent(cursor,_button,_keyQual);
    }
  } 
  else return dmAreaEdit::DoMouseEvent(cursor,_button,_keyQual);
}
//---------------------------------------------------------------
void dmRectEditEx::OnUpdateRegion( dmRegion& rgn )
{ 
  mRect.BuildRegion(rgn);
}
//---------------------------------------------------------------
void dmRectEditEx::SetOrientation( dm_double _orientation )
{
  mRect.SetOrientation(_orientation);
  UpdateObject();
}
//---------------------------------------------------------------
void dmRectEditEx::SetRectangle( const dmRect& _rect )
{
  mRect.SetRect(_rect);
  UpdateObject();
}
//----------------------------------------------------------------
void dmRectEditEx::OnDisplay( dmGraphics& gr ) const
{
  mRect.DoFrame(gr,-1,dmTk::eDrawDefault);
}
//---------------------------------------------------------------
dmTool* dmRectEditEx::Clone( dmTool* _parent ) const
{
  return new dmRectEditEx(*this,_parent);
}
//-------------------------------------------------------

