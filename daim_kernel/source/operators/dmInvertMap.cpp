
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

//----------------------------------------------------------------------
// Invert Map
//------------------------------------------------------------------------

namespace {

struct invertmap_impl
{
  dmBufferParameters& Params;
  dm_real             MinRange;
  dm_real             MaxRange;

  invertmap_impl( dmBufferParameters& _Params,
                  dm_real _MinRange, dm_real _MaxRange )
  : Params(_Params)
   ,MinRange(_MinRange)
   ,MaxRange(_MaxRange) {}
  //-------------------------------------------------------
  // Generic operation on scalar
  //-------------------------------------------------------
  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _img )
  {
    typedef typename dmIImage<_PixelFormat>::traits_type traits_type;

    typedef typename traits_type::value_type   value_type;

    dm_real aMin = MinRange,aMax = MaxRange;

    if(aMin < aMax) {
      gap<value_type> aMinMax =  minmax(Params.thisRegion,_img.Gen());
      aMax = aMinMax.max();
      aMin = aMinMax.min();
    }

    invert_map( _img.Gen(),Params.thisRegion, gap<value_type>(
        traits_type::clamp(aMin),
        traits_type::clamp(aMax))
     );
  }

  void operator()( dmIImage<dmPixelFormat8bppIndexed>&  _img )
  {
    invert_map(_img.Gen(),Params.thisRegion);
  }
};

}; // namespace
//----------------------------------------------------------------------
bool dmInvertMap::Apply( dmBufferParameters& _Params )
{
  invertmap_impl _filter(_Params,this->_MinRange,this->_MaxRange);
  return dmImplementScalarOperation(_filter,_Params.thisImage);
}
//----------------------------------------------------------------------
