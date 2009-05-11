
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

#define dmUseKernelTemplates
#include "daim_kernel.h"
#include "templates/processing/dmDensityMap.h"
#include "templates/processing/_dmHistogram.h"

namespace daim {
// specialize for 8 bits
void extend_density_range( image<dm_uint8>& _img, const dmRegion& _rgn,
                           const gap< image<dm_uint8>::value_type > _range_from,
                           const gap< image<dm_uint8>::value_type > _range_to )
{
  dmDEBUG_ASSERT( _img.rect() >= _rgn.Rectangle() );
  dmColorIndex _cmap[histogram_size];
  extend_histogram_map( _cmap,_range_from,_range_to );
  transform(_rgn,_img,_apply_map(_cmap));
}


#define __invert_8bits std::bind1st(std::minus<value_type>(),traits_type::object())

void invert_map( image<dm_uint8>& _img, const dmRegion& _rgn )
{      
  dmDEBUG_ASSERT( _img.rect() >= _rgn.Rectangle() );
  typedef image<dm_uint8>::traits_type traits_type;
  typedef image<dm_uint8>::value_type  value_type;
  transform(_rgn,_img,__invert_8bits);    
}

} // namespace daim
