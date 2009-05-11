
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

#include "templates/processing/dmObjectDetection.h"

namespace daim {

struct _append_to_vector {
   dmPoly& _p;
   _append_to_vector( dmPoly& aVector ) : _p(aVector) {}
   bool operator()( int x, int y ) {
     _p.Push_Back(dmPoint(x,y));
     return true;
   }
};

int extract_rgn_boundary( const dmRegion& _rgn, dmPoly& _p )
{
  if(!_rgn.IsEmptyRoi()) 
  {
    if(_rgn.IsRectRoi()) {
      _p.Push_Back(_rgn.Rectangle().TopLeft());
      _p.Push_Back(_rgn.Rectangle().TopRight());
      _p.Push_Back(_rgn.Rectangle().BottomRight());
      _p.Push_Back(_rgn.Rectangle().BottomLeft());
      _p.Push_Back(_rgn.Rectangle().TopLeft());    // close polygone
    } else  {
      _append_to_vector _operator(_p);
      dmPoint ps = _rgn.RegionStart();  // Get the first point in region
      boundary_search(ps,_rgn,bind_ref(_operator),check_point_true());
      if(!_p.Closed())
        _p.Push_Back(_p[0]);
    }
  }
  return _p.Size();  
}

}; // namespace daim
