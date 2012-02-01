#ifndef dmImageBase_hxx
#define dmImageBase_hxx

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
// File         : dmImageBase.hxx
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#include <algorithm>
#include <assert.h>

//----------------------------------------------------------
// class gen_image
//----------------------------------------------------------
__dmKernel void dmAssertValidImageData( void*,size_t,size_t,int,size_t);

namespace daim {
//----------------------------------------------------------
template<class T>
image<T>& image<T>::construct(value_type* _scan0,
                              size_t _width,
                              size_t _height,
                              int    _stride)
{
  dmAssertValidImageData(_scan0,_width,_height,_stride,sizeof(value_type));  
  if(im_lines) { 
     delete im_lines; 
     im_lines = NULL;
  }
  im_lines    = new value_type*[_height+1]; // sentinelle
  im_stride   = _stride; 
  im_width    = _width;
  im_height   = _height;
  im_lines[0] = _scan0;
  for(size_t i=1;i<=_height;++i) 
    im_lines[i] = daim::offset_ptr(im_lines[i-1],_stride);
  return *this;
}
//----------------------------------------------------------
template<class T>
image<T>::~image()
{
  if(im_lines) 
   delete [] im_lines; 
}
//-----------------------------------------------------------
template<class T> 
image<T>& image<T>::fill( value_type value )
{
  line_type line = begin();
  line_type last = end();
  for(;line!=last;++line) {
    std::fill(*line,*line+width(),value);
  } 
  return *this;
}
//-----------------------------------------------------------
template<class T> 
image<T>& image<T>::fill( const dmRect& _rect, value_type _value )
{
  dmRect rd(_rect);
  if(dmClipRectangle( rd,rect()) ) {
    line_type line = begin(rd);
    for(int i=rd.Height();--i>=0;++line) 
      std::fill(*line+rd.Left(),*line+rd.Right()+1,_value); 
  }
  return *this;
}
//------------------------------------------------------------
// Beware of regions overlapping !!
template<class T> 
image<T>& image<T>::copy( const image<T>& _image, 
                          const dmRect&   _rect, 
                          const dmPoint&  _point) 
{
  dmRect rd( _point.x, _point.y, _rect.Width(), _rect.Height() );

  if( dmClipRectangle( rd,rect()) ) 
  {
    dmRect rs(_rect.Left(),_rect.Top(),rd.Width(),rd.Height());
    if( dmClipRectangle( rs, _image.rect() ) ) 
    {
      line_type dest       = begin(rd);
      const_line_type src  = _image.begin(rs);

      for(int i=rd.Height();--i>=0;++dest,++src)
        std::copy(*src+rs.Left(),*src+rs.Right()+1,*dest+rd.Left());

    } else 
      dmTHROW( EInvalidImageSize() );
  } else 
    dmTHROW( EInvalidImageSize() );
  return *this;
}
//-----------------------------------------------------------
// class slice_image
//-----------------------------------------------------------
template<class T> 
slice_image<T>::slice_image( image<T>& _image,
                             const dmRect& r)
{
  im_parent = _image;
  this->construct(_image[r.Top()]+r.Left(),r.Width(),r.Height(),im_parent.stride());
}
//-----------------------------------------------------------
// class cImage
//-----------------------------------------------------------
template<class T> cont_image<T>& 
cont_image<T>::reserve( size_t _width, size_t _height ) 
{
  typedef typename image_type::value_type value_type;

  if(_width!=this->width() || _height!=this->height() ) {
    if(im_buffer) { 
      delete [] im_buffer; 
      im_buffer = NULL;
    }
    size_t _stride = dm_align32( _width * sizeof(value_type) );
    im_buffer = alloc_image_data<value_type>(_stride*_height); // allocate buffer
    this->construct(im_buffer,_width,_height,_stride);
  }
  return *this;    
}
//-----------------------------------------------------------

} // namespace daim

#endif // dmImageBase_hxx
