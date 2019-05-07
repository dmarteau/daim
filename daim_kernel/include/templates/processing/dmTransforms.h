#ifndef dmTransforms_h
#define dmTransforms_h

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
// File         : dmTransforms.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#include <algorithm>

namespace daim {
//-----------------------------------------------------------
// Flip the pixels from rectangular are r int src and copy
// the result at point p into dst. 
// Horizontal and vertical flip can be handled in one pass.
//-----------------------------------------------------------
template<class T>
void flip_copy( const image<T>& src, image<T>& dst,
                const dmRect& r, const dmPoint& p, bool flipHor,bool flipVer )
{
  dmRect rd(p.x,p.y,r.Width(),r.Height());

  if(dmClipRectangle(rd,dst.rect())) 
  {
    // Part of the image wich will be copied
    dmRect rs(r.TopLeft()+(rd.TopLeft()-p),rd.Width(),rd.Height());
    if(flipVer) rs.Translate(0,r.Top() -rs.Top() +r.Bottom()-rs.Bottom());
    if(flipHor) rs.Translate(  r.Left()-rs.Left()+r.Right() -rs.Right(),0);

    typename image<T>::line_type       ld = dst.begin(rd);
    typename image<T>::const_line_type ls = (flipVer ? src.last(rs) : src.begin(rs));
    for(int h=rd.Height();--h>=0;++ld)
    {
      if(flipHor) 
         std::reverse_copy(*ls+rs.Left(),*ls+rs.Right()+1,*ld+rd.Left());
      else        
         std::copy(*ls+rs.Left(),*ls+rs.Right()+1,*ld+rd.Left());

      if(flipVer) 
          --ls; 
        else 
          ++ls;
    }
  }
}
//----------------------------------------------------------
// create_motif :
// This function create regular representation of
// the rectangular area r over the whole image.
// Note: the position of the original area is left 
// unchanged in the image.
//----------------------------------------------------------
template<class T>
bool create_motif( image<T>& img, const dmRect& r, const dmRect& rdest )
{
  int xs,ys;
  dmRect rs( r     );
  dmRect rd( rdest );

  if( !dmClipRectangle(rd,img.rect()) || !dmClipRectangle(rs,rd) ) 
    return false;
  
  // rs contains the part to duplicate

  int __width  = rs.Width();
  int __height = rs.Height();

  int iw = rd.Width();
  int ih = rd.Height();

  xs = ((__width  - rs.Left() % __width ) % __width );
  ys = ((__height - rs.Top()  % __height) % __height);   

  dmPoint p;
  for(p.y = -ys; p.y < ih ; p.y += __height ) {
    for(p.x = -xs; p.x < iw; p.x += __width ) {
      if( p != rs.TopLeft() )  // Do not copy onto itself
        img.copy(img,rs,p);
    }
  } 
  return true;
}
//----------------------------------------------------------
// make_periodic :
// This function create a periodic representation of
// the rectangular area r over the whole image.
// Note: the position of the original area is left 
// unchanged in the image.
//----------------------------------------------------------
template<class T>
bool make_periodic( image<T>& img, const dmRect& r, const dmRect& rdest )
{
  int xs,ys,wflip,hflip,_wflip;
  dmRect rs( r     );
  dmRect rd( rdest );

  if( !dmClipRectangle(rd,img.rect()) || !dmClipRectangle(rs,rd) ) 
    return false;
  
  // rs contains the part to duplicate

  int __width  = rs.Width();
  int __height = rs.Height();

  int iw = rd.Width();
  int ih = rd.Height();

  xs = ((__width  - rs.Left() % __width ) % __width );
  ys = ((__height - rs.Top()  % __height) % __height);   

  wflip = (rs.Left() +xs)/ __width ;  // numbers of flips into x direction
  hflip = (rs.Top()  +ys)/ __height;  // numbers of flips into y direction

  dmPoint p;
  for(p.y = -ys; p.y < ih ; p.y += __height,--hflip ) 
  {
    _wflip = wflip;
    for(p.x = -xs; p.x < iw; p.x += __width,--_wflip ) {
      if( p != rs.TopLeft() )  // Do not copy onto itself
        flip_copy( img,img,rs,p,(_wflip & 0x1),(hflip & 0x1));
    }
  } 
  return true;
}
//-----------------------------------------------------------
// Rotate the pixels from rectangular are r int src and copy
// the result at point p into dst. 
//-----------------------------------------------------------
template<class T>
bool rotate_copy( const image<T>& src, image<T>& dst,
                  const dmRect& r, const dmPoint& p, int toright )
{
  dmRect rs = r;
  if( dmClipRectangle(rs,src.rect()) ) 
  {
    dmRect  rd(p.x,p.y,rs.Height(),rs.Width()); // Dest Rectangle (invert height and width)
    dmPoint ps = (toright?rd.TopRight():rd.TopLeft()); 
    if( dmClipRectangle(rd,dst.rect()) ) 
    {
      if(toright) 
      {
        ps = rd.TopRight() - ps;  // Offset  
        rs = dmRect(rs.Left()+ps.y,rs.Top()-ps.x,rd.Height(),rd.Width());

        typename image<T>::const_line_type  ls = src.begin(rs);
        typename image<T>::line_type        ld = dst.begin(rd);

        int w = rs.Width();

        for(int h=rs.Height();--h>=0;++ls) {
          for(int y=0;y<w;++y) 
            ld[y][h] = (*ls)[y];
        }
      } 
      else 
      {
        ps = rd.TopLeft() - ps;  // Offset  
        rs = dmRect(rs.Left(),rs.Top()+ps.x,rd.Height()-ps.y,rd.Width());

        typename image<T>::const_line_type  ls = src.begin(rs);
        typename image<T>::line_type        ld = dst.begin(rd);

        int h = rs.Height();

        for(int x=0;x<h;++x,++ls) {
          for(int yy=0,y=rs.Width();--y>=0;++yy)
            ld[yy][x] = (*ls)[y];
        }        
      }
      return true;
    }
  }
  return false;
}
//----------------------------------------------------------

}; // namespace daim

#endif // dmTransforms_h
