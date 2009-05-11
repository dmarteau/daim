
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

#define dmUseKernelImageTemplates
#define dmUseKernelBuffer
#include "daim_kernel.h"

#include "templates/processing/dmTransforms.h"
#include "common/dmUserLib.h"

using namespace daim;

//--------------------------------------------------------------------------
// Flip copy transformation
//--------------------------------------------------------------------------
struct __dm_impl_flipcopy
{
  const dmPoint& p;
  const dmRect&  rsrc;
  dmImage&       dst;
  int            flip;

  __dm_impl_flipcopy( dmImage& _dst, const dmRect& _r, const dmPoint& _p, int _flip ) 
  : p(_p),rsrc(_r),dst(_dst),flip(_flip) {} 

  template<EPixelFormat _PixelFormat>
  void operator()( const dmIImage<_PixelFormat>& _im ) 
  {
     dmIImage<_PixelFormat>* _dst = dmIImage<_PixelFormat>::Cast(&dst);
     dmASSERT(_dst != NULL );
     flip_copy(_im.Gen(),_dst->Gen(),rsrc,p,
       (flip & dmTk::FlipHor)!=0,
       (flip & dmTk::FlipVer)!=0 );
  }
};
//----------------------------------------------------------
bool dmFlipCopy( const dmImage& src, dmImage& dst, const dmRect& r, 
                 const dmPoint& p, int flip )
{
  if(src.TypeDescriptor()==dst.TypeDescriptor())
  {
    __dm_impl_flipcopy _filter(dst,r,p,flip);
    return dmImplementOperation(_filter,src);
  }
  return false;
}

//--------------------------------------------------------------------------
