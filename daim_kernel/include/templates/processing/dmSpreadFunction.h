#ifndef _dmSpreadFunction_h
#define _dmSpreadFunction_h

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
// File         : dmSpreadFunction.h
// Date         : 11/2005
// Author       : David Marteau
//--------------------------------------------------------

#include <vector>
#include <math.h>

namespace daim {
  
  //-----------------------------------------------------------
  // Compute the edge spread function from given region
  // by projecting pixels values on the perpendicular 
  // to the edge given by x1,y1,x2,y2  
  //-----------------------------------------------------------
  
  template<class T>
  struct _edge_spread_function
  { 
    const image<T>&       _im;
    std::vector<dm_real>& _xx;
    std::vector<dm_real>& _yy;

    dm_real _x1,_y1,_x2,_y2;
    dm_real _dx,_dy,_dl;
    dm_real _xscale;
    dm_real _yscale;

    _edge_spread_function( const image<T>&  img,
                           std::vector<dm_real>& xx,
                           std::vector<dm_real>& yy,
                           dm_real x1, 
                           dm_real y1,
                           dm_real x2, 
                           dm_real y2,
                           dm_real xscale = 1.0,
                           dm_real yscale = 1.0
                         )
    : _im(img)
     ,_xx(xx)
     ,_yy(yy)
     ,_x1(x1)
     ,_y1(y1)
     ,_x2(x2)
     ,_y2(y2)
     ,_xscale(xscale)
     ,_yscale(yscale)
    {
       _dx = _x2 - _x1;
       _dy = _y2 - _y1;
       _dl = sqrt(_dx*_dx+_dy*_dy);  
    }

    void operator()( int iy, int ixs, int ixe ) 
    { 
      dm_real x;
      dm_real y = iy * _yscale;
      
      typename image<T>::const_pointer_type line = _im[iy];
      for(int ix=ixs;ix<=ixe;++ix) 
      {
        x = ix * _xscale;
        _xx.push_back( ((x-_x1)*_dy - (y-_y1)*_dx)/_dl );
        _yy.push_back( pixel_traits<T>::scalar_value(line[ix]) );
      }     
    }
  };

  template<class T>
  int edge_spread_function(const dmRegion& rgn,
                           const image<T>& img,
                           std::vector<dm_real>& xx,
                           std::vector<dm_real>& yy,
                           dm_real x1, 
                           dm_real y1,
                           dm_real x2, 
                           dm_real y2,
                           dm_real xscale,
                           dm_real yscale )
                       
  {
    _edge_spread_function<T> esf(img,xx,yy,x1,y1,x2,y2,xscale,yscale);
    dmDigitalRegion(rgn,esf);
    return xx.size();
  }
  
} // namespace daim


#endif // _dmSpreadFunction_h

