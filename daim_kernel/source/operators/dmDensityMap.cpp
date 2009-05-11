
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
// dmDensityMap
//----------------------------------------------------------------------
struct __dm_impl_DensityMap
{
  dmBufferParameters& Params;
  dmHistogram&        Histo;
  dm_real&            MinRange;
  dm_real&            MaxRange;
  bool                AutoScan;

  __dm_impl_DensityMap(dmBufferParameters& _Params ,dmHistogram& _Histo,
                       dm_real& _MinRange,dm_real& _MaxRange, bool _AutoScan)
   :Params  (_Params  )
   ,Histo   (_Histo   )
   ,MinRange(_MinRange)
   ,MaxRange(_MaxRange)
   ,AutoScan(_AutoScan)
   {}

  // Generic operation on scalar
  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _img )
  {
     typedef typename dmIImage<_PixelFormat>::traits_type  traits_type;

     typedef typename traits_type::value_type   value_type;
     typedef typename traits_type::integer_type integer_type;

     gap<value_type> _g(traits_type::min(),traits_type::max());

     if(AutoScan) {
       _g = minmax(Params.thisRegion,_img.Gen());
       MinRange = _g.min();
       MaxRange = _g.max();
     } else {
       _g = gap<value_type>(
          _get_range_value(MinRange,traits_type(),integer_type()),
          _get_range_value(MaxRange,traits_type(),integer_type())
       );
     }

     Histo.GetHistogram(_img.Gen(),Params.thisRegion,
          daim::_extract_range<value_type>(_g));
  }

  // Specialization
  void operator()( dmIImage<dmPixelFormat24bppRGB>& _img ) {
    Histo.GetHistogram(_img.Gen(),Params.thisRegion);
    MinRange = 0;
    MaxRange = 255;
  }

  void operator()( dmIImage<dmPixelFormat8bppIndexed>&  _img ) {
    Histo.GetHistogram(_img.Gen(),Params.thisRegion);
    MinRange = 0;
    MaxRange = 255;
  }

};
//----------------------------------------------------------------------
bool dmDensityMap::Apply( dmBufferParameters& _Params )
{
  __dm_impl_DensityMap _filter(_Params,
                       this->_Histogram,
                       this->_MinRange,this->_MaxRange,
                       this->_AutoScan);

  return dmImplementOperation(_filter,_Params.thisImage);
}
//------------------------------------------------------------------------
