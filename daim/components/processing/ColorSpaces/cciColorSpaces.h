#ifndef cciColorSpaces_h
#define cciColorSpaces_h
/* :::BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2008 David Marteau
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
// File         : cciColorspaces.h
// Date         : 11/2008
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

#include "cciIColorSpace.h"

#define CCI_COLORSPACE_RGB_CONTRACTID  CCI_COLORSPACE_CONTRACTID(RGB)
#define CCI_COLORSPACE_RGB_CID \
{ /* 9eb15001-3647-4636-8559-e0e1592316df */       \
  0x9eb15001,                                      \
  0x3647,                                          \
  0x4636,                                          \
  {0x85, 0x59, 0xe0, 0xe1, 0x59, 0x23, 0x16, 0xdf} \
}

#define CCI_COLORSPACE_HSV_CONTRACTID  CCI_COLORSPACE_CONTRACTID(HSV)
#define CCI_COLORSPACE_HSV_CID \
{ /* c33ae4b3-b5a2-49d1-b364-6351b32fbff0 */       \
  0xc33ae4b3,                                      \
  0xb5a2,                                          \
  0x49d1,                                          \
  {0xb3, 0x64, 0x63, 0x51, 0xb3, 0x2f, 0xbf, 0xf0} \
}

#define CCI_COLORSPACE_HLS_CONTRACTID  CCI_COLORSPACE_CONTRACTID(HLS)
#define CCI_COLORSPACE_HLS_CID \
{ /* 2fdd7562-1d0f-48f8-82aa-bbbc9ef50009 */       \
  0x2fdd7562,                                      \
  0x1d0f,                                          \
  0x48f8,                                          \
  {0x82, 0xaa, 0xbb, 0xbc, 0x9e, 0xf5, 0x00, 0x09} \
}

#define CCI_COLORSPACE_YUV_CONTRACTID  CCI_COLORSPACE_CONTRACTID(YUV)
#define CCI_COLORSPACE_YUV_CID \
{ /* 7e3263d4-0de3-4d49-a5fd-0b25232dd06f */       \
  0x7e3263d4,                                      \
  0x0de3,                                          \
  0x4d49,                                          \
  {0xa5, 0xfd, 0x0b, 0x25, 0x23, 0x2d, 0xd0, 0x6f} \
}

#define CCI_COLORSPACE_YIQ_CONTRACTID  CCI_COLORSPACE_CONTRACTID(YIQ)
#define CCI_COLORSPACE_YIQ_CID \
{ /* defb001d-246a-442d-86a3-c2ae8d84dc37 */       \
  0xdefb001d,                                      \
  0x246a,                                          \
  0x442d,                                          \
  {0x86, 0xa3, 0xc2, 0xae, 0x8d, 0x84, 0xdc, 0x37} \
}

#define CCI_COLORSPACE_OCP_CONTRACTID  CCI_COLORSPACE_CONTRACTID(OCP)
#define CCI_COLORSPACE_OCP_CID \
{ /* 31e4f649-8424-424d-813a-808c8a039cbf */       \
  0x31e4f649,                                      \
  0x8424,                                          \
  0x424d,                                          \
  {0x81, 0x3a, 0x80, 0x8c, 0x8a, 0x03, 0x9c, 0xbf} \
}

#define CCI_COLORSPACE_GSC_CONTRACTID  CCI_COLORSPACE_CONTRACTID(GSC)
#define CCI_COLORSPACE_GSC_CID \
{ /* 705cc897-23bc-4de0-97b0-1bf7492c8f67 */       \
  0x705cc897,                                      \
  0x23bc,                                          \
  0x4de0,                                          \
  {0x97, 0xb0, 0x1b, 0xf7, 0x49, 0x2c, 0x8f, 0x67} \
}

#define CCI_COLORSPACE_GSL_CONTRACTID  CCI_COLORSPACE_CONTRACTID(GSL)
#define CCI_COLORSPACE_GSL_CID \
{ /* 0693514e-999e-4428-92ad-6c70e4c33e7c */       \
  0x0693514e,                                      \
  0x999e,                                          \
  0x4428,                                          \
  {0x92, 0xad, 0x6c, 0x70, 0xe4, 0xc3, 0x3e, 0x7c} \
}


#define CCI_COLORSPACE_HSI_CONTRACTID  CCI_COLORSPACE_CONTRACTID(HSI)
#define CCI_COLORSPACE_HSI_CID \
{ /* C21B43DD-3C5A-4B18-982D-7E865CA02650 */       \
  0xC21B43DD,                                      \
  0x3C5A,                                          \
  0x4B18,                                          \
  {0x98, 0x2D, 0x7E, 0x86, 0x5C, 0xA0, 0x26, 0x50} \
}


#endif // cciColorSpaces_h
