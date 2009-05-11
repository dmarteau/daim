#ifndef dmFTFilters_h
#define dmFTFilters_h

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
// File         : dmFTFilters.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

//--------------------------------------------------------
// define template functions for applying operators
// on ft data
//--------------------------------------------------------

#include "dmComplex.h"

namespace daim {

template<class OP>
OP ft_binary_operation( const dmFT_Data& _src, dmFT_Data& _dest,const OP& op ) 
{
  long  i,j,n1,n2,k,fi;

  dm_complex_t z;

  n1 = _src.Dim1;
  n2 = _src.Dim2;
  long  n2s2 = n2 >> 1,n1s2 = n1 >> 1;

  dm_float** ft1   = dmFT_DATA(_src);
  dm_float** ft2   = dmFT_DATA(_dest);
  dm_float*  speq1 = dmFT_SPEQ(_src);
  dm_float*  speq2 = dmFT_SPEQ(_dest);
  dm_float* _row1,*_row2;

  for(i=1;i<=n1;++i) {
    fi = (i<=n1s2 ? i-1 : i-n1-1 );
    k  = 2*i;
    z = op(-n2s2,fi,dm_complex_t(speq1[k-1],speq1[k]));
    speq2[k-1] = z.real();
    speq2[k]   = z.imag();
    for(_row1 = ft1[i],_row2= ft2[i],j=1;j<=n2s2;++j) {
      k = 2*j;
      z = op(j-1,fi,dm_complex_t(_row1[k-1],_row1[k]));
      _row2[k-1] = z.real();
      _row2[k]   = z.imag();
    }
  }
  return op;
}
//--------------------------------------------------------
template<class OP>
OP ft_unary_operation( const dmFT_Data& _ft, const OP& op ) 
{
  long  i,j,n1,n2,fi;

  n1 = _ft.Dim1;
  n2 = _ft.Dim2;
  
  long  n2s2 = n2 >> 1,n1s2 = n1 >> 1;

  dm_float** ft   = dmFT_DATA(_ft);
  dm_float*  speq = dmFT_SPEQ(_ft);
  dm_float*  row;

  for(i=1;i<=n1;++i) {
    fi = (i<=n1s2 ? i-1 : i-n1-1 );
    op(fi,-n2s2,dm_complex_t(speq[2L*i-1],speq[2L*i]));
    for(row = ft[i],j=1;j<=n2s2;++j)
      op(fi,j-1,dm_complex_t(row[2L*j-1],row[2L*j]));
  }
  return op;
}
//--------------------------------------------------------
struct _ft_vt_operation 
{
  virtual ~_ft_vt_operation() {}
  virtual dm_complex_t apply( long kx, long ky, dm_complex_t _cpx ) const = 0;
};
//--------------------------------------------------------
void _ft_roi_operation(  dmFT_Data&, const dmRgnHandle&, const _ft_vt_operation& );
void _ff_roi_operation(  dmFT_Data&, const dmRect&     , const _ft_vt_operation& );
//--------------------------------------------------------
template<class OP>
struct ft_vt_operation : public _ft_vt_operation
{
  OP op;
  ft_vt_operation( const OP& _op ) : op(_op) {}
  virtual ~ft_vt_operation() {}
  virtual dm_complex_t apply( long kx, long ky, dm_complex_t _cpx ) const {
    return op(kx,ky,_cpx); 
  }
};
//--------------------------------------------------------
template<class Op> 
Op ft_roi_operation( dmFT_Data& ft, const dmRegion& rgn, const Op& op)
{
  ft_vt_operation<Op> _ft_op(op);
  if(rgn.IsRectRoi()) 
    _ft_roi_operation(ft,rgn.Rectangle(),_ft_op); else
	_ft_roi_operation(ft,rgn.Region()   ,_ft_op);
	
  return _ft_op.op;
}
//--------------------------------------------------------
// Binder to unary operator
//--------------------------------------------------------
template<class UnOp> 
struct _ft_binder_to_unary_op 
{
  UnOp op;
  _ft_binder_to_unary_op( const UnOp& _op) : op(_op) {}
  dm_complex_t operator()( long kx, long ky, dm_complex_t _cpx ) const {
   return op(_cpx);
  }
};
//--------------------------------------------------------
template<class UnOp> 
inline _ft_binder_to_unary_op<UnOp> 
ft_bind_unary_op( const UnOp& op ) { return _ft_binder_to_unary_op<UnOp>(op); }
//--------------------------------------------------------
// Usage example : apply a filter on a region which
// multiplies spectrum values by a given complex number
//--------------------------------------------------------
void ft_mul_filter( dmFT_Data& ft, const dmRegion& rgn, dm_complex_t z );
//{
//  using namespace std;
//  ft_roi_operation(
//    ft,rgn,
//    ft_bind_unary_op( bind2nd(multiplies<dm_complex_t>(),z) )
//  );
//}
//--------------------------------------------------------
// build a region wich satifies symetry constraints
// for real 2D Fourier transforms
//--------------------------------------------------------
void ft_build_region( dmFT_Data& ft, const dmRegion& rgn, dmRegion& dst, bool quad );
//--------------------------------------------------------
// filter spectrum using an apodisation window of size fwidth 
// on region border
//--------------------------------------------------------
void ft_filter_region( dmFT_Data& ft, const dmRegion& rgn, int fwidth, bool fcut );
//--------------------------------------------------------
}; // namespace daim

#endif // dmFTFilters_h
