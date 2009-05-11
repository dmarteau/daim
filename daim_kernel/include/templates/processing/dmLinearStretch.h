#ifndef dmLinearStretch_h
#define dmLinearStretch_h

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
// File         : dmLinearStretch.h
// Date         : 6/2005
// Author       : David Marteau
//--------------------------------------------------------

namespace daim {

//-----------------------------------------------------------
// Apply a nearest-neighbourg stretch algorithm on source image
//-----------------------------------------------------------
template<class T>
bool nn_stretch( const image<T>& src, const dmRect& r,
                 image<T>& dst )
{
  dmRect rs = r;

  if(dmClipRectangle(rs,src.rect()))
  {
    dmRect rd = dst.rect();

    int i,j,win,hin,wout,hout;

    win = rs.Width() -1 ; wout = rd.Width()  -1;
    hin = rs.Height()-1 ; hout = rd.Height() -1;

    dm_int istart = rd.Left();
  //dm_int jstart = rd.Top();

    dm_int iistart = rs.Left();
    dm_int jjstart = rs.Top();

    typename image<T>::const_pointer_type ps;
    typename image<T>::line_type ld = dst.begin(rd);

    for(j=0;j<=hout;++j,++ld) {
      ps = src[ (j * hin) / hout + jjstart ];
      for(i=wout;i>=0;--i)
         (*ld)[i+istart] = ps[ (i * win) / wout + iistart ];
    }
    return true;
  }
  return false;
}

//-----------------------------------------------------------
// Apply a bilinear strectch on source image
// The easiest way is to construct an intermediate image
// for computing strecthing on rows, then on colums
//-----------------------------------------------------------
template<class T>
bool bilinear_stretch( const image<T>& src, const dmRect& r, image<T>& dst )
{
  dmRect rs = r;

  if(dmClipRectangle(rs,src.rect()))
  {
    dmRect rd = dst.rect();

    dm_int i,j,ii,jj,win,hin,wout,hout;

    win = rs.Width() - 1; wout = rd.Width() - 1;
    hin = rs.Height()- 1; hout = rd.Height()- 1;

    dm_uint istart = rd.Left();
 // dm_int jstart = rd.Top();

    dm_uint iistart = rs.Left();
 // dm_int jjstart = rs.Top();

    cont_image<T> buf(rd.Width(),rs.Height());

    int e,e100,f100;
    //int win100 = win * 100;
    //int hin100 = hin * 100;

    dm_real wstep100 = 100.0 * static_cast<dm_real>(win)/wout;
    dm_real hstep100 = 100.0 * static_cast<dm_real>(hin)/hout;

    // Stretch lines
    // Note that we work with integer !!

    typename image<T>::const_line_type ls = src.begin(rs);
    typename image<T>::line_type       ld = buf.begin();

    // Copy last column
    ii = win+iistart;
    for(jj=hin;jj>=0;++ld,++ls,--jj) {
      (*ld)[wout] = (*ls)[ii];
    }

    for(i=wout-1;i>=0;--i)
    {
      e100 = static_cast<int>(i*wstep100);  e = e100 / 100;
      f100 = (e100  - e * 100);

      ii = e + iistart;

      ls = src.begin(rs);
      ld = buf.begin();

      for(jj=hin;jj>=0;++ld,++ls,--jj) {
        (*ld)[i] = ((*ls)[e] * (100 - f100) + f100 * (*ls)[e+1] )/100;
      }
    }

    // Stretch columns
    // Note that we work with integer !!

    ls = buf.begin()   + hin;
    ld = dst.begin(rd) + hout;

    for(i=wout;i>=0;--i) {
      (*ld)[i+istart] = (*ls)[i];
    }

    ls = buf.begin();
    ld = dst.begin(rd) + hout - 1;

    for(j=hout-1;j>=0;--ld,--j)
    {
      e100 = static_cast<int>(j*hstep100);  e = e100 / 100;
      f100 = (e100  - e * 100);

      for(i=wout;i>=0;--i) {
        (*ld)[i+istart] = ( ls[e][i] * (100 - f100) + f100 * ls[e+1][i] )/100;
      }
    }
    return true;
  }
  return false;
}

//-----------------------------------------------------------
// Specialize for rgb
//-----------------------------------------------------------
bool bilinear_stretch( const image<dm_rgb24>& src,  const dmRect& r,
                       image<dm_rgb24>& dst );

} // namespace daim
//--------------------------------------------------------

#endif // dmLinearStretch_h


