#ifndef dmMatrixFunc_h
#define dmMatrixFunc_h

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
// File         : dmMatrixFunc.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#include "dmMatrix.h"
#include "dmRowCol.h"

BEGIN_MATRIX()
//------------------------------------------------------------------------
dmMatrixBase* dmMatrix_SwapRows( dmMatrixBase* _Matrix, int i, int j );
dmMatrixBase* dmMatrix_SwapCols( dmMatrixBase* _Matrix, int i, int j );
dmMatrixBase* dmMatrix_Identity( dmMatrixBase* _Matrix );
dmMatrixBase* dmMatrix_Diagonal( dmMatrixBase* _Matrix , const dmMatrixRowCol& _RowCol );
//------------------------------------------------------------------------
dm_matrix_t dmMatrix_BilinearProduct( const dmMatrix& _Matrix, const dmMatrixRowCol& _RowCol );
void        dmMatrix_Apply( const dmMatrix& _Matrix, const dmMatrixRowCol& _RowCol, dmMatrixRowCol& _Result ); 
//------------------------------------------------------------------------
dmMatrixBase* dmMatrix_Multiply ( const dmMatrix& Lhs,const dmMatrix& Rhs, dmMatrixType );
dmMatrixBase* dmMatrix_Add      ( const dmMatrix& Lhs,const dmMatrix& Rhs, dmMatrixType );
dmMatrixBase* dmMatrix_Substract( const dmMatrix& Lhs,const dmMatrix& Rhs, dmMatrixType );
dmMatrixBase* dmMatrix_Solve    ( const dmMatrix& Lhs,const dmMatrix& Rhs, dmMatrixType );
dmMatrixBase* dmMatrix_Shift    ( const dmMatrix& , dm_matrix_t Value, dmMatrixType );
dmMatrixBase* dmMatrix_Scale    ( const dmMatrix& , dm_matrix_t Value, dmMatrixType );
dmMatrixBase* dmMatrix_Transpose( const dmMatrix& , dmMatrixType );
dmMatrixBase* dmMatrix_Invert   ( const dmMatrix& , dmMatrixType );
//------------------------------------------------------------------------
dmMatrixBase* dmMatrix_Normalize( dmMatrix& , dm_matrix_t& MinValue, dm_matrix_t& MaxValue );
//------------------------------------------------------------------------

END_MATRIX()

#endif // dmMatrixFunc_h
