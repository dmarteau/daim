
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
// dmFillRGB
//----------------------------------------------------------------------
bool dmFillRGB::Apply( dmBufferParameters& _Params )
{
  return SetValues(_Params.thisImage,_Params.thisRegion,
                   this->_Red,this->_Green,this->_Blue,
                   this->_Alpha);
}
//--------------------------------------------------------------------------
bool dmFillRGB::SetValues( dmImage& _src, const dmRegion& rgn,
                           dm_uint16 red,dm_uint16 green, dm_uint16 blue,
                           dm_real alpha )
{
  dmRGBColor rgb;
  rgb.r = _get_range_value(red  ,pixel_traits<dm_uint8>(),integer_true());
  rgb.g = _get_range_value(green,pixel_traits<dm_uint8>(),integer_true());
  rgb.b = _get_range_value(blue ,pixel_traits<dm_uint8>(),integer_true());

  if(dmIsPixelFormatScalar(_src.TypeDescriptor()->PixelFormat())) {
    return dmFillScalar::SetValue(_src,rgn,dm_rgb24::intensity(rgb),alpha);
  } 
  else
  {
    dmIImage<dmPixelFormat24bppRGB>* _Im = dmIImage<dmPixelFormat24bppRGB>::Cast(&_src);
    if(_Im) {
      dmRegion _roi = rgn;
      _roi.ClipToRect(_Im->Rect());
      if(alpha >= 1.0) {
        fill(_roi,_Im->Gen(),rgb);
      } else if(alpha >= 0) {
        blend_fill_rgb(_roi,_Im->Gen(),rgb,static_cast<dm_float>(alpha));
      }
      return true;
    }
  }
  return false;
}
//------------------------------------------------------------------------
