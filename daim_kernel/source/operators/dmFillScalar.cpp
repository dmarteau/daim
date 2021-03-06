
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
#include "templates/rgb/dmRGBArithmetics.h"

using namespace daim;


//------------------------------------------------------------------------
// Fill scalar 
//----------------------------------------------------------------------

namespace {

struct fill_scalar_impl
{
  double   _value;
  double   _alpha;
  dmRegion _rgn;

  fill_scalar_impl( double _v, double alpha, const dmRegion& _roi ) 
  : _value(_v),_alpha(alpha),_rgn(_roi) {}

  // Generic operation on scalar
  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& anImage )
  {
    typedef typename dmIImage<_PixelFormat>::image_type  image_type;
    typedef typename dmIImage<_PixelFormat>::pixel_type  pixel_type;
    typedef typename dmIImage<_PixelFormat>::value_type  value_type;
    typedef typename dmIImage<_PixelFormat>::traits_type traits_type;

    image_type& _img = anImage.Gen();

    _rgn.ClipToRect( _img.rect() );
    if(_alpha >=1) {
      fill(_rgn,_img,traits_type::clamp(_value));
    } else if(_alpha >=0) {
     blend_fill(_rgn,_img,traits_type::clamp(_value),_alpha);
    }
  }

  // Specialize for RGB
  void operator()( dmIImage<dmPixelFormat24bppRGB>& anImage ) 
  {
    typedef dmIImage<dmPixelFormat24bppRGB>::image_type   image_type;
    typedef dmIImage<dmPixelFormat24bppRGB>::pixel_type   pixel_type;
    typedef dmIImage<dmPixelFormat24bppRGB>::traits_type  traits_type;

    image_type& _img = anImage.Gen();

    _rgn.ClipToRect( anImage.Rect() );
    dm_uint8 x = pixel_traits<dm_uint8>::clamp(_value);
    dm_rgb24 _rgb(x);
    if(_alpha >=1) {
      fill(_rgn,_img,_rgb.value());
    } else if(_alpha >=0) {
      blend_fill_rgb(_rgn,_img,_rgb.value(),_alpha);      
    }
  }
};

};
//--------------------------------------------------------------------------
bool dmFillScalar::SetValue( dmImage& _Image, const dmRegion& aRgn, dm_real aValue, dm_real aAlpha )
{
  fill_scalar_impl _filter(aValue,aAlpha,aRgn);
  return dmImplementOperation(_filter,_Image);
}
//--------------------------------------------------------------------------
bool dmFillScalar::Apply( dmBufferParameters& _Params )
{
  fill_scalar_impl _filter(this->_Value,this->_Alpha,_Params.thisRegion);
  return dmImplementOperation(_filter,_Params.thisImage);
} 
//------------------------------------------------------------------------
