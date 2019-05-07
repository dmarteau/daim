#define dmUseExtendedTools
#include "daim_tools.h"
#include "advtools/dmPtsCircleShape.h"

#include <limits>
//-----------------------------------------------------------------------------
dmPtsCircleShape::dmPtsCircleShape() 
: dmShape(3) 
{}
//-----------------------------------------------------------------------------
dmPtsCircleShape::dmPtsCircleShape( const dmPoint& p1, const dmPoint& p2, const dmPoint& p3 ) 
: dmShape(3)
 ,_valid(false)
{
  SetPoints(p1,p2,p3);
}
//-----------------------------------------------------------------------------
dmPtsCircleShape::dmPtsCircleShape( const dmPtsCircleShape& s) 
: dmShape(s)
 ,_valid(s._valid) 
{}
//---------------------------------------------------------
void dmPtsCircleShape::SetPoints( const dmPoint& p1, const dmPoint& p2, const dmPoint& p3 ) 
{
  mCarrets[0] = p1;
  mCarrets[1] = p2;
  mCarrets[2] = p3;
  _valid = false;
  SetMoveState(false);
}
//-----------------------------------------------------------------------------
void dmPtsCircleShape::DoFrame( dmGraphics& gr, int _index, int drawFlags ) const
{
  gr.DrawCircle(*this,drawFlags);
}
//-----------------------------------------------------------------------------
void dmPtsCircleShape::BuildRegion( dmRegion& _rgn) const
{
  dmCircle c = *this;
  if(!_valid) _rgn.SetRoi( mCarrets );
  _rgn.SetRoi( c ); 
}
//-----------------------------------------------------------------------------
dmPtsCircleShape::operator dmCircle() const
{
  dm_double a1,b1,c1; // paramètres des équations de droites définies par
  dm_double a2,b2,c2; // MP.AB = (OP.AB) - (OM.AB) = 0 ( M == milieu de AB )
  dm_double rx,ry,v;

  a1 = mCarrets[1].x - mCarrets[0].x;  
  b1 = mCarrets[1].y - mCarrets[0].y;  
  c1 = - 0.5 * ( ((mCarrets[1].x + mCarrets[0].x) * a1) + ((mCarrets[1].y + mCarrets[0].y) * b1) );

  a2 = mCarrets[2].x - mCarrets[1].x;
  b2 = mCarrets[2].y - mCarrets[1].y;
  c2 = - 0.5 * ( ((mCarrets[2].x + mCarrets[1].x) * a2) + ((mCarrets[2].y + mCarrets[1].y) * b2) );
    
  v   = a2*b1 - a1*b2;
  if(fabs(v) > std::numeric_limits<dm_real>::epsilon()) {
    rx = (b2*c1 - b1*c2) / v;
    ry = (a1*c2 - a2*c1) / v;
    _valid = true;
  } else {
    _valid = false;
    rx = mCarrets[1].x;
    ry = mCarrets[1].y;
  }
     
  a1 = mCarrets[1].x - rx;
  a2 = mCarrets[1].y - ry;

  return  dmCircle( 
        static_cast<dm_int>( daim::round(rx) ),
        static_cast<dm_int>( daim::round(ry) ),
        static_cast<dm_int>( daim::round(sqrt( a1*a1 + a2*a2)) )
  );
}
//---------------------------------------------------------------
