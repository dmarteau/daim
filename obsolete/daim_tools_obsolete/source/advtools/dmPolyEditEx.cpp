#define dmUseExtendedTools
#include "daim_tools.h"
#include "advtools/dmPolyEditEx.h"
#include "common/dmMetrics.h"

#define dmPolyEditEx_SCHEMA_VERSION 1
dmImplementClassInfo( dmPolyEditEx, dmAreaEdit, dmPolyEditEx_SCHEMA_VERSION );
//---------------------------------------------------------------
dmPolyEditEx::dmPolyEditEx(  dmTool* _parent )
: dmAreaEdit(_parent) 
 ,mbEditClose(true)
 ,mbEditPoints(true)
{  
  SetCarrets( &mPoly ); 
}
//---------------------------------------------------------------
dmPolyEditEx::dmPolyEditEx( bool _fill, bool _bEditClose, dmTool* _parent )
: dmAreaEdit(_fill,_parent)
  ,mbEditClose(_bEditClose)
  ,mbEditPoints(true)
{ 
  SetCarrets( &mPoly ); 
}
//---------------------------------------------------------------
dmPolyEditEx::dmPolyEditEx( const dmPolyEditEx& _toCopy, dmTool* _parent  )
: dmAreaEdit  (_toCopy,_parent ) 
 ,mPoly       (_toCopy.mPoly )
 ,mbEditClose (_toCopy.mbEditClose)
 ,mbEditPoints(_toCopy.mbEditPoints)
{  
  SetCarrets( &mPoly,_toCopy.IsCarretsEnabled()); 
}
//-------------------------------------------------------
void dmPolyEditEx::SetCoordinates(const dmPoly& p)
{
  mPoly = p;
  UpdateObject();
}
//---------------------------------------------------------------
dmPolyEditEx::~dmPolyEditEx()
{}
//---------------------------------------------------------------
void dmPolyEditEx::OnUpdateRegion( dmRegion& _rgn )
{
  mPoly.BuildRegion(_rgn); 
}
//---------------------------------------------------------------
void dmPolyEditEx::OnDisplay( dmGraphics& gr ) const
{
  gr.DrawPoly(mPoly);
}
//---------------------------------------------------------------
dmTool* dmPolyEditEx::Clone( dmTool* _parent ) const
{ 
  return new dmPolyEditEx(*this,_parent);
}
//---------------------------------------------------------------
bool dmPolyEditEx::OnMouseDown(const dmPoint& cursor,dm_uint _button,dm_uint _keyQual)
{
  if(IsNullObject()) 
  {
    if(mPoly.Size()==0)
    {
      mPoly.Clear();
      mPoly.Push_Back(cursor ); // on insere le premier point
      dmAreaEdit::OnMouseDown(cursor,_button,_keyQual);
      mPoly.Push_Back( cursor );    // on insere le point suivant
      SetCarretIndex( mPoly.Size()-1 );
    }
    return true;
  } 
  else if( (_keyQual & keyCtrl) && mbEditPoints) 
    return true;

  return dmAreaEdit::OnMouseDown(cursor,_button,_keyQual); 
}
//---------------------------------------------------------------
bool dmPolyEditEx::OnMouseMove(const dmPoint& cursor,dm_uint _button,dm_uint _keyQual)
{
  if(IsEditing()) 
  {
    if(IsNullObject()) dmAreaEdit::OnMouseMove(cursor,_button|buttonLeft,_keyQual);
    else dmAreaEdit::OnMouseMove(cursor,_button,_keyQual);
  } return true;
}
//---------------------------------------------------------------
bool dmPolyEditEx::OnMouseUp(const dmPoint& cursor,dm_uint _button,dm_uint _keyQual)
{
  if(IsEditing()) 
  {
    if(IsNullObject()) 
    {
      mPoly.SetMoveState(false);
      dmSurface gs = ObtainGraphics();
      if(gs) {
        gs->DrawLine(*(mPoly.End()-2),*(mPoly.End()-1)); 
        mPoly.Push_Back( cursor );    // Insert next point
        SetCarretIndex( mPoly.Size()-1 );
      }
      return true;
    } 
    else  
      return dmAreaEdit::OnMouseUp(cursor,_button,_keyQual);
  } 
  else 
  {
    if(!IsNullObject() && (_keyQual & keyCtrl) && mbEditPoints) 
    {
      dmLOG("CARRET\n");
      
      // Are we on a carret ?
      int index = mPoly.CarretIndex(cursor,mPoly.ScaledCarretSize(GetGraphics()));
      if(index>=0) 
      {
        if(mPoly.Size()<=3) return false;

        // WARNING : if the polygon is closed, we must take care when removing
        // the first or the last point

        bool was_closed = mPoly.Closed();
        mPoly.Remove(index);
        if(was_closed && !mPoly.Closed()) { // 
          if(index==0) mPoly.Back() = mPoly[0];
          else         mPoly[0]     = mPoly.Back();
        }
        UpdateObject();        
      } else {
        dmPoint ptNew;
        index = GetPoint(cursor,ptNew);
        if(index >= 0) {
          mPoly.Insert(index+1,ptNew);
          UpdateObject();
        }
      }
    }
  }
  return true; // handled 
}
//---------------------------------------------------------------
bool dmPolyEditEx::OnDblClick(const dmPoint&,dm_uint _button,dm_uint _keyQual)
{
  if(IsEditing() && IsNullObject()) 
  {
    mPoly.Pop_Back(); // cause deux OnLButtonUp appelé avant OnLButtonDblClk

    if( ((_keyQual & keyShift)||mbEditClose) && mPoly.Size()>2) 
       mPoly.Push_Back(mPoly[0]);

    MoveBy(0,0,true);
    SetEditMode(false);
  }
  // Seems that we got an OnLButtonUp after OnLButtonDblClk;
  return true;
}
//------------------------------------------------------------
struct __HIT_TEST
{
  std::vector<dmPoint> _tsts;
  dmPoint              _pwhere;
  int                  _size;
  
  __HIT_TEST(const dmPoint& p,int s) : _pwhere(p),_size(s) {}

  void clear() { _tsts.clear(); }
  bool empty() { return _tsts.empty(); }

  dmPoint& hitpoint()
  { 
    int found = -1;    
    for(size_t min=_size,i=0,m;i<_tsts.size();++i) {
      m = dmMetrics::Absolute_Distance(_tsts[i],_pwhere);
      if(m < min ) { min = m; found = i; }
    }
    return (found>=0 ? _tsts[found] : _pwhere);
  }     

  void operator()( int x,int y) 
  {
    if( (_pwhere.x<=x+_size) &&
        (_pwhere.x>=x-_size) &&
        (_pwhere.y<=y+_size) &&
        (_pwhere.y>=y-_size) ) {
           _tsts.push_back(dmPoint(x,y));
        }
  }
};
//---------------------------------------------------------------
int dmPolyEditEx::GetPoint( const dmPoint& pwhere, dmPoint& pt )
{
  int cnt = mPoly.Size()-1;
  __HIT_TEST op(pwhere,4);
  for(int i=1;i<=cnt;++i) {
    op.clear();
    if(!dmDigitalLine( mPoly[i-1],mPoly[i],op).empty()) {
      pt = op.hitpoint();
      return i-1;
    }
  }
  return -1;
}
//---------------------------------------------------------------
