
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

//----------------------------------------------------------------------
#define dmUserIncludes
#include "daim_kernel.h"
#include <algorithm>

//----------------------------------------------------------------------
__dmKernel dmPoly&  dmPoly::operator=(const dmPoly& poly) 
{
  if(&poly != this) { _store = poly._store; }
  return *this;
}
//----------------------------------------------------------------------
__dmKernel void dmPoly::Construct(unsigned long nn)
{
  _store.Resize(nn,dmPoint(0,0));
}
//----------------------------------------------------------------------
__dmKernel void dmPoly::Construct( const dm_point* points, unsigned long nn)
{
  _store.Resize(nn);
  dmASSERT( points != NULL && nn > 0 );
  std::copy( points,points+nn,Begin() );
}
//----------------------------------------------------------------------
__dmKernel void dmPoly::Construct(const dmPoly& poly)
{
  _store = poly._store;
}
//----------------------------------------------------------------------
// Compute the intersection between a polygon and a horizontal line
__dmKernel long dmPoly::IntersectHor(long y,long* xx, long* pos ) const
{
	long i,j,ni,k,length,_ii;
	long test,x1,x2,dx,dy;
	long b;

	dm_point p0,p1,p2;

#define _M_p(i) ( _ii = (i), ((_ii>=length)? _store[_ii % length] : _store[_ii]) )

	ni=0;
	length = Size()-1;
	for(i=0;i<length;i++) {
		j=i+length;
		p0 = _M_p(j-1); p1 = _M_p(j); p2 = _M_p(j+1);

		test = (y - p1.y)*(y - p2.y);
		if( (test < 0) ) { // on traverse "normalement" le segment P[j],P[j+1]
			dy = p2.y - p1.y;
			dx = (p2.x - p1.x) * dm_sgn(dy);
			dy = dm_abs(dy);
			// abcisse du point d'intersection
			x1 = p1.x +  ((y - p1.y)*dx) / dy ;

			pos[ni] = dx*(y - p1.y) - dy*(x1 - p1.x);
			// test > 0  we are on the left side
			// test < 0  we are on the right side
			// test = 0  we are on an edge
			xx[ni++] = x1;

		} else if (y == p1.y) { // on est sur un point
			// recherche le prochain changement de direction
			k=0;
			while( _M_p(k+j+1).y == p1.y) { k++;} // skip same `y' points
			x1 = p1.x; x2 = _M_p(k+j).x;
			test = (p0.y - p1.y)*( _M_p(k+j+1).y - p1.y);
			pos[ni] = 0; // we are on a edge
			if( test < 0) xx[ni++] = dm_max(x1,x2);
			i+=k;
		}
	}
	// trier dans l'ordre des abscisses croissantes (straight insertion)
	for(j=1;j<ni;j++) {
		test = xx[j]; b = pos[j];
		i = j-1;
		while (i >= 0 && xx[i] > test) {
			xx[i+1]=xx[i]; pos[i+1] = pos[i];
			i--;
		}
		xx[i+1] = test; pos[i+1]= b;
	}
	return ni;

#undef _M_p
}
//--------------------------------------------------------
struct __addPoint {
  dmVector<dm_point>& _points;
  __addPoint( dmVector<dm_point>& pts ) : _points(pts) {}
  void operator()( long x, long y ) { _points.Push_Back( dmPoint(x,y) ); }
};
//------------------------------------------------------------------------------------
__dmKernel int dmPoly::Bresenham( const dm_line& l )
{
  Clear();
  __addPoint _inserter(_store);
  dmDigitalLine( l, _inserter  ); 
  return Size();
}
//--------------------------------------------------------
struct __addPointQads {
  dmVector<dm_point>& _points;
  dm_point pt;
  __addPointQads( dmVector<dm_point>& _p ) : _points(_p) {}
  void operator()(long x, long y, const dm_point& c) {
     pt.x = x;  pt.y = y;
     _points.Push_Back( pt );
  }
};
//------------------------------------------------------------------------------------
__dmKernel int dmPoly::Bresenham( const dm_ellipse& e )
{
  Clear();
  dmVector<dm_point> __points;
  __addPointQads _inserter(__points);
  dmDigitalEllipse(e,_inserter);
  //-------------------------------------------------
  // Le Bresenham parcoure le quadrant d'abord dans
  // le sens directe, puis dans le sens indirect
  //-------------------------------------------------
  dmVector<dm_point>::iterator last,it = __points.Begin();
  while( it!= __points.End() && (*it).y!= 0) ++it;

  // BUG_BRESENHAM -------------------------------
  if(it== __points.End()) {
    dmASSERT(__points.Back().y==1);
    dm_point pt = { e.rx, 0 };
    __points.Push_Back( pt );
  } else  std::reverse(it,__points.End()-1);

  for(last=__points.End(),--last; (*last).y!=0 && last!= __points.Begin();--last);
  dmASSERT(last>__points.Begin());
  ++last;
  int n = last-__points.Begin() - 1;
  //-----------------------------------------------
  
  Resize(4*n-3);

  int x,y,cx,cy;
  cx = e.center.x;
  cy = e.center.y;

  iterator q1   = Begin();
  iterator q2   = Begin()+2*n-1;
  iterator q3   = Begin()+2*n-1;
  iterator q4   = Begin()+4*n-4;
  
  for(it=__points.Begin();q1<q2;++q1,--q2,++q3,--q4,++it) 
  {
    x=(*it).x; y=(*it).y; 
    
    (*q1).x = cx-x; (*q1).y = cy-y;   
    (*q2).x = cx-x; (*q2).y = cy+y;   
    (*q3).x = cx+x; (*q3).y = cy+y;   
    (*q4).x = cx+x; (*q4).y = cy-y;   
  }

  _store[2*n-1].x = cx;
  _store[2*n-1].y = cy + e.ry;
  _store[3*n-1].x = cx + e.rx;
  _store[3*n-1].y = cy;

  _store.Push_Back( *Begin() );
  return Size();
}
//------------------------------------------------------------------------------------
__dmKernel int dmPoly::Bresenham( const dmPoly& p )
{
  Clear();

  __addPoint _inserter(_store);

  if(p.Size()) {
    int cnt = p.Size()-1;
    for(int i=1;i<=cnt;++i) {
      dmDigitalLine( p[i-1],p[i], _inserter );
      Pop_Back(); // remove last point
    }

    if(!p.Closed()) 
        Push_Back( p.Back() );
  } 
  return Size();
}
//----------------------------------------------------------------
