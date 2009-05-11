#ifndef dmDigitalRegion_h
#define dmDigitalRegion_h

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
// File         : dmDigitalRegion.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

template<class Op> 
inline Op& _dmDigitalRegion( const dmRgnHandle& rgn,  Op& op ) 
{ 
  long h,ys,i,j;
  dmRgnPair *pair;

  if(rgn.Empty()) return op;
  h  = rgn.Box().BoxHeight();
  ys = rgn.Box().top_left.y;
  rgnline_iterator l = rgn.Begin();
  for(i=0;i<h;i++) {
    pair = (*l)->rl_xpair;
    for(j=0;j<(*l)->rl_npair;j++) {
      op( i+ys,(*pair).x1,(*pair).x2 );
      ++pair;
    }
    ++l;
  }  
  return op;
}

//------------------------------------------------------------
template<class Op> 
inline Op& dmDigitalRegion( const dmRegion& rgn,  Op& op ) 
{ 
  if(rgn.IsRectRoi()) 
  {
    const dmRect& r = rgn.Rectangle();
    const long xs = r.Left(), xe = r.Right();
    const long ys = r.Top() , ye = r.Bottom();
    for(long y=ys;y<=ye;++y) {
      op(y,xs,xe);
    }
    return op;  
  } else
    return _dmDigitalRegion(rgn.Region(),op);
}
//------------------------------------------------------------


#endif // dmDigitalRegion_h
