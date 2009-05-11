#ifndef _dmMeanFilter_h
#define _dmMeanFilter_h

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
// File         : _dmHistogramsFilters.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

namespace daim {

  namespace filter 
  {
    template<class K,class A> 
    struct meanFunctor
    {
     typedef K  kernel_type;
     typedef A accumulator_type;

     typedef typename K::value_type    value_type;
     typedef typename A::argument_type argument_type;
     typedef typename A::result_type   result_type;

     typedef pixel_traits<result_type>           traits_type;
     typedef typename traits_type::integer_type  integer_type;

     const K& _k;
     A        _a;

     int _stride ;
     int w,h;
   
     meanFunctor( const K& _kernel, const A& _result ) 
     :_k(_kernel)
     ,_a(_result)
     {
       w = _k.width();
       h = _k.height();
     }
  
     template<class In,class Out>
     void operator()( In _in, Out _out, long x1, long x2 )
     {
       _stride  = daim::line_stride(_in);
  
       int i,j;
       _in = _in - _k.y();
  
       const argument_type *_corner;
  
       // COMPILER: Seems that optimization of this piece of code
       // has to be done on a per machine/compiler basis.

       // Compute the first row
       typename A::value_type _result = 0;
       typename A::value_type _norm   = static_cast<typename A::value_type>(_k.size());

       _corner = *_in + (x1 - _k.x()); 

       for(j=h;--j>=0;) {
          for(i=0;i<w;++i)
            _result += _corner[i];

          _corner = daim::const_offset_ptr(_corner,_stride);
       }

       for(long x=x1;x<x2;++x) 
       {
         (*_out)[x] =  _round_value(static_cast<dm_real>(_result)/_norm,
                         traits_type(),integer_type() );

         _corner = *_in + (x-_k.x());

         for(j=h;--j>=0;) {
           _result -= _corner[0];
           _result += _corner[w];
           _corner = daim::const_offset_ptr(_corner,_stride);
         }
	   } 
       // Add last value 
       (*_out)[x2] = _round_value(static_cast<dm_real>(_result)/_norm,
                         traits_type(),integer_type() );

     }  

     A a() { return _a; }
  };

  template<class K,class A> 
  meanFunctor<K,A> getMeanFunctor( const K& _k, const A& _a ) {
    return meanFunctor<K,A>(_k,_a);
  }
    


  } // namespace filter
} // namespace daim

#endif // _dmMeanFilter_h
