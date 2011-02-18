
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
  const dmRegion&     mRegion;
  dmHistogram&        mHisto;
  dm_real&            mMinRange;
  dm_real&            mMaxRange;
  bool                mAutoScan;

  __dm_impl_DensityMap(const dmRegion& _Region ,dmHistogram& _Histo,
                       dm_real& _MinRange,dm_real& _MaxRange, bool _AutoScan)
   :mRegion (_Region  )
   ,mHisto   (_Histo   )
   ,mMinRange(_MinRange)
   ,mMaxRange(_MaxRange)
   ,mAutoScan(_AutoScan)
   {}

  // Generic operation on scalar
  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _img )
  {
     typedef typename dmIImage<_PixelFormat>::traits_type  traits_type;

     typedef typename traits_type::value_type   value_type;
     typedef typename traits_type::integer_type integer_type;

     gap<value_type> _g(traits_type::min(),traits_type::max());

     if(mAutoScan) {
       _g = minmax(mRegion,_img.Gen());
       mMinRange = _g.min();
       mMaxRange = _g.max();
     } else {
       _g = gap<value_type>(
          _get_range_value(mMinRange,traits_type(),integer_type()),
          _get_range_value(mMaxRange,traits_type(),integer_type())
       );
     }

     mHisto.GetHistogram(_img.Gen(),mRegion,
          daim::_extract_range<value_type>(_g));
  }

  // Specialization
  void operator()( dmIImage<dmPixelFormat24bppRGB>& _img ) {
    mHisto.GetHistogram(_img.Gen(),mRegion);
    mMinRange = 0;
    mMaxRange = 255;
  }

  void operator()( dmIImage<dmPixelFormat8bppIndexed>&  _img ) {
    mHisto.GetHistogram(_img.Gen(),mRegion);
    mMinRange = 0;
    mMaxRange = 255;
  }

};
//----------------------------------------------------------------------
bool dmDensityMap::Apply( dmImage& _Image, const dmRegion& _Region )
{
  __dm_impl_DensityMap _filter(_Region,
                       this->_Histogram,
                       this->_MinRange,this->_MaxRange,
                       this->_AutoScan);

  return dmImplementOperation(_filter,_Image);
}
//------------------------------------------------------------------------
