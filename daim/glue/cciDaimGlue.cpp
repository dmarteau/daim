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
// File         : cciDaimGlue.cpp
// Date         : 6 janv. 2009
// Author       : David Marteau
//--------------------------------------------------------

#ifndef DAIM_GLUE
#error You must compile with DAIM_GLUE !
#endif

#include "cciCore.h"
#include "cciId.h"
#include "cciStubs.h"
#include "cciMemory.h"
#include "cciSupportsUtils.h"
#include "cciIGenericFactory.h"
#include "cciIComponentManager.h"
#include "cciIModule.h"
#include "cciIAllocator.h"
#include "cciString.h"

#include "cciPrivateHook.h"

#include "cciDaimGlue.h"
#include "dmLoader.h"

#include "../../daim_utilities/include/dmLoader.h"

#include "dmCrt.h"

#ifdef DM_CONFIG_TARGET_UNIX
#include <unistd.h>
#endif



static dm_hmodule gDaimModule  = dm_null;
static dm_bool    gInitialized = dm_false;

extern "C" {

#undef  __daim_export
#define __daim_export

__daim_export cci_result __daim_call( DM_Initialize )(const char** argv, int argc, dmLOG_FUNCTION pfnLog) = dm_null;
__daim_export void       __daim_call( DM_Finalize   )(dm_bool force) = dm_null;

}

template<class _F>
static inline dm_int __loadsym1( _F* _ppfn, const char* procname )
{
  *_ppfn = reinterpret_cast<_F>(__dm_dlsym(gDaimModule,procname));
  return *_ppfn!=NULL;
}

#define __loadsym( _ppfn )          \
  if(!__loadsym1(&_ppfn,#_ppfn)) {  \
    return CCI_ERROR_NOT_AVAILABLE; \
  }

#define DAIM_DLL DM_DLL_PREFIX "daim" DM_DLL_SUFFIX

static const char* dependentLibs[] =
{
  DM_DLL_PREFIX "daim_utilities" DM_DLL_SUFFIX,
  DM_DLL_PREFIX "daim_kernel"    DM_DLL_SUFFIX,
  dm_null
};

cci_result DM_InitDaimGlue( const char* location, const char** argv, int argc, dmLOG_FUNCTION pfnLog )
{
  if(gInitialized)
     return CCI_ERROR_ALREADY_INITIALIZED;

  char libpath[MAX_PATH] = "\0";

  #ifdef DM_CONFIG_TARGET_LINUX
    // ====================================================
    //
    // Really ugly hack, chdir to the daim binaries location
    // Because on unix we need to preload the library with the
    // correct path as resolved by the dynamic linker
    //
    // ====================================================

    char curpath[MAX_PATH] = "\0";

    snprintf(libpath,MAX_PATH-1,"%s/bin",location);

    #ifdef DEBUG
      fprintf(stderr,"*** Switching current dir to : %s\n",libpath);
    #endif

    getcwd(curpath,MAX_PATH-1); // Get the current dir
    chdir(libpath);             // Jump to the library path


    // Preload libraries
    for( const char** libname = &dependentLibs[0];*libname;++libname)
    {
      //snprintf(libpath,MAX_PATH-1,"%s/bin/%s",location,*libname);
      snprintf(libpath,MAX_PATH-1,"%s",*libname);
      #ifdef DEBUG
        fprintf(stderr,"*** Preloading library: %s\n",libpath);
      #endif
      dm_hmodule module = __dm_dlopen( libpath );
      if(!module)
        __dm_log_dlerror();
    }

   // Return to previous dir;
   #ifdef DEBUG
    fprintf(stderr,"*** Switching current dir to : %s\n",curpath);
   #endif
   chdir(curpath);

  #endif //DM_CONFIG_TARGET_LINUX

  // Bootstrap the library
  snprintf(libpath,MAX_PATH-1,"%s/bin/%s",location,DAIM_DLL);

  gDaimModule = __dm_dlopen( libpath );


  if(!gDaimModule)
  {
    __dm_log_dlerror();

    if(pfnLog)
       pfnLog("Failed to load daim library !",DM_LOG_ERROR);

    return CCI_ERROR_FAILURE;
  }

  __loadsym(DM_Initialize);
  __loadsym(DM_Finalize);

  cciPrivateHook** ppHook = (cciPrivateHook**)__dm_dlsym(gDaimModule,"_CCI_Hook");
  if(!ppHook)
    return CCI_ERROR_NOT_AVAILABLE;

  cci_result rv = DM_Initialize(argv,argc,pfnLog);
  if(CCI_SUCCEEDED(rv) || rv == CCI_ERROR_ALREADY_INITIALIZED) {
     rv = CCI_InitCOMGlue2(*ppHook);
  }

  gInitialized = dm_true;
  return rv;
}

void DM_ReleaseDaimGlue( dm_bool force )
{
  if(gInitialized)
    CCI_ReleaseCOMGlue2();

  if(DM_Finalize)
     DM_Finalize(force);

  if(gDaimModule)
     __dm_dlclose(gDaimModule);

  gDaimModule  = dm_null;
  gInitialized = dm_false;
}
