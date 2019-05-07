
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


#include "dmMatrixData.h"
#include "dmComplex.h"

#include "math.h"

BEGIN_MATRIX()
//-----------------------------------------------------------------------
void _COMPLEX_T::_div(const dm_matrix_t& __z1_r, const dm_matrix_t& __z1_i,
                      const dm_matrix_t& __z2_r, const dm_matrix_t& __z2_i,
                      dm_matrix_t& __res_r, dm_matrix_t& __res_i) {
                        
  dm_matrix_t __ar = __z2_r >= 0 ? __z2_r : -__z2_r;
  dm_matrix_t __ai = __z2_i >= 0 ? __z2_i : -__z2_i;

  if (__ar <= __ai) 
  {
    dm_matrix_t __ratio = __z2_r / __z2_i;
    dm_matrix_t __denom = __z2_i * (1 + __ratio * __ratio);
    __res_r = (__z1_r * __ratio + __z1_i) / __denom;
    __res_i = (__z1_i * __ratio - __z1_r) / __denom;
  }
  else
  {
    dm_matrix_t __ratio = __z2_i / __z2_r;
    dm_matrix_t __denom = __z2_r * (1 + __ratio * __ratio);
    __res_r = (__z1_r + __z1_i * __ratio) / __denom;
    __res_i = (__z1_i - __z1_r * __ratio) / __denom;
  }
}
//-----------------------------------------------------------------------
void _COMPLEX_T::_div(const dm_matrix_t& __z1_r,
                      const dm_matrix_t& __z2_r, const dm_matrix_t& __z2_i,
                      dm_matrix_t& __res_r, dm_matrix_t& __res_i) {
  dm_matrix_t __ar = __z2_r >= 0 ? __z2_r : -__z2_r;
  dm_matrix_t __ai = __z2_i >= 0 ? __z2_i : -__z2_i;

  if (__ar <= __ai) 
  {
    dm_matrix_t __ratio = __z2_r / __z2_i;
    dm_matrix_t __denom = __z2_i * (1 + __ratio * __ratio);
    __res_r = (__z1_r * __ratio) / __denom;
    __res_i = - __z1_r / __denom;
  }
  else
  {
    dm_matrix_t __ratio = __z2_i / __z2_r;
    dm_matrix_t __denom = __z2_r * (1 + __ratio * __ratio);
    __res_r = __z1_r / __denom;
    __res_i = - (__z1_r * __ratio) / __denom;
  }
}
//-----------------------------------------------------------------------
dm_matrix_t _COMPLEX_T::_abs(const _COMPLEX_T& __z)
{
  return static_cast<dm_matrix_t>( hypot(__z._M_re,__z._M_im) );
}
//-----------------------------------------------------------------------
dm_matrix_t _COMPLEX_T::_arg(const _COMPLEX_T& __z) 
{
  return static_cast<dm_matrix_t>( atan2(__z._M_im, __z._M_re) );
}
//-----------------------------------------------------------------------
_COMPLEX_T _COMPLEX_T::_polar(const dm_matrix_t& __rho, const dm_matrix_t& __phi)
{
  return 
     _COMPLEX_T(static_cast<dm_matrix_t>(__rho * cos(__phi)),
                static_cast<dm_matrix_t>(__rho * sin(__phi)) );
}
//-----------------------------------------------------------------------
END_MATRIX()
