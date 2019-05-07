#ifndef dmUpSampling_h
#define dmUpSampling_h

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
// File         : dmUpSampling.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

namespace daim {

// Up sample the src image with step \a step

template<class T1,class T2>
void upSampling( const dmRect& rect,const dmPoint& to, 
                 const image<T1>& in, image<T2>& out, 
                 const dmPoint& step )
{
  #ifdef _DEBUG
   dmRect dstRect(to,rect.Width()*step.x,rect.Height()*step.y);
   dmASSERT( out.rect().Right()  >=  dstRect.Right()  );
   dmASSERT( out.rect().Bottom() >=  dstRect.Bottom() );
   dmASSERT( in.rect() >= rect );
  #endif

  out.fill(pixel_traits<T2>::zero());

  typename image<T1>::const_line_type lstart = in.begin(rect);
  typename image<T1>::const_line_type lend   = in.end(rect);
  typename image<T2>::line_type       lout   = out.begin() + to.y;

  dm_int x,xstart = to.x;
  size_t i,width  = rect.Width();

  for(;lstart != lend;++lstart,lout=lout+step.y) {
    for(x=xstart,i=0;i<width;++i,x+=step.x) 
      (*lout)[x] = (*lstart)[i];   
  }

} 



} // namespace daim

#endif // dmUpSampling_h
