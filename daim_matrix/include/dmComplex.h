#ifndef dmComplex_h
#define dmComplex_h

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

#ifndef DM_CONFIG_NO_STL_COMPLEX

#include <complex>

BEGIN_MATRIX()

typedef std::complex<dm_matrix_t> dm_complex_t;
 
END_MATRIX()
 

#else // DM_CONFIG_NO_STL_COMPLEX

BEGIN_MATRIX()

  struct _COMPLEX_T
  {
    typedef dm_matrix_t value_type;
    
    dm_matrix_t _M_re;
    dm_matrix_t _M_im;

    // Constructors, destructor, assignment operator.
    
    _COMPLEX_T() : _M_re(0), _M_im(0) {}
    
    _COMPLEX_T(const dm_matrix_t& __x)
    : _M_re(__x), _M_im(0) {}

    _COMPLEX_T(const dm_matrix_t& __x, const dm_matrix_t& __y)
    : _M_re(__x), _M_im(__y) {}

    _COMPLEX_T( const _COMPLEX_T& __z )
      : _M_re(__z._M_re), _M_im(__z._M_im) {}
      
    _COMPLEX_T& operator=(const _COMPLEX_T& __z) {
      _M_re = __z._M_re;
      _M_im = __z._M_im;
      return *this;
    }

    // Arithmetic op= operations involving one real argument.

    _COMPLEX_T& operator= (const dm_matrix_t& __x) {
      _M_re = __x;
      _M_im = 0;
      return *this;
    }
    
    _COMPLEX_T& operator+= (const dm_matrix_t& __x) {
      _M_re += __x;
      return *this;
    }
    
    _COMPLEX_T& operator-= (const dm_matrix_t& __x) {
      _M_re -= __x;
      return *this;
    }
  
    _COMPLEX_T& operator*= (const dm_matrix_t& __x) {
      _M_re *= __x;
      _M_im *= __x;
      return *this;
    }
    
    _COMPLEX_T& operator/= (const dm_matrix_t& __x) {
      _M_re /= __x;
      _M_im /= __x;
      return *this;
    }

    // Arithmetic op= operations involving two complex arguments.
  
    static void  _div(const dm_matrix_t& __z1_r, const dm_matrix_t& __z1_i,
                      const dm_matrix_t& __z2_r, const dm_matrix_t& __z2_i,
                      dm_matrix_t& __res_r, dm_matrix_t& __res_i);
                   
    static void  _div(const dm_matrix_t& __z1_r, 
                      const dm_matrix_t& __z2_r, const dm_matrix_t& __z2_i,
                      dm_matrix_t& __res_r, dm_matrix_t& __res_i);
                   
    _COMPLEX_T& operator+= (const _COMPLEX_T& __z) {
      _M_re += __z._M_re;
      _M_im += __z._M_im;
      return *this;
    }

    _COMPLEX_T& operator-= (const _COMPLEX_T& __z) {
      _M_re -= __z._M_re;
      _M_im -= __z._M_im;
      return *this;
    }
  
    _COMPLEX_T& operator*= (const _COMPLEX_T& __z) {
      value_type __r = _M_re * __z._M_re - _M_im * __z._M_im;
      value_type __i = _M_re * __z._M_im + _M_im * __z._M_re;
      _M_re = __r;
      _M_im = __i;
      return *this;
    }

    _COMPLEX_T& operator/= (const _COMPLEX_T& __z) {
      value_type __r;
      value_type __i;
      _div(_M_re, _M_im, __z._M_re, __z._M_im, __r, __i);
      _M_re = __r;
      _M_im = __i;
      return *this;
   }
    
   dm_matrix_t real() const { return _M_re; }
   dm_matrix_t imag() const { return _M_im; }

   static dm_matrix_t _abs(const _COMPLEX_T&);
   static dm_matrix_t _arg(const _COMPLEX_T&);

   static _COMPLEX_T  _polar(const dm_matrix_t& __rho, const dm_matrix_t& __phi);

}; 



// Unary non-member arithmetic operators.

inline _COMPLEX_T operator+(const _COMPLEX_T& __z) {
  return __z;
}

inline _COMPLEX_T operator-(const _COMPLEX_T& __z) {
  return _COMPLEX_T(-__z._M_re, -__z._M_im);
}

// Non-member arithmetic operations involving one real argument.

inline _COMPLEX_T operator+(const dm_matrix_t& __x, const _COMPLEX_T& __z) {
  return _COMPLEX_T(__x + __z._M_re, __z._M_im);
}

inline _COMPLEX_T operator+(const _COMPLEX_T& __z, const dm_matrix_t& __x) {
  return _COMPLEX_T(__z._M_re + __x, __z._M_im);
}

inline _COMPLEX_T operator-(const dm_matrix_t& __x, const _COMPLEX_T& __z) {
  return _COMPLEX_T(__x - __z._M_re, -__z._M_im);
}


inline _COMPLEX_T operator-(const _COMPLEX_T& __z, const dm_matrix_t& __x) {
  return _COMPLEX_T(__z._M_re - __x, __z._M_im);
}


inline _COMPLEX_T operator*(const dm_matrix_t& __x, const _COMPLEX_T& __z) {
  return _COMPLEX_T(__x * __z._M_re, __x * __z._M_im);
}

inline _COMPLEX_T operator*(const _COMPLEX_T& __z, const dm_matrix_t& __x) {
  return _COMPLEX_T(__z._M_re * __x, __z._M_im * __x);
}

inline _COMPLEX_T operator/(const dm_matrix_t& __x, const _COMPLEX_T& __z) {
  _COMPLEX_T __result;
  _COMPLEX_T::_div(__x, __z._M_re, __z._M_im,
                   __result._M_re, __result._M_im);
  return __result;
}

inline _COMPLEX_T operator/(const _COMPLEX_T& __z, const dm_matrix_t& __x) {
  return _COMPLEX_T(__z._M_re / __x, __z._M_im / __x);
}

// Non-member arithmetic operations involving two complex arguments

inline _COMPLEX_T operator+(const _COMPLEX_T& __z1, const _COMPLEX_T& __z2) {
  return _COMPLEX_T(__z1._M_re + __z2._M_re, __z1._M_im + __z2._M_im);
}

inline _COMPLEX_T operator-(const _COMPLEX_T& __z1, const _COMPLEX_T& __z2) {
  return _COMPLEX_T(__z1._M_re - __z2._M_re, __z1._M_im - __z2._M_im);
}

inline _COMPLEX_T operator*(const _COMPLEX_T& __z1, const _COMPLEX_T& __z2) {
  return _COMPLEX_T(__z1._M_re * __z2._M_re - __z1._M_im * __z2._M_im,
                    __z1._M_re * __z2._M_im + __z1._M_im * __z2._M_re);
}


inline _COMPLEX_T operator/(const _COMPLEX_T& __z1, const _COMPLEX_T& __z2) {
  _COMPLEX_T __result;
  _COMPLEX_T::_div(__z1._M_re, __z1._M_im,__z2._M_re, __z2._M_im,
                   __result._M_re, __result._M_im);
  return __result;
}

// Comparison operators.

inline bool operator==(const _COMPLEX_T& __z1, const _COMPLEX_T& __z2) {
  return __z1._M_re == __z2._M_re && __z1._M_im == __z2._M_im;
}

inline bool operator==(const _COMPLEX_T& __z, const dm_matrix_t& __x) {
  return __z._M_re == __x && __z._M_im == 0;
}
 
inline bool operator==(const dm_matrix_t& __x, const _COMPLEX_T& __z) {
  return __x == __z._M_re && 0 == __z._M_im;
}


inline bool operator!=(const _COMPLEX_T& __z1, const _COMPLEX_T& __z2) {
  return __z1._M_re != __z2._M_re || __z1._M_im != __z2._M_im;
}


template <class dm_matrix_t> 
inline bool operator!=(const _COMPLEX_T& __z, const dm_matrix_t& __x) {
  return __z._M_re != __x || __z._M_im != 0;
}

template <class dm_matrix_t> 
inline bool operator!=(const dm_matrix_t& __x, const _COMPLEX_T& __z) {
  return __x != __z._M_re || 0 != __z._M_im;
}

// Other basic arithmetic operations

inline dm_matrix_t real(const _COMPLEX_T& __z) { return __z._M_re; }
inline dm_matrix_t imag(const _COMPLEX_T& __z) { return __z._M_im; }
inline dm_matrix_t norm(const _COMPLEX_T& __z) {
  return __z._M_re * __z._M_re + __z._M_im * __z._M_im;
}

inline _COMPLEX_T conj(const _COMPLEX_T& __z) {
  return _COMPLEX_T(__z._M_re, -__z._M_im);
}

inline dm_matrix_t abs(const _COMPLEX_T& __z ) { return _COMPLEX_T::_abs(__z); }
inline dm_matrix_t arg(const _COMPLEX_T& __z ) { return _COMPLEX_T::_arg(__z); }

inline _COMPLEX_T  polar(const dm_matrix_t& __rho, const dm_matrix_t& __phi) {
  return _COMPLEX_T::_polar(__rho,__phi);
}


typedef _COMPLEX_T dm_complex_t;
    

END_MATRIX()

#endif // DM_CONFIG_NO_STL_COMPLEX

#endif // dmComplex_h
