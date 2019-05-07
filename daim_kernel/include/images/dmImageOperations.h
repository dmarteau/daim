#ifndef dmImageOperations_h
#define dmImageOperations_h

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
// File         : dmImageOperations.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

// Implementation of unary operations on image according
// to the available pixels types

template<class ImageType,class Operator>
inline bool dmCheckImageOperation( ImageType* _Image, Operator& op )  
{
  if(_Image) { 
    op( *_Image ); 
    return true; 
  }
  return false;
}
//-------------------------------------------------------------
template<class Operator>
inline bool dmImplementScalarOperation( Operator& op, const dmImage& _image ) 
{
  if(_image.TypeDescriptor()->IsPixelFormatScalar()) 
  {
    if(dmCheckImageOperation(dmIImage<dmPixelFormat8bppIndexed   >::Cast(&_image),op)) return true;
    if(dmCheckImageOperation(dmIImage<dmPixelFormat16bppGrayScale>::Cast(&_image),op)) return true;
    if(dmCheckImageOperation(dmIImage<dmPixelFormat32bppFloat    >::Cast(&_image),op)) return true;
  }

  return false;
}
//-------------------------------------------------------------
template<class Op>
inline bool dmImplementOperation( Op& op, const dmImage& _image ) 
{
  if(dmImplementScalarOperation(op,_image)) return true;
  if(dmCheckImageOperation(dmIImage<dmPixelFormat24bppRGB>::Cast(&_image),op)) return true;
  return false;
}

//-------------------------------------------------------------
#endif
