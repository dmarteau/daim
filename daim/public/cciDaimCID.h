#ifndef cciDaimCID_h
#define cciDaimCID_h
/* :::BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2010 David Marteau
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
// File         : cciDaimCID.h
// Date         : 21 janv. 2010
// Author       : David Marteau
//--------------------------------------------------------

#define CCI_DAIMSERVICE_CID \
{ /* 1452B55F-7191-4205-8A0D-87A994E5A789 */       \
  0x1452B55F,                                      \
  0x7191,                                          \
  0x4205,                                          \
  {0x8A, 0x0D, 0x87, 0xA9, 0x94, 0xE5, 0xA7, 0x89} \
}

#define CCI_DAIMSERVICE_CONTRACTID "@daim.org/daim-service;1"
#define CCI_DAIMSERVICE_CLASSNAME  "Daim service"


#define CCI_IMAGELIST_CID \
{ /* 57596c21-2af2-48a2-85c2-02f99c35633a */       \
  0x57596c21,                                      \
  0x2af2,                                          \
  0x48a2,                                          \
  {0x85, 0xc2, 0x02, 0xf9, 0x9c, 0x35, 0x63, 0x3a} \
}

#define CCI_IMAGELIST_CONTRACTID "@daim.org/image/list;1"
#define CCI_IMAGELIST_CLASSNAME  "Daim image list"


#define CCI_SCRIPTABLEREGION_CID \
{ /* 92C06121-6649-40D4-B8D5-B00B1CB3240C */       \
  0x92C06121,                                      \
  0x6649,                                          \
  0x40D4,                                          \
  {0xB8, 0xD5, 0xB0, 0x0B, 0x1C, 0xB3, 0x24, 0x0C} \
}

#define CCI_SCRIPTABLEREGION_CONTRACTID "@daim.org/region;1"
#define CCI_SCRIPTABLEREGION_CLASSNAME  "Daim scriptable region"


#define CCI_IMAGESHELL_CID \
{ /* C0208D94-419C-4CC2-8F38-020C5A26D4AE */       \
  0xC0208D94,                                      \
  0x419C,                                          \
  0x4CC2,                                          \
  {0x8F, 0x38, 0x02, 0x0C, 0x5A, 0x26, 0xD4, 0xAE} \
}

#define CCI_IMAGESHELL_CONTRACTID "@daim.org/image-shell;1"
#define CCI_IMAGESHELL_CLASSNAME  "Image shell"


#define CCI_IMAGELISTIMAGE_CID \
{ /* 00ee8919-4916-48cf-a60a-6070da557334 */       \
  0x00ee8919,                                      \
  0x4916,                                          \
  0x48cf,                                          \
  {0xa6, 0x0a, 0x60, 0x70, 0xda, 0x55, 0x73, 0x34} \
}

#define CCI_IMAGELISTIMAGE_CONTRACTID "@daim.org/imagelist-image;1"
#define CCI_IMAGELISTIMAGE_CLASSNAME  "Image list image container"

#define CCI_STORAGEIMAGE_CID \
{ /* 041a7c4d-d904-43de-be2a-8af627074160 */       \
  0x041a7c4d,                                      \
  0xd904,                                          \
  0x43de,                                          \
  {0xbe, 0x2a, 0x8a, 0xf6, 0x27, 0x07, 0x41, 0x60} \
}

#define CCI_STORAGEIMAGE_CONTRACTID "@daim.org/storage-image;1"
#define CCI_STORAGEIMAGE_CLASSNAME  "Storage image container"

#endif /* cciDaimCID_h */
