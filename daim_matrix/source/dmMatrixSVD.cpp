
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

#include <math.h>
#include <algorithm>
#include <limits>

#include "dmMatrixUtils.h"

#define FZERO MATRIX_PREC(0.0)
#define FONE  MATRIX_PREC(1.0)

BEGIN_MATRIX()
//------------------------------------------------------------------------------
// Singular value decomposition algorithm
// See "Numerical Recipes in C" (C),  Chapter �2.6, p 68-69
//------------------------------------------------------------------------------
static dm_matrix_t pythag( dm_double a, dm_double b )
{
  dm_double absa = fabs(a);
  dm_double absb = fabs(b);

  if(absa > absb)
    return static_cast<dm_matrix_t>( absa * sqrt(1.0+dm_numerical::sqr(absb/absa)) );
  else
    return static_cast<dm_matrix_t>(absb == 0.0  ? 0.0 : absb*sqrt(1.0+dm_numerical::sqr(absa/absb)) );
}
//------------------------------------------------------------------------------
dmMatrix* dmMatrix_SVD( dmMatrix& _Matrix, dmRectMatrix& V, dmColumnVector& SV,
                        int _MaxIters )
{
  dmMatrixBase* U = _Matrix.Evaluate();

  int n = U->NCols();
  int m = U->NRows();

  int flag,i,its,j,jj,k,l=0,nm=0;
  dm_matrix_t anorm,c,f,g,h,s,scale,x,y,z;

  V. Resize(n,n);
  SV.Resize(n);

  dmRowVector RV1(n);

  //
  dmMatrixArray a(U);
  dmMatrixArray v(V);
  dmVectorArray w(SV);
  dmVectorArray rv1(RV1);

  g=scale=anorm=FZERO;

  for (i=1;i<=n;i++)
  {
    l=i+1;
    rv1[i]=scale*g;
    g=s=scale=FZERO;
    if (i <= m)
    {
      for (k=i;k<=m;k++) {
           scale += static_cast<dm_matrix_t>(fabs(a[k][i]));
      }

      if (scale)
      {
        for (k=i;k<=m;k++)
        {
          a[k][i] /= scale;
          s += a[k][i]*a[k][i];
        }
        f=a[i][i];
        g = -dm_numerical::sign(static_cast<dm_matrix_t>(sqrt(s)),f);
        h=f*g-s;
        a[i][i]=f-g;

        for (j=l;j<=n;j++)
        {
          for (s=FZERO,k=i;k<=m;k++)
               s += a[k][i]*a[k][j];

          f=s/h;

          for (k=i;k<=m;k++)
               a[k][j] += f*a[k][i];
        }

        for (k=i;k<=m;k++)
            a[k][i] *= scale;
      }
    }
    w[i]=scale *g;
    g=s=scale=0.0;

    if (i <= m && i != n)
    {
      for (k=l;k<=n;k++)
           scale += static_cast<dm_matrix_t>(fabs(a[i][k]));

      if (scale)
      {
        for (k=l;k<=n;k++)
        {
          a[i][k] /= scale;
          s += a[i][k]*a[i][k];
        }

        f=a[i][l];
        g = -dm_numerical::sign(static_cast<dm_matrix_t>(sqrt(s)),f);
        h=f*g-s;
        a[i][l]=f-g;

        for (k=l;k<=n;k++)
             rv1[k]=a[i][k]/h;

        for (j=l;j<=m;j++)
        {
          for (s=FZERO,k=l;k<=n;k++)
               s += a[j][k]*a[i][k];

          for (k=l;k<=n;k++)
               a[j][k] += s*rv1[k];
        }
        for (k=l;k<=n;k++)
          a[i][k] *= scale;
      }
    }
    anorm=dm_numerical::max(anorm,(static_cast<dm_matrix_t>(fabs(w[i])+fabs(rv1[i]))));
  }
  for (i=n;i>=1;i--)
  {
    if (i < n)
    {
      if (g)
      {
        for (j=l;j<=n;j++)
          v[j][i]=(a[i][j]/a[i][l])/g;

        for (j=l;j<=n;j++)
        {
          for (s=FZERO,k=l;k<=n;k++)
               s += a[i][k]*v[k][j];

          for (k=l;k<=n;k++)
               v[k][j] += s*v[k][i];
        }
      }
      for (j=l;j<=n;j++)
         v[i][j]=v[j][i]=FZERO;
    }
    v[i][i]=FONE;
    g=rv1[i];
    l=i;
  }
  for (i=dm_numerical::min(m,n);i>=1;i--)
  {
    l=i+1;
    g=w[i];

    for (j=l;j<=n;j++)
         a[i][j]=FZERO;

    if (g)
    {
      g=FONE/g;
      for (j=l;j<=n;j++)
      {
        for (s=FZERO,k=l;k<=m;k++)
             s += a[k][i]*a[k][j];

        f=(s/a[i][i])*g;

        for (k=i;k<=m;k++)
             a[k][j] += f*a[k][i];
      }
      for (j=i;j<=m;j++)
           a[j][i] *= g;
    } else
      for (j=i;j<=m;j++)
         a[j][i]=FZERO;

    ++a[i][i];
  }
  for (k=n;k>=1;k--)
  {
    for (its=1;its<=_MaxIters;its++)
    {
      flag=1;
      for (l=k;l>=1;l--)
      {
        nm=l-1;
        if (static_cast<dm_matrix_t>(fabs(rv1[l])+anorm) == anorm) {
          flag=0;
          break;
        }
        if (static_cast<dm_matrix_t>(fabs(w[nm])+anorm) == anorm) break;
      }
      if (flag)
      {
        c=FZERO;
        s=FONE;
        for (i=l;i<=k;i++)
        {
          f=s*rv1[i];
          rv1[i]=c*rv1[i];

          if(static_cast<dm_matrix_t>(fabs(f)+anorm) == anorm)
             break;

          g=w[i];
          h=pythag(f,g);
          w[i]=h;
          h=FONE/h;
          c=g*h;
          s = -f*h;
          for (j=1;j<=m;j++)
          {
            y=a[j][nm];
            z=a[j][i];
            a[j][nm]=y*c+z*s;
            a[j][i]=z*c-y*s;
          }
        }
      }

      z=w[k];

      if (l == k)
      {
        if (z < FZERO)
        {
          w[k] = -z;
          for (j=1;j<=n;j++) v[j][k] = -v[j][k];
        }
        break;
      }

      if (its >= _MaxIters)
          MatrixError("No convergence");

      x=w[l];
      nm=k-1;
      y=w[nm];
      g=rv1[nm];
      h=rv1[k];
      f=((y-z)*(y+z)+(g-h)*(g+h))/(static_cast<dm_matrix_t>(2.0)*h*y);
      g=pythag(f,1.0);
      f=((x-z)*(x+z)+h*((y/(f+dm_numerical::sign(g,f)))-h))/x;
      c=s=FONE;

      for (j=l;j<=nm;j++)
      {
        i=j+1;
        g=rv1[i];
        y=w[i];
        h=s*g;
        g=c*g;
        z=pythag(f,h);
        rv1[j]=z;
        c=f/z;
        s=h/z;
        f=x*c+g*s;
        g = g*c-x*s;
        h=y*s;
        y *= c;

        for (jj=1;jj<=n;jj++)
        {
          x=v[jj][j];
          z=v[jj][i];
          v[jj][j]=x*c+z*s;
          v[jj][i]=z*c-x*s;
        }

        z=pythag(f,h);
        w[j]=z;

        if(z)
        {
          z=FONE/z;
          c=f*z;
          s=h*z;
        }

        f=c*g+s*y;
        x=c*y-s*g;

        for (jj=1;jj<=m;jj++)
        {
          y=a[jj][j];
          z=a[jj][i];
          a[jj][j]=y*c+z*s;
          a[jj][i]=z*c-y*s;
        }
      }
      rv1[l]=FZERO;
      rv1[k]=f;
      w[k]=x;
    }
  }

  return &V;
}
//------------------------------------------------------------------------------
dm_double dmMatrix_SVD_Cond( const dmMatrixRowCol& SV, dm_double factor )
{
  int i,N=SV.Length();
  dm_double wm = FZERO,wtmp,wi; // Singular values are expected to be positives
  for (i=1;i<=N;++i) {
     if( (wtmp = SV[i]) > wm)
        wm = wtmp;
  }

  // Use typical value
  if(factor > 1.0) {
     dmLOG_WARN( "Found matrix SV cond. factor > 1, changing to default value." );
     factor = 1.0e-6;
  }

  wm *= factor;
  for (i=1;i<=N;++i) {
    wi = SV[i];
    if( wi < wm )
        SV[i] = FZERO;
  }

  return wm;
}
//------------------------------------------------------------------------------
void dmMatrix_SVD_Solve( const dmMatrix& U,const dmMatrix& V,
                         const dmMatrixRowCol& SV,
                         const dmMatrixRowCol& _RowCol,
                         dmMatrixRowCol&  _Result,
                         dmMatrixRowCol&  _Tmp )
{
  dmMatrixBase* Ub  = const_cast<dmMatrix&>(U).Evaluate();
  dmMatrixBase* Vb  = const_cast<dmMatrix&>(V).Evaluate();

  if(_RowCol.Length() != Ub->NRows() ||
     _Result.Length() != Vb->NCols() ||
     _Tmp.Length()    != Vb->NCols() )
       MatrixError( "Invalid input vector size for SVD system resolution" );

  int n =_Result.Length();

  dmMatrixArray u(Ub);
  dmMatrixArray v(Vb);

  int j;
  dm_matrix_t s;

  dmMatrixCol UCol(Ub);

  for (j=1;j<=n;++j) {   // Comput UtB
    s=FZERO;
    if (SV[j])
    {
      Ub->GetCol(UCol,j);
      s += UCol * _RowCol;
      s /= SV[j];
    }
    _Tmp[j]=s;
  }

  dmMatrixRow VRow(Vb);

  for(j=1;j<=n;++j) {
     _Result[j] = VRow * _Tmp;
     VRow.Next();
  }
}
//------------------------------------------------------------------------------

END_MATRIX()
