#ifndef dmMatrixSpline_h
#define dmMatrixSpline_h

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
// File         : dmMatrixSpline.h
// Date         : 11/2005
// Author       : David Marteau
//--------------------------------------------------------
#include "dmMatrix.h"
#include "dmRowCol.h"

BEGIN_MATRIX()

// Generate spline interpolation

class dmVectorArray;

struct dm_spline {
  dm_matrix_t yp1;   // first derivative value at first point
  dm_matrix_t ypn;   // first derivative value at last  point
  dm_int      order; // order of derivative for evaluation 
  dm_int      khi;
  dm_int      klo; 
  dm_int      n1;
  dm_int      n2;   
};

extern const dm_matrix_t dmSPLINE_NATURAL;

void dmInitSpline( dm_spline* , dm_matrix_t yp1, dm_matrix_t ypn );

inline void dmInitNaturalSpline( dm_spline* spl ) {
  dmInitSpline(spl,dmSPLINE_NATURAL,dmSPLINE_NATURAL);
}


//------------------------------------------------------------
// Compute 1D spline for input data Y evaluated at X.
//------------------------------------------------------------

void dmMatrix_Spline( const dmMatrixRowCol& Y , const dmVectorArray& X, 
                      dmMatrixRowCol& Y2, 
                      dm_spline* );
                      
// Evaluate interpolated data at X evaluated at Y.
                                            
dm_matrix_t 
dmMatrix_Spline_Eval( const dmMatrixRowCol& Y , const dmVectorArray& X, 
                      const dmMatrixRowCol& Y2, dm_matrix_t x , 
                      dm_spline* );

// Evaluate interpolated values for each value of X . X must be a vector of ordonned
// values in ascending order.

dmMatrixRowCol& 
dmMatrix_Spline_Eval( const dmMatrixRowCol& Y , const dmVectorArray& X, 
                      const dmMatrixRowCol& Y2, 
                      dmRowVector&    XR ,
                      dmMatrixRowCol& YR , 
                      dm_spline* );

//-------------------------------------------------------------
// Compute 2D spline
//-------------------------------------------------------------
                      
void dmMatrix_Spline2D( const dmRectMatrix& YY , const dmRowVector&  X1, 
                        dmRectMatrix& YY2,
                        dm_spline* spl );
                        
void dmMatrix_Spline2D_Eval( const dmRectMatrix&  YY, 
                             const dmVectorArray& X, 
                             const dmVectorArray& XC,
                             const dmRectMatrix&  YY2,
                             const dmVectorArray& XR, 
                             const dmVectorArray& XRC, 
                             dmRectMatrix& YYR, 
                             dm_spline* spl );

//------------------------------------------------------------
END_MATRIX()

#endif // dmMatrixSpline_h
