
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

using namespace daim;

//------------------------------------------------------------------------
// Extend Map
//------------------------------------------------------------------------
struct __dm_impl_extend
{
  const dmRegion& mRegion;
  dm_real mMinExt  ,mMaxExt;
  dm_real mMinRange,mMaxRange;
  
  __dm_impl_extend( const dmRegion& _Region,
                    dm_real _MinExt  , dm_real _MaxExt,
                    dm_real _MinRange, dm_real _MaxRange )

  : mRegion(_Region)
   ,mMinExt(_MinExt),mMaxExt(_MaxExt)
   ,mMinRange(_MinRange),mMaxRange(_MaxRange) {}

  //-------------------------------------------------------
  // Generic operation on scalar
  //-------------------------------------------------------
  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _img ) 
  {
     typedef typename dmIImage<_PixelFormat>::traits_type  traits_type;

     typedef typename traits_type::value_type   value_type;
     typedef typename traits_type::integer_type integer_type;

     gap<value_type> range_from(
        _get_range_value(mMinExt,traits_type(),integer_type()),
        _get_range_value(mMaxExt,traits_type(),integer_type())
     );

    gap<value_type> range_to(
       _get_range_value(mMinRange,traits_type(),integer_type()),
       _get_range_value(mMaxRange,traits_type(),integer_type())
    );

    extend_density_range( _img.Gen(),mRegion,range_from,range_to );
  }

  // Specialization
  void operator()( dmIImage<dmPixelFormat8bppIndexed>& _img ) 
  { 
    typedef pixel_traits<dm_uint8> traits_type; 

    typedef traits_type::value_type  value_type;
    typedef traits_type::integer_type integer_type;

     gap<value_type> range_from( 
        _get_range_value(mMinExt,traits_type(),integer_type()),
        _get_range_value(mMaxExt,traits_type(),integer_type())
     );

     gap<value_type> range_to(
       _get_range_value(mMinRange,traits_type(),integer_type()),
       _get_range_value(mMaxRange,traits_type(),integer_type())
     );

     extend_density_range( _img.Gen(),mRegion,range_from,range_to );
  }
};
//----------------------------------------------------------------------
bool dmExtendMap::Apply( dmImage& _Image, const dmRegion&  _Region )
{
  __dm_impl_extend _filter(_Region,
                           this->_MinExt,this->_MaxExt,
                           this->_MinRange,this->_MaxRange);

  return dmImplementScalarOperation(_filter,_Image);
}
