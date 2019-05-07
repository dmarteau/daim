#ifndef CCI_CacheManager_h
#define CCI_CacheManager_h

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
// File         : CCI_CacheManager.h
// Date         : 06/2005
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_CacheManager _TXT("cachemngr.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( CacheManager, Notify )

  CCI_MODULE_UUID( CacheManager, 9503de6f-a838-4d23-a90a-607bdaceef30 )

  CCI_MODULE_VERSION( CacheManager, 1 ) 

  /* Methods */
  CCI_DECL_METHOD_PP( CacheManager,1, Add  ,
     _in CONST char_t* file
     _in dm_param key
     _in dm_cci(PropertyList) propertylist 
  )

  CCI_DECL_METHOD_PP( CacheManager,2, Find ,
     _in  CONST char_t*    file 
     _out dm_cci(Snapshot) snapshot
  )

  CCI_DECL_METHOD_PP( CacheManager,3, Clear , _in CONST char_t* file )
  CCI_DECL_METHOD_PP( CacheManager,4, Remove, _in CONST char_t* file )
  CCI_DECL_METHOD_NP( CacheManager,5, RemoveAll )
  CCI_DECL_METHOD_NP( CacheManager,6, ClearAll  )

  CCI_DECL_METHOD_PP( CacheManager,7, Synchronize, _in CONST char_t* file )  
  CCI_DECL_METHOD_PP( CacheManager,8, GetProperties,     
     _in dm_param key
     _in dm_cci(PropertyList) propertylist 
  )

  CCI_DECL_METHOD_PP( CacheManager, 9, Update   , _in CONST char_t* file )
  CCI_DECL_METHOD_NP( CacheManager,10, UpdateAll )

  /* Attributs */
  CCI_DECL_ATTR(CacheManager, 1, Count, dm_uint, [..G.] )


CCI_END_MODULE_DECL( CacheManager )

#endif //__doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 * Handle a list of snapshots images in cache
 *  
 */
//------------------------------------------------------------------------------------
class dmICacheManager : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(CacheManager)
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_CacheManager_h */
