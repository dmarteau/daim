#ifndef dmMatrixLSQ_h
#define dmMatrixLSQ_h

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
// File   : dmMatrixLSQ.h
// Date   : 11/2005
// Author : David Marteau
//--------------------------------------------------------
#include "dmMatrix.h"
#include "dmRowCol.h"

BEGIN_MATRIX()
//------------------------------------------------------------------------
// Linear and Non linear Least Square Fit (in the parameter) 
// using singular value decomposition  (SVD)
//------------------------------------------------------------------------
dm_matrix_t dmMatrixSVDFit( dmMatrixRowCol& P, 
                         const dmMatrixRowCol& Y, 
                         const dmMatrixRowCol& Sig, 
                         dmRectMatrix& U,dmRectMatrix& V, dmColumnVector& SV,
                         void (*funcs)(int,dmMatrixRowCol&,void*), void* _UserData );

void dmMatrixSVDVar( const dmRectMatrix& V, const dmColumnVector& SV, dmRectMatrix& CVM );
//------------------------------------------------------------------------
// Polygonal Fit, the order of the polygone is given by the size of the 
// parameter's vector P minus one. 
// Return the value of the Chi2
//------------------------------------------------------------------------
dm_matrix_t dmMatrixPolyFit( dmMatrixRowCol& P, 
                          const dmMatrixRowCol& Y, 
                          const dmMatrixRowCol& X, 
                          const dmMatrixRowCol& Sig, 
                          dmRectMatrix& U,dmRectMatrix& V, dmColumnVector& SV );
//------------------------------------------------------------------------
// Gaussian Fit, y = A * exp( - [ (x-b)/k ]^2 ) 
// fitted parameter are returned in P.
// P[1] -> A, P[2] -> k, P[3] -> b
// Return the value of the Chi2
//------------------------------------------------------------------------
dm_matrix_t dmMatrixGaussFit( dmMatrixRowCol& P, 
                           const dmMatrixRowCol& Y, 
                           const dmMatrixRowCol& X, 
                           const dmMatrixRowCol& Sig, 
                           dmRectMatrix& U,dmRectMatrix& V, dmColumnVector& SV );
//------------------------------------------------------------------------
// Exponential Fit,  A * exp( - k * x )  
// P[1] -> A, P[2] -> k
// Return the value of the Chi2
//------------------------------------------------------------------------
dm_matrix_t dmMatrixExpFit( dmMatrixRowCol& P, 
                         const dmMatrixRowCol& Y, 
                         const dmMatrixRowCol& X, 
                         const dmMatrixRowCol& Sig, 
                         dmRectMatrix& U,dmRectMatrix& V, dmColumnVector& SV );
//------------------------------------------------------------------------
END_MATRIX()

#endif // dmMatrixLSQ_h
