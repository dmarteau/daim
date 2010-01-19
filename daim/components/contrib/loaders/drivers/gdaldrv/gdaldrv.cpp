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
// File         : gdalmindrv.cpp
// Date         : 27 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#define dmUseUtilitiesExtra
#define dmUseKernelImageTemplates

#include "cciCOMPtr.h"
#include "ccidaim.h"
#include "cciString.h"
#include "cciISurface.h"
#include "cciISurfaceDriver.h"
#include "cciIInterfaceRequestor.h"
#include "cciInterfaceRequestorUtils.h"
#include "cciILoaderService.h"
#include "cciIMetaDataContainer.h"
#include "cciIColorTable.h"
#include "cciIRemoteSurface.h"

#include "gdaldrv.h"
#include "daim_kernel.h"

#define CPL_SUPRESS_CPLUSPLUS
#include "cpl_string.h"

#include "gdalSurface.h"
#include "gdalColorTable.h"

///////////////////////////////////////////////////
// cciGDALDriver implementation
class cciGDALDriver : public cciISurfaceDriver
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_ISURFACEDRIVER

  cciGDALDriver();

  cci_result Init( GDALDriverH aDriver );

public:
  ~cciGDALDriver();

protected:
  GDALDriverH mDriver;
  cci_Ptr<cciIInterfaceRequestor> mCallBacks;
};

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciGDALDriver, cciISurfaceDriver)

cciGDALDriver::cciGDALDriver()
: mDriver(dm_null)
{
}



cciGDALDriver::~cciGDALDriver()
{
}


cci_result cciGDALDriver::Init( GDALDriverH aDriver )
{
  CCI_ENSURE_FALSE(mDriver,CCI_ERROR_ALREADY_INITIALIZED);

  mDriver = aDriver;
  return CCI_OK;
}

/* attribute cciIInterfaceRequestor callbacks; */
CCI_IMETHODIMP cciGDALDriver::GetCallbacks(cciIInterfaceRequestor * *aCallbacks)
{
  CCI_ENSURE_ARG_POINTER(aCallbacks);

  CCI_IF_ADDREF(*aCallbacks = mCallBacks);
  return CCI_OK;
}
CCI_IMETHODIMP cciGDALDriver::SetCallbacks(cciIInterfaceRequestor *aCallbacks)
{
  mCallBacks = aCallbacks;
  return CCI_OK;
}

/* [noscript] cciISurface createSurfaceFromData (in string location, in dmImageDataRef imData, [array, size_is (count)] in dm_uint8 alphaBits, in long alphaStride, in string options); */
CCI_IMETHODIMP cciGDALDriver::CreateSurfaceFromData(const char * location, dmImageData & imData, 
                                                    dm_uint8 *alphaBits, dm_int32 alphaStride, 
                                                    const char * options, 
                                                    cciISurface * *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_TRUE(mDriver,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(_retval);

  if(GDALGetMetadataItem( mDriver,GDAL_DCAP_CREATECOPY,NULL ) == NULL)
     return CCI_ERROR_NOT_AVAILABLE;
  
  cci_Ptr<gdalSurface> surf;
  cci_result rv = gdalSurface::Create(mDriver,location,imData,alphaBits,alphaStride,options,
                                      getter_AddRefs(surf));

  CCI_IF_ADDREF( *_retval = surf );
  return rv;
}


/* cciISurface createSurface (in string location, in dm_uint32 width, in dm_uint32 height, in EPixelFormat format, in dm_bool hasAlpha, [array, size_is (count)] in string options, in dm_uint32 count); */
CCI_IMETHODIMP cciGDALDriver::CreateSurface(const char * location, dm_uint32 width, dm_uint32 height,
                                            EPixelFormat format, dm_bool hasAlpha,
                                            const char *options,
                                            cciISurface * *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_TRUE(mDriver,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(location);
  CCI_ENSURE_ARG_POINTER(_retval);

  if(GDALGetMetadataItem( mDriver,GDAL_DCAP_CREATE,NULL ) == NULL)
     return CCI_ERROR_NOT_AVAILABLE;
  
  cci_Ptr<gdalSurface> surf;
  cci_result rv = gdalSurface::Create(mDriver,location,width,height,format,hasAlpha,options,
                                      getter_AddRefs(surf));
  
  CCI_IF_ADDREF( *_retval = surf );
  return rv;
}

/* boolean isCompatibleDriver (in string type); */
CCI_IMETHODIMP cciGDALDriver::IsCompatibleDriver(const char * type, dm_bool *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_TRUE(mDriver,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(type);
  CCI_ENSURE_ARG_POINTER(_retval);

  const char* mime = GDALGetMetadataItem( mDriver, GDAL_DMD_MIMETYPE, NULL );

  *_retval = ((mime && strcmp(type,mime)==0)||strcmp(type,GDALGetDriverShortName(mDriver))==0);

  return CCI_OK;
}

/* readonly attribute boolean hasCreateCapabilities; */
CCI_IMETHODIMP cciGDALDriver::GetHasCreateCapabilities(dm_bool *aHasCreateCapabilities)
{
  CCI_ENSURE_TRUE(mDriver,CCI_ERROR_NOT_INITIALIZED);

  *aHasCreateCapabilities = DM_FALSE;

  if(GDALGetMetadataItem( mDriver, GDAL_DCAP_CREATE    , NULL ) != NULL ||
     GDALGetMetadataItem( mDriver, GDAL_DCAP_CREATECOPY, NULL ) != NULL )
    *aHasCreateCapabilities = DM_TRUE;

  return CCI_OK;
}


static CCI_METHOD writeMetaDataCallback(cciIMetaDataContainer *aSrcContainer, void *aClosure,
                                        const char* aDomain, char** data)
{
  GDALDatasetH hDS = reinterpret_cast<GDALDatasetH>(aClosure);

  CPLErr err = GDALSetMetadata(hDS,data,aDomain);
  if(err != CE_None)
     return CCI_ERROR_FAILURE;

  return CCI_OK;
}


/* [noscript] void saveImageBits (in string newLocation, in dmImageDataRef imData, in string options); */
CCI_IMETHODIMP cciGDALDriver::SaveImageBits(const char * newLocation, dmImageData & imData, const char * options )
{
  return SaveImageBitsWithAlpha(newLocation,imData,dm_null,0,dm_null,dm_null,options);
}

/* [noscript] void saveImageBitsWithAlpha (in string newLocation, in dmImageDataRef imData, [array, size_is (count)] in dm_uint8 alphaBits, in long alphaStride, in cciIMetaDataContainer exif, in cciIColorTable colorTable, in string options); */
CCI_IMETHODIMP cciGDALDriver::SaveImageBitsWithAlpha(const char * newLocation, dmImageData & imData, 
                                                     dm_uint8 *alphaBits, dm_int32 alphaStride, 
                                                     cciIMetaDataContainer *exif, 
                                                     cciIColorTable *colorTable, 
                                                     const char * options)
{
  CCI_ENSURE_TRUE(mDriver,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(newLocation);

  char** createOpts = dm_null;

  GDALDatasetH hDS = dm_null;

  cci_result rv = gdalSurface::CreateInMemorySurface(imData,alphaBits,alphaStride,hDS);
  CCI_ENSURE_SUCCESS(rv,rv);

  if(options)
     createOpts = CSLTokenizeStringComplex(options,",",DM_FALSE,DM_FALSE);

  // Clone meta data
  if(exif)
  {
    rv = exif->ReadMetaData(writeMetaDataCallback,hDS,"");
    if(rv == CCI_ERROR_NOT_AVAILABLE)
       rv = CCI_OK;

    if(CCI_FAILED(rv))
       dmLOG_ERROR("Failed to copy meta data !");

  }
  
  // Copy color table
  if(colorTable && imData.PixelFormat == dmPixelFormat8bppIndexed)
  {
    GDALRasterBandH hBand = GDALGetRasterBand(hDS,1);
    CCI_ENSURE_TRUE(hBand,CCI_ERROR_UNEXPECTED);

    GDALColorTableH hColorTable = GDALCreateColorTable(GPI_RGB);
    CCI_ENSURE_TRUE(hColorTable,CCI_ERROR_OUT_OF_MEMORY);
    
    // Copy data from src colortable
    rv = gdalColorTable::copyColorTable(colorTable,hColorTable);
    if(CCI_SUCCEEDED(rv))
    {
      CPLErr err = GDALSetRasterColorTable(hBand,hColorTable);
      GDALDestroyColorTable(hColorTable);

      if(err != CE_None)
        dmLOG_ERROR("Failed to copy color table !");
    }
  }
  
  //TODO set progress data from callbacks
  GDALDatasetH newDS = GDALCreateCopy(mDriver,newLocation,hDS,DM_FALSE,createOpts,
                                      dm_null,dm_null);
  if(newDS)
    GDALClose(newDS);
  else
    rv = CCI_ERROR_FAILURE;

  if(createOpts)
     CSLDestroy( createOpts );

  GDALClose(hDS);

  return rv;
}


/* void Unlink (in string location); */
CCI_IMETHODIMP cciGDALDriver::Unlink(const char * location)
{
  CCI_ENSURE_TRUE(mDriver,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(location);

  if(GDALDeleteDataset(mDriver,location)!=CE_None)
     return CCI_ERROR_FAILURE;

  return CCI_OK;
}

/* void Rename (in string newLocation, in string oldLocation); */
CCI_IMETHODIMP cciGDALDriver::Rename(const char * newLocation, const char * oldLocation)
{
  CCI_ENSURE_TRUE(mDriver,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(newLocation);
  CCI_ENSURE_ARG_POINTER(oldLocation);

  if(GDALRenameDataset(mDriver,newLocation,oldLocation)!=CE_None)
     return CCI_ERROR_FAILURE;

  return CCI_OK;
}

/* void CopyFiles (in string newLocation, in string oldLocation); */
CCI_IMETHODIMP cciGDALDriver::CopyFiles(const char * newLocation, const char * oldLocation)
{
  CCI_ENSURE_TRUE(mDriver,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(newLocation);
  CCI_ENSURE_ARG_POINTER(oldLocation);

  if(GDALCopyDatasetFiles(mDriver,newLocation,oldLocation)!=CE_None)
     return CCI_ERROR_FAILURE;

  return CCI_OK;
}

//=====================================
// Utilities
//=====================================

cci_result CCI_NewGDALDriver( GDALDriverH aDriver  , cciISurfaceDriver* *result )
{
  CCI_ENSURE_ARG_POINTER(aDriver);

  cciGDALDriver* pDriver = new cciGDALDriver();

  cci_result rv = pDriver->Init(aDriver);
  if(CCI_FAILED(rv))
  {
    delete pDriver;
  }

  CCI_IF_ADDREF( *result = pDriver);
  return rv;
}
