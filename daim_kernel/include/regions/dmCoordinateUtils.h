#ifndef dmCoordinateUtils_h
#define dmCoordinateUtils_h

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
// File         : dmCoordinateUtils.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

// Utility Macros and inlines --------------------------------

#define dmGetBoxIntersection(x1,y1,x2,y2,box1,box2) { \
  y1 = dm_max( (box1).top_left.y, (box2).top_left.y);    \
  x1 = dm_max( (box1).top_left.x, (box2).top_left.x);    \
  y2 = dm_min( (box1).bottom_right.y,(box2).bottom_right.y);   \
  x2 = dm_min( (box1).bottom_right.x,(box2).bottom_right.x);   \
}
//------------------------------------------------------------
inline bool dmPointInRectangle( long x, long y, const dm_rect& box) {
  return ((x >= box.top_left.x) && (x <= box.bottom_right.x) && 
   (y >= box.top_left.y) && (y <= box.bottom_right.y));
}
//------------------------------------------------------------
inline bool dmPointInRectangle( const dm_point& p, const dm_rect& box) {
  return dmPointInRectangle(p.x,p.y,box);
}
//------------------------------------------------------------
inline int dmRectCmp( const dm_rect& r1, const dm_rect& r2 )
{
  if( r1.top_left.x > r2.top_left.x ) 
  {
    if(r1.top_left.y > r2.top_left.y) { //
      if(r1.bottom_right.x < r2.bottom_right.x) {
        if(r1.bottom_right.y < r2.bottom_right.y) 
           return -1; // r1 inside r2 (strict)
      }
    }
  } else if( r1.top_left.y <= r2.top_left.y) {  //
    if(r1.bottom_right.x >= r2.bottom_right.x) {
      if(r1.bottom_right.y >= r2.bottom_right.y) 
        return 1; // r2 inside/equal r1
    }
  }
  return 0; // r1 overlap r2
}
//------------------------------------------------------------
inline bool operator>=(const dm_rect& r1, const dm_rect& r2) { return dmRectCmp(r1,r2)>0; }
inline bool operator< (const dm_rect& r1, const dm_rect& r2) { return dmRectCmp(r1,r2)<0; }
//------------------------------------------------------------
inline bool dmClipRectangle( dm_rect& box, dm_int x1, dm_int y1, dm_int x2, dm_int y2)
{
  box.top_left.x  = dm_max( x1, box.top_left.x);
  box.top_left.y  = dm_max( y1, box.top_left.y);
  box.bottom_right.x = dm_min( x2, box.bottom_right.x);
  box.bottom_right.y = dm_min( y2, box.bottom_right.y);
  return ( (box.top_left.x <= box.bottom_right.x) &&
           (box.top_left.y <= box.bottom_right.y) );
}
//------------------------------------------------------------
inline bool dmClipRectangle( dm_rect& box, const dm_rect& clip)
{
  return dmClipRectangle(box,
    clip.top_left.x ,clip.top_left.y,
    clip.bottom_right.x,clip.bottom_right.y);
}
//------------------------------------------------------------
#include "dmBresenham.h"
//------------------------------------------------------------
template<class Op> 
inline Op& dmDigitalLine( int x1,int y1,int x2,int y2, Op& op )
{
  daim::bresenham_line(op,x1,y1,x2,y2);
  return op;
}
//-----------------------------------------------------------
template<class Op> inline Op& dmDigitalLine( const dm_point& _start, const dm_point& _end, Op& op )
{  return dmDigitalLine(_start.x,_start.y,_end.x,_end.y,op); }
//------------------------------------------------------------
template<class Op> inline Op& dmDigitalLine( const dm_line& _line, Op& op ) 
{ return dmDigitalLine(_line.start,_line.end,op); }
//------------------------------------------------------------
template<class Op> inline Op& dmDigitalCircle( const dm_point& c, size_t r, Op& op )
{
  if(r==0)
    op(c.x,c.x,c.y);
  else {
    daim::bresenham<Op> _Br(op);
    _Br.circle(c.x,c.y,r);
  }
  return op;
}
//------------------------------------------------------------
template<class Op> inline Op& dmDigitalCircle( const dm_circle& c, Op& op )
{  return dmDigitalCircle(c.center,c.r,op); }
//------------------------------------------------------------
template<class Op> inline Op& dmDigitalEllipse( const dm_point& c, long rx,long ry, Op& op )
{
  #define _SetPixel_qads(_x,_y) op(_x,_y,c)
   _DigitalEllipse(rx,ry);
  #undef _SetPixel_qads
  return op;
}
//------------------------------------------------------------
template<class Op> inline Op dmDigitalEllipse( const dm_ellipse& e, Op& op ) 
{ return dmDigitalEllipse(e.center,e.rx,e.ry,op); }
//------------------------------------------------------------



#endif // dmCoordinateUtils_h
