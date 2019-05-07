#ifndef dmMatrixFT_h
#define dmMatrixFT_h

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
// File         : dmMatrixFT.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
BEGIN_MATRIX()


class dmRectMatrix;
//--------------------------------------------------------
struct dmFT_Data
{
  dm_matrix_t**  Data[2]; // Contains frequencies ki=0..ni/2-1,kj=0..nj/2-1,nj/2..-1
  dm_matrix_t**  Speq;    // Contains frequencies ki=ni/2     ,kj=0..nj/2-1,nj/2..-1
  bool        Shared;  // real data are shared 
  int         Dim1;
  int         Dim2;
};
//--------------------------------------------------------
#define dmFT_DATA( d ) d.Data[1] // return dm_matrix_t**
#define dmFT_SPEQ( d ) d.Speq[1] // return dm_matrix_t* 

#define dmFT_COMPUTE_FORWARD  1
#define dmFT_COMPUTE_REVERSE -1
//-------------------------------------------------------------
// FT Initialisation/Destruction/Copy
// NOTE: If you set non null _data parameter to dmFT_Initialize()
//       then the correponding memory blocks will be used 
//       to store ft/real data
//-------------------------------------------------------------
void dmFT_Initialize  ( dmFT_Data& _FT );
void dmFT_Construct   ( dmFT_Data& _FT, int rows, int cols, dm_matrix_t** _data = NULL);
void dmFT_CopyFTData  ( dmFT_Data&, const dmFT_Data& _From );
void dmFT_Destroy     ( dmFT_Data& );
void dmFT_Clear       ( dmFT_Data& );
//-------------------------------------------------------------
// FT Operations
//-------------------------------------------------------------
void dmFT_Compute( dmFT_Data& );
void dmFT_Compute_Inverse( dmFT_Data&, bool _normalize = true );
//-------------------------------------------------------------
// WARNING : for convolution, ft1 OR ft2 must have real data
//           swapped  ( use dmFT_SwapRealBlocks first )
// if _convolve == true  => perform convolution of ft1 with ft2
// if _convolve == false => perform deconvolution of ft1 with ft2
// for dmFT_Real_Convolution and dmFT_Real_Convolution :
// real1 et real2 are replaced with their respective ft transforms   
//-------------------------------------------------------------
void dmFT_Convolution( const dmFT_Data& ft1, const dmFT_Data& ft2, dmFT_Data& result, int way );
void dmFT_Correlation( const dmFT_Data& ft1, const dmFT_Data& ft2, dmFT_Data& result );
//-------------------------------------------------------------
// dmFT_Real_Convolution will swap blocks of real2   before processing
// dmFT_Real_Correlation will swap blocks of _result after processing
// _result is given in direct space (NOT Fourier space)
//-------------------------------------------------------------
void dmFT_Real_Convolution( dmFT_Data& real1, dmFT_Data& real2, dmFT_Data& _result, int way );
//{
//  dmFT_SwapRealBlocks(real2);
//  dmFT_Compute(real1);
//  dmFT_Compute(real2);
//  dmFT_Convolution(real1,real2,rsult,way);
//  dmFT_Compute_Inverse(rsult);
//}
//-------------------------------------------------------------
void dmFT_Real_Correlation( dmFT_Data& real1, dmFT_Data& real2, dmFT_Data& _result );
//{
//  dmFT_Compute(real1);
//  dmFT_Compute(real2);
//  dmFT_Correlation(real1,real2,rsult);
//  dmFT_Compute_Inverse(rsult);
//  dmFT_SwapRealBlocks(rsult);
//}
//-------------------------------------------------------------
// dmFT_PowerSpectrum return power spectrum (with correct blocks swapping )
// WARNING : ft and spectrum must not overlap in memory
// NOTE    : (kx=0,ky=0) corresponds to (dim1/2,dim2/2) int the matrix array.
//------------------------------------------------------------- 
void dmFT_PowerSpectrum ( const dmFT_Data& ft, dmRectMatrix& spectrum );
void dmFT_PhaseSpectrum ( const dmFT_Data& ft, dmRectMatrix& spectrum );
void dmFT_SwapRealBlocks( dmFT_Data& );
void dmFT_WindowRealData( int width, dmFT_Data& _real );
//------------------------------------------------------------- 
void dmFT_SwapBlocks  ( dm_matrix_t** _Data, int rows, int cols );
void dmFT_WindowData  ( int width, dm_matrix_t** _Data, int rows, int cols );
void dmFT_CopyRealData( dmFT_Data&, dm_matrix_t**, int rows, int cols );
//-------------------------------------------------------------
// FT Arithmetics
//-------------------------------------------------------------
dmFT_Data& dmFT_Add( const dmFT_Data& lhs, const dmFT_Data& rhs, dmFT_Data& _result );
dmFT_Data& dmFT_Sub( const dmFT_Data& lhs, const dmFT_Data& rhs, dmFT_Data& _result );
dmFT_Data& dmFT_Mul( const dmFT_Data& lhs, const dmFT_Data& rhs, dmFT_Data& _result );
//------------------------------------------------------------- 
END_MATRIX()

#endif // dmMatrixFT_h

