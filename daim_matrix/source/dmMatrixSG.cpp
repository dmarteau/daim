
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

#include "dmMatrixFunc.h"
#include "dmMatrixBase.h"
#include "dmMatrixSG.h"

#define FZERO 0.0f

#include <math.h> // for pow()

BEGIN_MATRIX()
//---------------------------------------------------------------------------
void dmMatrix_SGCoeffs( dmMatrixRowCol& _Coeffs, int nsize, int norder, int nderiv )
{
  int j,k;
  dm_matrix_t x;

  if( nderiv > norder || nsize <= norder)
     MatrixError( "Bad parameters in dmMatrix_SGCoeffs" );

  if(_Coeffs.Length() < nsize )
     MatrixError( "Bad vector size in dmMatrix_SGCoeffs" );

  dmRectMatrix A(nsize,norder+1);

  dmMatrixRow  ARow(&A);

  for(k = 0; k < nsize; ++k ) {
    x = -(nsize-1)/2.0f + k;
    for( j=0; j <= norder; ++j )
       ARow[j+1] = static_cast<dm_matrix_t>(pow( x , j ));

    ARow.Next(); 
  }

  dmRectMatrix At  = A.t();   // transpose matrix
  dmRectMatrix AtA = At * A; 
  //---------------------------------------------------------------------------
  // The coefficients are given by the row of rank 'nderiv' of inv(At*A)*At
  // instead of computing the inverse of At*A,( which mean solving the linear
  // system for nsize+1 unit vectors) we compute Cn = inv(At*A) * (At*Xn) 
  // where Xn is the unit vector for n At*Xn is then simply the n-th column of At
  // We solve inv(At*A) only for the unit vector of rank 'nderiv' which will
  // give us the corresponding row/column (AtA is symetric)
  //---------------------------------------------------------------------------
  dmColumnVector B(norder+1);

  B.Set(0.0);
  B(nderiv+1) = 1.0;
  B.SetStatus(dmMatrixBase::ToReuse);

  dmMatrix_Solve(B,AtA,B.Type());

  dmMatrixCol ACol(&At);
  dmMatrixCol BCol(&B);

  for(k=1;k<=At.NCols();++k) {
    _Coeffs[k] = BCol * ACol;  // dot product of B by a column of At
    ACol.Next();
  }
}
//---------------------------------------------------------------------------
void dmMatrix_ApplyFilter(  const dmMatrixRowCol& _Coeffs, 
                            const dmMatrixRowCol& _Input ,
                            dmMatrixRowCol& _Output )
{
  int flen    = _Coeffs.Length(); 
  int loffs   = flen / 2 + 1;
  int roffs   = flen - loffs;
  int imax    = _Input.Length() - roffs;

  dm_matrix_t acc;
  
  _Output.Copy(_Input);
  
  for(int i=loffs; i<=imax; ++i) {
     acc = FZERO;
     for(int k=-loffs+1; k<=roffs; ++k) {
        acc += _Coeffs[k+loffs] * _Input[i+k];
     }
     _Output[i] = acc;
  }
}
//---------------------------------------------------------------------------
void dmMatrix_SGCoeffs_2D( dmMatrixRowCol& _Coeffs, int nsize, int norder,
                           int nderivx, int nderivy ) 
{
  MatrixError( "Not implemented !" );
}
//---------------------------------------------------------------------------
END_MATRIX()
