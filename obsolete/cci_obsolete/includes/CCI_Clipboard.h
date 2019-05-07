#ifndef CCI_Clipboard_h
#define CCI_Clipboard_h

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
// Copyright (c) 2005 David Marteau
// 
// File         : CCI_Clipboard.h
// Date         : 03/2005
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

#define CCIC_Clipboard _TXT("clipboard.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Clipboard, Notify )

  CCI_MODULE_UUID( Clipboard, ad0bf961-3f41-4cf6-a0f1-2f235136aa3f)

  CCI_MODULE_VERSION( Clipboard, 1 ) 

  CCI_DECL_METHOD_NP( Clipboard,1, IsDataAvailable )

CCI_END_MODULE_DECL( Clipboard )

#define  CCI_InitClipboardService() \
  CCI_FindPubClass(CCI_GET_CLSID(Clipboard),CCI_GET_VERSION(Clipboard))



#endif //__doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//----------------------------------------------------------
// C++ interface
//----------------------------------------------------------
inline bool dmIInitClipboardService() {
  dmIModuleClass cl(CCI_InitClipboardService());
  return cl.IsValid();
}
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 * Handle Clipboard as common document protocol.
 *  
 */
//------------------------------------------------------------------------------------
class dmIClipboard : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(Clipboard)

   //-----------------------------------------------------------
   /*! \brief Check if data is avilable as image in clipboard  */
   //-----------------------------------------------------------
   bool IsDataAvailable() { 
     CCI_RETURN_IMETHOD_NP_OK(Clipboard,IsDataAvailable)
   }
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Clipboard_h */
