#ifndef _dmMorphoTemplates_h
#define _dmMorphoTemplates_h

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
// File         : _dmMorphoTemplates.h
// Date         : 11/2004
// Author       : David Marteau
//--------------------------------------------------------

namespace daim {


 // Accumulator for mathematical morphology operation

 namespace morphology {

    //  accumulator

    template<class T,class R>
    struct accumulator
    {
      typedef typename pixel_traits<T>::value_type argument_type;
      typedef typename pixel_traits<R>::value_type result_type;

      argument_type A1,A2;
     
      void reset() { 
        static const argument_type m1 = basic_traits<argument_type>::min();
        static const argument_type m2 = basic_traits<argument_type>::max();
        A1 = m1;
        A2 = m2;
      }

      void accumulate(const argument_type& _value, const int& _k) {
        switch( _k ) { 
          case  1: if(A2>=_value) A2 = _value; break; // => erosion 
          case -1: if(A1<=_value) A1 = _value; break; // => dilatation
        }
      }
    };

    // Erosion

    template<class T,class R>
    struct erosion : public accumulator<T,R> 
    {
       typedef typename accumulator<T,R>::argument_type  argument_type;
       typedef typename accumulator<T,R>::result_type    result_type;
       result_type result(const argument_type&) 
       {
          return accumulator<T,R>::A2;
       }
    };

    // Dilation

    template<class T,class R>
    struct dilation : public accumulator<T,R> 
    {
       typedef typename accumulator<T,R>::argument_type  argument_type;
       typedef typename accumulator<T,R>::result_type    result_type; 
       result_type result(const argument_type&) 
       {
          return accumulator<T,R>::A1;
       }
    };

    // Hit or Miss

    template<class T,class R>
    struct hitormiss : public accumulator<T,R> 
    { 
       typedef typename accumulator<T,R>::argument_type  argument_type;
       typedef typename accumulator<T,R>::result_type    result_type; 
       result_type result(const argument_type& x)
       {
          static const argument_type _empty  = basic_traits<argument_type>::zero();
          static const argument_type _object = basic_traits<argument_type>::object();

          return (accumulator<T,R>::A1 < accumulator<T,R>::A2 ? _object: _empty);
       }
    };

    // Thinning

    template<class T,class R>
    struct thinning : public accumulator<T,R> 
    {
       typedef typename accumulator<T,R>::argument_type  argument_type;
       typedef typename accumulator<T,R>::result_type    result_type; 
       result_type result(const argument_type& x)
       {
         return (accumulator<T,R>::A1 < x && x <= accumulator<T,R>::A2 ? accumulator<T,R>::A1 : x);
       }
    };

    // Thickening

    template<class T,class R>
    struct thickening : public accumulator<T,R>
    {
       typedef typename accumulator<T,R>::argument_type  argument_type;
       typedef typename accumulator<T,R>::result_type    result_type; 
       result_type result(const argument_type& x)
       {
         return  (accumulator<T,R>::A1 <= x && x < accumulator<T,R>::A2 ? accumulator<T,R>::A2 : x);
       }
    };


   // Functor for applying morphological operators
   // There is no difference with the conv::functor, except
   // that input pixel is passed to accumulator::result( )

   template<class K,class A> 
   struct functor
   {
     typedef K  kernel_type;
     typedef A accumulator_type;

     typedef typename K::value_type     value_type;
     typedef typename A::argument_type  argument_type;
     typedef typename A::result_type    result_type;

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
       In  in = _in - _k.y();
  
       const argument_type *_corner;
       const value_type    *_data;
  
       // COMPILER: Seems that optimization of this piece of code
       // has to be done on a per machine/compiler basis.
  
       for(long x=x1; x<=x2; ++x ) 
       {
         _a.reset();
         _data  = _k.data();
         _corner = *in + (x-_k.x());

         for(j=h;--j>=0;) 
         {
           for(i=0;i<w;++i)
             _a.accumulate(_corner[i],*_data++);

           _corner = daim::const_offset_ptr(_corner,_stride);
         }
         (*_out)[x] = _a.result((*_in)[x]);
	   }  
     }  

     A a() { return _a; }
  };

  template<class K,class A> 
  functor<K,A> getFunctor( const K& _k, const A& _a ) {
    return functor<K,A>(_k,_a);
  }

 } // namespace morphology


} // namespace daim

#endif // _dmMorphoTemplates_h
