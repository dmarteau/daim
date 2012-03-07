#ifndef dmPolar_h
#define dmPolar_h

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
// File         : dmPolar.h
// Date         : 12/2004
// Author       : David Marteau
//--------------------------------------------------------

#include <math.h>

namespace daim {

// Compute angle given to input sources 
// as x and y components 

namespace polar 
{


#ifdef DM_NO_PARTIAL_SPECIALIZATION

  // Because VC++ does not like partial specialization
  // but accept specialization declaration in templates
  // but this feature is not accepted in ISO compliant
  // compilers like g++

  template<class T,class integer_type> struct phase 
  {
     template<class integer_type> struct _promoted {};

     template<>
     struct _promoted<integer_true> : std::binary_function<T,T,dm_uint8> {
       dm_uint8 operator()( const T& x, const T& y ) {
         return pixel_traits<dm_uint8>()::clamp( atan2(y,x) * 40.58451048 + 127.5 ); // 1/Pi
       }
     };

     // return a value in [-1,1]
     template<>
     struct _promoted<integer_false> : std::binary_function<T,T,float> {
       float operator()( const T& x, const T& y ) {
         return static_cast<float>(atan2(y,x) * 0.318309886);
       }
     };

     typedef _promoted<integer_type> functor;
  };


  template<class T,class integer_type> struct norm 
  {
    template<class integer_type> struct _promoted {};

    template<>
    struct _promoted<integer_false> : std::binary_function<T,T,float> {
      float operator()( const T& x, const T& y ) {
        return static_cast<float>(
                 sqrt(static_cast<float>(x)*x +
                      static_cast<float>(y)*y ));
      }
    };

     typedef _promoted<integer_type> functor;

  };



#else //DM_NO_PARTIAL_SPECIALIZATION

  // This is the ISO compliant code for 
  // partial specialization

  template<class T,class integer_type> struct phase {};

  // return a value in [0,255]
  template<class T>
  struct phase<T,integer_true> : std::binary_function<T,T,dm_uint8> 
  {
    typedef phase<T,integer_true> functor;

    dm_uint8 operator()( const T& x, const T& y ) {
      return pixel_traits<dm_uint8>::clamp( atan2(y,x) * 40.58451048 + 127.5 ); // 1/Pi
    }
  };

  // return a value in [-1,1]
  template<class T>
  struct phase<T,integer_false> : std::binary_function<T,T,float>
  {
    typedef phase<T,integer_false> functor;

    float operator()( const T& x, const T& y ) {
      return static_cast<float>(atan2(y,x) * 0.318309886);
    }
  };


  template<class T,class integer_type>
  struct norm {};

  template<class T>
  struct norm<T,integer_false> : std::binary_function<T,T,float>
  {
    typedef phase<T,integer_false> functor;

    float operator()( const T& x, const T& y ) {
      return static_cast<float>(
               sqrt(static_cast<float>(x)*x +
                    static_cast<float>(y)*y ));
    }
  };

#endif//DM_NO_PARTIAL_SPECIALIZATION

} // namespace polar


} // namespace daim

#endif // dmPolar_h
