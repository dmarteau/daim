#ifndef dmConvolution_h
#define dmConvolution_h

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
// File         : dmConvolution.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

// Implementation of convolution operator

#include "_dmConvolution.h"

namespace daim {

//---------------------------------------------------------------------------
// in      : image to be processed (will be left inchanged) 
// tmp     : temporary buffer
// _rgn    : input region
// _src    : output location
// _family : kernel family
// iter    : number of iterations
//
// Note : the output image should be large enough to handle
// extra storage due to the kernel size, to find out how much
// of extra space is needed, use the GetRect() member function 
// from dmKernelFamily
//
//---------------------------------------------------------------------------

#include <vector>
//---------------------------------------------------------------------------
// Multipass convolution operators
// Result is output back in \a in
// \param in the input/output image data
// \param rgn the region in \a in
//---------------------------------------------------------------------------
template<class K,class T>                            
void convolution( const dmRegion& rgn,                
                  image<T>& in, image<T>& tmp,
                  const std::vector<K>& _family, 
                  bool _unsigned, 
                  int iter        
                ) 
{ 
   dmDEBUG_ASSERT( equal_size(in,tmp) );

   typename image<T>::line_type lin   = in.begin ( rgn.Rectangle() );
   typename image<T>::line_type lout  = tmp.begin( rgn.Rectangle() );
   typename image<T>::line_type lsave = lin; 

   for(int i=0;i<iter;++i) { 
     for(size_t k=0;k<_family.size();++k) {
       conv::_convolution(_family[k],_unsigned,rgn,lin,lout);
       std::swap(lin,lout);         
     }
   } 

   // Because of the swap we need 
   // to restore result in <in>                   
   if(lin!=lsave) 
     _copy(rgn,lin,lsave);
}
//---------------------------------------------------------------------------
// Convolve with a set of kernels and perform 
// a binary operation between the source and the result
// of each convolution
//---------------------------------------------------------------------------
template<class _BinOp,class T,class K>                                              
void convolution( _BinOp op,              
                  image<T>& in, 
                  image<T>& out, 
                  image<T>& tmp, 
                  const dmRegion& rgn,  
                  const dmPoint& _src,                                
                  const std::vector<K>& _family,
                  bool _unsigned,                             
                  int  _iter = 1) 
{ 
   dmDEBUG_ASSERT( equal_size(in,tmp) );

   typename image<T>::line_type lin   = in.begin ( rgn.Rectangle() );
   typename image<T>::line_type lout  = tmp.begin( rgn.Rectangle() );
   
   for(int i=0;i<_iter;++i)
   {
     conv::_convolution(_family[0],_unsigned,rgn,lin,lout);
     copy(rgn,_src,tmp,out);
     for(size_t k=1;k<_family.size();++k) {
       conv::_convolution(_family[k],_unsigned,rgn,lin,lout);
       combine(rgn,_src,tmp,out,op); 
     }
     if(_iter > 1) 
       swap(rgn,_src,in,out); // swap back into input
   }
} 

//---------------------------------------------------------------------------
// Implementation using one kernel element for checked-ROI binary operator
//---------------------------------------------------------------------------

template<class T1,class T2,class K,class A>
A convolve1( const dmRegion& roi,const dmPoint& p,
             const image<T1>& in,image<T2>& out,
             const K& k, const A& a)
{ 
  return RoiOperation(conv::getFunctor(k,a),in,out,roi,p).a(); 
}


template<class T1,class T2,class K>
void normalized_convolution( 
        const K& k,bool _unsigned, 
        const dmRegion& roi,const dmPoint& p,
        const image<T1>& in,image<T2>& out,
        integer_true ) 
{
  typedef typename image<T1>::traits_type traits_type1;  
  typedef typename image<T2>::traits_type traits_type2;  
  
  if(k.norm()>0) {
    if(_unsigned) 
     convolve1(roi,p,in,out,k,
               conv::bind_accumulator(k,unsigned_true(),
                                    true_assertion(),
                                    traits_type1(),
                                    traits_type2()));
    else
      convolve1(roi,p,in,out,k,
             conv::bind_accumulator(k,unsigned_false(),
                                    true_assertion(),
                                    traits_type1(),
                                    traits_type2()));
 
  } else {
     convolve1(roi,p,in,out,k,
           conv::bind_accumulator(k,unsigned_true(),
                                  false_assertion(),
                                  traits_type1(),
                                  traits_type2()));
  }
}


template<class K,class T1,class T2>
void normalized_convolution( 
        const K& k, bool _unsigned,
        const dmRegion& roi,const dmPoint& p,
        const image<T1>& in,image<T2>& out,
        integer_false ) 
{
  float n = static_cast<float>(k.norm());
  if(_unsigned)
    convolve1(roi,p,in,out,k,conv::unsigned_accumulator<T1,T2,float>(n==0?1.0f:n));
  else
    convolve1(roi,p,in,out,k,conv::accumulator<T1,T2,float>(n==0?1.0f:n));
}


template<class K,class T1,class T2>
inline void convolve( const K& k,bool _unsigned,
                      const dmRegion& roi,const dmPoint& p,
                      const image<T1>& in, image<T2>& out         
                    )
{
  typedef typename image<T2>::traits_type    traits_type;
  typedef typename traits_type::integer_type integer_type;
  normalized_convolution(k,_unsigned,roi,p,in,out,integer_type());
}


//---------------------------------------------------------------------------

} // namespace daim 

#endif // dmConvolution_h
