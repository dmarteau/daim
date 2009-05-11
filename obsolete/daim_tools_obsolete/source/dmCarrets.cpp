
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

#define dmUseServicesMngr
#include "daim_tools.h"
#include "dmStyleManager.h"

#define dmShape_EXPORT_SCHEMA 1
#define DRAW_LAYER dmTk::eDrawBorder|dmTk::eDrawLayer
#define DRAW_ERASE dmTk::eDrawBorder|dmTk::eDrawLayerErase

//---------------------------------------------------------
// Default Carrets Drawing function
//---------------------------------------------------------
static void __DfltDrawFunc( dmGraphics& gr,int i, const dmShape& _shape )
{
//size_t carret_size = _shape.CarretSize();
//size_t carret_size = (int)((_shape.CarretSize()*100.0)/gr.GetPageScale() + 0.5);

  size_t carret_size = _shape.ScaledCarretSize(&gr);
  size_t sz = (2*carret_size+1);

  gr.DrawRectangle ( dmRect( _shape[i].x-carret_size,_shape[i].y-carret_size,sz,sz) );
}
//---------------------------------------------------------
// Shape Handler
//---------------------------------------------------------

#define DFLT_CARRET_SIZE 3
 
//----------------------------------------------------------------------
dmShape::dmShape( const dmShape& s  )
:mDrawCarret(s.mDrawCarret)
,mPenAttrs(s.mPenAttrs)
,mRoiAttrs(NULL)
,mStates( s.mStates )
,mCarretSize(s.mCarretSize)
,mCarrets(s.mCarrets)
{
  mRoiAttrs = &dmGetDrawStylePen(_TXT("*roi"));
}
//----------------------------------------------------------------------
dmShape::dmShape() 
:mDrawCarret(&__DfltDrawFunc)
,mPenAttrs(NULL)
,mRoiAttrs(NULL)
,mStates(csVisible) 
,mCarretSize(DFLT_CARRET_SIZE)
{
  mPenAttrs = &dmGetDrawStylePen(_TXT("*carrets"));
  mRoiAttrs = &dmGetDrawStylePen(_TXT("*roi"));
}
//----------------------------------------------------------------------
dmShape::dmShape( size_t n ) 
:mDrawCarret(&__DfltDrawFunc)
,mPenAttrs(NULL)
,mRoiAttrs(NULL)
,mStates(csVisible)
,mCarretSize(DFLT_CARRET_SIZE)
,mCarrets(n)
{
  mPenAttrs = &dmGetDrawStylePen(_TXT("*carrets"));
  mRoiAttrs = &dmGetDrawStylePen(_TXT("*roi"));
  Initialize( dmPoint(0,0) );
}
//----------------------------------------------------------------------
dmShape::dmShape( const dmPoly& p) 
:mDrawCarret(&__DfltDrawFunc)
,mPenAttrs(NULL)
,mRoiAttrs(NULL)
,mStates(csVisible)
,mCarretSize(DFLT_CARRET_SIZE)
,mCarrets(p)
{
  mPenAttrs = &dmGetDrawStylePen(_TXT("*carrets"));
  mRoiAttrs = &dmGetDrawStylePen(_TXT("*roi"));
}
//----------------------------------------------------------------------
dmShape::~dmShape() 
{ 
  mCarrets.Clear(); 
}
//----------------------------------------------------------------------
dmShape::drawfunc dmShape::SetDrawFunc( dmShape::drawfunc _func )
{
  drawfunc _old = mDrawCarret;
  mDrawCarret = _func;
  if(mDrawCarret==NULL) mDrawCarret = __DfltDrawFunc;
  return _old;
}
//----------------------------------------------------------------------
dmPenAttributs* dmShape::SetPenAttrs( dmPenAttributs* _pen )
{
  dmPenAttributs* _oldp = mPenAttrs;
  mPenAttrs = (_pen ? _pen : &dmGetDrawStylePen(_TXT("*carrets")));
  return _oldp;
}
//----------------------------------------------------------------------
dmPenAttributs* dmShape::SetLineAttrs( dmPenAttributs* _pen )
{
  dmPenAttributs* _oldp = mRoiAttrs;
  mRoiAttrs = (_pen ? _pen : &dmGetDrawStylePen(_TXT("*roi")));
  return _oldp;
}

//----------------------------------------------------------------------
int dmShape::Initialize( const dmPoint& p )
{
  SetMoveState(false);
  for(iterator it = Begin(); it != End(); ++it ) { *it = p; }
  return 0;
}
//---------------------------------------------------------
void dmShape::DisplayCarrets(dmGraphics& gr,int _index)  const
{
  if(Visible() && gr.ObtainGraphics() ) 
  {
    gr.SetPenAttrs ( *mPenAttrs );
    if(_index<0) 
    {
      int idx = 0;
      for(const_iterator it=Begin();it!=End();++it,++idx) 
       (*mDrawCarret)(gr,idx,*this); 

    } else if(_index < (int)mCarrets.Size()) {
       (*mDrawCarret)(gr,_index,*this);
    }
    gr.ReleaseGraphics();
  }  
}
//---------------------------------------------------------
// Move carrets in XOR mode
//---------------------------------------------------------
void dmShape::MoveCarret(dmGraphics* gr, int _index, int dx, int dy )
{  
  if(gr) 
  {
    if(!IsMoving() && !mStates.Contains(csHoldGraphics))
    {                                   // We do an ObtainGraphics() in order to initialize
      gr->ObtainGraphics();             // the graphics so that following calls to 
      mStates.Set(csHoldGraphics,true); // ObtainGraphics() will not (re)allocate ressources
    }                                   // A call to EndMove() will call the corresponding
                                        // ReleaseGraphics() 

    gr->ObtainGraphics(eDrawLayer); 
    gr->SetPenAttrs( *mRoiAttrs );

    // TODO: XOR_MODE

    // Erase previous drawing

    if(IsMoving() && FrameMove())         // We are moving and the frame
      DoFrame( *gr, _index, DRAW_ERASE ); // should be displaying (FrameMove() test)

    SetMoveState(true);
    DoMove(_index,dx,dy);             // Move the carrets

    if(FrameMove()) DoFrame( *gr, _index, DRAW_LAYER ); 
    gr->ReleaseGraphics();

  } else
    DoMove(_index,dx,dy); // Only apply move to the carrets
}
//---------------------------------------------------------
// Terminate a move by erasing the last drawing and
// restore graphics state
//---------------------------------------------------------
void dmShape::EndMove(dmGraphics* gr, int _index, int dx, int dy )
{
  if(IsMoving()) {
    SetMoveState(false);
    MoveCarret(gr,_index,dx,dy);
    SetMoveState(false);
    
    if(gr && mStates.Contains(csHoldGraphics)) {
      gr->ReleaseGraphics();
      mStates.Set(csHoldGraphics,false);
    }
  }  
}
//---------------------------------------------------------
// Redraw shape frame in XOR/Layer mode 
//---------------------------------------------------------
void dmShape::RedrawFrame(dmGraphics* gr) const
{
  if(gr->ObtainGraphics(eDrawLayer))
  { 
    gr->SetPenAttrs( *mRoiAttrs );
    DoFrame( *gr, -1, DRAW_LAYER );
    gr->ReleaseGraphics();
  }        
}
//---------------------------------------------------------
void dmShape::DoMove(int _index , int dx, int dy )
{
  if(_index >= 0 && !RigidMode()) { DoMoveIndex(_index,dx,dy); } 
  else {
    for(iterator it = Begin(); it != End(); ++it ) {
     (*it).x += dx;
     (*it).y += dy;
    } 
  }  
}
//---------------------------------------------------------
void dmShape::DoMoveIndex(int _index, int dx, int dy) 
{
  mCarrets[_index].x += dx;
  mCarrets[_index].y += dy;
}
//---------------------------------------------------------
int dmShape::CarretIndex( const dmPoint& ptWhere , int hitTest) const
{ 
  int cnt = 0;
  for (const_iterator it=Begin(); it!=End(); ++it, ++cnt ) {
    const dmPoint& p = *it; 
    if( (ptWhere.x<=p.x+hitTest)&&
        (ptWhere.x>=p.x-hitTest)&&
        (ptWhere.y<=p.y+hitTest)&&
        (ptWhere.y>=p.y-hitTest) ) return cnt;
  }
  return -1;
}
//---------------------------------------------------------
size_t dmShape::ScaledCarretSize( dmGraphics* gr ) const
{ 
  return gr ? (size_t)((mCarretSize*100.0)/gr->GetPageScale() + 0.5) : mCarretSize;
}
//-----------------------------------------------------------------------------
// Rectangle
//-----------------------------------------------------------------------------
dmRectShape::dmRectShape() : dmShape(8) 
{
  operator=(dmRect(0,0,1,1));
}
//-----------------------------------------------------------------------------
dmRectShape::dmRectShape( const dmRect& r) : dmShape(8)
{ 
  operator=(r);
}
//-----------------------------------------------------------------------------
dmRectShape::dmRectShape( const dmRectShape& s) : dmShape(s) {}
//---------------------------------------------------------
// Rectangle : 
// index des points :
//  [0]--[4]--[1]
//   |         |
//  [7]       [5]
//   |         |
//  [3]--[6]--[2]
//----------------------------------------------------------------------
dmRectShape& dmRectShape::operator=( const dmRect& r)
{
  mCarrets[0] = r.TopLeft();
  mCarrets[1] = r.TopRight();
  mCarrets[2] = r.BottomRight();
  mCarrets[3] = r.BottomLeft();

  mCarrets[4] = dmPoint((mCarrets[0].x+mCarrets[1].x)/2,mCarrets[0].y); 
  mCarrets[5] = dmPoint(mCarrets[1].x,(mCarrets[1].y+mCarrets[2].y)/2); 
  mCarrets[6] = dmPoint(mCarrets[4].x,mCarrets[3].y);
  mCarrets[7] = dmPoint(mCarrets[3].x,mCarrets[5].y);
  return *this;
}
//-----------------------------------------------------------------------------
void dmRectShape::DoMoveIndex( int _index, int dx, int dy )
{
  if(_index < 4) {
    mCarrets[_index].x += dx;
    mCarrets[_index].y += dy;
    switch(_index) {
     case 0: 
       mCarrets[3].x += dx;  
       mCarrets[1].y += dy;
       break;
     case 1: 
       mCarrets[2].x += dx;
       mCarrets[0].y += dy;
       break;
     case 2: 
       mCarrets[1].x += dx;
       mCarrets[3].y += dy;
       break;
     case 3: 
       mCarrets[0].x += dx;
       mCarrets[2].y += dy;
       break;
    }
    mCarrets[4] = dmPoint((mCarrets[0].x+mCarrets[1].x)/2,mCarrets[0].y); 
    mCarrets[5] = dmPoint(mCarrets[1].x,(mCarrets[1].y+mCarrets[2].y)/2); 
    mCarrets[6] = dmPoint(mCarrets[4].x,mCarrets[3].y);
    mCarrets[7] = dmPoint(mCarrets[3].x,mCarrets[5].y);   
  } else {
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
  }
}
//-----------------------------------------------------------------------------
void dmRectShape::DoFrame( dmGraphics& gr, int _index, int drawFlags ) const
{
  gr.DrawRectangle( *this , drawFlags ); // affiche 
}
//-----------------------------------------------------------------------------
void dmRectShape::BuildRegion( dmRegion& _rgn) const
{
  _rgn.SetRectRoi( *this ); 
}
//-----------------------------------------------------------------------------
// Line
//-----------------------------------------------------------------------------
dmLineShape::dmLineShape() : dmShape(2) {}
//-----------------------------------------------------------------------------
dmLineShape::dmLineShape( const dmLine& l)  : dmShape(2)
{
  operator=(l);
}
//-----------------------------------------------------------------------------
dmLineShape::dmLineShape( const dmLineShape& s) : dmShape(s)  
{}
//---------------------------------------------------------
int dmLineShape::Initialize( const dmPoint& p )
{
  dmShape::Initialize(p);
  return 1;
}
//---------------------------------------------------------
// Line : 
// index des points : [0]----[1]
//---------------------------------------------------------
dmLineShape& dmLineShape::operator=( const dmLine& l)
{
  mCarrets[0] = l.start;
  mCarrets[1] = l.end;
  return *this;
}
//-----------------------------------------------------------------------------
void dmLineShape::DoFrame( dmGraphics& gr, int _index, int drawFlags ) const
{
  gr.DrawLine( *this , drawFlags ); 
}
//-----------------------------------------------------------------------------
void dmLineShape::BuildRegion( dmRegion& _rgn) const
{
  _rgn.SetRoi( operator dmLine() ); 
}
//-----------------------------------------------------------------------------
dmLineShape::operator  dmLine() const
{
  return dmLine( mCarrets[0], mCarrets[1] ); 
}
//-----------------------------------------------------------------------------
// Circle
//-----------------------------------------------------------------------------
dmCircleShape::dmCircleShape() : dmShape(2) {}
//-----------------------------------------------------------------------------
dmCircleShape::dmCircleShape( const dmCircle& c) : dmShape(2) 
{
  operator=(c); 
}
//-----------------------------------------------------------------------------
dmCircleShape::dmCircleShape( const dmCircleShape& s) : dmShape(s) {}
//---------------------------------------------------------
dmCircleShape& dmCircleShape::operator=( const dmCircle& c )
{
  mCarrets[0] = dmPoint(c.center.x-c.r,c.center.y);
  mCarrets[1] = dmPoint(c.center.x+c.r,c.center.y);
  SetMoveState(false);
  return *this;
}
//-----------------------------------------------------------------------------
void dmCircleShape::DoMoveIndex( int _index, int dx, int dy )
{
  mCarrets[_index].x += dx;
  mCarrets[_index].y += dy;
  mCarrets[1-_index].x -= dx;
  mCarrets[1-_index].y -= dy;
}
//-----------------------------------------------------------------------------
void dmCircleShape::DoFrame( dmGraphics& gr, int _index, int drawFlags ) const
{
  gr.DrawLine(mCarrets[0],mCarrets[1], drawFlags);
  gr.DrawCircle( *this , drawFlags );
}
//-----------------------------------------------------------------------------
void dmCircleShape::BuildRegion( dmRegion& _rgn) const
{
  _rgn.SetRoi( dmCircle( Center(), Radius() ) ); 
}
//-----------------------------------------------------------------------------
dmCircleShape::operator dmCircle() const
{
  return  dmCircle( Center(), Radius() );
}
//-----------------------------------------------------------------------------
dmPoint dmCircleShape::Center() const 
{ 
 return dmPoint( ( mCarrets[0].x + mCarrets[1].x)/2,  
                 ( mCarrets[0].y + mCarrets[1].y)/2 );
}
//-----------------------------------------------------------------------------
unsigned long dmCircleShape::Radius() const 
{
  dm_double dx = (mCarrets[1].x - mCarrets[0].x)/2.0f;
  dm_double dy = (mCarrets[1].y - mCarrets[0].y)/2.0f;
  return static_cast<long>(sqrt( dx*dx + dy*dy ) + 0.5);
}
//-----------------------------------------------------------------------------
// Ellipse
//-----------------------------------------------------------------------------
dmEllipseShape::dmEllipseShape() : dmShape(8) {}
//-----------------------------------------------------------------------------
dmEllipseShape::dmEllipseShape( const dmEllipse& e) : dmShape(8) 
{
  operator=(e);
}
//-----------------------------------------------------------------------------
dmEllipseShape::dmEllipseShape( const dmEllipseShape& s) : dmShape(s) {}
//---------------------------------------------------------
// Ellipse : cf cercle
//---------------------------------------------------------
dmEllipseShape& dmEllipseShape::operator=( const dmEllipse& e )
{
  mCarrets[0] = dmPoint(e.center.x,e.center.y+e.ry);
  mCarrets[1] = dmPoint(e.center.x+e.rx,e.center.y);
  mCarrets[2] = dmPoint(e.center.x,e.center.y-e.ry);
  mCarrets[3] = dmPoint(e.center.x-e.rx,e.center.y);
  mCarrets[4] = dmPoint(e.center.x-e.rx,e.center.y-e.ry);
  mCarrets[5] = dmPoint(e.center.x+e.rx,e.center.y-e.ry);
  mCarrets[6] = dmPoint(e.center.x+e.rx,e.center.y+e.ry);
  mCarrets[7] = dmPoint(e.center.x-e.rx,e.center.y+e.ry);
  return *this;
}
//-----------------------------------------------------------------------------
void dmEllipseShape::DoMoveIndex( int _index, int dx, int dy )
{
  if(_index > (int)Size() ) 
  {
    mCarrets[0].y -= dy;
    mCarrets[1].x -= dx;
    mCarrets[2].y += dy;
    mCarrets[3].x += dx;  

    mCarrets[4] = dmPoint(mCarrets[3].x,mCarrets[0].y);
    mCarrets[5] = dmPoint(mCarrets[1].x,mCarrets[0].y);
    mCarrets[6] = dmPoint(mCarrets[1].x,mCarrets[2].y);
    mCarrets[7] = dmPoint(mCarrets[3].x,mCarrets[2].y);

  } else {
    if(_index<4) {
      switch(_index) {
        case 0: dy = -dy;
        case 2: {
          mCarrets[0].y -= dy;
          mCarrets[2].y += dy;
        } break;
        case 1: dx = -dx;
        case 3: {
          mCarrets[1].x -= dx;
          mCarrets[3].x += dx;
        }  break;

      }

      mCarrets[4] = dmPoint(mCarrets[3].x,mCarrets[0].y);
      mCarrets[5] = dmPoint(mCarrets[1].x,mCarrets[0].y);
      mCarrets[6] = dmPoint(mCarrets[1].x,mCarrets[2].y);
      mCarrets[7] = dmPoint(mCarrets[3].x,mCarrets[2].y);

    } else {
      mCarrets[_index].x += dx;
      mCarrets[_index].y += dy;
      switch(_index) {
        case 4: 
          mCarrets[7].x += dx;  
          mCarrets[5].y += dy;
          break;
        case 5: 
          mCarrets[6].x += dx;
          mCarrets[4].y += dy;
          break;
        case 6: 
          mCarrets[5].x += dx;
          mCarrets[7].y += dy;
          break;
        case 7: 
          mCarrets[4].x += dx;
          mCarrets[6].y += dy;
          break;
      }
      mCarrets[0] = dmPoint((mCarrets[4].x+mCarrets[5].x)/2,mCarrets[4].y); 
      mCarrets[1] = dmPoint(mCarrets[5].x,(mCarrets[5].y+mCarrets[6].y)/2); 
      mCarrets[2] = dmPoint(mCarrets[0].x,mCarrets[6].y);
      mCarrets[3] = dmPoint(mCarrets[4].x,mCarrets[1].y);
    }
  }
}
//-----------------------------------------------------------------------------
void dmEllipseShape::DoFrame(dmGraphics& gr, int _index, int drawFlags ) const
{
  gr.DrawEllipse( *this , drawFlags ); 
}
//-----------------------------------------------------------------------------
void dmEllipseShape::BuildRegion( dmRegion& _rgn) const
{
  _rgn.SetRoi( dmEllipse( Center(), Rx(), Ry() ) ); 
}
//-----------------------------------------------------------------------------
dmEllipseShape::operator dmEllipse() const
{
  return dmEllipse( Center(), Rx(), Ry() );
}
//-----------------------------------------------------------------------------
dmPoint dmEllipseShape::Center() const 
{
  return dmPoint( (mCarrets[1].x + mCarrets[3].x)/2,
                  (mCarrets[0].y + mCarrets[2].y)/2 );                                   
}
//-----------------------------------------------------------------------------
unsigned long dmEllipseShape::Rx() const
{
  return daim::abs((mCarrets[1].x - mCarrets[3].x)/2);
}
//-----------------------------------------------------------------------------
unsigned long dmEllipseShape::Ry() const
{
  return daim::abs((mCarrets[0].y - mCarrets[2].y)/2);
}
//-----------------------------------------------------------------------------
// Polygone 
//-----------------------------------------------------------------------------
dmPolyShape::dmPolyShape() {}
//-----------------------------------------------------------------------------
dmPolyShape::dmPolyShape( const dmPoly& p) : dmShape( p ) {}
//-----------------------------------------------------------------------------
dmPolyShape::dmPolyShape( const dmPolyShape& s) : dmShape( s ) {}
//-----------------------------------------------------------------------------
dmPolyShape& dmPolyShape::operator=( const dmPoly& p)
{
  mCarrets =  p;
  return *this;
}
//-----------------------------------------------------------------------------
void dmPolyShape::DoMoveIndex(int _index, int dx, int dy )
{
  bool _closed = Closed();
  mCarrets[_index].x += dx;
  mCarrets[_index].y += dy;
  if(_index == 0 && _closed) { 
    mCarrets.Back() = *mCarrets.Begin(); 
  }
}
//-----------------------------------------------------------------------------
void dmPolyShape::DoFrame( dmGraphics& gr, int _index, int drawFlags ) const
{
 if(_index < 0 || RigidMode() ) gr.DrawPoly( mCarrets , drawFlags );
 else {
   int sz = Size()-1;
   if(_index > 0)    gr.DrawLine( dmLine(mCarrets[_index-1], mCarrets[_index]   ), drawFlags );
   else if(Closed()) gr.DrawLine( dmLine(mCarrets[_index],   mCarrets[sz-1]     ), drawFlags );
   if(_index < sz )  gr.DrawLine( dmLine(mCarrets[_index],   mCarrets[_index+1] ), drawFlags );   
 }
}
//-----------------------------------------------------------------------------
void dmPolyShape::BuildRegion( dmRegion& _rgn) const
{
  _rgn.SetRoi(Carrets()); 
}
//-----------------------------------------------------------------------------

