
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

#define dmUseKernelImage
#include "daim_kernel.h"
#include "common/dmUserLib.h"

bool __dmKernel _dmDoImageMath5( dm_int, dmImage&, const dmImage&, const dmRegion&, const dmPoint& ); 
bool __dmKernel _dmDoImageMath4( dmImage& , const dmRegion&, dm_real a, dm_real b ); 
bool __dmKernel _dmDoImageMath3( dmImage& , dm_real a, dm_real b );  

#define _MATH1( op )  { return _dmDoImageMath5(op,lhsImage,rhs,r,p); } 
#define _MATH0( op )  { return _dmDoImageMath5(op,lhsImage,rhs,rhs.Rect(),dmPoint(0,0)); } 

#define _DEF_MATH1( F ) bool dmImageMath::F( const dmImage& rhs, const dmRegion& r, const dmPoint& p )
#define _DEF_MATH0( F ) bool dmImageMath::F( const dmImage& rhs )

#define _DECLARE_MATH1( op , F ) _DEF_MATH1( F ) _MATH1( op )
#define _DECLARE_MATH0( op , F ) _DEF_MATH0( F ) _MATH0( op )

#define _DECLARE_MATH( op, F ) \
   _DECLARE_MATH0( op , F )    \
   _DECLARE_MATH1( op , F )

_DECLARE_MATH( dmTk::Math::AddPixels   , Add       ) 
_DECLARE_MATH( dmTk::Math::SubPixels   , SubNegate ) // Must invert beacause lhs is the output 
_DECLARE_MATH( dmTk::Math::MinPixels   , MinImage  ) 
_DECLARE_MATH( dmTk::Math::MaxPixels   , MaxImage  )
_DECLARE_MATH( dmTk::Math::NSubPixels  , Sub       ) // Must invert because lhs is the output 
_DECLARE_MATH( dmTk::Math::OrPixels    , OrImage   )
_DECLARE_MATH( dmTk::Math::XorPixels   , XorImage  )
_DECLARE_MATH( dmTk::Math::AndPixels   , AndImage  )
_DECLARE_MATH( dmTk::Math::DiffPixels  , AbsDiff   )


bool dmImageMath::Add( float a )                        { return _dmDoImageMath3(lhsImage,0,a);   }
bool dmImageMath::Add( float a, const dmRegion& r )     { return _dmDoImageMath4(lhsImage,r,0,a); }

bool dmImageMath::Multiply( dm_real a )                    { return _dmDoImageMath3(lhsImage,a,0);   }
bool dmImageMath::Multiply( dm_real a, const dmRegion& r ) { return _dmDoImageMath4(lhsImage,r,a,0); }

bool dmImageMath::AddMul( dm_real a, dm_real b )                    { return _dmDoImageMath3(lhsImage,a,b);   }
bool dmImageMath::AddMul( dm_real a, dm_real b, const dmRegion& r ) { return _dmDoImageMath4(lhsImage,r,a,b); }


