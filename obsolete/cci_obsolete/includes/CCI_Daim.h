#ifndef CCI_Daim_h
#define CCI_Daim_h

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
// File         : CCI_Daim.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#define CCIC_Daim  _TXT("Daim.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Daim, Notify )

  CCI_MODULE_UUID( Daim, 28bc1b99-f3a2-4b62-90da-eaad90f8174b)

  CCI_MODULE_VERSION( Daim, 1 ) 

  /* Operators */

  CCI_DECL_METHOD_PP(Daim,1,  Register      , _in dm_uint Key _in dm_uint  KeyVersion )

  CCI_DECL_METHOD_PP(Daim,2,  GetModule     , 
     _in CONST char_t* urn 
     _in CONST char_t* module_name 
     _in dm_uint       version 
     _out cci_Object*  module
  )

  CCI_DECL_METHOD_PP(Daim,3,  ReleaseModule , 
    _in CONST char_t* urn 
    _in CONST char_t* module_name 
  )

  CCI_DECL_METHOD_NP(Daim,4,  GetInstance   )

  CCI_DECL_METHOD_PP(Daim,7,  GetLibVersion,
    _out dm_real       version
    _out dm_int        revision
    _out CONST char_t* versionstr
  )

  CCI_DECL_METHOD_PP(Daim,8,  LogMessage,
    _in CONST char_t* message
  )

  CCI_DECL_METHOD_PP( Daim, 9, RegisterModules, 
    _in  CONST char_t* location 
  )

  /* Attributs */ 

  CCI_DECL_ATTR( Daim, 1, Exit, BOOL, [.SGN] )

CCI_END_MODULE_DECL( Daim )

#endif // __doxygen
//----------------------------------------------------------------

#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Interface giving access to the the run-time instance of the library.
 *
 *  This is a singleton object. It provides methods for accessing 
 *  daim library functions in "pure" modules using only
 *  dynamic calls to the daim library methods.
 */
//------------------------------------------------------------------------------------
class dmIDaim : public dmINotify
{  
  public:  
   dmDECLARE_IINTERFACE(Daim)

  ~dmIDaim() {} 
   dmIDaim() {}

   bool GetInstance() {
      cci_Class* cl = dmIDaim::CLASS();
      return (cl!=NULL && Attach(reinterpret_cast<cci_Object*>(
          CCI_InvokeService_NP(cl,Daim,GetInstance))));
   }

   cci_Object* GetModule(CONST char_t* urn, CONST char_t* module_name _in dm_uint version) {
     IF_CCI_IMETHOD_RP(Daim,GetModule, _in urn _in module_name _in version ) 
      THEN return CCI_RETVAL(module);
      ELSE return NULL;
     ENDIF
   }

   bool ReleaseModule(CONST char_t* urn, CONST char_t* module_name) { 
     CCI_RETURN_IMETHOD_PP_OK(Daim,ReleaseModule,_in urn _in module_name )
   }
};
//------------------------------------------------------------------------------------
#endif // __cplusplus

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Daim.h */
