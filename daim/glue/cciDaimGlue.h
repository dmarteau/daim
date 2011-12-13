#ifndef cciDaimGlue_h
#define cciDaimGlue_h
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
// File         : cciDaimGlue.h
// Date         : 6 janv. 2009
// Author       : David Marteau
//--------------------------------------------------------

#include "daim.h"

// Define CCI_DAIM_GLUE for function that we want to export 
// when linking directly with daim library
// otherwise we need to to link with the daimglue library

#ifdef DAIM_GLUE

cci_result DM_InitDaimGlue( const char* location, const char** argv, int argc, dmLOG_FUNCTION pfnLog );
void       DM_ReleaseDaimGlue( bool force );

#define DAIM_GLUE_EXPORT

#else

#define DAIM_GLUE_EXPORT __daim_export

#endif

#endif /* cciDaimGlue_h */
