
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

using namespace daim;

//------------------------------------------------------------------------
// Scale image between min and max value
//----------------------------------------------------------------------

namespace {

struct scale_image_impl
{
  const dmRegion& Rgn;
  dm_real  Min,Max;
  scale_image_impl(const dmRegion& _Rgn, dm_real _Min, dm_real _Max)
  : Rgn(_Rgn),Min(_Min),Max(_Max) {}
  //-------------------------------------------------------
  // Generic operation on scalar
  //-------------------------------------------------------
  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& anImage ) 
  {
    typedef typename dmIImage<_PixelFormat>::image_type  image_type;
    typedef typename dmIImage<_PixelFormat>::pixel_type  pixel_type;
    typedef typename dmIImage<_PixelFormat>::traits_type traits_type;
    typedef typename dmIImage<_PixelFormat>::value_type  value_type;

    image_type& _img = anImage.Gen();
        
    gap<value_type> _range(
        traits_type::clamp(Min),
        traits_type::clamp(Max)
    );
   
    daim::scale_convert_scalar_to_scalar(
          daim::minmax(Rgn,_img),_range,
          _img,_img,Rgn,dmPoint(0,0));
  }
};

}; // namespace
//------------------------------------------------------------------------
bool dmScaleImage( const dmImage& _Src, const dmRegion& _Rgn, dm_real _Min, dm_real _Max )
{
   scale_image_impl _filter(_Rgn,_Min,_Max);
   return dmImplementScalarOperation(_filter,_Src);
}
//------------------------------------------------------------------------
bool dmScaleImage( const dmImage& _Src, dm_real _Min, dm_real _Max )
{
  return dmScaleImage(_Src,_Src.Rect(),_Min,_Max);
}
//------------------------------------------------------------------------
bool dmScaleImage( dmBufferParameters& _Params, dm_real _Min, dm_real _Max )
{
  return dmScaleImage(_Params.thisImage,_Params.thisRegion,_Min,_Max);
}
//------------------------------------------------------------------------

































