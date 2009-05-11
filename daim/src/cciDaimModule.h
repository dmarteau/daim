#ifndef cciDaimModule_h
#define cciDaimModule_h
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
// File         : cciDaimModule.h
// Date         : 16 janv. 2009
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
{ /* B4FF5CD2-AF46-4DD7-BEA4-7FEBF1BC933A */       \
  0xB3E2CD87,                                      \
  0x5CAC,                                          \
  0x4378,                                          \
  {0x8A, 0x0B, 0x1A, 0x36, 0xAC, 0x33, 0xC6, 0x78} \
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


#endif /* cciDaimModule_h */
