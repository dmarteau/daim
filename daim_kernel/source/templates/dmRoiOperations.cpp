
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

#define dmUserIncludes
#include "daim_kernel.h"

namespace daim {

//---------------------------------------------------------------------------------
bool __dmKernel _AdjustRegions( dmRect& _rs, dmRect& _rd, dmRegion& _rgn, const dmPoint& p )
{
  dmRect rs = _rgn.Rectangle();
  if( dmClipRectangle(rs,_rs) )  // clip to source image rectangle
  {
    dmRect rd(p.x,p.y,rs.Width(),rs.Height() );  // get destination rectangle
    if( dmClipRectangle( rd,_rd ) )              // check if dest rect fit into dest image
    {
      dmPoint ps = rs.TopLeft();
      if(!dmPointInRectangle(p,rd)) { ps += rd.TopLeft() - p; }

      rs =  dmRect(ps.x,ps.y,rd.Width(),rd.Height()); // readjust source rectangle
      _rgn.ClipToRect( rs );                          // clip to source rectangle
      if(_rgn.IsValidRoi() && !_rgn.IsEmptyRoi())  {
         _rs = rs;
         _rd = rd;
         return true;
      }
    }
  }
  return false;
};
//---------------------------------------------------------------------------------

}; //  namespace daim
