#ifndef dmImageStats_h
#define dmImageStats_h

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
// File         : dmImageStats.h
// Date         : 7/2005
// Author       : David Marteau
//--------------------------------------------------------

namespace daim {

struct _imagestat
{
  dm_uint order;
  dm_real central;
  dm_real value;
  
  _imagestat( dm_uint _order, dm_real _central ) 
  : order(_order),central(_central),value(0) {}
  
  template<class In>
  void operator()( In _in, long x1, long x2 )
  { 
    dm_real v,tmp,c=central;
    for(long x=x1;x<=x2;++x) 
    {
      v = (*_in)[x] - c;
      tmp = 1.0;
      for(dm_uint i=0;i<order;++i)
        tmp = tmp * v;
      value += tmp;
    }
  }
  
  dm_real Value() { return value; }
};

struct _imagestats
{
  dm_real* q;
  dm_uint  order;
  dm_real  central;
  
  _imagestats( dm_real* _q, dm_uint _order, dm_real _central ) 
  : q(_q), order(_order),central(_central) {}
  
  template<class In>
  void operator()( In _in, long x1, long x2 )
  { 
    dm_real v,tmp,c=central;
    for(long x=x1;x<=x2;++x) 
    {
      v = (*_in)[x] - c;
      tmp = 1.0;
      for(dm_uint i=0;i<order;++i)
        q[i] += (tmp = tmp * v);
    }
  }
};


template<class ROI,class T> 
dm_real imagestat( const ROI& rgn,const image<T>& img, 
                    dm_uint order, dm_real central) 
{ 
  _imagestat  _Stat(order,central);
  return daim::apply(rgn,img,_Stat).Value(); 
}

template<class ROI,class T> 
void imagestats( const ROI& rgn,const image<T>& img, 
                 dm_real* q,dm_uint order, dm_real central) 
{   
  _imagestats  _Stats(q,order,central);
  daim::apply(rgn,img,_Stats); 
}

} // namespace daim

#endif // dmImageStats_h
