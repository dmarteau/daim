#ifndef gendrv_h
#define gendrv_h
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
// File         : gdalmindrv.h
// Date         : 3 déc. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "gdal.h"

class cciISurfaceDriver;
class cciISurface;
class cciIColorTable;

cci_result CCI_NewGDALDriver( GDALDriverH aDriver  , cciISurfaceDriver* *result );

cci_result CCI_NewGDALSurface(GDALDatasetH aDataset, dm_uint32 ioFlags, GDALDriverH tmpDriver,
                              cciISurface** result);

cci_result CCI_NewGDALSurface(GDALDriverH aDriver,const char * location,
                              dm_uint32 width, dm_uint32 height,
                              EPixelFormat format, dm_bool hasAlpha,
                              const char *options,
                              cciISurface** result);

cci_result CCI_NewGDALColorTable(GDALColorTableH aColorTable, cciIColorTable** result);


inline GDALDataType resolveDataType( EPixelFormat format )
{
  switch(format)
  {
    case dmPixelFormat8bppIndexed     :
    case dmPixelFormat24bppRGB        :
    case dmPixelFormat32bppARGB       : return GDT_Byte;
    case dmPixelFormat16bppGrayScale  : return GDT_UInt16;
    case dmPixelFormat32bppGrayScale  : return GDT_Int32;
    case dmPixelFormat32bppFloat      : return GDT_Float32;
    default:
      return GDT_Unknown;
  }
}

inline EPixelFormat resolveFormat( GDALDataType dataType )
{
  switch(dataType)
  {
    case GDT_Byte   : return dmPixelFormat8bppIndexed;
    case GDT_UInt16 : return dmPixelFormat16bppGrayScale;
    case GDT_Int32  : return dmPixelFormat32bppGrayScale;
    case GDT_Float32:
    case GDT_Float64: return dmPixelFormat32bppFloat;
    default:
      return dmPixelFormatUndefined;
  }
}

#endif /* gendrv_h */
