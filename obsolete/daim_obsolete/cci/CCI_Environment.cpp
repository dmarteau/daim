
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

#include "daim_utilities.h"
#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Environment.h"
//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_Environment
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Environment_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#define CCI_INTERNAL_MODULE
#include "daim_modules/CCI_Base.h"
//--------------------------------------------------------------------

CCI_DECL_DATA( Environment )
{
  dmString _FileBuffer;
  dmString _DirBuffer;
};
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( Environment )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( Environment )

  CCI_INIT_MEMBER( _FileBuffer )
  CCI_INIT_MEMBER( _DirBuffer  )

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( Environment )
{
  CCI_INSTANCE( Environment )

  CCI_DESTROY_MEMBER( _FileBuffer )
  CCI_DESTROY_MEMBER( _DirBuffer  )
  CCI_DESTROY_SUPER()
}
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Environment )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Environment )
CCI_END_SETTER()
*/
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( Environment )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( Environment )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method: GetFile
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Environment, GetFile )
{ 
  CCI_INSTANCE( Environment )

  _This->_FileBuffer.Clear();

  switch(CCI_P(flags))
  {
    case CCIV_Environment_User   :
      _This->_FileBuffer = dmPortability::SearchDir(dmEnvironment::UserDir().CStr(),CCI_P(name));
      break;
 
    case CCIV_Environment_Shared :
      _This->_FileBuffer = dmPortability::SearchEnv(__DAIM_SHAREDPATH__,CCI_P(name));
      break;

    case CCIV_Environment_Lib    :
      _This->_FileBuffer = dmPortability::SearchEnv(__DAIM_LIBPATH__,CCI_P(name));
      break;
  }

  if(!_This->_FileBuffer.Empty())
  {
     // Change '/' to native path separator
     _This->_FileBuffer.ReplaceTokens(_TXT("/"),dmPortability::PathSeparator()[0]);   

     CCI_RETVAL_P(location) = _This->_FileBuffer.CStr();
     CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetSpecialFolder
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Environment, GetSpecialFolder )
{ 
  CCI_INSTANCE( Environment )

  CONST char_t* name = CCI_P(name);

  _This->_DirBuffer.Clear();

  if(EMPTY_STRING(name))
    CCI_RETURN_INVALID_PARAM()

  if(EQUAL_STRING(name,"LibDir")) 
    _This->_DirBuffer = dmEnvironment::LibDir();
  else
  if(EQUAL_STRING(name,"RootDir"))
    _This->_DirBuffer = dmEnvironment::LibRootDir();
  else
  if(EQUAL_STRING(name,"UserDir"))
    _This->_DirBuffer = dmEnvironment::UserDir();
  else
  if(EQUAL_STRING(name,"CacheDir"))
    _This->_DirBuffer = dmEnvironment::LibCache();

  if(!_This->_DirBuffer.Empty()) 
  {
    // Change '/' to native path separator
    _This->_DirBuffer.ReplaceTokens(_TXT("/"),dmPortability::PathSeparator()[0]);   

    CCI_RETVAL_P(location) = _This->_DirBuffer.CStr();
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: CreateFolder
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Environment, CreateFolder )
{ 
  CCI_UNUSUED_INSTANCE( Environment )

  if(!EMPTY_STRING(CCI_P(name))) 
  {
    dmTRY
      dmString _UserDir = dmEnvironment::UserDir();
      dmPortability::CreateDir((dmStrStream(dmEnvironment::UserDir()) << CCI_P(name)).CStr());
      CCI_RETURN_OK()
    dmCATCH(_E)
      CCI_SET_ERROR_MSG(_E.Text().CStr());
    dmDONE

    CCI_RETURN_FAIL()
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method: RegisterModules
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Environment, RegisterModules )
{ 
  CCI_UNUSUED_INSTANCE( Environment )

  const char_t* location = CCI_P(location);

  if(!EMPTY_STRING(location)) 
  {
    dmTRY
      if(dmPortability::AccessFile(location,dmPortability::eEXISTS))
      {
        const dmString::E* _Sep = dmPortability::PathEnvSeparator();
        const dmString::E* _Env = dm_null;

        switch(CCI_P(flags))
        { 
          case CCIV_Environment_Shared : _Env = __DAIM_SHAREDPATH__; break;
          case CCIV_Environment_Lib    : _Env = __DAIM_LIBPATH__   ; break;
          default                      : _Env = __DAIM_CCI__       ; break;                     
        }
       
        dmPortability::SetEnv(_Env,(dmStrStream()
           << dmPortability::GetEnv(_Env) << location << _Sep).CStr()); 

        dmLOG("* Registering modules in : %s for %s\n", location, _Env );         
        CCI_RETURN_OK()   
      }
    dmCATCH(_E)
      CCI_SET_ERROR_MSG(_E.Text().CStr());
    dmDONE

    CCI_RETURN_FAIL()
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Environment )

  CCI_REGISTER_METHOD_PP(Environment, GetFile          )
  CCI_REGISTER_METHOD_PP(Environment, GetSpecialFolder )
  CCI_REGISTER_METHOD_PP(Environment, CreateFolder     )
  CCI_REGISTER_METHOD_PP(Environment, RegisterModules  )

CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Environment )
{   
  CCI_INVOKE_FACTORY(Environment)
}
//---------------------------------------------------------------------
