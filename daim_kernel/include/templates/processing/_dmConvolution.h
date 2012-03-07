#ifndef _dmConvolution_h
#define _dmConvolution_h

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
// File         : _dmConvolution.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#include "_dmFilterTemplates.h"
#include "_dmConvTemplates.h"

namespace daim {

//-----------------------------------------------------------------------
// The signed/unsigned caracteristic applies to the return value
// of the accumulator result
//-----------------------------------------------------------------------
namespace conv {

  // integer accumulators

  template<class K,class T1,class T2>
  inline accumulator<T1,T2,typename K::value_type>
  bind_accumulator(const K& k,unsigned_false,true_assertion,
                   pixel_traits<T1>,
                   pixel_traits<T2>)
  {
     return accumulator<T1,T2,typename K::value_type>(k.norm());
  }

  template<class K,class T1,class T2>
  inline denorm_accumulator<T1,T2,typename K::value_type>
  bind_accumulator(const K& k,unsigned_false,false_assertion,
                   pixel_traits<T1>,
                   pixel_traits<T2>)
  {
     return denorm_accumulator<T1,T2,typename K::value_type>();
  }

  template<class K,class T1,class T2>
  inline unsigned_accumulator<T1,T2,typename K::value_type>
  bind_accumulator(const K& k,unsigned_true,true_assertion,
                   pixel_traits<T1>,
                   pixel_traits<T2>)
  {
     return unsigned_accumulator<T1,T2,typename K::value_type>(k.norm());
  }

  template<class K,class T1,class T2>
  inline unsigned_denorm_accumulator<T1,T2,typename K::value_type>
  bind_accumulator(const K& k,unsigned_true,false_assertion,
                   pixel_traits<T1>,
                   pixel_traits<T2>)
  {
     return unsigned_denorm_accumulator<T1,T2,typename K::value_type>();
  }

}; // namespace conv

//----------------------------------------------------------------------

// Generic convolution operation
namespace conv {

template<class ROI,class In,class Out,class K,class A>
inline A _convolve1( const ROI& roi,In in, Out out,
                     const K& k, const A& a)
{
  return core::RoiOperation(roi,in,out,conv::getFunctor(k,a)).a();
}

// Apply convolution operation on integer pixel type

template<class kernel_type,class ROI, class In,class Out,class T1,class T2>
void _normalized_convolution(
     const kernel_type& k,bool _unsigned,
     const ROI& roi,
     In in, Out out,
     type_of<T1>, type_of<T2>, integer_true )
{
  typedef typename image<T1>::traits_type traits_type1;
  typedef typename image<T2>::traits_type traits_type2;

  if(k.norm()>0) {
   if(_unsigned)
    _convolve1(roi,in,out,k,
           conv::bind_accumulator(k,unsigned_true(),
                                  true_assertion(),
                                  traits_type1(),
                                  traits_type2()));
    else
    _convolve1(roi,in,out,k,
           conv::bind_accumulator(k,unsigned_false(),
                                  true_assertion(),
                                  traits_type1(),
                                  traits_type2()));

  } else {
    _convolve1(roi,in,out,k,
           conv::bind_accumulator(k,unsigned_true(),
                                  false_assertion(),
                                  traits_type1(),
                                  traits_type2()));
  }

}

// Apply convolution operation on non integer pixel type

template<class kernel_type,class ROI, class In,class Out,class T1,class T2>
void _normalized_convolution( const kernel_type& k, bool _unsigned,
            const ROI& roi,
            In in, Out out,
            type_of<T1>, type_of<T2>, integer_false )
{
  float n = static_cast<float>(k.norm());
  if(_unsigned)
    _convolve1(roi,in,out,k,conv::unsigned_accumulator<T1,T2,float>(n==0?1.0f:n));
  else
    _convolve1(roi,in,out,k,conv::accumulator<T1,T2,float>(n==0?1.0f:n));
}


template<class kernel_type,class ROI, class In,class Out>
void _convolution( const kernel_type& k,bool _unsigned,
                   const ROI& roi, In in,Out out
                 )
{
  _normalized_convolution(k,_unsigned,roi,in,out,
                          get_type_of_ptr(*in),
                          get_type_of_ptr(*out),
                          is_integer(**out));
}

}; // namespace conv

//-----------------------------------------------------------------------

}; // namespace daim

#endif // _dmConvolution_h
