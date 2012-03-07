
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

#include <functional>
#include <algorithm>

using namespace daim;

//----------------------------------------------------------------------
struct __dm_impl_create_roi
{
  dmRegion& result;
  dm_real   value;
  dm_uint   predicat;

  __dm_impl_create_roi(dmRegion& _result,dm_real _value,dm_uint _predicat)
   :result(_result)
   ,value(_value)
   ,predicat(_predicat) {}

  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& anImage ) 
  {
    typedef typename dmIImage<_PixelFormat>::image_type  image_type;
    typedef typename dmIImage<_PixelFormat>::pixel_type  pixel_type;
    typedef typename dmIImage<_PixelFormat>::traits_type traits_type;
    typedef typename dmIImage<_PixelFormat>::value_type  value_type;

    image_type& _img = anImage.Gen();
        
    value_type x = traits_type::clamp(value);
    switch(predicat) 
    {
      case dmTk::Math::Less         : create_roi(_img,std::bind2nd(std::less<value_type>()         ,x),result); break;
      case dmTk::Math::Greater      : create_roi(_img,std::bind2nd(std::greater<value_type>()      ,x),result); break;
      case dmTk::Math::EqualTo      : create_roi(_img,std::bind2nd(std::equal_to<value_type>()     ,x),result); break;
      case dmTk::Math::GreaterEqual : create_roi(_img,std::bind2nd(std::greater_equal<value_type>(),x),result); break;
      case dmTk::Math::LessEqual    : create_roi(_img,std::bind2nd(std::less_equal<value_type>()   ,x),result); break;
      case dmTk::Math::NotEqual     : create_roi(_img,std::bind2nd(std::not_equal_to<value_type>() ,x),result); break;
    }
  }
};
//----------------------------------------------------------------------
struct __dm_impl_create_rgnroi
{
  dmRegion&       result;
  dm_real         value;
  dm_uint         predicat;
  const dmRegion& roi;

  __dm_impl_create_rgnroi(dmRegion& _result,dm_real _value,dm_uint _predicat,
                          const dmRegion& _roi)
   :result(_result)
   ,value(_value)
   ,predicat(_predicat)
   ,roi(_roi) {}

  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& anImage ) 
  {
    typedef typename dmIImage<_PixelFormat>::image_type  image_type;
    typedef typename dmIImage<_PixelFormat>::pixel_type  pixel_type;
    typedef typename dmIImage<_PixelFormat>::traits_type traits_type;
    typedef typename dmIImage<_PixelFormat>::value_type  value_type;

    typedef typename traits_type::integer_type  integer_type;

    image_type& _img = anImage.Gen();
        
    value_type x = traits_type::clamp(value);
    switch(predicat) 
    {
      case dmTk::Math::Less         : create_rgnroi(_img,std::bind2nd(std::less<value_type>()         ,x),result,roi); break;
      case dmTk::Math::Greater      : create_rgnroi(_img,std::bind2nd(std::greater<value_type>()      ,x),result,roi); break;
      case dmTk::Math::EqualTo      : create_rgnroi(_img,std::bind2nd(std::equal_to<value_type>()     ,x),result,roi); break;
      case dmTk::Math::GreaterEqual : create_rgnroi(_img,std::bind2nd(std::greater_equal<value_type>(),x),result,roi); break;
      case dmTk::Math::LessEqual    : create_rgnroi(_img,std::bind2nd(std::less_equal<value_type>()   ,x),result,roi); break;
      case dmTk::Math::NotEqual     : create_rgnroi(_img,std::bind2nd(std::not_equal_to<value_type>() ,x),result,roi); break;
    }
    
  }
};
//----------------------------------------------------------------------
bool dmCreateRegion( const dmImage& src, dmRegion& result, 
                     dm_uint predicat,  
                     dm_real value )
{
  __dm_impl_create_roi _filter(result,value,predicat);
  return dmImplementScalarOperation(_filter,src);
}
//----------------------------------------------------------------------
bool dmCreateRegionRoi( const dmImage& src, dmRegion& result, 
                     dm_uint predicat,  
                     dm_real value,
                     const dmRegion& roi )
{
  __dm_impl_create_rgnroi _filter(result,value,predicat,roi);
  return dmImplementScalarOperation(_filter,src);
}
//----------------------------------------------------------------------
