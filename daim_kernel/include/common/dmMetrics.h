#ifndef dmMetrics_h
#define dmMetrics_h

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
// File         : dmMetrics.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#include "dmKernelBase.h"
#include "dmCoordinatesTypes.h"

namespace dmMetrics
{
//----------------------------------------------------------------------
dm_double DegreeToRadian( dm_double ); // convert degrees -> radians
dm_double RadianToDegree( dm_double ); // convert radians -> degrees
//----------------------------------------------------------------------
inline dm_int DotProduct( const dm_point& p1, const dm_point& p2) { return p1.x*p2.x + p1.y*p2.y; }
inline dm_int VecProduct( const dm_point& p1, const dm_point& p2) { return p1.x*p2.y - p1.y*p2.x; }
//----------------------------------------------------------------------
dm_int DotProduct( const dm_point& p1, const dm_point& p2, const dm_point& p3 );
dm_int VecProduct( const dm_point& p1, const dm_point& p2, const dm_point& p3 );
//----------------------------------------------------------------------
dm_double Angle( const dm_point& );
dm_double Angle( const dm_point& ,  const dm_point& );
dm_double Angle( const dm_point& ,  const dm_point&, const dm_point& );

void Rotate( dm_point& , dm_double _a, dm_double _b );
void Rotate( dm_point& , dm_double _theta );
void Rotate( dm_point& , const dm_point& ref, dm_double _a, dm_double _b );
void Rotate( dm_point& , const dm_point& ref, dm_double _theta );

dm_double Euclidian_Distance( const dm_point&, const dm_line&   );
dm_double Euclidian_Distance( const dm_point&, const dm_circle& );
dm_double Euclidian_Distance( const dm_point&, const dm_point&  );

dm_int Absolute_Distance( const dm_point&, const dm_point&  );
// 
void Projection( const dm_point&, const dm_line&  , dm_point& );
void Projection( const dm_point&, const dm_circle&, dm_point& );
//
void Scale( dm_point& p, const dm_point& ref, dm_double s  );
void Size ( dm_point& p, const dm_point& ref, dm_double sz );
void Grow ( dm_point& p, const dm_point& ref, dm_double rel);
//------------------------------------------------------------------------
inline dm_double Euclidian_Distance ( const dm_line& l ) { return Euclidian_Distance(l.start,l.end);    }
inline dm_int    Absolute_Distance  ( const dm_line& l ) { return Absolute_Distance (l.start,l.end); }
//-------------------------------------------------------------------------
inline void Rotate( dm_line& l, dm_double _theta ) { Rotate(l.end,l.start,_theta); }
//----------------------------------------------------------------------

}; // namespace dmMetrics

#endif // dmMetrics
