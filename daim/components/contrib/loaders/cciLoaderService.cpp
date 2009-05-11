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
// File         : cciLoaderService.cpp
// Date         : 27 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciCOMPtr.h"
#include "cciString.h"
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"
#include "cciICategoryManager.h"
#include "cciISurface.h"
#include "cciILoaderService.h"
#include "cciISurfaceDriver.h"

static const char* CATMNGR_CONTRACTID = "@daim.org/category-manager;1";

class cciLoaderService : public cciILoaderService
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_ILOADERSERVICE
  CCI_DECL_IDRIVERPROXY

  cciLoaderService();

private:
  ~cciLoaderService();

protected:
  /* additional members */
};

/* Implementation file */
CCI_IMPL_ISUPPORTS2(cciLoaderService, cciILoaderService, cciIDriverProxy)

static cci_result get_drivers( cciIStringEnumerator* *_retval)
{
  cci_result rv;
  cci_Ptr<cciICategoryManager> catmngr = do_GetService(CATMNGR_CONTRACTID,&rv);
  if(CCI_SUCCEEDED(rv))
     rv = catmngr->EnumerateCategory(DRIVER_PROXY_CATEGORY,_retval);

  return rv;
}


cciLoaderService::cciLoaderService()
{
  /* member initializers and constructor code */
}

cciLoaderService::~cciLoaderService()
{
  /* destructor code */
}

/* cciISurface openSurface (in string location, in unsigned long ioFlags ); */
CCI_IMETHODIMP cciLoaderService::OpenSurface(const char * location, dm_uint32 ioFlags,
                                             cciISurface * *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(_retval);
  CCI_ENSURE_ARG_POINTER(location);

  cci_Ptr<cciIStringEnumerator> drivers;

  cci_result rv = get_drivers(getter_AddRefs(drivers));
  if(CCI_FAILED(rv))
     return rv;

  dmCString contractid;

  cci_Ptr<cciIDriverProxy> proxy;

  while(drivers->HasMore())
  {
    rv = drivers->GetNext(contractid);
    if(CCI_SUCCEEDED(rv))
    {
      proxy = do_GetService(contractid.get(),&rv);
      if(proxy) {
         rv = proxy->OpenSurface(location,ioFlags,_retval);
         if(rv != CCI_ERROR_NOT_AVAILABLE)
           break;
      }
    }
  }

  return rv;
}

/* cciISurface createSurface (in string location, in string type, in dm_uint32 width, in dm_uint32 height, in EPixelFormat format, in dm_bool hasAlpha, [array, size_is (count)] in string options, in dm_uint32 count); */
CCI_IMETHODIMP cciLoaderService::CreateSurface(const char * location, const char * type,
                                               dm_uint32 width, dm_uint32 height,
                                               EPixelFormat format, dm_bool hasAlpha,
                                               const char *options,
                                               cciISurface * *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(_retval);
  CCI_ENSURE_ARG_POINTER(location);
  CCI_ENSURE_ARG_POINTER(type);

  CCI_ENSURE_ARG_POINTER(_retval);
  CCI_ENSURE_ARG_POINTER(type);

  cci_Ptr<cciIStringEnumerator> drivers;

  cci_result rv = get_drivers(getter_AddRefs(drivers));
  if(CCI_FAILED(rv))
     return rv;

  dmCString contractid;

  cci_Ptr<cciIDriverProxy> proxy;

  while(drivers->HasMore())
  {
    rv = drivers->GetNext(contractid);
    if(CCI_SUCCEEDED(rv))
    {
      proxy = do_GetService(contractid.get(),&rv);
      if(proxy) {
        rv = proxy->CreateSurface(location,type,width,height,format,hasAlpha,options,_retval);
        if(rv != CCI_ERROR_NOT_AVAILABLE)
          break;
      }
    }
  }

  return rv;
}

/* cciISurfaceDriver getDriver (in string type, in dm_bool createCaps); */
CCI_IMETHODIMP cciLoaderService::GetDriver(const char * type, dm_bool createCaps,
                                           cciISurfaceDriver * *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(_retval);
  CCI_ENSURE_ARG_POINTER(type);

  cci_Ptr<cciIStringEnumerator> drivers;

  cci_result rv = get_drivers(getter_AddRefs(drivers));
  if(CCI_FAILED(rv))
     return rv;

  dmCString contractid;

  cci_Ptr<cciIDriverProxy> proxy;

  while(drivers->HasMore())
  {
    rv = drivers->GetNext(contractid);
    if(CCI_SUCCEEDED(rv))
    {
      proxy = do_GetService(contractid.get(),&rv);
      if(proxy) {
        rv = proxy->GetDriver(type,createCaps,_retval);
        if(rv != CCI_ERROR_NOT_AVAILABLE)
          break;
      }
    }
  }

  return rv;
}


//=====================================
// Module
//=====================================

#include "cciIGenericFactory.h"

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciLoaderService)

static const cciModuleComponentInfo components[] = {
    {  CCI_LOADERSERVICE_CLASSNAME,
       CCI_LOADERSERVICE_CID,
       CCI_LOADERSERVICE_CONTRACTID,
       cciLoaderServiceConstructor
    },
};

CCI_IMPL_GETMODULE(cciLoaderServiceModule, components)



