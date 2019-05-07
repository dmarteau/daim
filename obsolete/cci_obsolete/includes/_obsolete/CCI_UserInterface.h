#ifndef CCI_UserInterface_h
#define CCI_UserInterface_h

/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2006 David Marteau
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
// Copyright (c) 2006 David Marteau
// 
// File         : CCI_UserInterface.h
// Date         : 08/2006
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

#define CCIC_UserInterface _TXT("ui.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( UserInterface, Notify )

  CCI_MODULE_UUID( UserInterface, 79c90b1-2a91-41b2-84e3-cb18178203ce)

  CCI_MODULE_VERSION( UserInterface, 1 ) 

CCI_END_MODULE_DECL( UserInterface )

#define  CCI_InitUserInterface() \
  CCI_FindPubClass(CCI_GET_CLSID(UserInterface),CCI_GET_VERSION(UserInterface))

//---------------------------
// Alternate implementation
// not using ui tools
//---------------------------
#define  CCI_InitDocumentInterface() \
  CCI_FindPubClass(_TXT("ntls.cci"),CCI_GET_VERSION(UserInterface))


#endif //__doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//----------------------------------------------------------
// C++ interface
//----------------------------------------------------------
inline bool dmInitUserInterface() {
  dmIModuleClass cl(CCI_InitUserInterface());
  return cl.IsValid();
}

inline bool dmInitDocumentInterface() {
  dmIModuleClass cl(CCI_InitDocumentInterface());
  return cl.IsValid();
}
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_UserInterface_h */
