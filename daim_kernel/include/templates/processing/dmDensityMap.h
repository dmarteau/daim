#ifndef dmDensityMap_h
#define dmDensityMap_h

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
// File         : dmDensityMap.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#include <algorithm>

namespace daim {
//------------------------------------------------------------------------
struct _apply_map : public std::unary_function<dm_uint8,dm_uint8>
{
  dmColorMapArray _cmap;
  _apply_map( dmColorMapArray cm ) : _cmap(cm) {}

  template<class T>
  dm_uint8 operator()( const T& x ) {
     return static_cast<dm_uint8>(_cmap[static_cast<dm_uint8>(x)]);
  }
};
//--------------------------------------------------------------
struct _apply_rgb_map : public std::unary_function<dmRGBColor,dmRGBColor>
{
  dmRGBColorArray map;
  _apply_rgb_map( dmRGBColorArray _rgb ) : map(_rgb) {}
  dmRGBColor& operator()( const dmRGBColor& src, dmRGBColor& dst ) {
     dst.r = map[src.r].r;
     dst.g = map[src.g].g;
     dst.b = map[src.b].b;
     return dst;
  }
};
//------------------------------------------------------------------------
// Apply 8 bits and rgb color mapping
//------------------------------------------------------------------------
inline void apply_map( image<dm_uint8>& _img, const dmRegion& _rgn, dmColorMapArray _cmap )
{
  dmDEBUG_ASSERT( _img.rect() >= _rgn.Rectangle() );
  transform(_rgn,_img,_apply_map(_cmap));
}

// RGB implementation
inline void apply_map( image<dm_rgb24>& _img, const dmRegion& _rgn, dmRGBColorArray _cmap )
{
  dmDEBUG_ASSERT( _img.rect() >= _rgn.Rectangle() );
  combine(_rgn,_rgn.Rectangle().TopLeft(),_img,_img,_apply_rgb_map(_cmap));
}

//------------------------------------------------------------------------
// Apply histogram extension from range_from to range_to
// for 8 bits images the destination range is always set to [0,255]
//------------------------------------------------------------------------
template<class T>
void extend_density_range( image<T>& _img, const dmRegion& _rgn,
                           const gap< typename image<T>::value_type > _range_from,
                           const gap< typename image<T>::value_type > _range_to )
{
  typedef typename pixel_traits<T>::pixel_category pixel_category;

  dmDEBUG_ASSERT( _img.rect() >= _rgn.Rectangle() );
  pixel_scalar_tag _tag = pixel_category();
  scale_convert(_range_from,_range_to,_rgn,_img);
}

// specialize for 8 bits
void extend_density_range( image<dm_uint8>& _img, const dmRegion& _rgn,
                           const gap<dm_uint8> _range_from,
                           const gap<dm_uint8> _range_to );

//{
//  dmDEBUG_ASSERT( _img.Rect() >= _rgn.Rectangle() );
//  histogram_type h;
//  dmColorIndex _cmap[histogram_size];
//  extend_histogram_map( _cmap,_range_from,_range_to );
//  transform(rgn,_img,_apply_map(_cmap));
//}


//------------------------------------------------------------------------
// Invert pixels in a specified range of values
//------------------------------------------------------------------------
template<class T>
void invert_map( image<T>& _img, const dmRegion& _rgn,
                 gap< typename image<T>::value_type > _range )
{
  dmDEBUG_ASSERT( _img.rect() >= _rgn.Rectangle() );

  dmRegion reg;

  typedef typename image<T>::value_type value_type;

  // Set pixels > max range to min range
  create_rgnroi(_img,std::bind2nd(std::greater<value_type>(),_range.max()),reg,_rgn);
  fill(reg,_img,_range.min());

  // Set pixels < min range to max range
  create_rgnroi(_img,std::bind2nd(std::less<value_type>(),_range.min()),reg,_rgn);
  fill(reg,_img,_range.max());

  // Apply transformation in two passes to prevent numeric overflow
  transform(_rgn,_img,std::bind1st(std::minus<value_type>(),_range.max()));
  transform(_rgn,_img,std::bind1st(std::plus<value_type>() ,_range.min()));
}



void invert_map( image<dm_uint8>& _img, const dmRegion& _rgn );
/*
{
  dmDEBUG_ASSERT( _img.rect() >= _rgn.Rectangle() );
  typedef image<dm_uint8>::traits_type traits_type;
  typedef image<dm_uint8>::value_type  value_type;
  transform(_rgn,_img,__invert_8bits);
}
*/

}; // namespace daim

//----------------------------------------------------------------------
#endif // dmDensityMap_h
