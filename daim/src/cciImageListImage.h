#ifndef cciImageListImage_h
#define cciImageListImage_h
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

/* $Id: cciImageListImage.h 15 févr. 2011 13:40:50 */

#include "cciScriptableImage.h"
#include "cciIImageListImage.h"
#include "cciIImageListContainer.h"

/**
 * @startuml
 * 
 * class  cciScriptableImage
 * interface cciIImageContainer
 * interface cciIImageListImage
 * interface cciIImage
 * 
 * cciIImageContainer <|-- cciIImage
 * cciIImage <|-- cciIImageListImage
 * cciIImageListImage  <|.. cciImageListImage
 * cciScriptableImage <|-- cciImageListImage
 * cciIImage <|.. cciScriptableImage
 *
 * @enduml
 */

class cciImageListImage : public cciScriptableImage,
                          public cciIImageListImage
{
public:
  CCI_DECL_ISUPPORTS_INHERITED
  CCI_DECL_IIMAGELISTIMAGE

  CCI_FORWARD_IIMAGE(cciScriptableImage::)
  CCI_FORWARD_IIMAGECONTAINER(cciScriptableImage::)

  cciImageListImage();

private:
  ~cciImageListImage();

protected:
  cci_Ptr<cciIImageListContainer> mImageList;
};

#endif /* cciImageListImage_h */
