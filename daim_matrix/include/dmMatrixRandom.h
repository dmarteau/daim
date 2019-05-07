#ifndef dmMatrixRandom_h
#define dmMatrixRandom_h

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
// File         : dmMatrixRandom.h
// Date         : 11/2005
// Author       : David Marteau
//--------------------------------------------------------

BEGIN_MATRIX()

// Generate uniform deviate 
// random number between 0 and 1 (exclusive)

struct dm_random {
  dm_int32    iy;
  dm_int32    iv[32];
  dm_int32    idum;
  dm_int      iset;
  dm_matrix_t gset;  
};

void dmInitRandomSequence( dm_random* , dm_int32 seed );
//------------------------------------------------------------
dm_matrix_t dmUniformDeviate( dm_random* );
dm_matrix_t dmExponentialDeviate( dm_random* );
dm_matrix_t dmGaussianDeviate( dm_random* );
//------------------------------------------------------------
// Generate random matrix
//------------------------------------------------------------
dmMatrixBase*   dmMatrix_Random( dmMatrixBase* _Matrix  , dm_random* , dm_matrix_t (*_DeviateFunc)(dm_random*));
dmMatrixRowCol& dmMatrix_Random( dmMatrixRowCol& _RowCol, dm_random* _random, dm_matrix_t (*_DeviateFunc)(dm_random*));
//------------------------------------------------------------
END_MATRIX()

#endif // dmMatrixRandom_h
