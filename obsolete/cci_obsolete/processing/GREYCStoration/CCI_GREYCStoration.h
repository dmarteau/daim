#ifndef CCI_GREYCStoration_h
#define CCI_GREYCStoration_h

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
// Copyright (c) 2006 David Marteau
// 
// File         : CCI_GREYCStoration.h
// Date         : 05/2006
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

#define CCIC_GREYCStoration _TXT("processing/greycstoration.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( GREYCStoration, CommandHandler )

  CCI_MODULE_UUID( GREYCStoration, 4cd9f2a2-94dc-4a0a-b892-a4e0fb2b204d)

  CCI_MODULE_VERSION( GREYCStoration, 1 ) 

  // Attributs 
  CCI_DECL_ATTR(GREYCStoration,1, Progress , dm_uint, [.SGN] )
  CCI_DECL_ATTR(GREYCStoration,2, Iter     , dm_uint, [.SGN] )

CCI_END_MODULE_DECL( GREYCStoration )

#endif //__doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//----------------------------------------------------------
// C++ interface
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  
 */
//------------------------------------------------------------------------------------
class dmIGREYCStoration : public dmICommandHandler
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(GREYCStoration)
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_GREYCStoration_h */
