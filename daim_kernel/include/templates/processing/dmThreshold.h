#ifndef dmThreshold_h
#define dmThreshold_h

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
// File         : dmThreshold.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

/* $ChangeLog  v2.5.1, 12 oct. 2006 10:52:57 , David
 *
 * Modifed algorithm to take account for thresholding
 * low values
 */ 

//--------------------------------------------------------------
// Hysteresis threshold
//--------------------------------------------------------------

namespace daim
{
template<class T1,class T2>
struct _hysteresis8_pass1
{
  typename pixel_traits<T1>::value_type s1,s2;

  _hysteresis8_pass1(
   typename pixel_traits<T1>::value_type& _s1,
   typename pixel_traits<T1>::value_type& _s2) : s1(_s1), s2(_s2) {}
  
  template<class In, class Out>
  void operator()( In in , Out out, long x1, long x2 )
  {
    typename pixel_traits<T2>::pointer_type lo1 = out[-1]; // upper line
    typename pixel_traits<T2>::pointer_type lo0 = out[0];
    
    typename pixel_traits<T2>::value_type m; 
    typename pixel_traits<T1>::value_type c;
    
    if(s1 >= s2)
    {
      m = pixel_traits<T2>::max();
      
      for(int x=x1; x<=x2;++x) {
        c = (*in)[x];
        if( c >= s1 ) lo0[x] = m;
        else if( c >= s2 ) {
          if( lo0[x-1] >= m ||
              lo1[x-1] >= m ||
              lo1[x]   >= m ||
              lo1[x+1] >= m ) lo0[x] = m; 
        }
      }
    } 
    else 
    {
      m = pixel_traits<T2>::min();

      for(int x=x1; x<=x2;++x) {
        c = (*in)[x];
        if( c <= s1 ) lo0[x] = m;
        else if( c <= s2 ) {
          if( lo0[x-1] <= m ||
              lo1[x-1] <= m ||
              lo1[x]   <= m ||
              lo1[x+1] <= m ) lo0[x] = m; 
        }
      }
    }
  }
};

template<class T1,class T2>
struct _hysteresis8_pass2
{
  typename  pixel_traits<T1>::value_type s1,s2;
  
  _hysteresis8_pass2(
    typename pixel_traits<T1>::value_type& _s1,
    typename pixel_traits<T1>::value_type& _s2) : s1(_s1), s2(_s2) {}
    
  template<class In, class Out>
  void operator()( In in , Out out, long x1, long x2 )
  {
    typename pixel_traits<T2>::pointer_type lo1 = out[1]; // lower line
    typename pixel_traits<T2>::pointer_type lo0 = out[0];
    
    typename pixel_traits<T2>::value_type m;

    if(s1 >= s2)
    {
      m = pixel_traits<T2>::max();

      for(int x=x1; x<=x2;++x) {
        if( (*in)[x] >= s2 ) {
          if( lo0[x+1] >= m ||
              lo1[x+1] >= m ||
              lo1[x]   >= m ||
              lo1[x-1] >= m ) lo0[x] = m; 
        }
      }
    }
    else 
    {
      m = pixel_traits<T2>::min();

      for(int x=x1; x<=x2;++x) {
        if( (*in)[x] <= s2 ) {
          if( lo0[x+1] <= m ||
              lo1[x+1] <= m ||
              lo1[x]   <= m ||
              lo1[x-1] <= m ) lo0[x] = m; 
        }
      }
    }
  }
};

template<class T1,class T2>
struct _hysteresis4_pass1
{
  typename pixel_traits<T1>::value_type s1,s2;

  _hysteresis4_pass1(
   typename pixel_traits<T1>::value_type& _s1,
   typename pixel_traits<T1>::value_type& _s2) : s1(_s1), s2(_s2) {}
  
  template<class In, class Out>
  void operator()( In in , Out out, long x1, long x2 )
  {
    typename pixel_traits<T2>::pointer_type lo1 = out[-1]; // upper line
    typename pixel_traits<T2>::pointer_type lo0 = out[0];
    
    typename pixel_traits<T2>::value_type m; 
    typename pixel_traits<T1>::value_type c;
    
    if(s1 >= s2)
    {
      m = pixel_traits<T2>::max();
      
      for(int x=x1; x<=x2;++x) {
        c = (*in)[x];
        if( c >= s1 ) lo0[x] = m;
        else if( c >= s2 ) {
          if( lo0[x-1] >= m ||
              lo1[x]   >= m ) lo0[x] = m; 
        }
      }
    } 
    else 
    {
      m = pixel_traits<T2>::min();

      for(int x=x1; x<=x2;++x) {
        c = (*in)[x];
        if( c <= s1 ) lo0[x] = m;
        else if( c <= s2 ) {
          if( lo0[x-1] <= m ||
              lo1[x]   <= m ) lo0[x] = m; 
        }
      }
    }
  }
};

template<class T1,class T2>
struct _hysteresis4_pass2
{
  typename  pixel_traits<T1>::value_type s1,s2;
  
  _hysteresis4_pass2(
    typename pixel_traits<T1>::value_type& _s1,
    typename pixel_traits<T1>::value_type& _s2) : s1(_s1), s2(_s2) {}
    
  template<class In, class Out>
  void operator()( In in , Out out, long x1, long x2 )
  {
    typename pixel_traits<T2>::pointer_type lo1 = out[1]; // lower line
    typename pixel_traits<T2>::pointer_type lo0 = out[0];
    
    typename pixel_traits<T2>::value_type m;

    if(s1 >= s2)
    {
      m = pixel_traits<T2>::max();

      for(int x=x1; x<=x2;++x) {
        if( (*in)[x] >= s2 ) {
          if( lo0[x+1] >= m ||
              lo1[x]   >= m ) lo0[x] = m; 
        }
      }
    }
    else 
    {
      m = pixel_traits<T2>::min();

      for(int x=x1; x<=x2;++x) {
        if( (*in)[x] <= s2 ) {
          if( lo0[x+1] <= m ||
              lo1[x]   <= m ) lo0[x] = m; 
        }
      }
    }
  }
};


template<class T1,class T2>
void hysteresis_threshold( const dmRegion& _roi, const dmPoint p, 
                           image<T1>& _src, image<T2>& _dest, 
                           typename pixel_traits<T1>::value_type s1, 
                           typename pixel_traits<T1>::value_type s2, 
                           dm_uint connect )
{
   if(connect <= connect4)
   {
    _TwoPassesRoiOperation( _roi,p,_src,_dest,
                            _hysteresis4_pass1<T1,T2>(s1,s2),
                            _hysteresis4_pass2<T1,T2>(s1,s2));
   } else {
    _TwoPassesRoiOperation( _roi,p,_src,_dest,
                            _hysteresis8_pass1<T1,T2>(s1,s2),
                            _hysteresis8_pass2<T1,T2>(s1,s2));   
   }
}

}; // namespace daim

#endif // dmThreshold_h
