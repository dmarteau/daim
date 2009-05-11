
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

#include "dmMatrixData.h"
#include "dmMatrixError.h"

#include <algorithm>
#include <math.h>

BEGIN_MATRIX()
//--------------------------------------------------------------
dm_matrix_t** __dm_alloc_matrix(  dm_int nrow, dm_int ncol )
{
   dm_int i;
   dm_matrix_t **m;

   m   = new dm_matrix_t*[nrow+1];
   m[1]= new dm_matrix_t[nrow*ncol+1];
   for(i=2;i<=nrow;i++) 
     m[i]=m[i-1]+ncol;

   return m;
}
//--------------------------------------------------------------
void __dm_destroy_matrix(dm_matrix_t **m)
{
  delete [] m[1];
  delete m;
}
//-----------------------------------------------------------------------------
void __dm_copy_matrix( dm_matrix_t** m1, dm_matrix_t** m2, dm_int rows, dm_int cols )
{
  for(int i=rows;i>0;--i) 
  {
    dm_matrix_t* from = m1[i];
    dm_matrix_t* to   = m2[i]; 
    std::copy(&from[1],&from[cols+1],&to[1]);
  }
}
//-----------------------------------------------------------------------------
// FFT algorithms
// See "Numerical Recipes in C" (C),  Chapter 12, p 525-531
//-----------------------------------------------------------------------------
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
#define REAL( x ) static_cast<dm_matrix_t>(x)

#define TWOPI 6.28318530717959
//-----------------------------------------------------------------------------
//  complex fft 1D
//-----------------------------------------------------------------------------
void __dm_complex_fft(dm_matrix_t* data, dm_uint nn, dm_int isign)
{
  dm_uint n,mmax,m,j,istep,i;
  
  dm_double wtemp,wr,wpr,wpi,wi,theta;
  dm_matrix_t  tempr,tempi;

  n=nn << 1;
  j=1;
  for (i=1;i<n;i+=2) {
    if (j > i) {
      SWAP(data[j],data[i]);
      SWAP(data[j+1],data[i+1]);
    }
    m=n >> 1;
    while (m >= 2 && j > m) {
      j -= m;
      m >>= 1;
    }
    j += m;
  }
  mmax=2;
  while (n > mmax) {
    istep=mmax << 1;
    theta=isign*(TWOPI/mmax);
    wtemp=sin(0.5*theta);
    wpr = -2.0*wtemp*wtemp;
    wpi=sin(theta);
    wr=1.0;
    wi=0.0;
    for (m=1;m<mmax;m+=2) {
      for (i=m;i<=n;i+=istep) {
        j=i+mmax;
        tempr=REAL(wr*data[j]-wi*data[j+1]);
        tempi=REAL(wr*data[j+1]+wi*data[j]);
        data[j]=data[i]-tempr;
        data[j+1]=data[i+1]-tempi;
        data[i] += tempr;
        data[i+1] += tempi;
      }
      wr=(wtemp=wr)*wpr-wi*wpi+wr;
      wi=wi*wpr+wtemp*wpi+wi;
    }
    mmax=istep;
  }  
}
//-----------------------------------------------------------------------------
//  real fft 1D
//  Remember : FT (real) value at 0   is stored at data[1]
//             FT (real) value at N/2 is stored at data[2]
//-----------------------------------------------------------------------------
void __dm_real_fft(dm_matrix_t* data, dm_uint n, dm_int isign)
{
  dm_uint i,i1,i2,i3,i4,np3;
  dm_matrix_t  c1=0.5,c2,h1r,h1i,h2r,h2i;
  dm_double wr,wi,wpr,wpi,wtemp,theta;

  theta=3.141592653589793/static_cast<dm_double>(n>>1);
  if (isign == 1)
  {
    c2 = -0.5;
    __dm_complex_fft(data,n>>1,1);
  }
  else
  {
    c2=0.5;
    theta = -theta;
  }
  
  wtemp=sin(0.5*theta);
  wpr = -2.0*wtemp*wtemp;
  wpi=sin(theta);
  wr=1.0+wpr;
  wi=wpi;
  np3=n+3;
  
  for (i=2;i<=(n>>2);i++) 
  {
    i4=1+(i3=np3-(i2=1+(i1=i+i-1)));
    h1r=c1*(data[i1]+data[i3]);
    h1i=c1*(data[i2]-data[i4]);
    h2r = -c2*(data[i2]+data[i4]);
    h2i=c2*(data[i1]-data[i3]);
    data[i1]=REAL(h1r+wr*h2r-wi*h2i);
    data[i2]=REAL(h1i+wr*h2i+wi*h2r);
    data[i3]=REAL(h1r-wr*h2r+wi*h2i);
    data[i4]=REAL(-h1i+wr*h2i+wi*h2r);
    wr=(wtemp=wr)*wpr-wi*wpi+wr;
    wi=wi*wpr+wtemp*wpi+wi;
  }
  if(isign == 1) 
  {
    data[1] = (h1r=data[1])+data[2];
    data[2] = h1r-data[2];
  }
  else
  {
    data[1]=c1*((h1r=data[1])+data[2]);
    data[2]=c1*(h1r-data[2]);
    __dm_complex_fft(data,n>>1,-1);
  }  
}
//-----------------------------------------------------------------------------
//  1D Spectrum from
//-----------------------------------------------------------------------------
void __dm_fft_spectrum_1d(dm_matrix_t* ft,dm_matrix_t* sp, dm_uint nn )
{
  // Compute the spectrum and store it in the 1 - N/2 values of sp
  dm_uint ns2 = nn >> 1;
  dm_matrix_t wi,wr;
    
  sp[1]     = ft[1] * ft[1];
  sp[ns2+1] = ft[2] * ft[2];
  for(dm_uint i=2;i<=ns2;++i) {
    wr = ft[2*i - 1];
    wi = ft[2*i];
    sp[i] = wr*wr + wi*wi;
  }
}
//-----------------------------------------------------------------------------
//  N dimensionnal fft
//-----------------------------------------------------------------------------
void __dm_compute_fft_n(dm_matrix_t* data, dm_uint nn[], dm_int ndim, dm_int isign)
{
  dm_int    idim;
  dm_uint   i1,i2,i3,i2rev,i3rev,ip1,ip2,ip3,ifp1,ifp2;
  dm_uint   ibit,k1,k2,n,nprev,nrem,ntot;
  dm_matrix_t  tempi,tempr;
  dm_double theta,wi,wpi,wpr,wr,wtemp;
  
  for(ntot=1,idim=1;idim<=ndim;idim++)
  	 ntot *= nn[idim];

  nprev=1;
  for(idim=ndim;idim>=1;idim--) 
  {
    n=nn[idim];
    nrem=ntot/(n*nprev);
    ip1=nprev << 1;
    ip2=ip1*n;
    ip3=ip2*nrem;
    i2rev=1;
    for(i2=1;i2<=ip2;i2+=ip1)
    {
      if(i2 < i2rev) {
        for(i1=i2;i1<=i2+ip1-2;i1+=2) {
          for(i3=i1;i3<=ip3;i3+=ip2) {
            i3rev=i2rev+i3-i2;
            SWAP(data[i3],data[i3rev]);
            SWAP(data[i3+1],data[i3rev+1]);
          }
        }
      }
      ibit=ip2 >> 1;
      while (ibit >= ip1 && i2rev > ibit) {
        i2rev -= ibit;
        ibit >>= 1;
      }
      i2rev += ibit;
    }
    ifp1=ip1;
    while(ifp1 < ip2) 
    {
      ifp2=ifp1 << 1;
      theta=isign*TWOPI/(ifp2/ip1);
      wtemp=sin(0.5*theta);
      wpr = -2.0*wtemp*wtemp;
      wpi=sin(theta);
      wr=1.0;
      wi=0.0;
      for(i3=1;i3<=ifp1;i3+=ip1) {
        for(i1=i3;i1<=i3+ip1-2;i1+=2) {
          for(i2=i1;i2<=ip3;i2+=ifp2) {
            k1=i2;
            k2=k1+ifp1;
            tempr=REAL(wr*data[k2]  -wi*data[k2+1]);
            tempi=REAL(wr*data[k2+1]+wi*data[k2]  );
            data[k2]=data[k1]-tempr;
            data[k2+1]=data[k1+1]-tempi;
            data[k1] += tempr;
            data[k1+1] += tempi;
          }
        }
        wr=(wtemp=wr)*wpr-wi*wpi+wr;
        wi=wi*wpr+wtemp*wpi+wi;
      }
      ifp1=ifp2;
    }
    nprev *= n;
  }
}
//-----------------------------------------------------------------------------
// 2D real fft
//-----------------------------------------------------------------------------
void __dm_compute_fft( dm_matrix_t ***data, dm_matrix_t **speq, 
                       dm_uint nn1, 
                       dm_uint nn2,
                       dm_uint nn3, 
                       dm_int  isign
                     )
{
  dm_uint   i1,i2,i3,j1,j2,j3,nn[4],ii3;
  dm_double theta,wi,wpi,wpr,wr,wtemp;
  dm_matrix_t  c1,c2,h1r,h1i,h2r,h2i;

  if(1+static_cast<dm_uint>(&data[nn1][nn2][nn3]-&data[1][1][1]) != nn1*nn2*nn3)
     MatrixError( "Problem with dimensions or contiguity of data array" );

  c1=0.5;
  c2 = REAL(-0.5*isign);
  theta=isign*(6.28318530717959/nn3);
  wtemp=sin(0.5*theta);
  wpr = -2.0*wtemp*wtemp;
  wpi=sin(theta);
  nn[1]=nn1;
  nn[2]=nn2;
  nn[3]=nn3 >> 1;

  if(isign == 1) 
  {
    __dm_compute_fft_n(&data[1][1][1]-1,nn,3,isign);
    for(i1=1;i1<=nn1;i1++) {
      for (i2=1,j2=0;i2<=nn2;i2++) {
        speq[i1][++j2]=data[i1][i2][1];
        speq[i1][++j2]=data[i1][i2][2];
      }
    }
  }

  for(i1=1;i1<=nn1;i1++) 
  {
    j1=(i1 != 1 ? nn1-i1+2 : 1);
    wr=1.0;
    wi=0.0;
    for(ii3=1,i3=1;i3<=(nn3>>2)+1;i3++,ii3+=2) {
      for(i2=1;i2<=nn2;i2++) {
        if(i3 == 1) {
          j2=(i2 != 1 ? ((nn2-i2)<<1)+3 : 1);
          h1r=c1*(data[i1][i2][1]+speq[j1][j2]);
          h1i=c1*(data[i1][i2][2]-speq[j1][j2+1]);
          h2i=c2*(data[i1][i2][1]-speq[j1][j2]);
          h2r= -c2*(data[i1][i2][2]+speq[j1][j2+1]);
          data[i1][i2][1]=h1r+h2r;
          data[i1][i2][2]=h1i+h2i;
          speq[j1][j2]=h1r-h2r;
          speq[j1][j2+1]=h2i-h1i;
        } else {
          j2=(i2 != 1 ? nn2-i2+2 : 1);
          j3=nn3+3-(i3<<1);
          h1r=c1*(data[i1][i2][ii3]+data[j1][j2][j3]);
          h1i=c1*(data[i1][i2][ii3+1]-data[j1][j2][j3+1]);
          h2i=c2*(data[i1][i2][ii3]-data[j1][j2][j3]);
          h2r= -c2*(data[i1][i2][ii3+1]+data[j1][j2][j3+1]);
          data[i1][i2][ii3]   = REAL(h1r+wr*h2r-wi*h2i);
          data[i1][i2][ii3+1] = REAL(h1i+wr*h2i+wi*h2r);
          data[j1][j2][j3]    = REAL(h1r-wr*h2r+wi*h2i);
          data[j1][j2][j3+1]  = REAL(-h1i+wr*h2i+wi*h2r);
        }
      }
      wr=(wtemp=wr)*wpr-wi*wpi+wr;
      wi=wi*wpr+wtemp*wpi+wi;
    }
  }

  if (isign == -1)
    __dm_compute_fft_n(&data[1][1][1]-1,nn,3,isign);
}
//-----------------------------------------------------------------------------
END_MATRIX()
