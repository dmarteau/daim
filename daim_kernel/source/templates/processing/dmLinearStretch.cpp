
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

namespace daim {

static inline void _bilinear_rgb(  dmRGBColor& y,
                                   const dmRGBColor& x1,
                                   const dmRGBColor& x2,
                                   int   f100 )
{
   y.a = (x1.a * (100 - f100) + f100 * x2.a )/100;
   y.r = (x1.r * (100 - f100) + f100 * x2.r )/100;
   y.g = (x1.g * (100 - f100) + f100 * x2.g )/100;
   y.b = (x1.b * (100 - f100) + f100 * x2.b )/100;
}

//-----------------------------------------------------------
// Apply a bilinear strectch on source image
// The easiest way is to construct an intermediate image
// for computing strecthing on rows, then on colums
//-----------------------------------------------------------
bool bilinear_stretch( const image<dm_rgb24>& src, const dmRect& r,
                       image<dm_rgb24>& dst )
{
  dmRect rs = r;

  if(dmClipRectangle(rs,src.rect()))
  {
    dmRect rd = dst.rect();

    int i,j,ii,jj,win,hin,wout,hout;

    win = rs.Width() - 1; wout = rd.Width() - 1;
    hin = rs.Height()- 1; hout = rd.Height()- 1;

    dm_int istart = rd.Left();
    DM_UNUSUED(dm_int jstart = rd.Top(););

    dm_int iistart = rs.Left();
    DM_UNUSUED(dm_int jjstart = rs.Top(););

    cont_image<dm_rgb24> buf(rd.Width(),rs.Height());

    int e,e100,f100;
    //int win100 = win * 100;
    //int hin100 = hin * 100;

    dm_real wstep100 = 100.0 * static_cast<dm_real>(win)/wout;
    dm_real hstep100 = 100.0 * static_cast<dm_real>(hin)/hout;

    // Stretch lines
    // Note that we work with integer !!

    image<dm_rgb24>::const_line_type ls = src.begin(rs);
    image<dm_rgb24>::line_type       ld = buf.begin();

    ii = win+iistart;
    for(jj=hin;jj>=0;++ld,++ls,--jj) {
      (*ld)[wout] = (*ls)[ii];
    }

    for(i=wout-1;i>=0;--i)
    {
      //e100 = (i*win100) / wout;  e = e100 / 100;
      e100 = static_cast<int>(i*wstep100);  e = e100 / 100;
      f100 = (e100  - e * 100);

      ii = e + iistart;

      ls = src.begin(rs);
      ld = buf.begin();

      for(jj=hin;jj>=0;++ld,++ls,--jj) {
        _bilinear_rgb( (*ld)[i], (*ls)[e], (*ls)[e+1], f100);
        //(*ld)[i] = ((*ls)[e] * (100 - f100) + f100 * (*ls)[e+1] )/100;
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
      //e100 = (j*hin100) / hout;  e = e100 / 100;
      e100 = static_cast<int>(j*hstep100);  e = e100 / 100;
      f100 = (e100  - e * 100);

      for(i=wout;i>=0;--i) {
        _bilinear_rgb( (*ld)[i+istart], ls[e][i], ls[e+1][i], f100);
        // (*ld)[i+istart] = ( ls[e][i] * (100 - f100) + f100 * ls[e+1][i] )/100;
      }
    }
    return true;
  }
  return false;
}

//--------------------------------------------------------
} // namespace daim



