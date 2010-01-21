#ifndef daim_prolog_h
#define daim_prolog_h

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
// File         : daim_prolog.h
// Date         : 4/2008
// Author       : David Marteau
//--------------------------------------------------------

#ifndef daim_config_h
#include "daim_config.h"
#endif

#ifndef dmTypes_h
#include "dmTypes.h"
#endif

#if defined(DAIM_GLUE)

  // DAIM_GLUE modules must be linked with cciglue static lib

  #define __daim_export extern
  #define __daim_call( _proc ) (*_proc)

#else

 #ifdef DAIM_BUILD
  #define __daim_export __DLL_EXPORT_TAG
 #else
  #define __daim_export __DLL_IMPORT_TAG
 #endif

 #define __daim_call( _proc ) _proc

#endif //DAIM_GLUE

#endif //  daim_prolog_h
