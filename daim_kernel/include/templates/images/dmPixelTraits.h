#ifndef dmPixelsTraits_h
#define dmPixelsTraits_h

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
// File         : dmPixelsTraits.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

//-------------------------------------------------------------------------
// default traits for pixel base types
//-------------------------------------------------------------------------
#include <limits> // stl numeric_limits

namespace daim {

struct pixel_cat_tag {};
struct pixel_compound_tag : public pixel_cat_tag {};
struct pixel_scalar_tag   : public pixel_cat_tag {};

template<class T>
struct  basic_traits {
  typedef T                 value_type;
  typedef T*                pointer_type;
  typedef T&                reference_type;
  typedef const T*          const_pointer_type;
  typedef const T&          const_reference_type;
  typedef pixel_scalar_tag  pixel_category;

  typedef typename type_tag<T>::signature_type  signature_type;
  typedef typename type_tag<T>::integer_type    integer_type;

  typedef std::numeric_limits<T>  limits_type;

  static value_type min()  { return limits_type::min(); }
  static value_type max()  { return limits_type::max(); }

  // By default, we assume that the type is numeric so we set the zero value
  // to the number 0
  static value_type zero()   { return 0; }
  static value_type object() { return basic_traits<T>::max(); }

};

//------------------------------------------------------------------
template<> struct basic_traits<float>
{
  typedef float T;

  typedef std::numeric_limits<T>  limits_type;
  
  typedef T                 value_type;
  typedef T*                pointer_type;
  typedef T&                reference_type;
  typedef const T*          const_pointer_type;
  typedef const T&          const_reference_type;

  typedef pixel_scalar_tag  pixel_category;

  typedef type_tag<T>::signature_type  signature_type;
  typedef type_tag<T>::integer_type    integer_type;

  static  value_type min()    { return - limits_type::max(); }
  static  value_type max()    { return   limits_type::max(); }
  
  static  value_type zero()   { return 0;   }
  static  value_type object() { return 1.0; }

};

template<class T>
struct pixel_traits
{
  typedef typename basic_traits<T>::pixel_category       pixel_category;
  typedef typename basic_traits<T>::value_type           value_type;
  typedef typename basic_traits<T>::pointer_type         pointer_type;
  typedef typename basic_traits<T>::reference_type       reference_type;
  typedef typename basic_traits<T>::const_pointer_type   const_pointer_type;
  typedef typename basic_traits<T>::const_reference_type const_reference_type;
  typedef typename basic_traits<T>::signature_type       signature_type;
  typedef typename basic_traits<T>::integer_type         integer_type;

  typedef typename basic_traits<T>::limits_type limits_type;

  static bool is_scalar()       { return true; }

  static size_t          unit_size()   { return sizeof(value_type);        }
  static size_t          bits_depth()  { return sizeof(value_type) << 3;   }
  static value_type      min()         { return basic_traits<T>::min();    }
  static value_type      max()         { return basic_traits<T>::max();    }
  static value_type      zero()        { return basic_traits<T>::zero();   }
  static value_type      object()      { return basic_traits<T>::object(); }

  static gap<value_type> bounds()      { return gap<value_type>(min(),max()); }

  static value_type  scalar_value(value_type _v) { return _v; }
    
  //-----------------------------------------------------------------
  // pixels operations 
  //-----------------------------------------------------------------
  struct subpixel : public std::binary_function<value_type,value_type,value_type> {
     value_type _min;
     subpixel() : _min(pixel_traits<T>::min()) {} 
     value_type operator()( const value_type& _x1, const value_type& _x2 ) const
     { return (_x1 - _min >= _x2 ? (_x1 - _x2) : _min ); }
  };
  //
  struct addpixel : public std::binary_function<value_type,value_type,value_type> {
     value_type _max;
     addpixel() : _max(pixel_traits<T>::max()) {} 
     value_type operator()( const value_type& _x1, const value_type& _x2 ) const
     { 
       return (_x1 <= _max - _x2 ? (_x1 + _x2) : _max ); 
     }
  };

  template<class S = value_type>
  struct static_convert : public std::unary_function<T,value_type> { 
    value_type operator()( const S& x ) {
      return static_cast<value_type>(x);
    } 
  };

}; // pixel_traits
//-------------------------------------------------------------------

template<class _A1,class _A2,class _R> 
struct pixel_binary_function
{
  typedef typename pixel_traits<_A1>::value_type first_argument_type;
  typedef typename pixel_traits<_A2>::value_type second_argument_type;
  typedef typename pixel_traits<_R>::value_type  result_type;
};

template<class _A,class _R> 
struct pixel_unary_function
{
  typedef typename pixel_traits<_A>::value_type argument_type;
  typedef typename pixel_traits<_R>::value_type result_type;
};


//-------------------------------------------------------------------

}; //namespace daim

#endif //dmPixelsTraits_h

