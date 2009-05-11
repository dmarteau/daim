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
#include "cciIMatrix.h"

CCI_COM_GLUE
cci_result CCI_NewImageList( cciIImageList** _result );

CCI_COM_GLUE
cci_result CCI_NewColorSpace( const char* colorSpace, cciIColorSpace** _result );

CCI_COM_GLUE
cci_result CCI_NewFilterContext( cciIFilterContext** _result);

CCI_COM_GLUE
cci_result CCI_NewMatrix( const dm_real* data, dm_uint32 rows, dm_uint32 columns,
                          cciIMatrix** _result );

#endif // cciImageUtils_h
