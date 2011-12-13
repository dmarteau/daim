#ifndef dmKernelTemplates_h
#define dmKernelTemplates_h

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
// File         : dmKernelTemplates.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#include <functional> // STL

namespace daim {
//-----------------------------------------------------------------
// These are not always defined in some stl ports
//-----------------------------------------------------------------
template<class T> inline T max( const T& x, const T& y) { return (x >= y ? x : y); } 
template<class T> inline T min( const T& x, const T& y) { return (x <= y ? x : y); } 
template<class T> inline T abs( const T& x )     { return (x<0?-x:x); }
template<class T> inline T sgn( const T& x )     { return (x >= 0 ? 1 : -1); }
template<class T> inline T round( const T& x )   { return (x>=0 ? x+0.5f : x-0.5f); }   

template<class T> inline T absdiff( const T& x, const T& y ) {
  return ( x >= y ? x - y : y - x );   
}   

template<class T> inline T minimum( const T& x, const T& y, const T& z ) {
  return daim::min( x , daim::min( y, z ) );
}

template<class T> inline T maximum( const T& x, const T& y, const T& z ) {
  return daim::max( x , daim::max( y, z ) );
}

template<class T> inline T range( const T& x, const T& _min, const T& _max ) {
  return (x > _max) ? _max : (x < _min ? _min : x);
}

//-------------------------------------------------------------
// When dealing with numerical conversion 
// some operations have to deal with signature and "integerness"
//-------------------------------------------------------------

struct null_op  {};

template<bool _bool>
struct _boolean  { operator bool() { return _bool; } };

typedef null_op undefined_assertion;

typedef _boolean<true>  true_assertion;
typedef _boolean<false> false_assertion;

typedef true_assertion  integer_true;
typedef false_assertion integer_false;

typedef true_assertion  unsigned_true;
typedef false_assertion unsigned_false;

typedef unsigned_true  signed_false;
typedef unsigned_false signed_true;

template<class _integer_type,class _unsigned_type> 
struct type_traits {
  typedef _integer_type   integer_type;
  typedef _unsigned_type  signature_type;
}; 

template<class T> struct type_tag {
  typedef undefined_assertion  integer_type;
  typedef undefined_assertion  signature_type;
};

template<> struct type_tag<dm_uint8>   : type_traits<integer_true ,unsigned_true>  {}; 
template<> struct type_tag<dm_uint16>  : type_traits<integer_true ,unsigned_true>  {}; 
template<> struct type_tag<dm_uint32>  : type_traits<integer_true ,unsigned_true>  {}; 
template<> struct type_tag<dm_int8>    : type_traits<integer_true ,unsigned_false> {}; 
template<> struct type_tag<dm_int16>   : type_traits<integer_true ,unsigned_false> {}; 
template<> struct type_tag<dm_int32>   : type_traits<integer_true ,unsigned_false> {}; 
template<> struct type_tag<double>     : type_traits<integer_false,unsigned_false> {}; 
template<> struct type_tag<float>      : type_traits<integer_false,unsigned_false> {}; 


template<class T> 
typename type_tag<T>::integer_type  is_integer ( const T& ) { 
    typedef typename type_tag<T>::integer_type integer_type;
    return integer_type();  
} 

template<class T> 
typename type_tag<T>::signature_type is_unsigned( const T& ) { return type_tag<T>::signature_type();  }

template<class T> type_tag<T> type_traits_of( T )            { return type_tag<T>(); }
template<class T> type_tag<T> type_traits_of_ref( const T& ) { return type_tag<T>(); }
template<class T> type_tag<T> type_traits_of_ptr( const T* ) { return type_tag<T>(); }

// define 32 bits signed integer as a holder for integer operations
// where intermediates results can be out of bounds of the input/output 
// types. 
typedef dm_int32 integer_holder;

//-------------------------------------------------------------
// gap structure
//-------------------------------------------------------------
template<class T>
struct gap : public std::unary_function<T,T> {
  T upper;
  T lower;

  gap( T x1, T x2 ) : upper(x2), lower(x1)  {}

  T diff() const { return upper - lower; }

  T operator()( const T& x ) { 
    if(x>upper) upper=x; else if(x<lower) lower = x;
    return x;
  }
  
  T check( const T& x ) const { 
    return (x > upper ? upper : (x < lower ? lower : x )); 
  }

  T min() const { return daim::min(upper,lower); }
  T max() const { return daim::max(upper,lower); }
  gap<T> inverted() const { return gap<T>(upper,lower); }
};
//-------------------------------------------------------------

//-------------------------------------------------------------
// Others useful operators
//-------------------------------------------------------------
template<class T> 
struct max_of : public std::binary_function<T,T,T> {
	T operator()( const T& x,  const T& y ) { 
      return daim::max(x,y); 
    } 
}; 
//-------------------------------------------------------------
template<class T> 
struct min_of : public std::binary_function<T,T,T> {
	T operator()( const T& x, const T& y ) { 
      return daim::min(x,y); 
    } 
}; 
//-------------------------------------------------------------
// More useful operators
//-------------------------------------------------------------
template<class T> inline bool in_range( const T& x, const T& a, const T& b ) { 
  return (x >= a && x <= b); 
}
template<class T> inline bool in_range_excl( const T& x, const T& a, const T& b ) { 
  return (x > a && x < b);
}
template<class T> inline bool in_range_excl_left( const T& x, const T& a, const T& b ) { 
  return (x > a && x <= b); 
}
template<class T> inline bool in_range_excl_right( const T& x, const T& a, const T& b ) { 
  return (x > a && x <= b);
}
//--------------------------------------------------------------
// We define the same functions but as fonctors
// because using fonctors as operator ensure inlining
//--------------------------------------------------------------
// Test if x is in range [x1,x2]
template<class T> struct between : public std::unary_function<T,bool> {
  T lower,upper;
  between( const T& x1, const T& x2 ) : lower(x1),upper(x2) {}
  bool operator()( const T& x ) const { return (x >= lower && x <= upper); }
};
// Test if x is in range ]x1,x2[
template<class T> struct between_excl : public std::unary_function<T,bool> {
  T lower,upper;
  between_excl( const T& x1, const T& x2 ) : lower(x1),upper(x2) {}
  bool operator()( const T& x ) const { return (x > lower && x < upper); }
};
// Test if x is in range [x1,x2[
template<class T> struct between_excl_right : public std::unary_function<T,bool> {
  T lower,upper;
  between_excl_right( const T& x1, const T& x2 ) : lower(x1),upper(x2) {}
  bool operator()( const T& x ) const { return (x >= lower && x < upper); }
};
// Test if x is in range ]x1,x2]
template<class T> struct between_excl_left : public std::unary_function<T,bool> {
  T lower,upper;
  between_excl_left( const T& x1, const T& x2 ) : lower(x1),upper(x2) {}
  bool operator()( const T& x ) const { return (x > lower && x <= upper); }
};
//-------------------------------------------------------------
// Absolute value as fonctor
//-------------------------------------------------------------
template<class T,class R>
struct abs_value : public std::unary_function<T,R>
{
  R operator()( const T& x ) { return (x >= 0 ? x : -x ); }  
};
//-------------------------------------------------------------
// bounded value as fonctor
//-------------------------------------------------------------
template<class U>
struct limited_value : public std::unary_function<U,U>
{
  U _min_value;
  U _max_value;
  limited_value( U _minv, U _maxv )
  : _min_value(_minv),
    _max_value(_maxv) {}

  U operator()(const U& x) { 
       return (x<_min_value
               ? _min_value
               : (x>_max_value
                  ? _max_value 
                  :x
                 )
               ); 
   }
};
//-------------------------------------------------------------
// Unary function binder : apply (_fun2 o _fun1) ( argument_type ) 
//-------------------------------------------------------------
template<class _Ufn1,class _Ufn2> 
class unary_func_binder : public std::unary_function<typename _Ufn1::argument_type,
                                                     typename _Ufn2::result_type>
{
  protected:
    _Ufn1 _fun1;
    _Ufn2 _fun2;
  public:     
    typedef typename _Ufn2::result_type   result_type;
    typedef typename _Ufn1::argument_type argument_type;

    unary_func_binder(const _Ufn1& f1,const _Ufn2& f2) : _fun1(f1),_fun2(f2) {}
    result_type operator()( const argument_type& x ) { 
       return _fun2( static_cast<typename _Ufn2::argument_type>(_fun1(x)) ); 
    }
};
//-------------------------------------------------------------
template<class _Ufn1,class _Ufn2> 
unary_func_binder<_Ufn1,_Ufn2> bind_func( const _Ufn1& f1,const _Ufn2& f2 )
{
  return unary_func_binder<_Ufn1,_Ufn2>(f1,f2);
}
//-------------------------------------------------------------
// Bind 1 unary function and a 1 binary function
// i.e apply  ( _fun2( _fun1(x,y) )
//-------------------------------------------------------------
template<class _Bfn,class _Ufn> 
class binary_func_binder : public std::binary_function<typename _Bfn::first_argument_type,
                                                       typename _Bfn::second_argument_type,
                                                       typename _Ufn::result_type>
{
  protected:
    _Bfn _fun1;
    _Ufn _fun2;
  public:    
    typedef typename _Bfn::first_argument_type   first_argument_type;
    typedef typename _Bfn::second_argument_type  second_argument_type;
    typedef typename _Ufn::result_type           result_type;

    binary_func_binder(const _Bfn& f1, const _Ufn& f2) : _fun1(f1),_fun2(f2) {}
    result_type operator()( const first_argument_type&  x,
                            const second_argument_type& y ) {
       return _fun2( _fun1(x,y) );
    }
};
//-------------------------------------------------------------
template<class _Bfn,class _Ufn> 
binary_func_binder<_Bfn,_Ufn> bind_binary_func( const _Bfn& f1,const _Ufn& f2 )
{
  return binary_func_binder<_Bfn,_Ufn>(f1,f2);
}
//-------------------------------------------------------------
template<class T>
struct identity : public std::unary_function<T,T> { 
  T operator()( const T& x ) { return x; }
};
//-----------------------------------------------------------------------
// Use types as arguments in templates from representatives element of
// a given type. This is useful when type is not accesible through templates
// parameters but from indirection. Somewhat redundant with type_tag defined 
// above
//-----------------------------------------------------------------------
template<class U> struct type_of { typedef U value_type; };
template<class U> type_of<U> get_type_of( const U&  ) { return type_of<U>(); }
template<class U> type_of<U> get_type_of_ptr( const U*  ) { return type_of<U>(); }
template<class U> type_of<U> get_type_of_ref( const U&  ) { return type_of<U>(); }
//-------------------------------------------------------------

} //namespace daim

#endif // dmKernelTemplates_h
