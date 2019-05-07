#ifndef dmRGBThreshold_h
#define dmRGBThreshold_h

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
// File         : dmRGBThreshold.h
// Date         : 9/2005
// Author       : David Marteau
//--------------------------------------------------------

namespace daim {

  struct rgb_threshold : public std::unary_function<dmRGBColor,bool>
  {
    float   rr,rg,rb;
    dmRGBColor color;

    rgb_threshold( const dmRGBColor& _color, float _rr, float _rg, float _rb ) 
    : rr(_rr)
    , rg(_rg)
    , rb(_rb)
    , color(_color)
    {}
/*
    bool operator()( const dmRGBColor& val ) {
      return  (sqrt(pow(static_cast<dm_real>(daim::absdiff(val.red,  color.red  )),2.0))/rr +
               sqrt(pow(static_cast<dm_real>(daim::absdiff(val.green,color.green)),2.0))/rg +
               sqrt(pow(static_cast<dm_real>(daim::absdiff(val.blue, color.blue )),2.0))/rb) <= 1.0f;
    }
*/

    bool operator()( const dmRGBColor& val ) {
      return  (static_cast<float>(daim::absdiff(val.r,color.r))/rr +
               static_cast<float>(daim::absdiff(val.g,color.g))/rg +
               static_cast<float>(daim::absdiff(val.b,color.b))/rb) <= 1.0f;
    }

  };

} // namespace daim

//--------------------------------------------------------
#endif // dmRGBThreshold_h
