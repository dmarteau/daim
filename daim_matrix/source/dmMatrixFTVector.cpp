
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
 *  ::: END LICENSE BLOCK:::
 */
#include "dmMatrixFunc.h"
#include "dmMatrixBase.h"
#include "dmMatrixArray.h"
#include "dmMatrixFTVector.h"

BEGIN_MATRIX()

void __dm_real_fft(dm_matrix_t* data, dm_uint n, dm_int isign);
void __dm_fft_spectrum_1d(dm_matrix_t* ft,dm_matrix_t* sp, dm_uint nn);

static inline bool is_power_of_2( unsigned long x )  {
   return ((((x) - 1) & (x)) == 0);
}
//------------------------------------------------------------------------------
void dmMatrix_FT( dmRowVector& _Data )
{
  dmVectorArray data(_Data);
  
  if(!is_power_of_2(_Data.NCols()))
     MatrixError( "Bad size for data vector in dmMatrix_FT" );
  
  __dm_real_fft(data.m(),_Data.NCols(),1);
}
//------------------------------------------------------------------------------
void dmMatrix_FT_Reverse ( dmRowVector& _FT, bool _Normalize )
{
  dmVectorArray ft(_FT);

  if(!is_power_of_2(_FT.NCols()))
      MatrixError( "Bad size for data vector in dmMatrix_FT_Reverse" );
  
  __dm_real_fft(ft.m(),_FT.NCols(),-1);
  
  if(_Normalize) {
     dmMatrixRow _Row(&_FT);
     _Row.Div( static_cast<dm_matrix_t>(_FT.NCols()/2) );
  }
}
//------------------------------------------------------------------------------
void dmMatrix_FT_Spectrum( const dmRowVector& _FT , dmRowVector& _Sp )
{
  if(_Sp.NCols() < (_FT.NCols()/2 + 1))
     MatrixError( "Bad size for data vectors in dmMatrix_FT_Spectrum" );
  
  dmVectorArray ft(_FT);
  dmVectorArray sp(_Sp);
 
  __dm_fft_spectrum_1d(ft.m(),sp.m(),_FT.NCols());
}
//------------------------------------------------------------------------------
void dmMatrix_FT_Derivate( dmRowVector& _FT , dm_int norder )
{

}
//------------------------------------------------------------------------------
END_MATRIX()
