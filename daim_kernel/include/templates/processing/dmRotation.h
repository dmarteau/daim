#ifndef dmRotation_h
#define dmRotation_h

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
// File         : dmRotation.h
// Date         : 9/2005
// Author       : David Marteau
//--------------------------------------------------------

namespace daim {

//-----------------------------------------------------------
// Rotate image from specified rectangle to image desination
// Nearest neighbours algorithm : for any point in source image, compute the 
// coordinates of the corresponding source pixel by the inverse
// rotation transform.  
// Reference points are taken from the respective center of 
// the destination and source rectangle.
//-----------------------------------------------------------
bool create_rotate_region( dmRegion& _result,  dm_real angle, 
                           const dmRect& _rs, 
                           const dmRect& _rd );

namespace rotate_impl {

  template<class T>
  struct compute_pixels
  {
    const daim::image<T>&  _src;
    daim::image<T>&        _dst;
    dm_point   pcs,pcd;
    dm_rect    rs;
    dm_real    s,c;
 
    typedef typename pixel_traits<T>::value_type value_type;
  
    compute_pixels( const daim::image<T>& src, daim::image<T>& dst, dm_rect& _rs, dm_real _c, dm_real _s )
    : _src(src),_dst(dst),rs(_rs),s(_s),c(_c)
    {
      rs  = _src.rect();
      pcd = _dst.rect().Center();
      pcs = _src.rect().Center();
    }

    void operator()( int y, int x1, int x2 ) 
    { 
      dm_int xx,yy,xp,yp;

      yp  = y - pcd.y;
      x1 -= pcd.x;
      x2 -= pcd.x;

      typename image<T>::pointer_type ld = _dst[y];

      for(xp=x1;xp<=x2;++xp) 
      { 
        xx = static_cast<dm_int>(daim::round(xp * c - yp * s + pcs.x));
        yy = static_cast<dm_int>(daim::round(xp * s + yp * c + pcs.y));
        if(dmPointInRectangle(xx,yy,rs))
            ld[xp+pcd.x]= _src[yy][xx];
      }     
    }
  };

} // namespace rotate_impl
//-----------------------------------------------------------
template<class T>
bool nn_rotate_copy( dm_real angle, const image<T>& src, const dmRect& r, image<T>& dst )
{
  dmRect rs = r;

  typedef typename rotate_impl::compute_pixels<T> compute_pixels;

  if(dmClipRectangle(rs,src.rect())) 
  {
    dmRect rd = dst.rect();
    dmRegion rgn;

    if(create_rotate_region(rgn,angle,rs,rd))
    {
      dm_double c = cos(-angle);
      dm_double s = sin(-angle);

      compute_pixels _cpxls(src,dst,rs,c,s);
      dmDigitalRegion(rgn,_cpxls);
      return true;
    }
  }
  return false;
}

} // namespace daim
//--------------------------------------------------------

#endif // dmRotation_h


