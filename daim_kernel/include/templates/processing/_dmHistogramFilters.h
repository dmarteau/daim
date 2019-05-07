#ifndef _dmHistogramFilters_h
#define _dmHistogramFilters_h

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
// File         : _dmHistogramFilters.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

namespace daim {

  namespace filter {

    //===========================================================
    // histogram based filter accumulator for 8 bits pixels values
    //===========================================================

    template<class R>
    struct histogram
    {
      typedef dm_uint8 argument_type;
      typedef dm_uint  value_type;
      typedef R        result_type;
      
      enum { length = 256, max_value = 255, min_value = 0 };

      // min/max accumulator
      struct minmax
      {
        dm_uint _min;
        dm_uint _max;

        void insert( argument_type x, value_type _data[] ) {
          ++_data[x];
          if(x > _max) _max = x; else
          if(x < _min) _min = x;
        }

        void remove( argument_type x, value_type _data[] ) {
          if(--_data[x] == 0) {
            if(x==_max) { while(_max>min_value && !_data[_max]) --_max; } else 
            if(x==_min) { while(_min<max_value && !_data[_min]) ++_min; }  
          }
        }

        void reset() {
          _min = max_value;
          _max = 0;
        }
      };
    }; // struct histogram 

    // filter min
    template<class R>
    struct histogram_min : public histogram<R>::minmax 
    {
      enum { length = histogram<R>::length };
      
      typedef typename histogram<R>::minmax minmax_type;
      
      typedef typename histogram<R>::argument_type argument_type;
      typedef typename histogram<R>::value_type    value_type   ;
      typedef typename histogram<R>::result_type   result_type  ;

       result_type result( value_type _data[] ) { 
         return static_cast<result_type>(minmax_type::_min); 
       }
    };

    // filter max
    template<class R>
    struct histogram_max : public histogram<R>::minmax 
    {
      typedef typename histogram<R>::minmax minmax_type;
      
      enum { length = histogram<R>::length };
      
      typedef typename histogram<R>::argument_type argument_type;
      typedef typename histogram<R>::value_type    value_type   ;
      typedef typename histogram<R>::result_type   result_type  ;

       result_type result( value_type _data[] ) { 
         return static_cast<result_type>(minmax_type::_max); 
       }
    };

    // filter middle
    template<class R>
    struct histogram_middle : public histogram<R>::minmax 
    {
      typedef typename histogram<R>::minmax minmax_type;

      enum { length = histogram<R>::length };

      typedef typename histogram<R>::argument_type argument_type;
      typedef typename histogram<R>::value_type    value_type   ;
      typedef typename histogram<R>::result_type   result_type  ;

      result_type result( value_type _data[] ) { 
        return static_cast<result_type>(
           daim::round((minmax_type::_max+minmax_type::_min)/2.0));
      }
    };

   //===========================================================
   // Functor for applying histogram filters in a fast way
   // accumulators must implements :
   //
   // insert( argument_typ&, result_type _data[]  ) for inserting a value
   // remove( argument_typ&, result_type _data[] ) for removing  a value
   // reset() is only called before each new processed line
   //===========================================================
   template<class K,class A> 
   struct histogramFunctor
   {  
      typedef K  kernel_type;
      typedef A accumulator_type;

      typedef typename K::value_type    value_type;
      typedef typename A::argument_type argument_type;
      typedef typename A::result_type   result_type;

      const K& _k;
      A        _a;

      typename A::value_type _data[A::length];

      int _stride ;
      int w,h;
   
      histogramFunctor( const K& _kernel, const A& _result ) 
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

        const argument_type *corner,*p_in  = *_in - _k.x(); 

        corner = p_in + x1; 

        std::fill(_data,_data+A::length,0);
        _a.reset();
        for(j=h;--j>=0;) {
          for(i=0;i<w;++i) _a.insert(corner[i],_data);
          corner = daim::const_offset_ptr(corner,_stride);
        }

        for(i=x1; i<x2; ++i) 
        {
          (*_out)[i] = _a.result(_data);
          corner =  p_in + i;
          for(j=h;--j>=0;) {
            _a.remove(corner[0],_data);
            _a.insert(corner[w],_data);
            corner = daim::const_offset_ptr(corner,_stride);
          }
        }
        (*_out)[x2] = _a.result(_data);
      } 

      A a() { return _a; }
   };
  
   template<class K,class A> 
   histogramFunctor<K,A> getHistogramFunctor( const K& _k, const A& _a ) {
      return histogramFunctor<K,A>(_k,_a);
   }

 } // namespace filter

} // namespace daim


#endif // _dmHistogramFilters_h

