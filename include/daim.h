#ifndef daim_h
#define daim_h

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

//--------------------------------------------------------
// File         : daim.h
// Date         : 04/2008
// Author       : David Marteau
//--------------------------------------------------------

/**
 * Include some kernel api definitions
 */

#include "daim/dmKernelBase.h"
#include "daim/dmCoordinatesTypes.h"
#include "daim/dmPixelFormats.h"
#include "daim/dmImageData.h"
#include "daim/dmOperators.h"

#ifndef cciCore_h
#include "cciCore.h"
#endif

typedef CCI_CALLBACK_(void,dmLOG_FUNCTION)( const char*, dm_int32 );

extern "C" {

__daim_export cci_result __daim_call( DM_Initialize )( const char** argv, int argc, dmLOG_FUNCTION pfnLog );
__daim_export void       __daim_call( DM_Finalize   )( bool force );

}

#endif // daim_h
