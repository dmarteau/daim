
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

#include "daim_kernel.h"
#include "dmMatrixData.h"
#include "dmMatrixFT.h"

#include "templates/processing/dmFTFilters.h"

#include <math.h>

namespace daim {

void _ft_roi_operation(  dmFT_Data& _ft, const dmRgnHandle& _rgn, 
                         const _ft_vt_operation& vt )
{
  dm_complex_t r;

  if(!_rgn.Empty()) 
  {
    float*  _row;
    long ky,kx,kx1,kx2,nx,ny,nxs2,nys2,j,i,y;
    
    nx =  _ft.Dim2; nxs2 = nx/2;
    ny =  _ft.Dim1; nys2 = ny/2;

    dmRgnHandle rgn(_rgn);
    rgn.ClipToRect( dmRect(nxs2,0,nx,ny) );

    y = rgn.Box().Top();
    dmRgnHandle::iterator it  = rgn.Begin();
    dmRgnHandle::iterator end = rgn.End();

    for(dmRgnPair *p,*pend;it!=end; ++it,++y) 
    {
      ky = y - nys2;      
      j = (ky<0? ny + 1 + ky : ky + 1);
      _row = dmFT_DATA(_ft)[j];
      for(p=it.begin(),pend=it.end();p!=pend;++p) 
      {
        kx1 = (*p).x1 - nxs2;
        kx2 = (*p).x2 - nxs2;
        for(kx=kx1;kx<=kx2;++kx) 
        {
          i = 2*(kx+1);
          r = vt.apply( kx, ky, dm_complex_t(_row[i-1],_row[i]) );
          _row[i-1] = r.real();
          _row[i]   = r.imag();
        }
      }
    }
    // handle speq region
    rgn = _rgn;
    rgn.ClipToRect( dmRect(0,0,1,ny) );
    if(!rgn.Empty()) 
    {
      _row = dmFT_SPEQ(_ft);
      y = rgn.Box().Top();
  	  dmRgnHandle::iterator it  = rgn.Begin();
  	  dmRgnHandle::iterator end = rgn.End();
  	  for(;it!=end;++it,++y) {
        if((*it)->rl_npair) 
        {
          ky = y - nys2;
          kx = - nxs2;      
          j = 2*(ky<0? ny + 1 + ky : ky + 1);
          r = vt.apply( kx, ky, dm_complex_t(_row[j-1],_row[j]) );
          _row[j-1] = r.real();
          _row[j]   = r.imag();
        }    
      }
    }
  }
}
//-------------------------------------------------------------
void _ft_roi_operation(  dmFT_Data& _ft, const dmRect& _r, 
                         const _ft_vt_operation& vt )
{
  dm_complex_t _c;

  dmRect r = _r;
  float*  _row;

  long ky,kx,kx1,kx2,nx,ny,nxs2,nys2,j,i,y;

  nx =  _ft.Dim2; nxs2 = nx/2;
  ny =  _ft.Dim1; nys2 = ny/2;

  if(dmClipRectangle(r,nxs2,0,nx-1,ny-1)) 
  {
    for(y = r.Top();y <= r.Bottom();++y) 
    {
      ky = y - nys2;      
      j = (ky<0? ny + 1 + ky : ky + 1);
      _row = dmFT_DATA(_ft)[j];
      kx1 = r.Left()  - nxs2;
      kx2 = r.Right() - nxs2;
      for(kx=kx1;kx<=kx2;++kx) 
      {
        i = 2*(kx+1);
        _c = vt.apply( kx, ky, dm_complex_t(_row[i-1],_row[i]) );
        _row[i-1] = _c.real();
        _row[i]   = _c.imag();
      }
    }
  }

  // handle speq region
  r  = _r;
  if(dmClipRectangle(r,nx,0,nx,ny)) 
  {
    _row = dmFT_SPEQ(_ft);
    for(y = r.Top();y <= r.Bottom();++y) {
      ky = y - nys2;
      kx = - nys2;      
      j = 2*(ky<0? ny + 1 + ky : ky + 1);
      _c = vt.apply( kx, ky, dm_complex_t(_row[j-1],_row[j]) );
      _row[j-1] = _c.real();
      _row[j]   = _c.imag();
    }    
  }
}
//------------------------------------------------------------------------
void ft_mul_filter( dmFT_Data& ft, const dmRegion& rgn, dm_complex_t z )
{
  using namespace std;
  ft_roi_operation(
    ft,rgn,
    ft_bind_unary_op( bind2nd(multiplies<dm_complex_t>(),z) )
  );
}
//------------------------------------------------------------------------
void ft_build_region( dmFT_Data& ft, const dmRegion& rgn, dmRegion& dst, 
                      bool quad )
{
  if(rgn.IsEmptyRoi()) {
    dst.KillRoi();
    return;
  }

  dmPoint ref(ft.Dim1 >> 1, ft.Dim2 >> 1);
  dmPoint p = rgn.Rectangle().Center();

  if(quad) 
  {
    dst = rgn;
    dmRegion tmp;

    rgn.FlipRoi( tmp, true, false );      // Symetry /x
    tmp.Translate( 2*(ref.x - p.x), 0 );
    dst.AddRoi( tmp );

    rgn.FlipRoi( tmp, false, true );      // Symetry /y
    tmp.Translate( 0, 2*(ref.y - p.y) );
    dst.AddRoi( tmp );

    rgn.FlipRoi( tmp, true, true );       // Symetry /xy
    tmp.Translate( 2*(ref.x - p.x), 2*(ref.y - p.y) );
    dst.AddRoi( tmp );

  } else {
    rgn.FlipRoi( dst, true, true );
    p.x = 2*(ref.x - p.x);
    p.y = 2*(ref.y - p.y);
    dst.Translate( p.x, p.y );
    dst.AddRoi(rgn);
  }
}
//------------------------------------------------------------------------
void ft_filter_region( dmFT_Data& ft, const dmRegion& rgn, int fwidth, bool fcut )
{
  dmRegion _outline,_rgn = rgn;

  if(fwidth>1) 
  {
    float w,z = static_cast<float>(2.0*acos(-1.0)/(fwidth-1));
    for(int i=0;i<fwidth; ++i) 
    {
      _rgn.GetOutLine(_outline);
      _rgn.SubRoi(_outline);
      
      if(fcut) w = static_cast<float>( 0.5 * ( 1.0 - cos( z * (fwidth - i - 1))) ); 
      else     w = static_cast<float>( 0.5 * ( 1.0 - cos( z * i )) ); 

      ft_mul_filter(ft,_outline,w);
      if(_rgn.IsEmptyRoi()) break;
    }
  }

  if(fcut)
    ft_mul_filter(ft,_rgn,0.0f);
  else {
    _outline.SetRectRoi(dmRect(dmPoint(0,0),ft.Dim2,ft.Dim1));
    _outline.SubRoi(rgn);
    ft_mul_filter(ft,_outline,0.0f);
  }
}
//------------------------------------------------------------------------

}; //namespace daim

