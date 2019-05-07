#ifndef _dmConvTemplates_h
#define _dmConvTemplates_h

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
// File         : _dmConvTemplates.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

namespace daim {
//----------------------------------------------------------------------
// Convolution operator for rectangular filters
//----------------------------------------------------------------------

  namespace conv {

    //  accumulator

    template<class T,class R,class U>
    struct accumulator
    {
      public:

      typedef typename pixel_traits<T>::value_type argument_type;
      typedef typename pixel_traits<R>::value_type result_type;
      typedef U value_type;

      U _result;
      U _norm;

      accumulator(U n = U(1)) : _norm(n) {}

      void reset() { _result = 0; }

      void accumulate( argument_type _data, U _k) {
         _result += _data * _k;
      }

      result_type result() {
         return  static_cast<result_type>(_result/_norm);
      }

      void setNorm( U n ) { _norm = n; }
    };

    // denormalized accumulator

    template<class T,class R,class U>
    struct denorm_accumulator : public accumulator<T,R,U>
    {
      typedef accumulator<T,R,U> accumulator_type;
      typedef typename accumulator_type::result_type result_type;

      U _max_value;
      U _min_value;

      denorm_accumulator() :
          _max_value(pixel_traits<R>::max()),
          _min_value(pixel_traits<R>::min()) {}

      result_type result() {
        return static_cast<result_type>(
                accumulator_type::_result<_min_value?_min_value
              :(accumulator_type::_result>_max_value?_max_value
              : accumulator_type::_result));
      }
    };

    // unsigned accumulator (absolute value)

    template<class T,class R,class U>
    struct unsigned_accumulator : public accumulator<T,R,U>
    {
      typedef accumulator<T,R,U> accumulator_type;
      typedef typename accumulator_type::result_type result_type;

      unsigned_accumulator(U n = U(1)) : accumulator<T,R,U>(n) {}

      result_type result() {
        return static_cast<result_type>(
           daim::abs(accumulator_type::_result/accumulator_type::_norm));
      }
    };

    // unsigned denormalized accumulator (absolute value)

    template<class T,class R,class U>
    struct unsigned_denorm_accumulator : public accumulator<T,R,U>
    {
      typedef accumulator<T,R,U> accumulator_type;
      typedef typename accumulator_type::result_type result_type;

      U _max_value;
      unsigned_denorm_accumulator() :
          _max_value(pixel_traits<R>::max()) {}

      result_type result() {
        return static_cast<result_type>(
               (accumulator_type::_result = daim::abs(accumulator_type::_result)) > _max_value ?
                _max_value : accumulator_type::_result);
      }
    };


   // Functor for applying convolution

   template<class K,class A>
   struct functor
   {
     typedef K  kernel_type;
     typedef A accumulator_type;

     typedef typename K::value_type     value_type;
     typedef typename A::argument_type  argument_type;
     typedef typename A::result_type    result_type;
     typedef typename A::value_type     accumulate_type;

     const K& _k;
     A        _a;

     int _stride ;
     int w,h;

     functor( const K& _kernel, const A& _result )
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
       const value_type    *_data;

       // COMPILER: Seems that optimization of this piece of code
       // has to be done on a per machine/compiler basis.

       for(long x=x1; x<=x2; ++x )
       {
         _a.reset();
         _data  = _k.data();
         _corner = *_in + (x-_k.x());

         for(j=h;--j>=0;) {
           for(i=0;i<w;++i)
             _a.accumulate(_corner[i],static_cast<accumulate_type>(*_data++));

           _corner = daim::const_offset_ptr(_corner,_stride);
         }
         (*_out)[x] = _a.result();
       }
     }

     A a() { return _a; }
  };

  template<class K,class A>
  functor<K,A> getFunctor( const K& _k, const A& _a ) {
    return functor<K,A>(_k,_a);
  }

 } // namespace conv

} // namespace daim


#endif //_dmConvTemplates_h
