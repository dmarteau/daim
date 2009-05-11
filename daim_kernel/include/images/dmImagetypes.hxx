#ifndef dmImagetypes_hxx
#define dmImagetypes_hxx

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
// File         : dmImagetypes.hxx
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

//------------------------------------------------------
// class dmIImage<_PixelFormat>
//------------------------------------------------------
template<EPixelFormat _PixelFormat>
dmIImage<_PixelFormat>::dmIImage( size_t w, size_t h ) 
: dmImage(w,h)
{
  dmASSERT( w > 0 );
  dmASSERT( h > 0 );
  size_t _stride = dm_align32( w * sizeof(value_type) );
  _LocalBuffer = daim::alloc_image_data<value_type>( _stride*h ); // allocate buffer
  _Gen.Construct(_LocalBuffer,w,h,_stride);
}
//-------------------------------------------------------
template<EPixelFormat _PixelFormat>
dmIImage<_PixelFormat>::dmIImage( const daim::image<pixel_type>& im ) 
: dmImage(im.width(),im.height())
 ,_LocalBuffer(NULL)
{
  size_t _stride = dm_align32( im.width() * sizeof(value_type) );
  _LocalBuffer = daim::alloc_image_data<value_type>( _stride * im.height() ); // allocate buffer
  _Gen.Construct(_LocalBuffer,im.width(),im.height(),_stride);
  _Gen.copy(im);
}
//-------------------------------------------------------
template<EPixelFormat _PixelFormat>
dmIImage<_PixelFormat>::dmIImage( const dmImageData& _Data )
: dmImage(_Data.Width,_Data.Height)
 ,_LocalBuffer(NULL)
{
  dmASSERT(_Data.PixelFormat == _PixelFormat );
  _Gen.Construct(
    static_cast<value_type*>(_Data.Scan0),
    _Data.Width,
    _Data.Height,
    _Data.Stride
  );
}
//-------------------------------------------------------
template<EPixelFormat _PixelFormat>
dmIImage<_PixelFormat>::~dmIImage() 
{
  if(_LocalBuffer)
    daim::free_image_data(_LocalBuffer);
}
//---------------------------------------------------
template<EPixelFormat _PixelFormat>
bool dmIImage<_PixelFormat>::GetImageData( dmImageData& _data ) const
{
  _data.PixelFormat = _PixelFormat;
  _data.Width       = _Gen.width();
  _data.Height      = _Gen.height();
  _data.Stride      = _Gen.stride();
  _data.Scan0       = reinterpret_cast<void*>(const_cast<value_type*>(_Gen[0]));
  return true;
}
//---------------------------------------------------
template<EPixelFormat _PixelFormat>
void dmIImage<_PixelFormat>::FillArea( const dmRect&  _aRect, bool _object )
{
  Fill( _aRect, _object? traits_type::object() : traits_type::zero() );
}
//---------------------------------------------------
template<EPixelFormat _PixelFormat>
void dmIImage<_PixelFormat>::FillArea( const dmRegion& _aRoi, bool _object )
{
  Fill( _aRoi, _object? traits_type::object() : traits_type::zero() );
}
//---------------------------------------------------
template<EPixelFormat _PixelFormat>
void dmIImage<_PixelFormat>::Fill(const dmRegion& _aRoi, value_type x )
{
  if( Rect() < _aRoi.Rectangle() ) { 
    dmRegion tmpRoi(_aRoi);
    tmpRoi.ClipToRect( Rect() );
    daim::fill(tmpRoi,_Gen,x);
  } else {
    daim::fill(_aRoi,_Gen,x);
  }
}
//------------------------------------------------------------
template<EPixelFormat _PixelFormat>
dmImageDescriptor* dmIImage<_PixelFormat>::TypeDescriptor() const
{
  return dmImageDescriptor::GetDescriptor(_PixelFormat);
}
//------------------------------------------------------------
template<EPixelFormat _PixelFormat>
dmIImage<_PixelFormat>*  dmIImage<_PixelFormat>::Cast( const dmImage* anImage )
{
  if(anImage && _PixelFormat==anImage->PixelFormat())
      return static_cast< dmIImage<_PixelFormat>* >(
         const_cast< dmImage* >( anImage )
      );

  return NULL;
}
//------------------------------------------------------------

#endif
