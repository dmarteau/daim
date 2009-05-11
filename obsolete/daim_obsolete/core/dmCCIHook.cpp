/* ::: BEGIN LICENSE BLOCK:::
 * Copyright (c) 2005 David Marteau
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

#include "daim_cci.h"
#include "daim_utilities.h"
#include "dmCCIHook.h"

#if defined(DM_CONFIG_TARGET_LINUX) || defined(DM_CONFIG_TARGET_CYGWIN)
#include "../daim_ports/linux/_dmLoader.cpp"
#elif defined(DM_CONFIG_TARGET_WIN32)
#include "../daim_ports/win32/_dmLoader.cpp"
#endif
//--------------------------------
// Vector that will be passed to
// the modules for "pure" CCI modules
//--------------------------------
extern "C" {

#define __setup_call( _pfn, _proc_id )  (void*)_pfn,
#define _PROC_ENTRY( n ) n

__daim_export void* __dm_CCI_LibVector[] = {
  NULL,

  // CCI functions
  __setup_call( CCI_ResolveExternal,_PROC_ENTRY(1) )
  __setup_call( CCI_FindPubClass   ,_PROC_ENTRY(2) )
  __setup_call( CCI_MakeClass      ,_PROC_ENTRY(3) )
  __setup_call( CCI_FreeClass      ,_PROC_ENTRY(4) )
  __setup_call( CCI_MakeNewObject  ,_PROC_ENTRY(5) )
  __setup_call( CCI_NewObject      ,_PROC_ENTRY(6) )
  __setup_call( CCI_FreeObject     ,_PROC_ENTRY(7) )
  __setup_call( CCI_InstData       ,_PROC_ENTRY(8) )
  __setup_call( CCI_DoMethod       ,_PROC_ENTRY(9) )
  __setup_call( CCI_DoSuperMethod  ,_PROC_ENTRY(10) )
  __setup_call( CCI_CoerceMethod   ,_PROC_ENTRY(11) )
  __setup_call( CCI_SetAttrs       ,_PROC_ENTRY(12) )
  __setup_call( CCI_GetAttrs       ,_PROC_ENTRY(13) )
  __setup_call( CCI_ObjectType     ,_PROC_ENTRY(14) )
  __setup_call( CCI_ObjectClass    ,_PROC_ENTRY(15) )
  __setup_call( CCI_Message        ,_PROC_ENTRY(16) )
  __setup_call( CCI_Exit           ,_PROC_ENTRY(17) )
  __setup_call( CCI_LogFunction    ,_PROC_ENTRY(18) )
  __setup_call( CCI_Flush          ,_PROC_ENTRY(21) )
  __setup_call( CCI_UnLock         ,_PROC_ENTRY(22) )
  __setup_call( CCI_LockObject     ,_PROC_ENTRY(23) )
  __setup_call( CCI_Assert         ,_PROC_ENTRY(24) )
// Pad
NULL,
};

} // extern "C"
//--------------------------------------------------------------
// Dynamic loading of CCI modules
//--------------------------------------------------------------
long __dm_GetCustomHook( const dmString::E* name , void* *_result )
{
  dmString _tmp = name;

  // Find module in CCI PATH
  _tmp = dmPortability::SearchEnv(__DAIM_CCI__,_tmp.Get());
  if(_tmp.Empty()) {
    dmLOG(_TXT("Module %s not found - please check CCI path(s)\n"),name);
    return CCI_ERR_IOERROR;
  }
    
  // Allocate a hook structure
  CCI_Custom_Hook* hook = new CCI_Custom_Hook;
  if(!hook)
     return CCI_ERR_MEMORY;

  dmLOG(_TXT("CCI: Loading module %-48s"),_tmp.Get());
  dm_hmodule module = __dm_dlopen( _tmp.Get() );

  long rv;

  if(module) 
  {
    // Look for "_CCI_Tag" symbol
    CCI_Custom_Tag *tag = (struct CCI_Custom_Tag*)__dm_dlsym( module,"_CCI_Tag" );
    
    // Magic ?
    if(tag && tag->ccih_Magic == CCI_MAKE_ID('C','C','I','C')
           && tag->ccih_CCI_VER <= CCI_SYSTEM_VERSION)
    {
      // Set the library entry points vector
      *tag->ccih_LibVector = &__dm_CCI_LibVector[0];

      // Initialize 'hook' struct
      hook->cc_Tag    = tag;
      hook->cc_Module = module;
        
      // Call 'init(hook)'
      if( ( (CCI_METHOD (*)(CCI_Custom_Hook*))tag->ccih_Init )(hook) ) 
      {
        dm_uint ver = hook->cc_Tag->ccih_Version;    // Class version number          
	      dm_uint rev = hook->cc_Tag->ccih_Revision;   // Class revision number  
        dmLOG(_TXT(" [version %ld.%ld loaded]\n"),ver,rev);
          
        *_result = (void*)hook; // That's all good !
        return CCI_OK;
      } 
      else
        rv = CCI_ERR_INIT_FAILURE
    }    
    else 
      rv = CCI_ERR_INVALID_OBJECT;
	    
    __dm_dlclose( module );
	} 
  else 
  {
    __dm_log_dlerror();
    rv = CCI_ERR_IOERROR;
  }

  delete hook;
  dmLOG(_TXT(" [FAILED]\n"));
   
  return rv;
}
//--------------------------------------------------------------
void __dm_FreeCustomHook(  void* hook, const char_t* name )
{
  CCI_Custom_Hook* _hook = static_cast<CCI_Custom_Hook*>(hook);

  dmLOG(_TXT("CCI: UnLoading module %s\n"),name);

  CCI_Custom_Tag *tag = _hook->cc_Tag;    // Get the  'tag' pointer
  ( (void (*)())tag->ccih_Close )();      // Call the 'close()' function
  __dm_dlclose(_hook->cc_Module);         // UnLoad
  delete _hook;
}
//--------------------------------------------------------------
cci_Class* __dm_MakeClassFromHook( void* hook )
{
   dmASSERT( hook != NULL );
   CCI_Custom_Tag *tag = static_cast<CCI_Custom_Hook*>(hook)->cc_Tag;  // Get the Tag Pointer
   // Execute the User 'main' function
   return ( ( (cci_Class* (*)())tag->ccih_Main )() );
}
//-------------------------------------------------------------
