
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
#include "dmMatrixLU.h"

#include <math.h>
#include <algorithm>
#include <limits>

BEGIN_MATRIX()
//------------------------------------------------------------------------------
dmMatrix* dmMatrix_LU( dmMatrix& _Matrix, int* p, int *sigperm )
{
  dmMatrixBase* Matrix = _Matrix.Evaluate();
  if(Matrix->NRows()!=Matrix->NCols()) 
     MatrixError( "Square matrix required for LU decomposition" );

  int i,j,k,N = Matrix->NCols();
  int nPivot=0,_sigperm = 1;

  dm_matrix_t fBig,fTemp,fSum;

  dmRowVector V(N);
  
  dmMatrixRow MRow(Matrix);
  dmMatrixRow VRow(&V);

  for(i=1;i<=N;++i) {
    fBig = 0.0;
    for(j=1;j<=N;++j) { 
      fTemp = static_cast<dm_matrix_t>(fabs(MRow[j]));
      if(fTemp > fBig) fBig = fTemp;
    }
    // TODO : should be tested against 'epsilon'
    if(fBig == 0.0) MatrixError( "Singular matrix in LU decomposition" );
    VRow[i] = 1.0f/fBig;
    MRow.Next();
  }

  dmMatrixCol MCol(Matrix),MColk(Matrix);

  for(j=1;j<=N;++j)
  {
    for(i=1;i<j;++i) {
      fSum = MCol[i]; 
      Matrix->GetCol(MColk,1);
      for(k=1;k<i;++k) {
         fSum -= MColk[i] * MCol[k];
         MColk.Next();
      } 
      MCol[i] = fSum;    
    }
   
    fBig = 0.0f;
    for(i=j;i<=N;i++) {
      fSum = MCol[i];
      Matrix->GetCol(MColk,1);
      for (k=1;k<j;++k) {
        fSum -= MColk[i] * MCol[k];
        MColk.Next();
      }
      MCol[i] = fSum;

      fTemp = VRow[i] * static_cast<dm_matrix_t>(fabs(fSum));
      if(fTemp >= fBig) {
        fBig   = fTemp;
        nPivot = i;
      }
    }

    if(nPivot != j) {
      dmMatrix_SwapRows(Matrix,j,nPivot);
      VRow[nPivot] = VRow[j];
      _sigperm = -_sigperm;
    }

    p[j-1] = nPivot; 
    if(MCol[j] == 0.0f) 
       MCol[j] = std::numeric_limits<dm_matrix_t>::epsilon();

    if(j != N) {
      fTemp = 1.0f/MCol[j];
      for(i=j+1;i<=N;i++) MCol[i] *= fTemp;
    }
    MCol.Next();
  }

  if(sigperm) *sigperm = _sigperm;
  return Matrix;
}
//------------------------------------------------------------------------------
dmMatrix* dmMatrix_LU_Solve( const dmMatrix& _Matrix, dmMatrixRowCol& _RowCol, int* p )
{
  dmMatrixBase* LU = const_cast<dmMatrix&>(_Matrix).Evaluate();

  if(_RowCol.Length()!=LU->NRows())
    MatrixError( "Invalid input vector size for LU system resolution" );

   dm_matrix_t    fSum;
   dmMatrixRow MRow(LU);

   int ii=0,i,j,ip,N = LU->NRows();
   for (i=1;i<=N;++i) {
     fSum = _RowCol[ ip = p[i-1] ];      
     _RowCol[ip] = _RowCol[i];    // Permute vector data
     if(ii)                       // and do forward subsitution
      for(j=ii;j<=i-1;++j) 
        fSum -= MRow[j] * _RowCol[j];
     else  if(fSum) ii = i;       
     _RowCol[i] = fSum;
     MRow.Next();
   }

   for(i=N;i>=1;--i) {    // Backward substitution
     LU->GetRow(MRow,i);
     fSum = _RowCol[i];
     for(j=i+1;j<=N;++j) 
       fSum -= MRow[j] * _RowCol[j];

     _RowCol[i] = fSum / MRow[i];
   }     
   return LU;
}
//------------------------------------------------------------------------------
END_MATRIX()

