
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

 
/* $ChangeLog  v2.5, 22 août 2006 23:05:48 , David
 *
 * Added LoadUI() method
 */

#define dmUseKernelImage
#include "daim_kernel.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Daim.h"

#if defined(DM_CONFIG_TARGET_LINUX) || defined(DM_CONFIG_TARGET_CYGWIN)
#include "../daim_ports/linux/_dmLoader.cpp"
#elif defined(DM_CONFIG_TARGET_WIN32)
#include "../daim_ports/win32/_dmLoader.cpp"
#endif

//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_Daim
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Daim_Version
#define CCI_REVISION  1
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#define CCI_INTERNAL_MODULE
#include "daim_modules/CCI_Base.h"
//--------------------------------------------------------------------

extern cci_Object* __cci_application_base;

#include "daim/daim_base.h"

typedef daim::hashmap<dmString,cci_Object*> _hash_mod_t;

CCI_DECL_DATA( Daim )
{
  dm_bool          OnExit;
  dmRWLock         mModAccess;
  dmString         mVersionStr;
  _hash_mod_t      mModules;  
};
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( Daim )
{
  CCI_CONSTRUCT_SUPER()

  CCI_INSTANCE( Daim )
  
  dmMemory::Memset(_This,0,sizeof(*_This));

  CCI_INIT_MEMBER( mModAccess )
  CCI_INIT_MEMBER( mModules )

  CCI_INIT_MEMBER( mVersionStr )

  _This->mVersionStr = dmEnvironment::VersionStr();

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( Daim )
{
  CCI_INSTANCE( Daim )

  // Release all the modules
  _hash_mod_t::iterator it   = _This->mModules.begin();  
  _hash_mod_t::iterator last = _This->mModules.end();  
  for(;it!=last;++it) 
    CCI_Release((*it).second);

  _This->mModules.clear();

  CCI_DESTROY_MEMBER( mModules     )
  CCI_DESTROY_MEMBER( mModAccess   )
  CCI_DESTROY_MEMBER( mVersionStr  )
     
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Daim )
  CCI_ATTR_GET( Daim, Exit, CCIA_RETVAL = _This->OnExit ) 
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Daim )
  CCI_ATTR_SET( Daim, Exit, _This->OnExit = CCIA_DATA ) 
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method : Register
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Daim, Register )
{
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method : GetModule
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Daim, GetModule )
{
  if(EMPTY_STRING(CCI_P(module_name)))
    CCI_RETURN_INVALID_PARAM()

  dm_uint  _version    = CCI_P(version);
  dmString _module_key = CCI_P(urn);

  if(_version == 0) 
     _version = TGZ_DEFAULT;

  if(_module_key.Empty()) 
     _module_key = _TXT("daim://");

  CCI_INSTANCE( Daim )

  _module_key.Append(CCI_P(module_name));

  dmREAD_LOCK(_This->mModAccess);

  _hash_mod_t::iterator it = _This->mModules.find(_module_key);
  if(it==_This->mModules.end())
  {
     cci_Object* _object = CCI_NewObject(CCI_P(module_name),_version,NULL);
     if(_object)  
     {
       // Change the lock state => read
       _This->mModAccess.Unlock();
       _This->mModAccess.WrLock(); 
       _This->mModules[_module_key.Get()] = _object;

       CCI_AddRef(_object);   // AddRef for the Caller
       CCI_RETVAL_P(module) = _object;
       CCI_RETURN_OK()
     }
  }
  else 
  {
    if(CCI_P(version) != TGZ_DEFAULT)
    {
      dmIModuleClass cl((*it).second);
      if(cl.Version() < CCI_P(version)) {
         CCI_SET_ERROR_MSG(_TXT("CCI ERROR: Bad module version !"));
         CCI_RETURN_FAIL()
      }
    }
    CCI_AddRef((*it).second);  // AddRef for the Caller
    CCI_RETVAL_P(module) = (*it).second;
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : ReleaseModule
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Daim, ReleaseModule )
{
  CCI_INSTANCE( Daim )

  dmString _module_key = CCI_P(urn);

  if(_module_key.Empty()) 
      _module_key = _TXT("daim://");

  // POTENTIAL_DEAD_LOCK
  // Dead lock can occurs if releasing the object
  // trig a call to ReleaseModule from another thread

  dmWRITE_LOCK(_This->mModAccess);

  if(!EMPTY_STRING(CCI_P(module_name))) 
  {
    _module_key.Append(CCI_P(module_name));

    _hash_mod_t::iterator it = _This->mModules.find(_module_key);  
    if(it!=_This->mModules.end()) {
      CCI_Release((*it).second);
      _This->mModules.erase(it);     
      CCI_RETURN_OK()
    }
  } 
  else if(!_module_key.Empty())
  {
    _hash_mod_t::iterator it   = _This->mModules.begin();  
    _hash_mod_t::iterator last = _This->mModules.end();  
    while(it!=last) 
    {
     if(_module_key.Compare(0,_module_key.Size(),(*it).first)) {
        CCI_Release((*it).second);
        _This->mModules.erase(it++);    
     } else
        ++it;
    }
    CCI_RETURN_OK()
  } 

  CCI_RETURN_FAIL()
} 
//---------------------------------------------------------------------
// Method : GetInstance
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NN( Daim, GetInstance )
{
  return dm_ptr_to_long(__cci_application_base);
}
//---------------------------------------------------------------------
// Method : GetLibVersion
//---------------------------------------------------------------------
static  CCI_IMPL_METHOD(Daim, GetLibVersion )
{
  CCI_INSTANCE( Daim )

  CCI_RETVAL_P(version)     = dmEnvironment::Version();
  CCI_RETVAL_P(revision)    = dmEnvironment::Revision(); 
  CCI_RETVAL_P(versionstr)  = _This->mVersionStr.Get();

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method : GetLibVersion
//---------------------------------------------------------------------
static  CCI_IMPL_METHOD(Daim, LogMessage )
{
  CCI_UNUSUED_INSTANCE( Daim )

  dmException::Trace( CCI_P(message) );
  
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: RegisterModules
//---------------------------------------------------------------------
static CCI_IMPL_METHOD(Daim, RegisterModules )
{ 
  CCI_UNUSUED_INSTANCE( Daim )

  const char_t* location = CCI_P(location);

  if(!EMPTY_STRING(location)) 
  {
    dmTRY
      if(dmPortability::AccessFile(location,dmPortability::eEXISTS))
      {
        const dmString::E* _Sep = dmPortability::PathEnvSeparator();
        const dmString::E* _Env = __DAIM_CCI__;
       
        dmPortability::SetEnv(_Env,(dmStrStream()
           << dmPortability::GetEnv(_Env) << location << _Sep).Get()); 

        dmLOG("* Registering modules in : %s for %s\n", location, _Env );         
        CCI_RETURN_OK()   
      }
    dmCATCH(_E)
      CCI_SET_ERROR_MSG(_E.Text().Get());
    dmDONE

    CCI_RETURN_FAIL()
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Daim )
  CCI_REGISTER_METHOD_NN( Daim, GetInstance     )
  CCI_REGISTER_METHOD_PP( Daim, Register        )
  CCI_REGISTER_METHOD_PP( Daim, GetModule       )
  CCI_REGISTER_METHOD_PP( Daim, ReleaseModule   )
  CCI_REGISTER_METHOD_PP( Daim, GetLibVersion   )
  CCI_REGISTER_METHOD_PP( Daim, LogMessage      )
  CCI_REGISTER_METHOD_PP( Daim, RegisterModules )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT(Daim)
{
  CCI_INVOKE_FACTORY(Daim);
}
//---------------------------------------------------------------------
