#define dmUseExtendedTools
#include "daim_tools.h"
#include "advtools/dmRectShapeEx.h"
#include "common/dmMetrics.h"
//---------------------------------------------------------
dmRectShapeEx::dmRectShapeEx() 
: dmShape(8)
 ,mShape(5)
 ,mAngle(0)
 ,mFixedsize(false) 
{}
//---------------------------------------------------------
dmRectShapeEx::dmRectShapeEx( const dmRect& _r, dm_real _angle )
: dmShape(8)
 ,mShape(5)
 ,mFixedsize(false)
{ 
  SetShape( _r, _angle );
}
//---------------------------------------------------------
dmRectShapeEx::dmRectShapeEx( const dmRectShapeEx& s )
: dmShape(s),
  mShape(s.mShape)
 ,mRect(s.mRect)
 ,mAngle(s.mAngle)
 ,mFixedsize(s.mFixedsize)
{}
//-----------------------------------------------------------------------------
void dmRectShapeEx::SetShape( const dmRect& _r, dm_real _angle, bool _update )
{
  mRect  = _r;
  mAngle = _angle;
  if(_update) DoUpdate();
}
//-----------------------------------------------------------------------------
void dmRectShapeEx::SetRect( const dmRect& _r, bool _update )
{
  mRect  = _r;
  if(_update) DoUpdate();
}
//-----------------------------------------------------------------------------
void dmRectShapeEx::SetOrientation( dm_double _angle, bool _update )
{
  mAngle = _angle;
  if(_update) DoUpdate();
}
//---------------------------------------------------------
int dmRectShapeEx::Initialize( const dmPoint& p )
{
  dmShape::Initialize(p);
  mRect = dmRect(p,1,1);
  return 0;
}
//-----------------------------------------------------------------------------
// Compute rotating object
//-----------------------------------------------------------------------------
void dmRectShapeEx::DoUpdate()
{
  dm_double c = cos( mAngle );
  dm_double s = sin( mAngle );

  dm_double dx1,dy1,dx2,dy2;

  dm_double a = mRect.Width() /2;
  dm_double b = mRect.Height()/2;

  dx1 =  a * c;
  dy1 =  a * s;

  dx2 = - b * s;
  dy2 =   b * c;

  dm_double cx = (mRect.Left() + mRect.Right() )/2.0;
  dm_double cy = (mRect.Top()  + mRect.Bottom())/2.0;

  mCarrets[0].x = static_cast<dm_int>(daim::round(cx - dx1 - dx2));
  mCarrets[0].y = static_cast<dm_int>(daim::round(cy - dy1 - dy2));
  mCarrets[2].x = static_cast<dm_int>(daim::round(cx + dx1 + dx2));
  mCarrets[2].y = static_cast<dm_int>(daim::round(cy + dy1 + dy2));

  mCarrets[1].x = static_cast<dm_int>(daim::round(cx + dx1 - dx2));
  mCarrets[1].y = static_cast<dm_int>(daim::round(cy + dy1 - dy2));
  mCarrets[3].x = static_cast<dm_int>(daim::round(cx - dx1 + dx2));
  mCarrets[3].y = static_cast<dm_int>(daim::round(cy - dy1 + dy2));

  if(mCarrets[0].y == mCarrets[1].y || mCarrets[0].x == mCarrets[1].x) mAngle = 0;

  for(int i=0;i<4;++i)
     mShape[i] = mCarrets[i];

  mShape[4] = mShape[0];

  mCarrets[4] = (mShape[0]+mShape[1])/2;
  mCarrets[5] = (mShape[1]+mShape[2])/2; 
  mCarrets[6] = (mShape[2]+mShape[3])/2; 
  mCarrets[7] = (mShape[3]+mShape[0])/2; 
}
//-----------------------------------------------------------------------------
void dmRectShapeEx::DoFrame( dmGraphics& gr , int _index, int drawFlags ) const
{
  gr.DrawLine(mCarrets[0],mCarrets[1],drawFlags);
  gr.DrawLine(mCarrets[1],mCarrets[2],drawFlags);
  gr.DrawLine(mCarrets[2],mCarrets[3],drawFlags);
  gr.DrawLine(mCarrets[3],mCarrets[0],drawFlags);
}
//-----------------------------------------------------------------------------
void dmRectShapeEx::DoMove(int _index, int dx, int dy )
{ 
  if(_index < 0 || RigidMode()) { 
    mRect.Translate(dx,dy);
    _index = -1; 
  }

  dmShape::DoMove(_index,dx,dy);
  if(_index == -1) 
  {
    for(int i=0;i<4;++i)
       mShape[i] = mCarrets[i];

    mShape[4] = mShape[0];
  }
}
//-----------------------------------------------------------------------------
void dmRectShapeEx::DoMoveIndex(int _index, int dx, int dy )
{
  if(!mFixedsize) 
  {
    if(_index<4) 
    {
      mCarrets[_index].x += dx;
      mCarrets[_index].y += dy;

      // vecteur unitaire
      dm_double dux = cos(mAngle);
      dm_double duy = sin(mAngle);
      dm_double L;
      dmPoint p; // point milieu

      switch(_index) {
       case 0: 
       case 2: 
         p = (mCarrets[0]+mCarrets[2]);
         L = (mCarrets[2].x-mCarrets[0].x)*dux + (mCarrets[2].y-mCarrets[0].y)*duy;
         mCarrets[1].x = mCarrets[0].x + static_cast<int>( daim::round(dux * L) );
         mCarrets[1].y = mCarrets[0].y + static_cast<int>( daim::round(duy * L) );
         mCarrets[3] = p - mCarrets[1];
         break;
       case 1: 
       case 3: 
         p = (mCarrets[1]+mCarrets[3]);
         L = (mCarrets[3].x-mCarrets[1].x)*dux + (mCarrets[3].y-mCarrets[1].y)*duy;
         mCarrets[0].x = mCarrets[1].x + (int)daim::round(dux * L);
         mCarrets[0].y = mCarrets[1].y + (int)daim::round(duy * L);
         mCarrets[2] = p - mCarrets[0];
         break;      
      }
  
      dm_double W = dmMetrics::Euclidian_Distance(mCarrets[0],mCarrets[1]);
      dm_double H = dmMetrics::Euclidian_Distance(mCarrets[1],mCarrets[2]);

      mRect.top_left.x     = static_cast<dm_int>(daim::round((p.x - W)/2.0)); 
      mRect.top_left.y     = static_cast<dm_int>(daim::round((p.y - H)/2.0)); 
      mRect.bottom_right.x = static_cast<dm_int>(daim::round((p.x + W)/2.0)); 
      mRect.bottom_right.y = static_cast<dm_int>(daim::round((p.y + H)/2.0)); 

      for(int i=0;i<4;++i)
         mShape[i] = mCarrets[i];

      mShape[4] = mShape[0];

      mCarrets[4] = (mShape[0]+mShape[1])/2;
      mCarrets[5] = (mShape[1]+mShape[2])/2; 
      mCarrets[6] = (mShape[2]+mShape[3])/2; 
      mCarrets[7] = (mShape[3]+mShape[0])/2;

      return; 

    } else if(mAngle == 0) {
      switch(_index) {
        case 4: 
          mCarrets[4].y += dy;
          mCarrets[0].y += dy;
          mCarrets[1].y += dy;
          mCarrets[5].y = (mCarrets[1].y+mCarrets[2].y)/2;
          mCarrets[7].y = mCarrets[5].y;
          break;
        case 5: 
          mCarrets[5].x += dx;
          mCarrets[1].x += dx;
          mCarrets[2].x += dx;
          mCarrets[4].x = (mCarrets[0].x+mCarrets[1].x)/2;
          mCarrets[6].x = mCarrets[4].x;        
          break;
        case 6: 
          mCarrets[6].y += dy;
          mCarrets[2].y += dy;
          mCarrets[3].y += dy;
          mCarrets[5].y = (mCarrets[1].y+mCarrets[2].y)/2;
          mCarrets[7].y = mCarrets[5].y;
          break;
        case 7: 
          mCarrets[7].x += dx;
          mCarrets[3].x += dx;
          mCarrets[0].x += dx;
          mCarrets[4].x = (mCarrets[0].x+mCarrets[1].x)/2;
          mCarrets[6].x = mCarrets[4].x;        
          break;
      }

      for(int i=0;i<4;++i)
         mShape[i] = mCarrets[i];

      mShape[4] = mShape[0];
      mRect     = dmRect(mShape[0],mShape[2]); 
      return;
    }
  } 
  DoUpdate();  
}
//-----------------------------------------------------------------------------
void dmRectShapeEx::BuildRegion( dmRegion& _rgn) const
{  
  _rgn.SetRoi(mShape); 
}
//-----------------------------------------------------------------------------
