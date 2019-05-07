
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

#define dmUseUtilitiesExtra // Include dmVector

#include "dmMatrixFunc.h"
#include "dmMatrixBase.h"
#include "dmMatrixArray.h"
#include "dmMatrixGaussJ.h"


#define SWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}

BEGIN_MATRIX()
//------------------------------------------------------------------------------
// WARNING: int array must be offset by one
//------------------------------------------------------------------------------
void dmMatrix_GaussJ_1( dmMatrixArray& a,dmMatrixArray& b, int n,
                        int *indxc,int *indxr,int *ipiv )
{
  int i,icol,irow,j,k,l,ll;
  dm_matrix_t big,dum,pivinv,temp;
  
  int m = b.NCols();
  
  for (j=1;j<=n;j++) 
   ipiv[j]=0;
  
  for (i=1;i<=n;i++) 
  {
    big=0.0;
    for (j=1;j<=n;j++) {
      if (ipiv[j] != 1) {
        for (k=1;k<=n;k++) {
          if (ipiv[k] == 0) 
          {
            temp = static_cast<dm_matrix_t>(fabs(a[j][k]));
            if(temp >= big) {
              big=temp;
              irow=j;
              icol=k;
            }
          } else 
            if(ipiv[k] > 1) 
               MatrixError( "Singular Matrix-1 in Gauss Jordan elimination" );
        }
      }
    }
    
    ++(ipiv[icol]);
    
    if (irow != icol) {
      for (l=1;l<=n;l++) SWAP(a[irow][l],a[icol][l])
      for (l=1;l<=m;l++) SWAP(b[irow][l],b[icol][l])
    }
    
    indxr[i]=irow;
    indxc[i]=icol;
    
    if(a[icol][icol] == 0.0) 
       MatrixError( "Singular Matrix-2 in Gauss Jordan elimination" );
       
    pivinv= MATRIX_PREC(1.0)/a[icol][icol];
    a[icol][icol]=1.0;
    
    for (l=1;l<=n;++l) a[icol][l] *= pivinv;
    for (l=1;l<=m;++l) b[icol][l] *= pivinv;
    
    for (ll=1;ll<=n;++ll)
      if (ll != icol) 
      {
        dum=a[ll][icol];
        a[ll][icol]=0.0;
        for (l=1;l<=n;++l) a[ll][l] -= a[icol][l]*dum;
        for (l=1;l<=m;++l) b[ll][l] -= b[icol][l]*dum;
      }
  }
  for (l=n;l>=1;--l) 
  {
    if (indxr[l] != indxc[l])
      for (k=1;k<=n;k++)
        SWAP(a[k][indxr[l]],a[k][indxc[l]]);
  }
  
}
//------------------------------------------------------------------------------
dmMatrix* dmMatrix_GaussJ( dmMatrix& _Lhs, dmMatrix& _Rhs )
{
  dmMatrixBase* Lhs = _Lhs.Evaluate();
  if(Lhs->NRows()!=Lhs->NCols()) 
     MatrixError( "Square matrix required for Gauss Jordan elimination" );

  dmMatrixBase* Rhs = _Rhs.Evaluate();
  if(Lhs->NCols()!=Rhs->NRows()) 
     MatrixError( "Invalid Rhs matrix for Gauss Jordan elimination" );

  int n = Lhs->NCols();

  // Allocate temporary data
  dmVector<int> indxc(n);
  dmVector<int> indxr(n);
  dmVector<int> ipiv (n);

  dmMatrixArray a(Lhs);
  dmMatrixArray b(Rhs);

  dmMatrix_GaussJ_1(a,b,n,
     indxc.Begin()-1,
     indxr.Begin()-1,
     ipiv .Begin()-1
   );
   
  return Lhs;
}
//------------------------------------------------------------------------------
END_MATRIX()

