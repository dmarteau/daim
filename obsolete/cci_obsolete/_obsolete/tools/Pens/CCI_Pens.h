#ifndef CCI_Pens_h
#define CCI_Pens_h

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
// File         : CCI_Pens.h
// Date         : 3/2006
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_AdvTools  _TXT("drawing/pens.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Pens, Notify )

  CCI_MODULE_UUID( Pens, 588b3300-d6f7-47b9-8d56-d3a6c230f006)

  CCI_MODULE_VERSION( Pens, 1 ) 

CCI_END_MODULE_DECL( Pens )

#define  CCI_RegisterPens() \
  CCI_FindPubClass(CCI_GET_CLSID(Pens),CCI_GET_VERSION(Pens))

#endif // __doxygen
//----------------------------------------------------------------

#ifdef __cplusplus

#include "CCI_Interface.h"
//----------------------------------------------------------
// C++ interface
//----------------------------------------------------------
inline bool dmIRegisterPens() {
  dmIModuleClass cl(CCI_RegisterPens());
  return cl.IsValid();
}
//----------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_AdvTools_h */
