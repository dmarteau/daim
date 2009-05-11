
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

#include "templates/processing/_dmHistogram.h"

#include <math.h>

namespace daim {

// cumulate
void cumulate_histogram( histogram_type& _H, const histogram_type& _data )
{
  _H[0] = _data[0];
  for(int p=1;p<histogram_size;++p) _H[p] = _H[p-1]+_data[p];
}

// equalize
void equalize_histogram( dmColorMapArray _cmap, const histogram_type& _data ) 
{
  histogram_type _H;
  cumulate_histogram(_H,_data);

  dm_double _hmax = histogram_max;
  dm_double _size = _H[histogram_max];
  for(int p=0;p<histogram_size;++p) {    
    _cmap[p]= (dmColorIndex)daim::round( (_hmax*_H[p])/_size );
  }  
}

// specify
void specify_histogram( dmColorMapArray _cmap, const histogram_type& _data, 
                        dmColorMapArray _input ) 
{
  dmColorIndexTable _eq;
  equalize_histogram(_eq,_data);
  
  // build the look up table
  for(int j=0,i=0;i<histogram_size;++i) {
    if( _eq[i] <= _input[j] ) _cmap[i] = j;
    else {
      while( _eq[i] > _input[j] ) ++j;
      if( (_input[j] - _eq[i]) > (_eq[i] - _input[j-1]) )
         _cmap[i] = j--;
      else
         _cmap[i] = j;
    }
  }
}


//extend
void extend_histogram_map( dmColorMapArray _cmap, 
                           const daim::gap<dm_uint8>& _range_from,
                           const daim::gap<dm_uint8>& _range_to
                         )
{
  int _maxh = _range_from.max(), _maxr = _range_to.max();
  int _minh = _range_from.min(), _minr = _range_to.min();
  dm_double _d = _maxh-_minh;
  for(int pp,p=0;p<histogram_size;++p) 
  {    
    pp = _minr + static_cast<int>( (_maxr-_minr) * ((p - _minh)/_d) + 0.5);
    if(pp<0) 
      pp = 0; 
    else 
     if(pp>histogram_max) 
       pp = histogram_max;

    _cmap[p] = pp;
  } 
}

//compute optimal threshold value 
//when assuming bimodal histogram distribution

histogram_value 
bimodal_optimal_thrindex( const histogram_type& _data, dm_uint8 _thr )
{
  histogram_type _H;
  cumulate_histogram(_H,_data);

  histogram_value bgcount,fgcount;
  histogram_value bgsum,fgsum;
  histogram_value i,T=_thr,Tp=0;
 
  dm_double mbg,mfg;

  do {
    Tp = T;
    bgsum = fgsum = 0;
    bgcount  = _H[T];
    fgcount  = _H[histogram_max] - bgcount;

    for(i=0;i<=T;++i)               bgsum += i*_data[i];
    for(i=T+1;i<histogram_size;++i) fgsum += i*_data[i];

    mbg = bgsum; mbg /= bgcount;
    mfg = fgsum; mfg /= fgcount;

    T = (int)( (mbg + mfg)/2.0f + 0.5 );
    
  } while(T!=Tp);
  return T;
}

//create a density mapping for 
//linear contrast/luminosity transformation

#define IMAX dmLUT8_MAX_COLOR_INDEX
#define __F( u ) \
 for(int i=0;i<=IMAX;++i) { \
   X = u;                   \
   if(X<=0)    { gp.lower = i; cm[i] = 0;    } else \
   if(X>=IMAX) { gp.upper = i; cm[i] = IMAX; } else \
    cm[i] = static_cast<dmColorIndex>(X + 0.5);     \
 }

gap<dm_uint16> enhance_contrast( dmColorMapArray cm, 
                                 dm_uint16 bright, dm_uint16 contr )
{
  gap<dm_uint16> gp(0,IMAX);
  dm_real X,B = bright,C = contr;
 
  gp.upper = gp.lower = 127;

  if( C >= 100.0 ) {  // special case 
    B = IMAX - B; 
    for(int i=0;i<=IMAX;++i) { \
      if(i< B) { gp.lower = i; cm[i] = 0;    } else 
      if(i> B) { gp.upper = i; cm[i] = IMAX; } else
         cm[i] = bright;
    }
    return gp;
  } 

  C /= (100.0-C);

  if( C >= 1.0 ) {
    B = IMAX - B; 
    X = IMAX/(2.0*C);
    if( B <= X )              __F( C*(i-2.0*B)+IMAX )
    else if( B >= IMAX - X )  __F( C*(i+IMAX-2.0*B) )
    else                      __F( C*(i-B)+ IMAX/2.0 )
  } 
  else 
  {
    X = IMAX*C/2.0;
    if( B >= IMAX - X )  __F( C*i-IMAX+2.0*B ) 
    else if( B <= X )    __F( C*(i-IMAX)+2.0*B )
    else                 __F( C*(i-IMAX/2.0)+B )
  }
  return gp;
}

void gamma_correction(  dmColorMapArray cm, dm_real gamma ) 
{
  dm_real x,dmax = IMAX;
  for(dm_int i=0;i<IMAX;++i)
  { 
    x     = static_cast<dm_real>(i)/dmax ;
    cm[i] = static_cast<dm_uint8>(255.0 * daim::round( pow(x,gamma) ));
  }
}

}; // namespace daim
