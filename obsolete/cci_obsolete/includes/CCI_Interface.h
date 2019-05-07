
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
// File         : CCI_Interface.h
// Date         : 7/2004
// Author       : David Marteau
// Description  : This is an attempt to have boilerplate code for accessing
//                CCI methods.
//
//--------------------------------------------------------

// The following should be redefined each time
// the file is included
#undef _in
#undef _out

#ifndef CCI_Interface_h
#define CCI_Interface_h

#include "daim_utilities/dmCCIObject.h"

//------------------------------------------------------

#define CCI_NULL_P
#define CCI_DFLT_P

#define CCI_PACK_MSG( _TheClass, _TheMethod, _TheParams ) \
   CCIM_DECL_(_TheClass,_TheMethod) _TheParams

#define CCI_DECL_MSG( _This_msg, _TheClass, _TheMethod, _TheParams )  \
    struct CCIP_DECL_(_TheClass,_TheMethod) _This_msg = { CCIM_DECL_(_TheClass,_TheMethod) _TheParams }

//------------------------------------------------------

#define IF_CCI_InvokeService_RP( _Handle, _TheClass, _TheMethod,  _TheParams  ) {\
    struct CCIP_DECL_(_TheClass,_TheMethod) __msg = { CCIM_DECL_(_TheClass,_TheMethod) _TheParams };\
    if( CCI_SUCCEEDED( CCI_CoerceMethod(_Handle,NULL, CCI_TRANSLATE_MSG(cci_Msg,&__msg)) ) )

#define IF_CCI_INVOKE_RP( _TheObject, _TheClass, _TheMethod, _TheParams ) {\
    struct CCIP_DECL_(_TheClass,_TheMethod) __msg = { CCIM_DECL_(_TheClass,_TheMethod) _TheParams };\
    if( CCI_SUCCEEDED( CCI_DoMethod( _TheObject, CCI_TRANSLATE_MSG(cci_Msg,&__msg)) ) )

#define UNLESS_CCI_INVOKE_RP( _TheObject, _TheClass, _TheMethod, _TheParams ) {\
    struct CCIP_DECL_(_TheClass,_TheMethod) __msg = { CCIM_DECL_(_TheClass,_TheMethod) _TheParams };\
    if( !CCI_SUCCEEDED( CCI_DoMethod( _TheObject, CCI_TRANSLATE_MSG(cci_Msg,&__msg)) ) )


#define IF_CCI_INVOKE_SUPER_RP( _TheObject, _TheClass, _TheMethod, _TheParams ) {\
    struct CCIP_DECL_(_TheClass,_TheMethod) __msg = { CCIM_DECL_(_TheClass,_TheMethod) _TheParams };\
    if( CCI_SUCCEEDED( CCI_DoSuperMethod( _TheObject, CCI_TRANSLATE_MSG(cci_Msg,&__msg)) ) )

//------------------------------------------------------

#define CCI_Invoke_NP( _TheObject, _TheClass, _TheMethod ) \
   CCI_DoMethodNP(_TheObject,CCIM_DECL_(_TheClass,_TheMethod)) 

#define CCI_InvokeSuper_NP( _TheObject, _TheClass, _TheMethod ) \
   CCI_DoSuperMethodNP(_TheObject,CCIM_DECL_(_TheClass,_TheMethod)) 

#define CCI_Invoke_Class_NP( _Class, _TheObject, _TheClass, _TheMethod ) \
   CCI_CoerceMethodNP(_Class,_TheObject,CCIM_DECL_(_TheClass,_TheMethod)) 

#define CCI_InvokeService_NP( _Handle, _TheClass, _TheMethod ) \
   CCI_CoerceMethodNP(_Handle,NULL,CCIM_DECL_(_TheClass,_TheMethod)) 

//------------------------------------------------------

#define CCI_IMETHOD_PP_( result , _TheObject, _TheClass, _TheMethod, _TheParams ) {\
    struct CCIP_DECL_(_TheClass,_TheMethod) __msg = { CCIM_DECL_(_TheClass,_TheMethod) _TheParams };\
    result = CCI_DoMethod(_TheObject,CCI_TRANSLATE_MSG(cci_Msg,&__msg)); }

#define CCI_INVOKE_PP_( _TheObject, _TheClass, _TheMethod, _TheParams ) {\
    struct CCIP_DECL_(_TheClass,_TheMethod) __msg = { CCIM_DECL_(_TheClass,_TheMethod) _TheParams };\
    CCI_DoMethod(_TheObject,CCI_TRANSLATE_MSG(cci_Msg,&__msg)); }

#define CCI_RETURN_IMETHOD_PP_( _TheObject, _TheClass, _TheMethod, _TheParams ) {\
    struct CCIP_DECL_(_TheClass,_TheMethod) __msg = { CCIM_DECL_(_TheClass,_TheMethod) _TheParams };\
    return CCI_DoMethod(_TheObject,CCI_TRANSLATE_MSG(cci_Msg,&__msg)); }

#define CCI_IMETHOD_CLASS_PP_( _Class, _TheObject, _TheClass, _TheMethod, _TheParams ) {\
    struct CCIP_DECL_(_TheClass,_TheMethod) __msg = { CCIM_DECL_(_TheClass,_TheMethod) _TheParams };\
    result = CCI_CoerceMethod(_Class,_TheObject,CCI_TRANSLATE_MSG(cci_Msg,&__msg)); }

#define CCI_INVOKE_CLASS_PP_( _Class, _TheObject, _TheClass, _TheMethod, _TheParams ) {\
    struct CCIP_DECL_(_TheClass,_TheMethod) __msg = { CCIM_DECL_(_TheClass,_TheMethod) _TheParams };\
    CCI_CoerceMethod(_Class,_TheObject,CCI_TRANSLATE_MSG(cci_Msg,&__msg)); }

#define CCI_RETURN_IMETHOD_CLASS_PP_( _Class, _TheObject, _TheClass, _TheMethod, _TheParams ) {\
    struct CCIP_DECL_(_TheClass,_TheMethod) __msg = { CCIM_DECL_(_TheClass,_TheMethod) _TheParams };\
    return CCI_CoerceMethod(_Class,_TheObject,CCI_TRANSLATE_MSG(cci_Msg,&__msg)); }

//------------------------------------------------------

#define CCI_IMETHOD_PP( result , _TheClass, _TheMethod, _TheParams ) {\
    struct CCIP_DECL_(_TheClass,_TheMethod) __msg = { CCIM_DECL_(_TheClass,_TheMethod) _TheParams };\
    result = CCI_DoMethod(_IInstance,CCI_TRANSLATE_MSG(cci_Msg,&__msg)); }

#define CCI_IINVOKE_PP( _TheClass, _TheMethod, _TheParams ) {\
    struct CCIP_DECL_(_TheClass,_TheMethod) __msg = { CCIM_DECL_(_TheClass,_TheMethod) _TheParams };\
    CCI_DoMethod(_IInstance,CCI_TRANSLATE_MSG(cci_Msg,&__msg)); }

#define CCI_IINVOKE_NP( _TheClass, _TheMethod ) \
    CCI_DoMethodNP(_IInstance,CCIM_DECL_(_TheClass,_TheMethod))

#define CCI_IMETHOD_NP( result, _TheClass, _TheMethod ) \
    result = CCI_IINVOKE_NP(_TheClass,_TheMethod)

#define CCI_RETURN_IMETHOD_NP( _TheClass, _TheMethod ) \
    return CCI_IINVOKE_NP(_TheClass,_TheMethod)

#define CCI_RETURN_IMETHOD_PP( _TheClass, _TheMethod, _TheParams ) {\
    struct CCIP_DECL_(_TheClass,_TheMethod) __msg = { CCIM_DECL_(_TheClass,_TheMethod) _TheParams };\
    return CCI_DoMethod(_IInstance,CCI_TRANSLATE_MSG(cci_Msg,&__msg)); }

#define CCI_RETURN_IMETHOD_PP_OK( _TheClass, _TheMethod, _TheParams ) {\
    struct CCIP_DECL_(_TheClass,_TheMethod) __msg = { CCIM_DECL_(_TheClass,_TheMethod) _TheParams };\
    return CCI_SUCCEEDED( CCI_DoMethod(_IInstance,CCI_TRANSLATE_MSG(cci_Msg,&__msg)) ); }

#define CCI_RETURN_IMETHOD_NP_OK( _TheClass, _TheMethod ) \
    return CCI_SUCCEEDED( CCI_Invoke_NP(_IInstance,_TheClass,_TheMethod) );
//------------------------------------------------------

#define IF_CCI_IMETHOD_RP( _TheClass, _TheMethod, _TheParams ) {\
    struct CCIP_DECL_(_TheClass,_TheMethod) __msg = { CCIM_DECL_(_TheClass,_TheMethod) _TheParams };\
    if( CCI_SUCCEEDED( CCI_DoMethod(_IInstance,CCI_TRANSLATE_MSG(cci_Msg,&__msg)) ) )

#define CCI_RETVAL( _TheParam )   __msg._TheParam
#define CCI_RETURN_P( _TheParam ) _TheParam = CCI_RETVAL( _TheParam );

#ifndef NO_CCI_MACROS

#define THEN  {
#define ELSE  } else {
#define ENDIF }}

#define ENDIF_RETURN_BOOL()  return true; } else return false; } 

#endif // NO_CCI_MACROS
//------------------------------------------------------

#define CCI_ISET( _TheClass, _TheAttr, _TheValue ) Set(CCIA_DECL_(_TheClass,_TheAttr),_TheValue)
#define CCI_IGET( _TheClass, _TheAttr, _TheValue ) Get(CCIA_DECL_(_TheClass,_TheAttr),_TheValue)

#define CCI_GET_VERSION( _TheClass )  CCI_DECL_NAME_(CCI_,_TheClass,Version)
#define CCI_GET_CLSID( _TheClass )    CCIC_##_TheClass

//----------------------------------------------------------------
#define _MAKE_CCI_IDSTRING(_name,_author,_cpr) \
   _TXT("$VER: ") _name _TXT(" %d.%d, ") _author _TXT(" , ") _TXT(__DATE__) _cpr
//------------------------------------------------------

#define dmDECLARE_IINTERFACE( _TheClass ) \
  static CONST char_t* CCI_CLSID() { return CCI_GET_CLSID(_TheClass);   } \
  static dm_uint       VERSION()   { return CCI_GET_VERSION(_TheClass); } \
  static cci_Class*    CLASS()     { return CCI_FindPubClass(dmI##_TheClass::CCI_CLSID(),dmI##_TheClass::VERSION()); } \
  bool   QueryInterface( cci_Object* ob ) { \
    return dmIUnknown::QueryInterface(dmI##_TheClass::CCI_CLSID(),ob); \
  }

#define dmGENERIC_ICONSTRUCTORS( _TheClass ) \
  dmI##_TheClass() {}                        \
  dmI##_TheClass( tg_TagItem* attrs ) { CreateObject(dmI##_TheClass::CCI_CLSID(),dmI##_TheClass::VERSION(),attrs); }\
  \
   bool Create( tg_TagItem* attrs ) { return CreateObject(dmI##_TheClass::CCI_CLSID(),dmI##_TheClass::VERSION(),attrs); }\
   bool Create()                    { return CreateObject(dmI##_TheClass::CCI_CLSID(),dmI##_TheClass::VERSION(),NULL); }

#define dmDECLARE_GENERIC_IINTERFACE( _TheClass ) \
   dmDECLARE_IINTERFACE(_TheClass)\
   dmGENERIC_ICONSTRUCTORS(_TheClass)

//------------------------------------------------------

#endif // CCI_Interface_h

#ifndef CCI_IN_OUT_DEFINED
  #define _in ,
  #define _out(name) ,
  #define CCI_IN_OUT_DEFINED
#endif 
