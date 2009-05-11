
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
#include "templates/processing/dmLinearStretch.h"

#include <math.h>

namespace daim {

bool create_rotate_region( dmRegion& _result,  dm_real angle, 
                           const dmRect& _rs, 
                           const dmRect& _rd )
{
  dmRect rs = _rs;
  dmRect rd = _rd;
  
  // Compute the corresponding ROI in dest image
  dmPoint pcs = rs.Center();
  dmPoint pcd = rd.Center(); 

  dmPoly pts(5);
  pts[0] = rs.TopLeft()     - pcs;
  pts[1] = rs.TopRight()    - pcs;
  pts[2] = rs.BottomRight() - pcs;
  pts[3] = rs.BottomLeft()  - pcs;
 
  dm_double x,y;
  dm_double c = cos(angle);
  dm_double s = sin(angle);

  // Direct Rotation
  for( int i=0; i < 4; ++i )  {
     x = pts[i].x;      
     y = pts[i].y;
  
     pts[i].x = static_cast<dm_int>(daim::round(x * c - y * s + pcd.x));
     pts[i].y = static_cast<dm_int>(daim::round(x * s + y * c + pcd.y));
  }

  pts[4] = pts[0];

  // Build the region
  _result.SetRoi(pts);
  _result.AndCoordinates(_rd);

  return !_result.IsEmptyRoi();
}
//--------------------------------------------------------


} // namespace daim



