
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

#include "dmMatrixBase.h"
#include "dmMatrixArray.h"
#include "dmMatrixSpline.h"

BEGIN_MATRIX()

const dm_matrix_t dmSPLINE_NATURAL = MATRIX_PREC(1e30);

#define SPLINE_NATURAL MATRIX_PREC(0.99e30)
//------------------------------------------------------------------------------
void dmInitSpline( dm_spline* spl , dm_matrix_t yp1, dm_matrix_t ypn )
{
  spl->yp1   = yp1;
  spl->ypn   = ypn;
  spl->order = 0;
  spl->khi   = 0;
  spl->klo   = 0; 
  spl->n1    = 0;
  spl->n2    = 0; 
}
//------------------------------------------------------------------------------
static void _dmMatrix_Spline( const dmMatrixRowCol& Y,
                              const dmVectorArray&  x,
                              dmMatrixRowCol& Y2, 
                              dmVectorArray& u,
                              dm_matrix_t yp1, 
                              dm_matrix_t ypn )
{  
  dm_int i,k, n = x.Size();
  dm_matrix_t p,qn,sig,un;

  if (yp1 > SPLINE_NATURAL)
    Y2[1]=u[1]=MATRIX_PREC(0.0);
  else {
    Y2[1] = -MATRIX_PREC(0.5);
    u[1]  = (MATRIX_PREC(3.0)/(x[2]-x[1]))*((Y[2]-Y[1])/(x[2]-x[1])-yp1);
  }
  for (i=2;i<=n-1;i++) 
  {
    sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
    p=sig*Y2[i-1]+MATRIX_PREC(2.0);
    Y2[i]=(sig-MATRIX_PREC(1.0))/p;
    u[i]=(Y[i+1]-Y[i])/(x[i+1]-x[i]) - (Y[i]-Y[i-1])/(x[i]-x[i-1]);
    u[i]=(MATRIX_PREC(6.0)*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
  }
  if (ypn > SPLINE_NATURAL)
    qn=un=MATRIX_PREC(0.0);
  else {
    qn=MATRIX_PREC(0.5);
    un=(MATRIX_PREC(3.0)/(x[n]-x[n-1]))*(ypn-(Y[n]-Y[n-1])/(x[n]-x[n-1]));
  }
  Y2[n]=(un-qn*u[n-1])/(qn*Y2[n-1]+MATRIX_PREC(1.0));
  for (k=n-1;k>=1;k--)
    Y2[k]=Y2[k]*Y2[k+1]+u[k];
}
//------------------------------------------------------------------------------
void dmMatrix_Spline( const dmMatrixRowCol& Y , const dmVectorArray& X, 
                      dmMatrixRowCol& Y2, 
                      dm_spline* spl )
{
  if(Y.Length() != X.Size() )
     MatrixError( "Bad size for X vector in dmMatrix_Spline" );
  if(Y.Length() != Y2.Length() )
     MatrixError( "Bad size for Y2 vector in dmMatrix_Spline_Eval" );

  dmRowVector U(Y.Length()-1);
  dmVectorArray u(U);

  _dmMatrix_Spline(Y,X,Y2,u,spl->yp1,spl->ypn);

  spl->n1  = Y.Length();
  spl->n2  = 1;

  spl->klo = 1;
  spl->khi = Y.Length();
}
//------------------------------------------------------------------------------
static dm_matrix_t _dmMatrix_Spline_Eval( 
                      const dmVectorArray&  xa , 
                      const dmMatrixRowCol& Y  , 
                      const dmMatrixRowCol& Y2 ,
                      dm_matrix_t x , 
                      int klo , int khi , int ord )
{
  dm_matrix_t h,a,b;
 
  h = xa[khi]-xa[klo];
  if(h==0) 
     MatrixError("Bad input array to dmMatrix_Spline_Eval");
       
  a=(xa[khi]-x)/h;
  b=(x-xa[klo])/h;  
 
  switch(ord) {
     case 0: return a*Y[klo]+b*Y[khi]+((a*a*a-a)*Y2[klo]+(b*b*b-b)*Y2[khi])*(h*h)/MATRIX_PREC(6.0);
     case 1: return (Y[khi]-Y[klo])/h - ((MATRIX_PREC(3.0)*a*a-1)*Y2[klo]-(MATRIX_PREC(3.0)*b*b-1)*Y2[khi])*h/MATRIX_PREC(6.0);
     case 2: return a*Y2[klo] + b*Y2[khi];
     default:
      MatrixError("Bad eval order in dmMatrix_Spline_Eval");
  }
  return 0;   
}
//------------------------------------------------------------------------------
dm_matrix_t dmMatrix_Spline_Eval( const dmMatrixRowCol& Y , const dmVectorArray& X, 
                                  const dmMatrixRowCol& Y2, dm_matrix_t x , 
                                  dm_spline* spl )
{
  dm_int khi = spl->khi;
  dm_int klo = spl->klo;
  
  if(khi == klo)
     klo = 0;
  
  if((khi-klo > 1) || (x < X[klo]) || (x > X[khi]))
  {
    dm_int k;
    klo=1;
    khi=X.Size();
    while (khi-klo > 1) {
      k=(khi+klo) >> 1;
      if (X[k] > x) khi=k;
      else klo=k;
    }
    spl->khi = khi;
    spl->klo = klo;
  }
  
  return _dmMatrix_Spline_Eval(X,Y,Y2,x,spl->klo,spl->khi,spl->order);
}
//------------------------------------------------------------------------------
dmMatrixRowCol& 
dmMatrix_Spline_Eval( const dmMatrixRowCol& Y , const dmVectorArray& X, 
                      const dmMatrixRowCol& Y2, 
                      const dmVectorArray&  XR,
                      dmMatrixRowCol& YR , 
                      dm_spline* spl )
{
  if(Y.Length() != X.Size() )
     MatrixError( "Bad size for X vector in dmMatrix_Spline_Eval" );

  if(Y.Length() != Y2.Length() )
     MatrixError( "Bad size for Y2 vector in dmMatrix_Spline_Eval" );
     
  if(YR.Length() != XR.Size() )
     MatrixError( "Incompatible size for output vectors in dmMatrix_Spline_Eval" );
  
  dm_int n = Y.Length(),m = YR.Length();  
  dm_int ord = spl->order;
  
  dm_matrix_t x;
  
  dm_int khi = 2;

  // Eval for each x
  // Assume that X && XR are ordonned
  for(dm_int i=1;i<=m;++i) 
  {
    x = XR[i];
    while( x > X[khi] ) ++khi;
       
    YR[i] = _dmMatrix_Spline_Eval(X,Y,Y2,x,khi-1,khi,ord);
  }
  return YR;  
}
//------------------------------------------------------------------------------
void dmMatrix_Spline2D( const dmRectMatrix&  YY ,
                        const dmVectorArray& X  , 
                        dmRectMatrix&        YY2,
                        dm_spline* spl )
{
  if(YY.NCols() != X.Size() )
     MatrixError( "Bad size for X vector in dmMatrix_Spline2D" );
     
  if(YY.NRows() != YY.NRows() ||
     YY.NCols() != YY.NCols() )
     MatrixError( "Incompatible size for matrix in dmMatrix_Spline2D" );

  spl->n1 = YY.NCols();
  spl->n2 = YY.NRows();
  
  dmRowVector U(spl->n1-1);
  dmVectorArray u(U);

  dmMatrixRow YR(&YY);
  dmMatrixRow Y2R(&YY2);

  for(dm_int k = spl->n2; --k >= 0;) 
  {
    _dmMatrix_Spline(YR,X,Y2R,u,spl->yp1,spl->ypn);  
    YR.Next(); Y2R.Next();
  }
}
//------------------------------------------------------------------------------
void dmMatrix_Spline2D_Eval( const dmRectMatrix&   YY, 
                             const dmVectorArray&  X, 
                             const dmVectorArray&  XC, 
                             const dmRectMatrix&   YY2,
                             const dmVectorArray&  XR, 
                             const dmVectorArray&  XRC, 
                             dmRectMatrix&       YYR, 
                             dm_spline* spl )
{
  dmMatrixRow Y (&YY);
  dmMatrixRow Y2(&YY2);
  
  dmRectMatrix YYTMP(YY.NRows(),YYR.NCols());
  dmMatrixRow  YR(&YYTMP);
    
  dm_int k;  

  // Evaluate for each rows
  for(k=YY.NRows();--k>=0;) {
    dmMatrix_Spline_Eval(Y,X,Y2,XR,YR,spl);
    Y.Next(); Y2.Next(); YR.Next(); 
  }
 
  // Compute spline for columns
  dmMatrixCol YC (&YYTMP); // input
  dmMatrixCol YRC(&YYR);   // output
  
  dmRowVector   U(YC.Length()-1);
  dmVectorArray u(U);
  
  dmRowVector Y2TMP(YYR.NRows());
  dmMatrixRow Y2C(&Y2TMP);
  
  dm_matrix_t yp1 = spl->yp1, ypn = spl->ypn;
  
  for(k=YYTMP.NCols();--k>=0;) 
  {
    _dmMatrix_Spline(YC,XC,Y2C,u,yp1,ypn);
    dmMatrix_Spline_Eval(YC,XC,Y2C,XRC,YRC,spl);
    YC.Next(); YRC.Next();
  }
}
//------------------------------------------------------------------------------
END_MATRIX()
