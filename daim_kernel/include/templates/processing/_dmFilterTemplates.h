#ifndef _dmFilterTemplates_h
#define _dmFilterTemplates_h

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

//----------------------------------------------------------------------------------------
/*! \file _dmFilterTemplates.h
 *  \date 7/2005
 *  \author David Marteau 
 *
 * Definition of basic accumulators for filters operation  
 */ 
//----------------------------------------------------------------------------------------
   
#include <vector>

namespace daim {

  namespace filter {

   template<class T>
   class kernel
   {
     public:
      typedef T value_type;  

      const T* _data;
      size_t   _width,_height;
      dm_int   _x,_y;
      T        _norm;
    
      const T* data() const { return _data; }

      T  norm() const { return _norm; }
    
      size_t width()  const { return _width;  }
      size_t height() const { return _height; }
      size_t size()   const { return _width * _height; } 

      dm_int x() const { return _x; }
      dm_int y() const { return _y; }

      kernel() 
        :_data(NULL)
        ,_width(0)
        ,_height(0)
        ,_x(0)
        ,_y(0)
        ,_norm(value_type(1))
        {}

      void fromDescription    ( const dmKernelDescription& );
      void fromMaskDescription( const dmMaskDescription&   ); 
   }; 


    //  accumulator

    template<class T,class R,class U>
    struct accumulator_base
    {
      typedef typename pixel_traits<T>::value_type argument_type;
      typedef typename pixel_traits<R>::value_type result_type;
      typedef U value_type;
    };


    //===========================================================
    // Generic vector accumulator, push values in array for 
    // processing in the result() method
    //===========================================================

    template<class T,class R,class U>
    struct vector_accumulator : public accumulator_base<T,R,U>
    {
      typedef typename accumulator_base<T,R,U>::argument_type argument_type;
      typedef typename accumulator_base<T,R,U>::value_type    value_type;

      std::vector<value_type>  _data;

      vector_accumulator( dm_uint _size ) { _data.reserve(_size); }
      vector_accumulator( const vector_accumulator& _a ) 
      : _data(_a._data) {}

      void reset() { _data.clear(); }
       
      void accumulate( argument_type x ) { _data.push_back(x); }
      void accumulate( argument_type x,  value_type _k) { 
        if(_k) _data.push_back(x);
      }

    };


   //===========================================================
   // Functor for applying generic filter 
   //===========================================================

   template<class K,class A> 
   struct generic_functor
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
   
     generic_functor( const K& _kernel, const A& _result ) 
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

       for(long x=x1; x<=x2; ++x ) 
       {
         _a.reset();
         _corner = *_in + (x-_k.x());

         for(j=h;--j>=0;) {
           for(i=0;i<w;++i)
           _a.accumulate(_corner[i]);
           _corner = daim::const_offset_ptr(_corner,_stride);
         }
         (*_out)[x] = _a.result();
	   }
     }  

     A a() { return _a; }
  };


  template<class K,class A> 
  generic_functor<K,A> getGenericFunctor( const K& _k, const A& _a ) {
    return generic_functor<K,A>(_k,_a);
  }

 } // namespace filter

} // namespace daim

#endif // _dmFilterTemplates_h
