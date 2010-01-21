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
// File         : gdalProxy.cpp
// Date         : 8 déc. 2008
// Author       : David Marteau
//--------------------------------------------------------

#define dmUseUtilitiesExtra
#define dmUseKernelImageTemplates

#include "cciCOMPtr.h"
#include "ccidaim.h"
#include "cciString.h"
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"
#include "cciICategoryManager.h"
#include "cciISurface.h"
#include "cciIDriverProxy.h"
#include "cciISurfaceDriver.h"
#include "cciIColorTable.h"

#include "gdaldrv.h"
#include "daim_kernel.h"

#include "gdalSurface.h"

#include "dmCrt.h"

struct driverTableEntry {
  const char* typeMime;
  const char* gdalName;
};

#define DRIVER_ENTRY(  type, name, ext ) { type, name  },

static const driverTableEntry driverTable[] = {
  DRIVER_ENTRY("image/bmp" ,"BMP"  , "bmp")
  DRIVER_ENTRY("image/gif" ,"GIF"  , "gif")
  DRIVER_ENTRY("image/jpeg","JPEG" ,"jpg")
  DRIVER_ENTRY("image/png" ,"PNG"  ,"png")
  DRIVER_ENTRY("image/tiff","GTiff","tif")
  DRIVER_ENTRY("image/x-portable-anymap","PNM","pnm")
};


static const char* resolveDriverName( const char* aMIMEType )
{
  int tableSize = sizeof(driverTable)/sizeof(driverTableEntry);

  const driverTableEntry* entry = &driverTable[0];
  for(int i=0;i<tableSize;++i,++entry)
  {
    if(_dm_strcmp(aMIMEType,entry->typeMime)==0)
       return entry->gdalName;
  }

  return aMIMEType;
}

///////////////////////////////////////////////////
// Handle gdal errors
static void CPL_STDCALL gdalErrorHandler(CPLErr eErrClass, int nError,
                                         const char * pszErrorMsg )
{
  switch( eErrClass ) {
    case  CE_Debug   : dmLOG("gdal debug  %d : %s !"           ,nError,pszErrorMsg);break;
    case  CE_Warning : dmLOG_WARN("gdal warning  %d : %s !"    ,nError,pszErrorMsg);break;
    case  CE_Failure : dmLOG_ERROR("gdal error %d : %s !"      ,nError,pszErrorMsg);break;
    case  CE_Fatal   : dmLOG_ERROR("gdal fatal error %d : %s !",nError,pszErrorMsg);break;
    default:
      dmLOG_WARN("gdal returned error class %d : %d : %s !" ,eErrClass,nError,pszErrorMsg);break;
  }
}



///////////////////////////////////////////////////
// cciGDALProxy implementation

class cciGDALProxy : public cciIDriverProxy
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IDRIVERPROXY

  cciGDALProxy();

private:
  ~cciGDALProxy();

protected:
  /* additional members */
};

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciGDALProxy, cciIDriverProxy)

cciGDALProxy::cciGDALProxy()
{
  /* member initializers and constructor code */
}

cciGDALProxy::~cciGDALProxy()
{
  /* destructor code */
}

/* cciISurface openSurface (in string location, in unsigned long ioFlags ); */
CCI_IMETHODIMP cciGDALProxy::OpenSurface(const char * location, dm_uint32 ioFlags, cciISurface * *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(_retval);
  CCI_ENSURE_ARG_POINTER(location);

  cci_result rv;

  GDALDatasetH hDS = GDALOpenShared(location,static_cast<GDALAccess>(ioFlags));
  if(hDS)
  {
    cci_Ptr<gdalSurface> surf;
    rv = gdalSurface::Create(hDS,ioFlags,dm_null,getter_AddRefs(surf));
    if(CCI_FAILED(rv))
       GDALClose(hDS);
    
    CCI_IF_ADDREF(*_retval =  surf);
  }
  else {
    CPLErr err = CPLGetLastErrorType();
    if(err == CPLE_NotSupported)
       rv = CCI_ERROR_NOT_AVAILABLE; // Only guess that driver is not found
    else
       rv = CCI_ERROR_FAILURE;
  }
  return rv;
}


/* cciISurfaceDriver getDriver (in string type, in dm_bool createCaps); */
CCI_IMETHODIMP cciGDALProxy::GetDriver(const char * type, dm_bool createCaps,
                                       cciISurfaceDriver * *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(_retval);
  CCI_ENSURE_ARG_POINTER(type);

  const char* gdalName = resolveDriverName(type);

  GDALDriverH hDriver = GDALGetDriverByName(gdalName);
  if(!hDriver)
     return CCI_ERROR_NOT_AVAILABLE;

  if(createCaps)
  {
    if(GDALGetMetadataItem(hDriver,GDAL_DCAP_CREATE,NULL )     == NULL  &&
       GDALGetMetadataItem(hDriver,GDAL_DCAP_CREATECOPY,NULL ) == NULL)
     return CCI_ERROR_NOT_AVAILABLE;
  }

  return CCI_NewGDALDriver(hDriver,_retval);
}


//=====================================
// Module
//=====================================

#include "cciIGenericFactory.h"

#define CCI_GDALPROXY_CID \
{ /* D6D4ACB5-3FBB-4B86-870C-A26D5A462211 */       \
  0xD6D4ACB5,                                      \
  0x3FBB,                                          \
  0x4B86,                                          \
  {0x87, 0x0C, 0xA2, 0x6D, 0x5A, 0x46, 0x22, 0x11} \
}

#ifdef GDAL_MINIMAL
#define CCI_GDALPROXY_CLASSNAME  "GDAL (minimal) driver proxy"
#define CCI_GDALPROXY_CONTRACTID "@daim.org/contrib/gdal-minimal-proxy;1"
#else
#define CCI_GDALPROXY_CLASSNAME  "GDAL (full) driver proxy"
#define CCI_GDALPROXY_CONTRACTID "@daim.org/contrib/gdal-full-proxy;1"
#endif

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciGDALProxy)

static CCI_METHOD GDALProxyRegisterSelfProc(cciIComponentManager *aCompMgr,
                                            const char    *aPath,
                                            const char    *aLoaderStr,
                                            const char    *aType,
                                            const cciModuleComponentInfo *aInfo)
{
  // Register the proxy to the category manager
  cci_Ptr<cciICategoryManager> catmngr = do_GetService("@daim.org/category-manager;1");
  if(catmngr)
     return catmngr->AddCategoryEntry(DRIVER_PROXY_CATEGORY,CCI_GDALPROXY_CONTRACTID,
                                      CCI_GDALPROXY_CLASSNAME,DM_TRUE,DM_TRUE);

  return CCI_ERROR_FAILURE;
}

static CCI_METHOD GDALProxyUnRegisterSelfProc(cciIComponentManager *aCompMgr,
                                              const char *aPath,
                                              const char *aLoaderStr,
                                              const cciModuleComponentInfo *aInfo)
{
  // Register the proxy to the category manager
  cci_Ptr<cciICategoryManager> catmngr = do_GetService("@daim.org/category-manager;1");
  if(catmngr)
     catmngr->DeleteCategoryEntry(DRIVER_PROXY_CATEGORY,CCI_GDALPROXY_CONTRACTID,DM_TRUE);

  return CCI_OK;
}

static const cciModuleComponentInfo components[] = {
    {  CCI_GDALPROXY_CLASSNAME,
       CCI_GDALPROXY_CID,
       CCI_GDALPROXY_CONTRACTID,
       cciGDALProxyConstructor,
       GDALProxyRegisterSelfProc,
       GDALProxyUnRegisterSelfProc
    },
};


static dm_bool gInitialized = DM_FALSE;

static cci_result cciGDALProxyModuleInit(cciIModule* aSelf)
{
  #ifdef DEBUG
    dmLOG("Initializing gdal driver proxy module");
  #endif

  CCI_PRECONDITION(!gInitialized, "gdal module already initialized");
  if(gInitialized) {
     return CCI_OK;
  }

  GDALAllRegister();

  // Set Error handlers
  CPLSetErrorHandler( gdalErrorHandler );

  gInitialized = DM_TRUE;
  return CCI_OK;
}

static void cciGDALProxyModuleShutDown(cciIModule* aSelf)
{
  #ifdef DEBUG
    dmLOG("Shutdown gdal generic driver");
  #endif

  GDALDestroyDriverManager();
  gInitialized = DM_FALSE;
}


CCI_IMPL_GETMODULE_WITH_CTOR_DTOR(cciGDALProxyModule,
                                  components,
                                  cciGDALProxyModuleInit,
                                  cciGDALProxyModuleShutDown)

