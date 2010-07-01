
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

//------------------------------------------------------------------
//
// template class dmImageDescriptor<EPixelFormat> :
//
// descripteur g�n�rique d'image et d�finition des images associ�es
// chaque descripteur de type est d�fini comme un singleton 
//------------------------------------------------------------------

template<EPixelFormat _PixelFormat>
class dmIImageDescriptor : public dmImageDescriptor
{
  public:
    dmIImageDescriptor() {}
    virtual ~dmIImageDescriptor() {}

    virtual EPixelFormat PixelFormat() const;
    virtual dmLink<dmImage> CreateImage(size_t _width,size_t _height) const;
    virtual dmLink<dmImage> CreateImage( const dmImageData& )         const;

    typedef dmIImage<_PixelFormat> image_type;

    static dmIImageDescriptor<_PixelFormat> This;
};
//----------------------------------------------------------
template<EPixelFormat _PixelFormat>
dmIImageDescriptor<_PixelFormat> dmIImageDescriptor<_PixelFormat>::This;

//------------------------------------------------------
template<EPixelFormat _PixelFormat>
dmLink<dmImage> dmIImageDescriptor<_PixelFormat>::CreateImage(size_t w, size_t h) const
{
  dmASSERT(w>0);
  dmASSERT(h>0);
  return dmCreateImage<_PixelFormat>(w,h);
}
//------------------------------------------------------
template<EPixelFormat _PixelFormat>
dmLink<dmImage> dmIImageDescriptor<_PixelFormat>::CreateImage( const dmImageData& _Data ) const
{
  return dmCreateImage<_PixelFormat>(_Data);
}
//------------------------------------------------------
template<EPixelFormat _PixelFormat>
EPixelFormat dmIImageDescriptor<_PixelFormat>::PixelFormat() const
{
  return _PixelFormat;
}
//---------------------------------------------------
dmImageDescriptor* dmImageDescriptor::GetDescriptor( EPixelFormat _PixelFmt )
{
  switch(_PixelFmt) 
  {
    case dmPixelFormat8bppIndexed    : return &dmIImageDescriptor<dmPixelFormat8bppIndexed>   ::This;
    case dmPixelFormat24bppRGB       : return &dmIImageDescriptor<dmPixelFormat24bppRGB>      ::This;
    case dmPixelFormat32bppARGB      : return &dmIImageDescriptor<dmPixelFormat24bppRGB>      ::This;
    case dmPixelFormat16bppGrayScale : return &dmIImageDescriptor<dmPixelFormat16bppGrayScale>::This;
    case dmPixelFormat32bppGrayScale : return &dmIImageDescriptor<dmPixelFormat32bppGrayScale>::This;
    case dmPixelFormat32bppFloat     : return &dmIImageDescriptor<dmPixelFormat32bppFloat>    ::This;
    default: break;
  }
  return NULL;
}
//---------------------------------------------------
