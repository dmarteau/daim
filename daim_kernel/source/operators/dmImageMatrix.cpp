
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

#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "operators/dmImageMatrix.h"

#include "dmMatrixBase.h"
#include "templates/processing/dmImageMatrix.h"
//-------------------------------------------------------
struct __dm_impl_image_to_matrix
{
  dmRectMatrix& m;
  const dmRect& r;
  
  __dm_impl_image_to_matrix( dmRectMatrix& _m, const dmRect& _r ) 
  : m(_m),r(_r) {}

  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _img ) 
  {
    daim::image_to_matrix(_img.Gen(),m,r); 
  }
};
//-------------------------------------------------------
bool dmImageToMatrix( const dmImage& img, const dmRect& r, dmRectMatrix& m )
{
  __dm_impl_image_to_matrix _filter(m,r);
  return dmImplementScalarOperation(_filter,img); 
} 
//-------------------------------------------------------
struct __dm_impl_matrix_to_image
{
  const dmRectMatrix&  m;
  const dmPoint& p;
  dm_real rmin;
  dm_real rmax; 
  
  __dm_impl_matrix_to_image( const dmRectMatrix& _m, const dmPoint& _p, 
                             dm_real _rmin, dm_real _rmax) 
  : m(_m),p(_p),rmin(_rmin),rmax(_rmax) {}

  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _img ) 
  {
     typedef typename dmIImage<_PixelFormat>::traits_type  traits_type; 
     typedef typename traits_type::value_type   value_type; 
     typedef typename traits_type::integer_type integer_type; 

     daim::gap<value_type> range(
        daim::_get_range_value(rmin,traits_type(),integer_type()),
        daim::_get_range_value(rmax,traits_type(),integer_type())
     );

     if(range.diff()<=0)
       range = traits_type::bounds();
 
     daim::matrix_to_image(_img.Gen(),m,range,p);  
  }

  // Specialize for float
  void operator()( dmIImage<dmPixelFormat32bppFloat>& _img ) 
  {
     typedef dmIImage<dmPixelFormat32bppFloat>::traits_type  traits_type; 
     typedef traits_type::value_type value_type; 

     daim::gap<value_type> range(
        daim::_get_range_value(rmin,traits_type(),traits_type::integer_type()),
        daim::_get_range_value(rmax,traits_type(),traits_type::integer_type())
     );

     if(range.diff()<=0) {
       dmRect r = dmRect(0,0,m.NCols(),m.NRows());
       daim::tmp_image<dm_float> _tmp(m.GetData(),m.NCols(),m.NRows(),
                                      sizeof(dm_real)*m.NCols());
       
       daim::copy(r,p,_tmp,_img.Gen());
     } else
       daim::matrix_to_image(_img.Gen(),m,range,p);       
  }

};
//-------------------------------------------------------
bool dmMatrixToImage( dmImage& img, const dmPoint& p,  
                      dm_real minrange, dm_real maxrange,
                      const  dmRectMatrix& m )
{
  __dm_impl_matrix_to_image _filter(m,p,minrange,maxrange);
  return dmImplementScalarOperation(_filter,img); 
}
//-------------------------------------------------------

