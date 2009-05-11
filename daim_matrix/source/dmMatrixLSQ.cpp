
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
#include "dmMatrixArray.h"
#include "dmMatrixSVD.h"
#include "dmMatrixLSQ.h"

#include <math.h>
#include <algorithm>
#include <limits>

#include "dmMatrixUtils.h"

BEGIN_MATRIX()

#define EPS 1.2e-7f

#define FZERO 0.0f
#define FONE  1.0f
//------------------------------------------------------------------------------
// Linear least square fit
// See "Numerical Recipes in C" (C),  Chapter §15.4, p 678-679
//------------------------------------------------------------------------------
dm_matrix_t dmMatrixSVDFit( dmMatrixRowCol& P, 
                         const dmMatrixRowCol& Y, 
                         const dmMatrixRowCol& Sig, 
                         dmRectMatrix& U,dmRectMatrix& V,dmColumnVector& SV,
                         void (*funcs)(int,dmMatrixRowCol&,void*), 
                         void* _UserData )
{
  int ma    = P.Length();
  int ndata = Y.Length();

  dmColumnVector YY(ma);
  dmMatrixCol   _YY(&YY);

  dmColumnVector BVec(ndata);
  dmMatrixCol   _BVec(&BVec);

  dmColumnVector Tmp(ma);  
  dmMatrixCol   _Tmp(&Tmp);
    
  int i;
  dm_matrix_t chisq = 0.0;
  dm_matrix_t tmp,sum;

  U. Resize(ndata,ma);
  V. Resize(ma,ma);
  SV.Resize(ma);
  
  dmMatrixRow _URow(&U);

  for (i=1;i<=ndata;i++) 
  {
    (*funcs)(i,_YY,_UserData);
    tmp=1.0f/Sig[i];
    _BVec[i]=Y[i]*tmp;
    
    //for (j=1;j<=ma;j++) u[i][j]=_YY[j]*tmp;
    _URow.Copy(_YY,tmp);
    _URow.Next();
  }
  
 
  dmMatrixCol SVCol(&SV);
  dmMatrix_SVD(U,V,SV);                        // SVD decomposition

  dmMatrix_SVD_Cond(SVCol,1.e-14);             // Matrix conditionning
  dmMatrix_SVD_Solve(U,V,SVCol,_BVec,P,_Tmp);  // Solve system
                           
  for (i=1;i<=ndata;i++)                   // Evaluate chi-square
  {
    (*funcs)(i,_YY,_UserData);
    sum    = P * _YY;
    tmp    = (Y[i]-sum)/Sig[i];   
    chisq += tmp*tmp;
  }
  
  return chisq;
}
//------------------------------------------------------------------------------
void dmMatrixSVDVar( const dmRectMatrix& V, const dmColumnVector& SV, dmRectMatrix& CVM )
{
  int k,j,i;
  dm_matrix_t sum;

  int ma = SV.NRows();

  dmColumnVector _wti(ma);
  
  dmMatrixCol wti(&_wti);
  dmMatrixCol w(&SV);

  dmMatrixArray cvm(CVM);
  dmMatrixArray v(V);

  for (i=1;i<=ma;i++) {
    wti[i]=FZERO;
    if (w[i]) 
        wti[i]=FONE/(w[i]*w[i]);
  }
  for (i=1;i<=ma;i++) {
    for (j=1;j<=i;j++) {
      for (sum=FZERO,k=1;k<=ma;k++) 
           sum += v[i][k]*v[j][k]*wti[k];
           
      cvm[j][i]=cvm[i][j]=sum;
    }
  }
}
//------------------------------------------------------------------------------
// Construct the basis function set
//------------------------------------------------------------------------------
static void _dmMatrixPolyFit( int index, dmMatrixRowCol& P, void* _UserData )
{
  const dmMatrixRowCol* X = reinterpret_cast<const dmMatrixRowCol*>(_UserData);
  
  int j,n=P.Length();
  dm_matrix_t x = (*X)[index];
  
  P[1] = FONE;
  for(j=2;j<=n;++j)
      P[j] = P[j-1] * x;
}
//------------------------------------------------------------------------------
dm_matrix_t dmMatrixPolyFit( dmMatrixRowCol& P, 
                          const dmMatrixRowCol& Y, 
                          const dmMatrixRowCol& X, 
                          const dmMatrixRowCol& Sig, 
                          dmRectMatrix& U, dmRectMatrix& V, dmColumnVector& SV )
{
  return dmMatrixSVDFit(P,Y,Sig,U,V,SV,_dmMatrixPolyFit,(void*)(&X));   
}
//------------------------------------------------------------------------------
// Evaluate y = A * exp( -k * (x-b)^2 ) 
//  log(y) = [ log(A) - kb^2 ] + 2bkx - kx^2
// TODO Set the correct log-transformation on the chi2 value
//       (and for sigma too)
//------------------------------------------------------------------------------
dm_matrix_t dmMatrixGaussFit( dmMatrixRowCol& P, 
                           const dmMatrixRowCol& Y, 
                           const dmMatrixRowCol& X, 
                           const dmMatrixRowCol& Sig, 
                           dmRectMatrix& U, dmRectMatrix& V, dmColumnVector& SV )
{
  if(P.Length() != 3)
     MatrixError( "Bad size for parameters vector in dmMatrixGaussFit" );
    
  dmColumnVector lYVec(Y.Length());
  dmMatrixCol lY(&lYVec);
  lY.Log(Y);  // Get the log of input Y values

  dm_matrix_t chi2 = dmMatrixPolyFit(P,lY,X,Sig,U,V,SV);
  
  dm_double k = - P[3];
  if(k>0) {
    dm_double b = P[2] / (2.0 * k);
    dm_double A = exp( P[1] + k * b * b );
    
    P[1] = static_cast<dm_matrix_t>(A);
    P[2] = static_cast<dm_matrix_t>(k);
    P[3] = static_cast<dm_matrix_t>(b);
    
  } else {
    chi2 = 1e10f;
  }

  return chi2;
}
//------------------------------------------------------------------------------
dm_matrix_t dmMatrixExpFit( dmMatrixRowCol& P, 
                         const dmMatrixRowCol& Y, 
                         const dmMatrixRowCol& X, 
                         const dmMatrixRowCol& Sig, 
                         dmRectMatrix& U,dmRectMatrix& V, dmColumnVector& SV )
{
  if(P.Length() != 2)
     MatrixError( "Bad size for parameters vector in dmMatrixExpFit" );
    
  dmColumnVector lYVec(Y.Length());
  dmMatrixCol lY(&lYVec);
  lY.Log(Y);  // Get the log of input Y values

  dm_matrix_t chi2 = dmMatrixPolyFit(P,lY,X,Sig,U,V,SV);
  
  dm_double k = - P[2];
  if(k>0) {
    P[1] = static_cast<dm_matrix_t>(exp( P[1] ));
    P[2] = static_cast<dm_matrix_t>(k);    
  } else {
    chi2 = 1e10f;
  }

  return chi2;
}
//------------------------------------------------------------------------------

END_MATRIX()
