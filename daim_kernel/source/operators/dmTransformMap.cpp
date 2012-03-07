
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

#include "templates/processing/dmDensityMap.h"
#include "templates/processing/dmArithmetics.h"

using namespace daim;

// TODO : Use fitting models for mapping non 8 bits values.
//         may be should create an another component

//----------------------------------------------------------------------
// Transform Map
//------------------------------------------------------------------------

namespace {

#define __invert1 std::bind1st(std::minus<value_type>(),traits_type::max())
//
struct transform_map_impl
{
  const dmRegion&     mRegion;
  dmColorMapArray     mMap;
  dm_real             mMinRange;
  dm_real             mMaxRange;   

  transform_map_impl(const dmRegion& _Region, dmColorMapArray _Map,
                     dm_real _MinRange, dm_real _MaxRange )
  :mRegion(_Region)
  ,mMap(_Map)
  ,mMinRange(_MinRange)
  ,mMaxRange(_MaxRange) {}   

  //-------------------------------------------------------
  // Generic operation on scalar
  //-------------------------------------------------------
  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _img ) 
  {
    typedef typename dmIImage<_PixelFormat>::traits_type traits_type;
    typedef typename dmIImage<_PixelFormat>::image_type  image_type;

    typedef typename traits_type::value_type   value_type;

     gap<value_type> _range(
         traits_type::clamp(mMinRange),
         traits_type::clamp(mMaxRange)
     );

    image_type& _image = _img.Gen();

    dmRegion reg;        
    if(_range.upper>_range.lower) { // Create pixel range region
      create_rgnroi(_image,between<value_type>(_range.min(),_range.max()),
                    reg,mRegion);
    } else {
      reg    = mRegion; // Get the min and the max for the region
      _range = minmax(_image.rect(),_image);
    }
    scale_convert(_range,gap<value_type>(0,255),reg,_image); // convert values between [0..255]
    transform(reg,_image,_apply_map(mMap));                   // apply transformation
    scale_convert(gap<value_type>(0,255),_range,reg,_image); // convert values back [0..255]
  }

  void operator()( dmIImage<dmPixelFormat8bppIndexed>&  _img ) 
  { 
     apply_map( _img.Gen(),mRegion,mMap); 
  }
};

}; // namespace
//----------------------------------------------------------------------
bool dmTransformMap::Apply( dmImage& _Image, const dmRegion& _Region )
{
  transform_map_impl _filter(_Region,this->_Map,this->_MinRange,this->_MaxRange);
  return dmImplementScalarOperation(_filter,_Image);
}
//------------------------------------------------------------------------
