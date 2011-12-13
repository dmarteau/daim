#ifndef dmMatrixSVD_h
#define dmMatrixSVD_h

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

//--------------------------------------------------------
// File   : dmMatrixSVD.h
// Date   : 11/2005
// Author : David Marteau
//--------------------------------------------------------
#include "dmMatrix.h"
#include "dmRowCol.h"

BEGIN_MATRIX()
//------------------------------------------------------------------------
// Singular value decomposition of the matrix _Matrix
// i.e A = U.SV.Vt where SV is a diagonal matrix holding the
// singular values. _Matrix will be replaced by U at output
//
//  dmMatrix_SVD_Solve : Solve the SVD System for unknown vector _Result
//                       an right hand side _RowCol. 
//------------------------------------------------------------------------
dmMatrix* dmMatrix_SVD( dmMatrix& _Matrix, dmRectMatrix& V, dmColumnVector& SV, int _MaxIters = 30 );
double    dmMatrix_SVD_Cond ( const dmMatrixRowCol& SV, double factor );
void      dmMatrix_SVD_Solve( const dmMatrix& U,const dmMatrix& V,
                              const dmMatrixRowCol& SV,
                              const dmMatrixRowCol& _RowCol, 
                              dmMatrixRowCol& _Result,
                              dmMatrixRowCol& _Tmp );
//------------------------------------------------------------------------
END_MATRIX()

#endif // dmMatrixSVD_h
