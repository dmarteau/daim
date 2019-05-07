
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

#define dmUserIncludes
#include "daim_kernel.h"
#include "common/dmMetrics.h"
#include <math.h>

namespace dmMetrics {

//-----------------------------------------------------------------
double DegreeToRadian( double a ) { return a*3.14159265358979323846/180.0; }
double RadianToDegree( double a ) { return a*180.0/3.14159265358979323846; }
//------------------------------------------------------------------
dm_int DotProduct( const dm_point& p1, const dm_point& p2, const dm_point& p3 )
{
  return (p1.x-p2.x)*(p3.x-p2.x) + (p1.y-p2.y)*(p3.y-p2.y);
}
//------------------------------------------------------------------
dm_int VecProduct( const dm_point& p1, const dm_point& p2, const dm_point& p3 )
{
  return (p1.x-p2.x)*(p3.y-p2.y) - (p1.y-p2.y)*(p3.x-p2.x);
}
//------------------------------------------------------------------
double Angle( const dm_point& p )
{
  return atan2( static_cast<double>(-p.y),
                static_cast<double>( p.x));
}
//------------------------------------------------------------------
double Angle( const dm_point& p1 ,  const dm_point& p2 )
{
  return atan2( static_cast<double>(VecProduct(p1,p2)),
                static_cast<double>(DotProduct(p1,p2)) );
}
//------------------------------------------------------------------
double Angle( const dm_point& p1 , const dm_point& p2, const dm_point& p3 )
{
  dmPoint pa = p1 - p2;
  dmPoint pb = p3 - p2;
  return atan2( static_cast<double>(- VecProduct(pa,pb)),
                static_cast<double>(  DotProduct(pa,pb)) );
}
//------------------------------------------------------------------
void Rotate( dm_point& p, const dm_point& ref, double _a, double _b )
{
  dm_int dx = p.x - ref.x;
  dm_int dy = p.y - ref.y;
  p.x = ref.x + static_cast<dm_int>( daim::round(dx * _a - dy * _b) );
  p.y = ref.y + static_cast<dm_int>( daim::round(dx * _b + dy * _a) );
}
//------------------------------------------------------------------
void Rotate( dm_point& p, double _a, double _b )
{
  dm_point ref = { 0, 0 };
  Rotate( p, ref, _a, _b );
}
//------------------------------------------------------------------
void Rotate( dm_point& p, double _theta )
{
  Rotate( p, cos(_theta), sin(_theta) );
}
//------------------------------------------------------------------
void Rotate( dm_point& p , const dm_point& ref, double _theta )
{
  Rotate( p, ref, cos(_theta), sin(_theta) );
}
//------------------------------------------------------------------
double Euclidian_Distance( const dm_point& p1, const dm_point& p2 )
{
  double dx = p1.x - p2.x;
  double dy = p1.y - p2.y;
  return sqrt( dx*dx + dy*dy );
}
//------------------------------------------------------------------
double Euclidian_Distance( const dm_point& p, const dm_line& l )
{
  return VecProduct( p,l.start,l.end )/Euclidian_Distance(l); 
}
//------------------------------------------------------------------
double Euclidian_Distance( const dm_point& p, const dm_circle& c )
{
  return Euclidian_Distance( p, c.center ) - c.r;
}
//------------------------------------------------------------------
dm_int Absolute_Distance( const dm_point& p1, const dm_point& p2 )
{
  return abs( p1.x - p2.x ) + abs( p1.y - p2.y );
}
//------------------------------------------------------------------
void Projection( const dm_point& p0, const dm_circle& c, dm_point& p )
{
  double dx = p0.x-c.center.x;
  double dy = p0.y-c.center.y;
  double L = sqrt( dx*dx + dy*dy );
  p.x = c.center.x + static_cast<dm_int>( daim::round( (dx * c.r)/L ) );
  p.y = c.center.y + static_cast<dm_int>( daim::round( (dy * c.r)/L ) );
}
//------------------------------------------------------------------
void Projection( const dm_point& p0, const dm_line& l, dm_point& p )
{
  dm_int xs = l.start.x;
  dm_int ys = l.start.y;
  dm_int dx = l.end.x - xs;
  dm_int dy = l.end.y - ys;

  dm_real L = static_cast<dm_real>( (p0.x-xs)*dx + (p0.y-ys)*dy ) / 
              static_cast<dm_real>(dx*dx+dy*dy);

  p.x = xs + static_cast<dm_int>( daim::round(dx * L) );
  p.y = ys + static_cast<dm_int>( daim::round(dy * L) );
}
//------------------------------------------------------------------
void Scale( dm_point& p, const dm_point& ref , double s )
{
  p.x = ref.x + static_cast<dm_int>( daim::round( s * (p.x-ref.x)) );
  p.y = ref.y + static_cast<dm_int>( daim::round( s * (p.y-ref.y)) );
}
//------------------------------------------------------------------
void Size( dm_point& p, const dm_point& ref, double sz )
{
  double d = Euclidian_Distance(p,ref);
  if(d>0) 
   Scale( p, ref , sz/d );
}
//------------------------------------------------------------------
void Grow( dm_point& p, const dm_point& ref, double rel )
{
  double d = Euclidian_Distance(p,ref);
  Scale( p, ref , (1.0f + rel)*d );
}
//------------------------------------------------------------------

}; // namespace dmMetrics

