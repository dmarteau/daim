
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

#include "dmMatrixArray.h"
#include "dmMatrixFT.h"
#include "dmComplex.h"

#include <vector>
#include <algorithm>
#include <functional>

BEGIN_MATRIX()

//-----------------------------------------------------------------------------
dm_matrix_t** __dm_alloc_matrix(  dm_int nrow, dm_int ncol );
void          __dm_destroy_matrix(dm_matrix_t**);
void          __dm_copy_matrix( dm_matrix_t** m1, dm_matrix_t** m2, dm_int rows, dm_int cols );
void          __dm_compute_fft( dm_matrix_t***,dm_matrix_t **,dm_uint,dm_uint,dm_uint,dm_int );
//-----------------------------------------------------------------------------
void dmFT_Initialize( dmFT_Data& _FT )
{
  dmDEBUG_ASSERT( _FT.Data[1] == NULL );

  _FT.Data[1] = NULL;
  _FT.Speq    = NULL;
  _FT.Shared  = false;
  _FT.Dim1    = 0;
  _FT.Dim2    = 0;
} 
//-----------------------------------------------------------------------------
void dmFT_Construct( dmFT_Data& _FT, int rows, int cols, dm_matrix_t** m ) 
{
  dmDEBUG_ASSERT( rows > 0 );
  dmDEBUG_ASSERT( cols > 0 );

  bool sharedData = (m!=NULL);

  if(rows!=_FT.Dim1 || cols!=_FT.Dim2 || sharedData!=_FT.Shared )
  { 
    dmFT_Destroy(_FT);

    _FT.Shared  = sharedData;
    _FT.Dim1    = rows;
    _FT.Dim2    = cols;
    _FT.Data[0] = NULL;

    if(!_FT.Shared)
      _FT.Data[1] = __dm_alloc_matrix(_FT.Dim1,_FT.Dim2);

    _FT.Speq = __dm_alloc_matrix(1,2*_FT.Dim1);
  } 

  if(_FT.Shared) 
     _FT.Data[1] = m;
}
//-----------------------------------------------------------------------------
// Initialize from real data
//-----------------------------------------------------------------------------
void dmFT_CopyRealData( dmFT_Data& _FT, dm_matrix_t** m, int rows, int cols )
{
  dmDEBUG_ASSERT( _FT.Data[1] != NULL );

  dmDEBUG_ASSERT( m != NULL );
  dmDEBUG_ASSERT( rows == _FT.Dim1 );
  dmDEBUG_ASSERT( cols == _FT.Dim2 );

  __dm_copy_matrix( m,dmFT_DATA(_FT),_FT.Dim1,_FT.Dim2); 
}
//-----------------------------------------------------------------------------
// Initialize from ft data
//-----------------------------------------------------------------------------
void dmFT_CopyFTData( dmFT_Data& _FT, const dmFT_Data& _From ) 
{
  dmDEBUG_ASSERT( _From.Data[1] != NULL );
  dmFT_Construct( _FT, _From.Dim1, _From.Dim2, NULL );

  __dm_copy_matrix( dmFT_DATA(_From),dmFT_DATA(_FT),_FT.Dim1,_FT.Dim2 );
  __dm_copy_matrix( _From.Speq,_FT.Speq,1,2*_FT.Dim1 );
}
//-----------------------------------------------------------------------------
void dmFT_Destroy( dmFT_Data& _FT )
{
  if(!_FT.Shared && _FT.Data[1] != NULL) {
    __dm_destroy_matrix(_FT.Data[1]);
    __dm_destroy_matrix(_FT.Speq);
  }

  _FT.Data[1] = NULL;
  _FT.Speq    = NULL;
  _FT.Shared  = false;
  _FT.Dim1    = 0;
  _FT.Dim2    = 0;
}
//-------------------------------------------------------------
void dmFT_Clear(  dmFT_Data& _FT )
{
  if(_FT.Data[1] != NULL) 
  {
    dm_matrix_t** rows = _FT.Data[1];
    dm_matrix_t*  speq = dmFT_SPEQ(_FT);
    size_t    size = _FT.Dim2;

    for(int i=_FT.Dim1;i>0;--i)
      std::fill(rows[i],rows[i]+size,0.0f);
    
    std::fill(speq,speq+2*_FT.Dim1,0.0f);
  }
}
//-------------------------------------------------------------
// Operations
//-------------------------------------------------------------
void dmFT_Compute( dmFT_Data& d )
{
  __dm_compute_fft(d.Data,d.Speq,1,d.Dim1,d.Dim2,dmFT_COMPUTE_FORWARD);
}
//-------------------------------------------------------------
void dmFT_Compute_Inverse( dmFT_Data& d, bool _normalize )
{
  long i,j,n1,n2;
  n1 = d.Dim1;
  n2 = d.Dim2;

  if(_normalize)
  {
    dm_matrix_t** data = dmFT_DATA(d);
    dm_matrix_t*  speq = dmFT_SPEQ(d);
    dm_matrix_t*  row;
    dm_matrix_t d   = static_cast<dm_matrix_t>(n1 * n2);
    dm_matrix_t ds2 = d/2.0f;
    for(i=1;i<=n1;++i) {
      speq[2*i-1] /= d;
      speq[2*i]   /= d;
      for(row = data[i],j=1;j<=n2;j++)
        row[j] /= ds2; 
    }
  }
  __dm_compute_fft(d.Data,d.Speq,1,n1,n2,dmFT_COMPUTE_REVERSE);
}
//-------------------------------------------------------------
void dmFT_Convolution( const dmFT_Data& _ft1, 
                       const dmFT_Data& _ft2,
                       dmFT_Data& _rslt, int way 
                      )
{
  long    i,j,n1,n2,k;
  dm_matrix_t wi1,wr1,wi2,wr2,mag2;

  n1 = _ft1.Dim1;
  n2 = _ft2.Dim2;

  long    ns2 = n2 >> 1;
  dm_matrix_t norm = static_cast<dm_matrix_t>(ns2 * (n1 >> 1));

  dm_matrix_t** ft1 = dmFT_DATA(_ft1);
  dm_matrix_t** ft2 = dmFT_DATA(_ft2);
  dm_matrix_t** ft0 = dmFT_DATA(_rslt);

  dm_matrix_t* speq1 = dmFT_SPEQ(_ft1);
  dm_matrix_t* speq2 = dmFT_SPEQ(_ft1);
  dm_matrix_t* speq0 = dmFT_SPEQ(_rslt);

  dm_matrix_t *a1,*a2,*a0;

  for(i=1;i<=n1;++i)
  {
    k = 2*i;
    wr1 = speq1[k-1]; 	// real part
    wi1 = speq1[k];		// imaginary part
    wr2 = speq2[k-1];
    wi2 = speq2[k];

    if(way==dmFT_COMPUTE_REVERSE) {  // deconvolution
      mag2 = wr2*wr2 + wi2*wi2;
      speq0[k-1] = (wr1*wr2+wi1*wi2)/(mag2*norm) ;	
      speq0[k]   = (wi1*wr2-wi2*wr1)/(mag2*norm) ;	
    } else {		   // convolution
      speq0[k-1] = (wr1*wr2-wi1*wi2)/(norm) ;	
      speq0[k]   = (wi1*wr2+wi2*wr1)/(norm) ;	
    }
		
    a1 = ft1[i]; a2 = ft2[i]; a0 = ft0[i];
    for(j=1;j<=ns2;++j) 
    {
      k = 2*j;
      wr1 = a1[k-1];
      wi1 = a1[k];
      wr2 = a2[k-1];
      wi2 = a2[k];

      if(way==dmFT_COMPUTE_REVERSE) {           // Deconvolution
        mag2 = wr2*wr2 + wi2*wi2;
        a0[k-1] = (wr1*wr2+wi1*wi2)/(mag2*norm) ;// real part
        a0[k]   = (wi1*wr2-wi2*wr1)/(mag2*norm) ;// imaginary part
      } else {	                                 // Convolution
        a0[k-1] = (wr1*wr2-wi1*wi2)/(norm) ;     // real part
        a0[k]   = (wi1*wr2+wi2*wr1)/(norm) ;     // imaginary part				
      }
    }
  }
}
//-------------------------------------------------------------
void dmFT_Correlation( const dmFT_Data& _ft1,
                       const dmFT_Data& _ft2, 
                       dmFT_Data& _rslt )
{
  long    i,j,n1,n2,k;
  dm_matrix_t wi1,wr1,wi2,wr2;

  n1 = _ft1.Dim1;
  n2 = _ft2.Dim2;

  long    ns2 = n2 >> 1;
  dm_matrix_t norm = static_cast<dm_matrix_t>(ns2 * (n1 >> 1));

  dm_matrix_t** ft1 = dmFT_DATA(_ft1);
  dm_matrix_t** ft2 = dmFT_DATA(_ft2);
  dm_matrix_t** ft0 = dmFT_DATA(_rslt);

  dm_matrix_t* speq1 = dmFT_SPEQ(_ft1);
  dm_matrix_t* speq2 = dmFT_SPEQ(_ft1);
  dm_matrix_t* speq0 = dmFT_SPEQ(_rslt);

  dm_matrix_t *a1,*a2,*a0;

  for(i=1;i<=n1;++i) 
  {
    k = 2*i;
    wr1 = speq1[k-1];  // real part
    wi1 = speq1[k];	   // imaginary part
    wr2 = speq2[k-1];
    wi2 = speq2[k];
		
    speq0[k-1] = (wr1*wr2+wi1*wi2)/norm ;	
    speq0[k]   = (wi1*wr2-wi2*wr1)/norm ;	

    a1 = ft1[i]; a2 = ft2[i]; a0 = ft0[i];
    for(j=1;j<=ns2;++j) 
    {
      k   = 2*j;
      wr1 = a1[k-1];
      wi1 = a1[k];
      wr2 = a2[k-1];
      wi2 = a2[k];

      a0[k-1] = (wr1*wr2+wi1*wi2)/norm ; // real part
      a0[k]   = (wi1*wr2-wi2*wr1)/norm ; // imaginary part
    }
  }
}
//-----------------------------------------------------------------------------
void dmFT_SwapBlocks( dm_matrix_t** data, int rows, int cols )
{
  long i,j,n1,n2;
	
  n1 = rows >> 1;
  n2 = cols >> 1;
  
  dm_matrix_t p,*a1,*a2;
  
  for(i=1;i<=n1;i++) {
    a1 = data[i];
    a2 = data[i+n1];
    for(j=1;j<=n2;j++) {
      p = a2[j+n2]; a2[j+n2] = a1[j]   ; a1[j]    = p; // NW and SE quadrant  
      p = a2[j]   ;	a2[j]    = a1[j+n2]; a1[j+n2] = p; // NE and SW quadrant
    }
  }  
}
//-----------------------------------------------------------------------------
void dmFT_SwapRealBlocks( dmFT_Data& _real )
{
  dmFT_SwapBlocks( dmFT_DATA(_real),_real.Dim1,_real.Dim2 );
}
//-----------------------------------------------------------------------------
void dmFT_WindowData( int width, dm_matrix_t** data, int rows, int cols )
{
  long i,j,n1,n2;
  n1 = rows;
  n2 = cols;
  
  long n = dm_min(n1,n2) >> 1;

  if(width < 1) return;
  if(width > n) width = n;

  dm_matrix_t *a,*b,z,zi; 
  
  z = static_cast<dm_matrix_t>(2.0*acos(-1.0)/(width-1));

  std::vector<dm_matrix_t> vz(width);
  for(i=0;i<width;++i) 
    vz[i] = static_cast<dm_matrix_t>( 0.5 * ( 1.0 - cos( z * i )) ); 
    
  n = n2>>1;

  for(i=0;i<width;++i){
    a = data[i+1];
    b = data[n1-i];
    zi = vz[i];
    for(j=0;j<n;++j) {
      z = zi;
      if(j<width) z *= vz[j];
      a[j+1]  *= z;
      a[n2-j] *= z;
    }
  }
}
//-----------------------------------------------------------------------------
void dmFT_WindowRealData( int width, dmFT_Data& _real )
{
  dmFT_WindowData( width, dmFT_DATA(_real), _real.Dim1, _real.Dim2 );
}
//-------------------------------------------------------------
void dmFT_Real_Convolution( dmFT_Data& real1, 
                            dmFT_Data& real2, 
                            dmFT_Data& rsult, 
                            int way )
{
  dmFT_SwapRealBlocks(real2);
  dmFT_Compute(real1);
  dmFT_Compute(real2);
  dmFT_Convolution(real1,real2,rsult,way);
  dmFT_Compute_Inverse(rsult);
}
//-------------------------------------------------------------
void dmFT_Real_Correlation( dmFT_Data& real1, 
                            dmFT_Data& real2, 
                            dmFT_Data& rsult )
{
  dmFT_Compute(real1);
  dmFT_Compute(real2);
  dmFT_Correlation(real1,real2,rsult);
  dmFT_Compute_Inverse(rsult);
  dmFT_SwapRealBlocks(rsult);
}
//-----------------------------------------------------------------------------
void dmFT_PowerSpectrum( const dmFT_Data& _ft, dmRectMatrix& _Spectrum )
{
  long    i,j,n1,n2;
  dm_matrix_t wi,wr;

  n1 = _ft.Dim1;
  n2 = _ft.Dim2;

  _Spectrum.Resize(n1,n2);
  dmMatrixArray spectr(_Spectrum);

  long  ns2 = n2 >> 1;
  dm_matrix_t** ft   = dmFT_DATA(_ft);
  dm_matrix_t*  speq = dmFT_SPEQ(_ft);
  dm_matrix_t  *a,*b,*_row;

  // Compute spectrum
  {
   	for(i=1;i<=n1;++i) {
      a  = spectr[i];
      wr = speq[2*i-1]; 
      wi = speq[2*i];	
      a[ns2+1] = wr*wr + wi*wi;
      for(_row = ft[i],j=1;j<=ns2;++j) {
        wr   = _row[2*j-1];
        wi   = _row[2*j];
        a[j] = wr*wr + wi*wi; 
      }
    }   
  }

  // Restore frequencies order
  a = spectr[1];
  for(j=2;j<=ns2;++j) { a[n2+2-j] = a[j]; }
  for(i=2;i<=n1 ;++i) {
    a = spectr[i];
    b = spectr[n1+2-i];
    for(j=2;j<=ns2;++j) { b[n2+2-j] = a[j]; }
  }

  dmFT_SwapBlocks(spectr.m(),spectr.NRows(),spectr.NCols());
}
//-----------------------------------------------------------------------------
void dmFT_PhaseSpectrum( const dmFT_Data& _ft, dmRectMatrix& _Spectrum )
{
  long i,j,n1,n2;
  dm_matrix_t wi,wr;

  n1 = _ft.Dim1;
  n2 = _ft.Dim2;

  _Spectrum.Resize(n1,n2);
  dmMatrixArray spectr(_Spectrum);

  long ns2 = n2 >> 1;
  dm_matrix_t** ft  = dmFT_DATA(_ft);
  dm_matrix_t*  speq = dmFT_SPEQ(_ft);
  dm_matrix_t  *a,*b,*_row;

  for(i=1;i<=n1;++i) {
    a  = spectr[i];
    wr = speq[2L*i-1]; 
    wi = speq[2L*i];
    a[ns2+1] = static_cast<dm_matrix_t>(atan2(wi,wr));
    for(_row = ft[i],j=1;j<=ns2;++j) {
       wr = _row[2L*j-1];
       wi = _row[2L*j];
       a[j] = static_cast<dm_matrix_t>(atan2(wi,wr));
    }
  }

  // Restore frequencies order
  a = spectr[1];
  for(j=2;j<=ns2;++j) { a[n2+2-j] = a[j]; }
  for(i=2;i<=n1;++i) {
    a = spectr[i];
    b = spectr[n1+2-i];
    for(j=2;j<=ns2;++j) { b[n2+2-j] = a[j]; }
  }

  dmFT_SwapBlocks(spectr.m(),spectr.NRows(),spectr.NCols());
}
//-----------------------------------------------------------------------------
// Arithmetics
//-----------------------------------------------------------------------------
template<class OP>
static inline void __dm_apply_ft_op( const dmFT_Data& lhs, 
                                     const dmFT_Data& rhs, 
                                     dmFT_Data& rsl , OP op )
{
  long  i,j,n1,n2,k;
  dm_complex_t r;

  n1 = lhs.Dim1;
  n2 = lhs.Dim2;

  long  ns2 = n2 >> 1;

  dm_matrix_t** ft1 = dmFT_DATA(lhs);
  dm_matrix_t** ft2 = dmFT_DATA(rhs);
  dm_matrix_t** ft0 = dmFT_DATA(rsl);

  dm_matrix_t* speq1 = dmFT_SPEQ(lhs);
  dm_matrix_t* speq2 = dmFT_SPEQ(rhs);
  dm_matrix_t* speq0 = dmFT_SPEQ(rsl);

  dm_matrix_t *a1,*a2,*a0;

  for(i=1;i<=n1;++i) 
  {
    k = 2*i;		
    r = op( dm_complex_t(speq1[k-1],speq1[k]),
            dm_complex_t(speq2[k-1],speq2[k]));

    speq0[k-1] = r.real();	
    speq0[k]   = r.imag();

    a1 = ft1[i]; a2 = ft2[i]; a0 = ft0[i];
    for(j=1;j<=ns2;++j) 
    {
      k = 2*j;
      r = op( dm_complex_t(a1[k-1],a1[k]),
              dm_complex_t(a2[k-1],a2[k]));

      a0[k-1] = r.real();	
      a0[k]   = r.imag();
    }
  }
}
//-----------------------------------------------------------------------------
dmFT_Data& dmFT_Mul( const dmFT_Data& lhs, 
                     const dmFT_Data& rhs, 
                     dmFT_Data& result )
{
  __dm_apply_ft_op(lhs,rhs,result,std::multiplies<dm_complex_t>());
  return result;
}
//-----------------------------------------------------------------------------
dmFT_Data& dmFT_Add( const dmFT_Data& lhs, 
                     const dmFT_Data& rhs, 
                     dmFT_Data& result )
{
  __dm_apply_ft_op(lhs,rhs,result,std::plus<dm_complex_t>());
  return result;
}
//-----------------------------------------------------------------------------
dmFT_Data& dmFT_Sub( const dmFT_Data& lhs, 
                     const dmFT_Data& rhs, 
                     dmFT_Data& result )
{
  __dm_apply_ft_op(lhs,rhs,result,std::minus<dm_complex_t>());
  return result;
}
//-----------------------------------------------------------------------------
END_MATRIX()
