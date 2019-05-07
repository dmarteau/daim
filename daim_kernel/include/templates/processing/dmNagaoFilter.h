#ifndef dmNagaoFilter_h
#define dmNagaoFilter_h

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
// File         : dmNagaoFilter.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

//-----------------------------------------------------------------------
// Nagao filter implementation
//-----------------------------------------------------------------------

namespace daim_nagao_impl {

typedef dm_point __Nagao_A[8];
typedef int      __Nagao_K[9];

extern __Nagao_A _A;
extern __Nagao_K _K[];

enum {
  NAGAO_SIZE         = 7,
  NAGAO_COUNT        = 8,
  NAGAO_NATURAL_SIZE = 5,
  NAGAO_W            = 3,
};

//-----------------------------------------------------------------------
template<class T1,class T2> 
struct _Filter
{
  _Filter() {}

  template<class In,class Out>
  void operator()( In _in,Out _out, long x1, long x2 )
  {
  	int i,j,_stride = daim::line_stride(_in);
 
 
    int* data;
    typename daim::pixel_traits<T1>::const_pointer_type corner;

    float m=0,m1,m2,v;
    float s,smin,fsize = NAGAO_SIZE;
  
    for(int ix=x1; ix<=x2; ++ix) {
      smin = std::numeric_limits<float>::max();
      for( int k=0;k<NAGAO_COUNT;++k)
      {
         m1 = m2 = 0;
         data   = _K[k];
         corner = *(_in - _A[k].y) - _A[k].x  + ix;

         for(j=0;j<NAGAO_W;++j) {    // add value if non zero
           for(i=0;i<NAGAO_W;++i) { 
              if(*data++) {
                 v = static_cast<float>(corner[i]);
                 m1 += v; m2 +=  v*v;
              }  
           }
	       corner = daim::const_offset_ptr(corner,_stride);
         }
         s = m2 - (m1*m1)/fsize;
         if(s<smin) { smin = s; m = m1; }
      }
      (*_out)[ix] = daim::pixel_traits<T2>::clamp( m/fsize);
	}
  }   
};


} // daim_nagao_impl
//-----------------------------------------------------------------------
namespace daim {

// binary implemention
template<class In,class Out>
void nagao_filter( const dmRegion& roi, const dmPoint& p,
                   const image<In>& in, image<Out>& out ) 
{ 
  RoiOperation(daim_nagao_impl::_Filter<In,Out>(),in,out,roi,p); 
}

}; // namespace daim

//-----------------------------------------------------------------------

#endif // dmNagaoFilter_h
