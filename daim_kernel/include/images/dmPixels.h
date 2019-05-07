#ifndef dmPixels_h
#define dmPixels_h

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

//--------------------------------------------------------
// File         : dmPixels.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#include "templates/images/dmPixelTraits.h"
#include "templates/rgb/dmRGBTraits.h"

// Define pixel implementation

namespace daim {

template<class T,int _ScalarTag>
struct pixel_implementation
{
  typedef T                                     pixel_type;
  typedef pixel_traits<T>                       traits_type;
  typedef typename traits_type::value_type      value_type;
  typedef typename traits_type::pixel_category  pixel_category; 
  typedef typename traits_type::signature_type  signature_type;

  enum { scalar_tag = _ScalarTag, };

  static int ord() { return scalar_tag; }
};

}; // namespace daim

template<EPixelFormat _PixelFormat> struct dmPixelFormat {}; 
template<> struct dmPixelFormat<dmPixelFormat8bppIndexed>    : daim::pixel_implementation<dm_uint8  ,1>  {};
template<> struct dmPixelFormat<dmPixelFormat24bppRGB>       : daim::pixel_implementation<dm_rgb24  ,1>  {};
template<> struct dmPixelFormat<dmPixelFormat16bppGrayScale> : daim::pixel_implementation<dm_uint16 ,2>  {};
template<> struct dmPixelFormat<dmPixelFormat32bppGrayScale> : daim::pixel_implementation<dm_int32  ,3>  {};
template<> struct dmPixelFormat<dmPixelFormat32bppFloat>     : daim::pixel_implementation<float     ,4>  {};

#endif // dmPixels_h
