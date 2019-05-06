#ifndef cciStorageImage_h
#define cciStorageImage_h
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
// File         : cciStorageImage.h
// Date         : 15 févr. 2011
// Author       : David Marteau
//--------------------------------------------------------

#include "cciScriptableImage.h"
#include "cciIStorageImage.h"
#include "cciIImageListContainer.h"

/**
 * @startuml
 * 
 * class  cciScriptableImage
 * interface cciIImageContainer
 * interface cciIStorageImage
 * interface cciIImage
 * 
 * cciIImageContainer  <|-- cciIImage
 * cciIImage           <|-- cciIStorageImage
 * cciIStorageImage    <|.. cciStorageImage
 * cciScriptableImage  <|-- cciStorageImage
 * cciIImage           <|.. cciScriptableImage
 *
 * @enduml
 */

class cciStorageImage final : public cciScriptableImage,
                              public cciIStorageImage
{
public:
  CCI_DECL_ISUPPORTS_INHERITED
  CCI_DECL_ISTORAGEIMAGE

  CCI_FORWARD_IIMAGE(cciScriptableImage::)
  CCI_FORWARD_IIMAGECONTAINER(cciScriptableImage::)
  
  cciStorageImage();

private:
  ~cciStorageImage();
};

#endif /* cciStorageImage_h */
