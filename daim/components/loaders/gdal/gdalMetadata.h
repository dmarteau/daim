#ifndef gdalMetadata_h
#define gdalMetadata_h
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
// File         : gdalMetadata.h
// Date         : 9 févr. 2010
// Author       : David Marteau
//--------------------------------------------------------

#include "cciISupports.h"

/* Header file */
#define CCI_METADATA_PRIVATE_IID \
    { 0xfc67a37e, 0x1f9c, 0x4c82, { 0x9a, 0x18, 0xb0, 0xa3, 0x49, 0x43, 0x09, 0x39 } }
class gdalMetadata : public cciISupports
{
public:
  CCI_DECLARE_STATIC_IID_ACCESSOR(CCI_METADATA_PRIVATE_IID);
  
  CCI_IMETHOD SetMetaData(const char * aDomain, char* * data)  = 0;
  
  CCI_IMETHOD_(char**) GetMetaData(const char * aDomain) = 0;
};

CCI_DEFINE_STATIC_IID_ACCESSOR(gdalMetadata,CCI_METADATA_PRIVATE_IID)

#endif /* gdalMetadata_h */
