
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
// File         : CCI_Epilog.h
// Date         : 7/2004
// Author       : David Marteau
//
//  *NOTE* : This file is designed for multiple inclusions
//  you must disabled any preprocessor  "header protection" 
//  options to enable reentrancy of this header
//--------------------------------------------------------

#ifndef CCI_PROLOG
#error  "EPILOG header has no PROLOG header !"
#endif

#ifdef DM_CONFIG_MSVC_BUILD
#pragma pack(pop)
#endif

#ifdef NO_CCI_MACROS

#undef THEN
#undef ELSE
#undef ENDIF 
#undef ENDIF_RETURN_BOOL

#endif // NO_CCI_MACROS

#undef _in 
#undef _out 
#undef CCI_IN_OUT_DEFINED

#undef CCI_PROLOG
