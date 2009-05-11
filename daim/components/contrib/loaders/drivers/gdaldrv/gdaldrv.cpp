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

  cci_result CreateTemporarySurface( dmImageData & imData, GDALDatasetH& hDS );
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


cci_result cciGDALDriver::CreateTemporarySurface( dmImageData & imData, GDALDatasetH& hDS)
{
  GDALDataType dataType = resolveDataType(imData.PixelFormat);
  if(dataType == GDT_Unknown)
  {
    dmLOG_ERROR("Type not suppported by driver !");
    return CCI_ERROR_FAILURE;
  }

  int Bands       = 1;
  int PixelStride = dmGetPixelFormatBits(imData.PixelFormat) >> 3;
  int LineStride  = imData.Stride;
  int BandStride  = imData.Stride * imData.Height;

  const char* pszType = GDALGetDataTypeName(dataType);

  if(imData.PixelFormat==dmPixelFormat24bppRGB) {
    Bands      = 3;
    BandStride = 1;
  }

  dmCString buf = dmCString::FormatString(256,
                                "MEM:::DATAPOINTER=%ld,PIXELS=%ld,LINES=%ld,"
                                "BANDS=%ld,DATATYPE=%s,PIXELOFFSET=%ld,"
                                "LINEOFFSET=%ld,BANDOFFSET=%ld",
                                 imData.Scan0,
                                 imData.Width,
                                 imData.Height,
                                 Bands,pszType,
                                 PixelStride,
                                 LineStride,
                                 BandStride);

  hDS = GDALOpen(buf.get(),GA_Update);

  return hDS ? CCI_OK : CCI_ERROR_UNEXPECTED;

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


/* cciISurface createSurface (in string location, in dm_uint32 width, in dm_uint32 height, in EPixelFormat format, in dm_bool hasAlpha, [array, size_is (count)] in string options, in dm_uint32 count); */
CCI_IMETHODIMP cciGDALDriver::CreateSurface(const char * location, dm_uint32 width, dm_uint32 height,
                                            EPixelFormat format, dm_bool hasAlpha,
                                            const char *options,
                                            cciISurface * *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_TRUE(mDriver,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(location);
  CCI_ENSURE_ARG_POINTER(_retval);

  return CCI_NewGDALSurface(mDriver,location,width,height,format,hasAlpha,options,_retval);
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


/* [noscript] void saveImageBits (in string newLocation, in dmImageDataRef imData, in cciIMetaDataContainer exif, in string options); */
CCI_IMETHODIMP cciGDALDriver::SaveImageBits(const char * newLocation, dmImageData & imData,
                                            cciIMetaDataContainer *exif, const char * options )
{
  CCI_ENSURE_TRUE(mDriver,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(newLocation);

  char** createOpts = dm_null;

  GDALDatasetH hDS = dm_null;

  cci_result rv = CreateTemporarySurface(imData,hDS);
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

  //TODO set progress data from callbacks
  GDALDatasetH newDS = GDALCreateCopy(mDriver,newLocation,hDS,DM_FALSE,createOpts,
                                      dm_null,dm_null);
  if(newDS)
  {
    GDALClose(newDS);
    rv = CCI_OK;
  }
  else
     rv = CCI_ERROR_FAILURE;

  if(createOpts)
     CSLDestroy( createOpts );

  GDALClose(hDS);

  return rv;
}


/* [noscript] cciIRemoteSurface createTmpSurfaceFromImageBits (in dmImageDataRef imData, in cciIMetaDataContainer exif); */
CCI_IMETHODIMP cciGDALDriver::CreateTmpSurfaceFromImageBits(dmImageData & imData, cciIMetaDataContainer *exif,
                                                            cciIRemoteSurface * *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(_retval);

  GDALDatasetH hDS = dm_null;

  cci_result rv = CreateTemporarySurface(imData,hDS);
  CCI_ENSURE_SUCCESS(rv,rv);

  if(exif)
  {
    rv = exif->ReadMetaData(writeMetaDataCallback,hDS,"");
    if(rv == CCI_ERROR_NOT_AVAILABLE)
       rv = CCI_OK;

    if(CCI_FAILED(rv))
       dmLOG_ERROR("Failed to copy meta data !");
  }

  cci_Ptr<cciISurface> tmpSurface;

  rv = CCI_NewGDALSurface(hDS,GA_Update,mDriver,getter_AddRefs(tmpSurface));
  if(CCI_SUCCEEDED(rv))
     rv = CallQueryInterface(tmpSurface,_retval);

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
