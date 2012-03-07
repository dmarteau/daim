
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
// dmCopyIfMaskCondition
//----------------------------------------------------------------------
struct __dm_copy_mask_cond
{
  const dmImage& src;
  const dmImage& mask;
  dm_uint  predicat;
  dm_real  value;

  __dm_copy_mask_cond(const dmImage& _src, dm_uint _predicat,
                      const dmImage& _mask,dm_real _value)
   :src(_src)
   ,mask(_mask) 
   ,predicat(_predicat)
   ,value(_value)
   {}

  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& DstImage ) 
  {
    typedef typename dmIImage<_PixelFormat>::image_type image_type;
    typedef typename dmIImage<_PixelFormat>::value_type value_type;

    typedef dmIImage<dmMaskFormat>::image_type  mask_type;
    typedef dmIImage<dmMaskFormat>::value_type  mask_value;
    typedef dmIImage<dmMaskFormat>::traits_type mask_traits;

    image_type& _dst = DstImage.Gen();
    image_type& _src = dmIImage<_PixelFormat>::Cast(&src)->Gen();
        
    mask_type&  _mask = dmIImage<dmMaskFormat>::Cast(&mask)->Gen(); 
    mask_value x      = mask_traits::clamp(value);

    dmRegion rgn;
    dmPoint  p(0,0);
    rgn.SetRectRoi(DstImage.Rect());

    switch(predicat) 
    {
      case dmTk::Math::Less         : combine(rgn,p,_mask,_src,_dst,cpy::bind_mask_cond(std::bind2nd(std::less<mask_value>()         ,x),type_of<value_type>())); break;
      case dmTk::Math::Greater      : combine(rgn,p,_mask,_src,_dst,cpy::bind_mask_cond(std::bind2nd(std::greater<mask_value>()      ,x),type_of<value_type>())); break;
      case dmTk::Math::EqualTo      : combine(rgn,p,_mask,_src,_dst,cpy::bind_mask_cond(std::bind2nd(std::equal_to<mask_value>()     ,x),type_of<value_type>())); break;
      case dmTk::Math::GreaterEqual : combine(rgn,p,_mask,_src,_dst,cpy::bind_mask_cond(std::bind2nd(std::greater_equal<mask_value>(),x),type_of<value_type>())); break;
      case dmTk::Math::LessEqual    : combine(rgn,p,_mask,_src,_dst,cpy::bind_mask_cond(std::bind2nd(std::less_equal<mask_value>()   ,x),type_of<value_type>())); break;
      case dmTk::Math::NotEqual     : combine(rgn,p,_mask,_src,_dst,cpy::bind_mask_cond(std::bind2nd(std::not_equal_to<mask_value>() ,x),type_of<value_type>())); break;
    }
  }
};
//---------------------------------------------------------------------
bool dmCopyIfMaskCondition( const dmImage& src, dmImage& dst,
                            dmImage& mask, dm_uint  predicat, 
                            dm_real  value )
{
  if(dmSameImageTypeAndSize(src,dst) &&
     dmSameImageSize(src,mask)       &&
     mask.PixelFormat()==dmMaskFormat )
  {
    __dm_copy_mask_cond _filter(src,predicat,mask,value);
    return dmImplementOperation(_filter,dst);

  }
  return false;   
}
//---------------------------------------------------------------------
