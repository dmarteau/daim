
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

//------------------------------------------------------------------------
bool __dmKernel _dmDoImageMath5( dm_int op, dmImage& lhs, 
                                 const dmImage& rhs , 
                                 const dmRegion& r, const dmPoint& p )
{
  // lhs and rhs are swapped because dmArithmetics define
  // output as rhs
  return dmArithmetics(op,rhs,lhs,r,p);
} 
//------------------------------------------------------------------------
bool __dmKernel _dmDoImageMath4( dmImage& lhs , const dmRegion& r,
                                 dm_real a, dm_real b )
{
  return dmAddMulImage(lhs,r,a,b);
}
//------------------------------------------------------------------------
bool __dmKernel _dmDoImageMath3( dmImage& lhs , dm_real a, dm_real b )
{
  return dmAddMulImage(lhs,a,b);  
}
//------------------------------------------------------------------------
bool __dmKernel _dmDoImageMath2( dmBufferParameters& _Params, dm_int operation )
{
  return dmArithmetics(_Params,operation);  
}
//------------------------------------------------------------------------
