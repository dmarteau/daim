#ifndef dmSplitRGB_h
#define dmSplitRGB_h

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
// File         : dmSplitRGB.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

namespace daim 
{


template<class S,class T>
bool split_rgb( S& splitter, 
                const image<dm_rgb24>& in, 
                image<T>& ch_x,
                image<T>& ch_y,
                image<T>& ch_z,
                const dmRect& rect,
                const dmPoint& to  )
{
   dmRect srcRect(rect);
   if(srcRect.Clip(in.rect()))
   {      
     dmRect dstRect(to,srcRect.Width(),srcRect.Height());
     if( dstRect.Clip(ch_x.rect()) && 
         dstRect.Clip(ch_y.rect()) &&
         dstRect.Clip(ch_z.rect()))
     {
       image<dm_rgb24>::const_line_type lstart = in.begin(srcRect);
       image<dm_rgb24>::const_line_type lend   = in.end(srcRect);

       typename image<T>::line_type lx = ch_x.begin() + dstRect.Top();
       typename image<T>::line_type ly = ch_y.begin() + dstRect.Top();
       typename image<T>::line_type lz = ch_z.begin() + dstRect.Top();

       dm_int x,xstart = dstRect.Left();
       size_t i,width  = srcRect.Width();

       image<dm_rgb24>::const_pointer_type pstart;

       for(;lstart != lend;++lstart,++lx,++ly,++lz) 
       {
         pstart = (*lstart) + srcRect.Left();
         for(x=xstart,i=0;i<width;++i,++x) {    
           splitter( pstart[i], (*lx)[x], (*ly)[x], (*lz)[x] );
         }
       }

       return true;
     }
   }
   return false;
}

template<class M,class T>
bool merge_rgb( M& merger, 
                image<dm_rgb24>& out,
                const image<T>& ch_x,
                const image<T>& ch_y,
                const image<T>& ch_z,
                const dmRect& rect,
                const dmPoint& to )
{
   dmRect srcRect(rect);
   if( srcRect.Clip(ch_x.rect()) && 
       srcRect.Clip(ch_y.rect()) &&
       srcRect.Clip(ch_z.rect()))
   {
     dmRect dstRect(to,srcRect.Width(),srcRect.Height());
     if(dstRect.Clip(out.rect()))
     {       
       image<dm_rgb24>::line_type lstart = out.begin(dstRect);
       image<dm_rgb24>::line_type lend   = out.end(dstRect);

       typename image<T>::const_line_type lx = ch_x.begin(srcRect);
       typename image<T>::const_line_type ly = ch_y.begin(srcRect);
       typename image<T>::const_line_type lz = ch_z.begin(srcRect);

       dm_int x,xstart = srcRect.Left();
       size_t i,width  = dstRect.Width();

       image<dm_rgb24>::pointer_type pstart;

       for(;lstart != lend;++lstart,++lx,++ly,++lz) 
       {         
         pstart = (*lstart) +  dstRect.Left();
         for(x=xstart,i=0;i<width;++i,++x) {
           merger( pstart[i], (*lx)[x], (*ly)[x], (*lz)[x] );
         }
       }

       return true;
     }
   }
   return false;
}


} // namespace daim

#endif // dmSplitRGB_h
