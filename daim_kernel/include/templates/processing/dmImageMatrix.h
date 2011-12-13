#ifndef dmImageMatrix_h
#define dmImageMatrix_h

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
// File         : dmImageMatrix.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

namespace daim {

//-------------------------------------------------------
template<class T>
void image_to_matrix( const image<T>& _src, dmRectMatrix& _Matrix, 
                      const dmRect& r )
{
  _Matrix.Resize(r.Height(),r.Width());
  _Matrix.Set(0);
  tmp_image<float> _tmp(_Matrix.GetData(),_Matrix.NCols(),_Matrix.NRows(),
                     sizeof(float)*_Matrix.NCols() );

  daim::copy(r,dmPoint(0,0),_src,_tmp);
}
//-------------------------------------------------------
template<class T>
void matrix_to_image( image<T>& _dst, const dmRectMatrix& _Matrix, 
                      const gap<typename pixel_traits<T>::value_type>& _range,
                      const dmPoint& p = dmPoint(0,0) )
{
  dmRect r = dmRect(0,0,_Matrix.NCols(),_Matrix.NRows());
  tmp_image<float> _tmp(_Matrix.GetData(),_Matrix.NCols(),_Matrix.NRows(),
                           sizeof(float)*_Matrix.NCols());

  scale_convert_scalar_to_scalar(
       minmax(_tmp.rect(),_tmp),
      _range,_tmp,_dst,_tmp.rect(),p );
}
//-------------------------------------------------------

}; // namespace daim

#endif // dmImageMatrix_h
