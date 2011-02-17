#ifndef cciImageContainerUtils_h
#define cciImageContainerUtils_h
/* :::BEGIN LICENSE BLOCK:::
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
 *  :::END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : cciImageContainerUtils.h
// Date         : 23 juil. 2010
// Author       : David Marteau
//--------------------------------------------------------

#include "cciIImageContainer.h"
#include "daim_kernel.h"

/* Header file */

#define CCI_IMAGE_PRIVATE_IID \
    { 0xf06ccafb, 0x80ca, 0x4f6f, { 0xa2, 0x02, 0x33, 0x46, 0x77, 0x2c, 0xa5, 0x75 } }

class cciImageWrapper : public cciIImageContainer
{
public:
  CCI_DECLARE_STATIC_IID_ACCESSOR(CCI_IMAGE_PRIVATE_IID)

  CCI_DECL_IIMAGECONTAINER
  
  dmLink<dmImage>& GetImage() { return mImage; }

protected:
  dmLink<dmImage> mImage;
};

CCI_DEFINE_STATIC_IID_ACCESSOR(cciImageWrapper, CCI_IMAGE_PRIVATE_IID)

inline 
CCI_IMETHODIMP_(dmImage *) cciImageWrapper::GetNative()
{
  return mImage.Pointer();
}


#endif /* cciImageContainerUtils_h */
