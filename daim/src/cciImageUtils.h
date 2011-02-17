#ifndef cciImageUtils_h
#define cciImageUtils_h
/* ::: BEGIN LICENSE BLOCK:::
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
 *  ::: END LICENSE BLOCK::: */

#include "cciIImageList.h"
#include "cciIColorSpace.h"
#include "cciIFilterContext.h"
#include "cciIImage.h"
#include "cciIMatrix.h"
#include "cciILoaderService.h"
#include "cciISurfaceDriver.h"
#include "cciDaimglue.h"


DAIM_GLUE_EXPORT
cci_result CCI_NewImageList( cciIImageList** _result );

DAIM_GLUE_EXPORT
cci_result CCI_NewColorSpace( const char* colorSpace, cciIColorSpace** _result );

DAIM_GLUE_EXPORT
cci_result CCI_NewFilterContext( cciIFilterContext** _result);

DAIM_GLUE_EXPORT
cci_result CCI_NewMatrix( const dm_real* data, dm_uint32 rows, dm_uint32 columns,
                          cciIMatrix** _result );

DAIM_GLUE_EXPORT 
cci_result CCI_NewImage(dm_uint32 width, dm_uint32 height, EPixelFormat format, 
                        cciIImage* *_result);
DAIM_GLUE_EXPORT 
cci_result CCI_NewImage(dmImageData & data, cciIImage* *_result);

DAIM_GLUE_EXPORT
cci_result CCI_GetLoader( cciILoaderService*, const char* type, dm_bool createCaps, 
                          cciISurfaceDriver** _result );

DAIM_GLUE_EXPORT
cci_result CCI_SaveImage( cciISurfaceDriver* loader, const char* nativePath, cciIImage* image, 
                          const char* options = dm_null );

DAIM_GLUE_EXPORT
cci_result CCI_LoadImage( cciILoaderService* srvc, const char* nativePath, 
                          cciIImage* *_result);

#endif // cciImageUtils_h
