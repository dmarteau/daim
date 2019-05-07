#ifndef dmCCIHook_h
#define dmCCIHook_h

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
// File         : dmCCIHook.h
// Date         : 5/2001
// Author       : David Marteau
//--------------------------------------------------------

//--------------------------------------------------------
// Hook (Use C naming convention for exporting symbol)
//--------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

struct CCI_Custom_Tag  {
  dm_uint         ccih_Magic;      // Identifier as a CCI class     
  dm_uint         ccih_CCI_VER;    // CCI System version number     
  dm_uint         ccih_Version;    // Class version number          
  dm_uint         ccih_Revision;   // Class revision number         
  char_t*         ccih_IDString;   // String identifier              
  dm_ptr          ccih_Init;       // Custom init function  pointer 
  dm_ptr          ccih_Close;      // Custom close function pointer 
  dm_ptr          ccih_Main;       // User Class entry point
  dm_ptr**        ccih_LibVector;  // Filled by the CCI handler      
};
//--------------------------------------------------------
// Public Custom class                                  
//--------------------------------------------------------
struct CCI_Custom_Hook {
	struct CCI_Custom_Tag*  cc_Tag;
	dm_hmodule              cc_Module;
};
//--------------------------------------------------------

#ifdef __cplusplus
}
#endif


#endif // dmCCIHook_h
