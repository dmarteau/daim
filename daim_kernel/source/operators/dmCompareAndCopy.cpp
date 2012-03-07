
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

#include "common/dmUserLib.h"
#include "templates/processing/dmArithmetics.h"
#include "templates/processing/dmMaskCopy.h"

#define dmMaskFormat dmPixelFormat8bppIndexed

using namespace daim;
//----------------------------------------------------------------------
// dmCompareAndCopy
//----------------------------------------------------------------------

namespace {

struct copy_predicat
{
  const dmImage&  src;
  const dmRegion& rgn;
  const dmPoint&  p;
  dm_uint  predicat;

  copy_predicat(const dmImage&  _src, dm_uint _predicat,
                const dmRegion& _rgn ,const dmPoint& _p)
   :src(_src)
   ,rgn(_rgn) 
   ,p(_p)
   ,predicat(_predicat)
   {}

  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& DstImage ) 
  {
    typedef typename dmIImage<_PixelFormat>::image_type image_type;
    typedef typename dmIImage<_PixelFormat>::value_type value_type;

          image_type& _dst = DstImage.Gen();
    const image_type& _src = dmIImage<_PixelFormat>::Cast(&src)->Gen();
        
    switch(predicat) 
    {
      case dmTk::Math::Less         : copy_if(rgn,p,_src,_dst,std::less<value_type>()         ); break;
      case dmTk::Math::Greater      : copy_if(rgn,p,_src,_dst,std::greater<value_type>()      ); break;
      case dmTk::Math::EqualTo      : copy_if(rgn,p,_src,_dst,std::equal_to<value_type>()     ); break;
      case dmTk::Math::GreaterEqual : copy_if(rgn,p,_src,_dst,std::greater_equal<value_type>()); break;
      case dmTk::Math::LessEqual    : copy_if(rgn,p,_src,_dst,std::less_equal<value_type>()   ); break;
      case dmTk::Math::NotEqual     : copy_if(rgn,p,_src,_dst,std::not_equal_to<value_type>() ); break;
    }
  }
};

}; // namespace
//---------------------------------------------------------------------
// Source is lhs operand
//---------------------------------------------------------------------
bool dmCompareAndCopy( const dmImage&  src, dmImage& dst, dm_uint  predicat, 
                       const dmRegion& rgn, const dmPoint& p )
{
  if(src.PixelFormat()==dst.PixelFormat())
  {
    copy_predicat _filter(src,predicat,rgn,p);

    return dmImplementScalarOperation(_filter,dst);
  }
  return false;   
}
//---------------------------------------------------------------------
