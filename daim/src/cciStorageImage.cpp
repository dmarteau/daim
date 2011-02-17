/* :::BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2011 David Marteau
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
 *  :::END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : cciStorageImage.cpp
// Date         : 15 févr. 2011
// Author       : David Marteau
//--------------------------------------------------------

#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"
#include "cciStorageImage.h"

/* Implementation file */
CCI_IMPL_ISUPPORTS_INHERITED1(cciStorageImage,cciScriptableImage,
                              cciIStorageImage)

cciStorageImage::cciStorageImage()
{
  /* member initializers and constructor code */
}

cciStorageImage::~cciStorageImage()
{
  /* destructor code */
}

/* void create (in unsigned long width, in unsigned long height, in EPixelFormat format); */
CCI_IMETHODIMP cciStorageImage::Create(dm_uint32 width, dm_uint32 height, EPixelFormat format)
{
  return Initialize(width,height,format);
}

/* [noscript] void shareImageData (in dmImageDataRef data); */
CCI_IMETHODIMP cciStorageImage::ShareImageData(dmImageData & data)
{
  return Initialize(data);
}
