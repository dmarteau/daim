#ifndef gdalColorTable_h
#define gdalColorTable_h
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
// File         : gdalColorTable.h
// Date         : 15 janv. 2010
// Author       : David Marteau
//--------------------------------------------------------

/* Header file */
#define CCI_COLORTABLE_PRIVATE_IID \
    { 0x20ec68de, 0xb94f, 0x4cb4, { 0xa6, 0x3f, 0x2f, 0xa1, 0x58, 0x5c, 0x65, 0xa6 } }
class gdalColorTable final : public cciIColorTable
{
friend class gdalSurface;
friend class cciGDALDriver;
public:
  CCI_DECLARE_STATIC_IID_ACCESSOR(CCI_COLORTABLE_PRIVATE_IID)
    
  CCI_DECL_ISUPPORTS
  CCI_DECL_ICOLORTABLE

  gdalColorTable( GDALColorTableH colorTable );

  static cci_result copyColorTable( cciIColorTable* src, GDALColorTableH &dst );
  
private:
  ~gdalColorTable();

protected:
  GDALColorTableH mColorTable;
};

#endif /* gdalColorTable_h */
