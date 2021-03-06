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
// File         : nullsurface.cpp
// Date         : 28 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------


#define dmUseUtilitiesExtra
#define dmUseKernelImageTemplates

#include "cciCOMPtr.h"
#include "ccidaim.h"
#include "cciString.h"
#include "cciILoaderService.h"
#include "cciIStringIterator.h"
#include "cciIStringEnumerator.h"
#include "cciIInterfaceRequestor.h"
#include "cciIColorTable.h"
#include "cciIImageContainer.h"

#include "gdaldrv.h"
#include "daim_kernel.h"

#define CPL_SUPRESS_CPLUSPLUS
#include "cpl_string.h"

#include "gdalSurface.h"
#include "gdalColorTable.h"

/* gdalSurface Implementation */
CCI_IMPL_ISUPPORTS5(gdalSurface, cciISurface,
                                 cciIRemoteSurface,
                                 cciIScriptableSurface,
                                 cciIMetaDataContainer,
                                 gdalMetadata)

static GDALColorInterp bandsInterp[5] = {
  GCI_Undefined,                                  
  GCI_RedBand,
  GCI_GreenBand,
  GCI_BlueBand,
  GCI_AlphaBand
};

                                 
gdalSurface::gdalSurface()
: mDS(dm_null)
, mOutputDriver(dm_null)
, mTmpDriver(dm_null)
, mAccess(GA_ReadOnly)
, mAlphaBand(0)
, mFormat(dmPixelFormatUndefined)
, mRequireUserBuffer(false)
, mCreateOpts(dm_null)
, mLock(0)
{
}

gdalSurface::~gdalSurface()
{
  Close();
}

// Static method
cci_result gdalSurface::CreateInMemorySurface( const dmImageData& imData, GDALDatasetH& hDS )
{
  GDALDataType dataType = resolveDataType(imData.PixelFormat);
  if(dataType == GDT_Unknown) {
    dmLOG_ERROR("Type not suppported by driver !");
    return CCI_ERROR_INVALID_ARG;
  }

  int nBands      = 1;
  int PixelStride = dmGetPixelFormatBits(imData.PixelFormat) >> 3;
  int LineStride  = imData.Stride;
  int BandStride  = imData.Stride * imData.Height;

  const char* pszType = GDALGetDataTypeName(dataType);
  
  dm_byte* bands[4];
  dm_byte* pScan0 = reinterpret_cast<dm_byte*>(imData.Scan0);

  if(imData.PixelFormat==dmPixelFormat24bppRGB) {
    nBands     = 3;
    BandStride = 1; // 1 Byte between each band 
  } else
  if(imData.PixelFormat==dmPixelFormat32bppARGB) { // Take alpha channel 
    nBands     = 4;
    BandStride = 1;  // 1 Byte between each band    
  }
   
  if(nBands>1) {
    #ifdef DM_IS_LITTLE_ENDIAN
    // BGRA
    bands[0] = pScan0 + 2; // Red
    bands[1] = pScan0 + 1; // Green
    bands[2] = pScan0 + 0; // Blue      
    bands[3] = pScan0 + 3; // Alpha      
    #else
    // ARGB
    bands[0] = pScan0 + 1; // Red
    bands[1] = pScan0 + 2; // Green
    bands[2] = pScan0 + 3; // Blue
    bands[3] = pScan0 + 0; // Alpha
    #endif
  } else {
    bands[0] = pScan0;
  }
  
  dmCString buf = dmCString::FormatString(256,
                                "MEM:::DATAPOINTER=%ld,PIXELS=%ld,LINES=%ld,"
                                "BANDS=1,DATATYPE=%s,PIXELOFFSET=%ld,"
                                "LINEOFFSET=%ld,BANDOFFSET=%ld",bands[0],
                                (long)imData.Width,
                                (long)imData.Height,
                                 pszType,
                                 (long)PixelStride,
                                 (long)LineStride,
                                 (long)BandStride);

  hDS = GDALOpen(buf.get(),GA_Update);
  if(!hDS)
     return CCI_ERROR_FAILURE;
  
  cci_result rv = CCI_OK;
  

  if(nBands > 1)
  {
    // Set band interpretation
    GDALSetRasterColorInterpretation(GDALGetRasterBand(hDS,1),bandsInterp[1]);
  
    // Add remaining bands
    for(int i=1;i<nBands;++i) 
    {
      char **papszOptions = dm_null;
      papszOptions = CSLAppendPrintf(papszOptions,"DATAPOINTER=%ld",(long)bands[i]);
      papszOptions = CSLAppendPrintf(papszOptions,"PIXELOFFSET=%ld",(long)PixelStride);
      papszOptions = CSLAppendPrintf(papszOptions,"LINEOFFSET=%ld" ,(long)LineStride);
      papszOptions = CSLAppendPrintf(papszOptions,"BANDOFFSET=%ld" ,(long)BandStride);
  
      CPLErr err = GDALAddBand(hDS,GDT_Byte,papszOptions);
  
      CSLDestroy( papszOptions );
      if(err != CE_None) {
         rv = CCI_ERROR_FAILURE;
         break;
      }
      
      GDALSetRasterColorInterpretation(GDALGetRasterBand(hDS,i+1),bandsInterp[i+1]);
    }
  }
  
  if(CCI_FAILED(rv)) {
     GDALClose(hDS);
     hDS = NULL;
  } 

  return rv;
}

// Init from dataset
cci_result gdalSurface::Init(GDALDatasetH aDS, dm_uint32 ioFlags, GDALDriverH tmpDriver )
{
  CCI_ENSURE_FALSE(mDS,CCI_ERROR_ALREADY_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(aDS);

  int nBands = GDALGetRasterCount(aDS);

  if(nBands>=3 &&
     GDALGetRasterColorInterpretation(GDALGetRasterBand(aDS,1))==GCI_RedBand   &&
     GDALGetRasterColorInterpretation(GDALGetRasterBand(aDS,2))==GCI_GreenBand &&
     GDALGetRasterColorInterpretation(GDALGetRasterBand(aDS,3))==GCI_BlueBand) 
  {
     mFormat = dmPixelFormat24bppRGB;
     
     // Check for alpha band
     if(GDALGetRasterColorInterpretation(GDALGetRasterBand(aDS,nBands))==GCI_AlphaBand)
        mAlphaBand = nBands;
  }
  else
  {
    // Other color format, use band access
    // Set default format as the first data format

    mFormat = resolveFormat(GDALGetRasterDataType(GDALGetRasterBand(aDS,1)));
    if(mFormat == dmPixelFormatUndefined) {
      dmLOG_ERROR("Unsupported IO data format !");
      return CCI_ERROR_FAILURE;
    }
  }

  mDS                = aDS;
  mOutputDriver      = dm_null;
  mTmpDriver         = tmpDriver;
  mRequireUserBuffer = true;
  mAccess            = static_cast<GDALAccess>(ioFlags);

  return CCI_OK;
}

// Init from image data
cci_result gdalSurface::Init(GDALDriverH aDriver,const char * location, dmImageData& imData,
                             const char *options)
{
  // Ensure that provided driver has create copy capabilities
  if(aDriver && GDALGetMetadataItem(aDriver,GDAL_DCAP_CREATECOPY,NULL) == NULL)
     return CCI_ERROR_NOT_AVAILABLE;

  GDALDatasetH hDS = dm_null;

  // Wrap image data
  dmImageDescriptor* pDesc = dmGetDescriptor(imData.PixelFormat);
  if(pDesc) {
    mImageBuffer = pDesc->CreateImage(imData);
    if(!mImageBuffer)
       return CCI_ERROR_OUT_OF_MEMORY;
    
  } else {
    // Huuu, no descriptor ?
    return CCI_ERROR_UNEXPECTED;
  }
  
  // Create in-memory gdal surface
  cci_result rv = CreateInMemorySurface(imData,hDS);
  if(CCI_FAILED(rv))
     return rv;

  // Clone options
  if(options)
     mCreateOpts = CSLTokenizeStringComplex(options,",",false,false);

  mFormat            = pDesc->PixelFormat();

  mDS                = hDS;
  mLocation          = location;
  mOutputDriver      = aDriver;
  mRequireUserBuffer = false;
  mAccess            = GA_Update;
  
  // Set alpha band
  mAlphaBand         = imData.PixelFormat == dmPixelFormat32bppARGB ? 4 : 0;

  return rv;
}

// Init from a driver
cci_result gdalSurface::Init(GDALDriverH aDriver,const char * location,
                             dm_uint32 width, dm_uint32 height,
                             EPixelFormat format,
                             const char *options)
{
  CCI_ENSURE_ARG_POINTER(aDriver);
  CCI_ENSURE_ARG_POINTER(location);

  GDALDataType dataType = resolveDataType(format);
  GDALDatasetH hDS      = dm_null;

  if(dataType == GDT_Unknown) {
    dmLOG_ERROR("Type not suppported by driver !");
    return CCI_ERROR_FAILURE;
  }

  if(GDALGetMetadataItem(aDriver,GDAL_DCAP_CREATE,NULL ) == NULL)
     return CCI_ERROR_NOT_AVAILABLE;
    
  char** createOpts = dm_null;

  if(options)
     createOpts = CSLTokenizeStringComplex(options,",",false,false);

  int nBands = 1;
  if(format==dmPixelFormat24bppRGB)
     nBands = 3;
  else
  if(format==dmPixelFormat32bppARGB)
     nBands = 4;
    
  hDS = GDALCreate(aDriver,location,width,height,nBands,dataType,createOpts);
  if(createOpts)
     CSLDestroy(createOpts);

  if(!hDS)
     return CCI_ERROR_FAILURE;

  // Set alpha channel
  if(nBands > 1) {
    for(int i=1;i<=nBands;++i) 
      GDALSetRasterColorInterpretation( GDALGetRasterBand(hDS,i),bandsInterp[i] );
  }
    
  mDS                = hDS;
  mLocation          = location;
  mFormat            = format;
  mOutputDriver      = dm_null;
  mRequireUserBuffer = true;
  mAccess            = GA_Update;
  mAlphaBand         = 0; 
 
  // Set alpha band
  if(format==dmPixelFormat32bppARGB) {
    mAlphaBand = 4;
    mFormat    = dmPixelFormat24bppRGB;
  }
  
  return CCI_OK;
}

//==================================
// cciISurface
//==================================

/* [noscript,notxpcom] dm_uint32 Width (); */
CCI_IMETHODIMP_(dm_uint32) gdalSurface::Width()
{
  return GDALGetRasterXSize(mDS);
}

/* [noscript,notxpcom] dm_uint32 Height (); */
CCI_IMETHODIMP_(dm_uint32) gdalSurface::Height()
{
  return GDALGetRasterYSize(mDS);
}

/* [noscript,notxpcom] dm_uint32 Bands (); */
CCI_IMETHODIMP_(dm_uint32) gdalSurface::Bands()
{
  return GDALGetRasterCount(mDS);
}

/* [noscript,notxpcom] EPixelFormat PixelFormat (); */
CCI_IMETHODIMP_(EPixelFormat) gdalSurface::PixelFormat()
{
  return mFormat;
}

/* [noscript,notxpcom] boolean HasAlpha (); */
CCI_IMETHODIMP_(bool) gdalSurface::HasAlpha()
{
  return mAlphaBand!=0;
}

/* [noscript,notxpcom] boolean HasSamplingCapabilities (); */
CCI_IMETHODIMP_(bool) gdalSurface::HasSamplingCapabilities()
{
  return true;
}

/* [noscript,notxpcom] boolean HasConvertCapabilities(); */
CCI_IMETHODIMP_(bool) gdalSurface::HasConvertCapabilities()
{
  return true;
}

/* [noscript,notxpcom] boolean IsLocked (); */
CCI_IMETHODIMP_(bool) gdalSurface::IsLocked()
{
  return mLock != 0;
}

/* [noscript,notxpcom] boolean IsWritable (); */
CCI_IMETHODIMP_(bool) gdalSurface::IsWritable()
{
  return mAccess==GA_Update;
}


/* readonly attribute dm_uint32 width; */
CCI_IMETHODIMP gdalSurface::GetWidth(dm_uint32 *aWidth)
{
  CCI_ENSURE_TRUE(mDS,CCI_ERROR_NOT_INITIALIZED);
  *aWidth = Width();
  return CCI_OK;
}

/* readonly attribute dm_uint32 height; */
CCI_IMETHODIMP gdalSurface::GetHeight(dm_uint32 *aHeight)
{
  CCI_ENSURE_TRUE(mDS,CCI_ERROR_NOT_INITIALIZED);
  *aHeight = Height();
  return CCI_OK;
}

/* readonly attribute CCI_OK bands; */
CCI_IMETHODIMP gdalSurface::GetBands(dm_uint32 *aBands)
{
  CCI_ENSURE_TRUE(mDS,CCI_ERROR_NOT_INITIALIZED);
  *aBands = Bands();
  return CCI_OK;
}

/* readonly attribute EPixelFormat pixelFormat; */
CCI_IMETHODIMP gdalSurface::GetPixelFormat(EPixelFormat *aPixelFormat)
{
  *aPixelFormat = PixelFormat();
  return CCI_OK;
}

/* readonly attribute bool hasAlpha; */
CCI_IMETHODIMP gdalSurface::GetHasAlpha(bool *aHasAlpha)
{
  *aHasAlpha = HasAlpha();
  return CCI_OK;
}

cci_result gdalSurface::CreateUserBuffer(dmRect & rect, EPixelFormat format, dmImageData& imData)
{
   if(format==dmPixelFormatUndefined)
      format = mFormat;

   EPixelFormat bufferFmt = format;
   if(bufferFmt == dmPixelFormat32bppARGB)
     bufferFmt = dmPixelFormat24bppRGB;

  if(mImageBuffer.IsNull() || mImageBuffer->PixelFormat()!=bufferFmt
  || mImageBuffer->Width()  != rect.Width()
  || mImageBuffer->Height() != rect.Height())
  {
    dmImageDescriptor* pDesc = dmGetDescriptor(bufferFmt);
    if(pDesc)
    {
      mImageBuffer = pDesc->CreateImage(mLockRect.Width(),mLockRect.Height());

      if(!mImageBuffer)
         return CCI_ERROR_OUT_OF_MEMORY;

      mImageBuffer->GetImageData(imData);
      
      // Ensure that imData is returned with the correct format
      imData.PixelFormat = format;
      
      return CCI_OK;
    }
  }

  return CCI_ERROR_FAILURE;
}

static void FillAlphaChannel( dm_byte* pData, int w, int h, int nLineSpace )
{
  // Fill Alpha channel with 0xff
  for(int j=0;j<h;++j) {
    dm_uint32* pixbuf = reinterpret_cast<dm_uint32*>(pData);
    for(int i=0;i<w;++i,pixbuf++) {
      *pixbuf = 0xff000000;
    }
    pData += nLineSpace;
  }
}


static void MapColorTable( GDALColorTableH hColorTable, EPixelFormat format, dmImageData& imData )
{
  dm_uint32 entryCount = GDALGetColorEntryCount(hColorTable);
  GDALColorEntry colors[256],*entry;

  if(entryCount > 256) {
     dmLOG_WARN("Truncating colortable to 256 (%d original entries)",entryCount);
     entryCount = 256;
  }

  for(dm_uint32 i=0;i<entryCount;++i)
     GDALGetColorEntryAsRGB(hColorTable,i,&colors[i]);  

  dm_byte* pixbuf = static_cast<dm_byte*>(imData.Scan0);
  
  int width  = imData.Width;
  int height = imData.Height;
  int stride = imData.Stride;
  
  if(format == dmPixelFormat32bppARGB)
  {
    for(int y=0;y<height;++y) {
      for(int x=0;x<width;++x) {
        dm_uint32&  pixel = ((dm_uint32*)(pixbuf))[x];        
        entry = &colors[(pixel & 0x00ff0000) >> 16];
        pixel = DM_ARGB(entry->c4,entry->c1,entry->c2,entry->c3);
      }
      pixbuf += stride;    
    }
  }
  else
  {
    // Assume RGB
    for(int y=0;y<height;++y) {
      for(int x=0;x<width;++x) {
        dmRGBColor&  pixel = ((dmRGBColor*)(pixbuf))[x];        
        entry = &colors[pixel.r];
        pixel.a = 0xff;
        pixel.r = entry->c1;
        pixel.g = entry->c2;
        pixel.b = entry->c3;
      }
      pixbuf += stride;    
    }    
  }
}


cci_result gdalSurface::ReadData(EPixelFormat format, dmImageData& imData, dm_uint32 lockModes)
{
  GDALDataType dataType = resolveDataType(format);

  if(dataType == GDT_Unknown)
  {
    dmLOG_ERROR("Unknown data type !");
    return CCI_ERROR_FAILURE;
  }

  int nPixelSpace = dmGetPixelFormatBits(format) >> 3;
  int nLineSpace  = imData.Stride;

  dm_byte* pScan0 = static_cast<dm_byte*>(imData.Scan0);

  int nBandSpace;
  int nBandCount;
  int panBanMap[4] = { 1,2,3,4 };

  GDALColorTableH hColorTable = dm_null;
  
  // We are putting data into rgb format 
  if(format == dmPixelFormat24bppRGB || format == dmPixelFormat32bppARGB)
  {
    // Source format is scalar
    if(dmIsPixelFormatScalar(mFormat))
    {
      nBandCount = 3;
      nBandSpace = 1;

      // Assign all channels to same band
      panBanMap[0] = panBanMap[1] = panBanMap[2] = 1;
      
      GDALRasterBandH hBand = GDALGetRasterBand(mDS,1);
      if(GDALGetRasterColorInterpretation(hBand)==GCI_PaletteIndex)
         hColorTable = GDALGetRasterColorTable(hBand);

      // Fill alpha channel 
      FillAlphaChannel(pScan0,imData.Width,imData.Height,nLineSpace);
      #ifndef DM_IS_LITTLE_ENDIAN
      pScan0 += 1; // Advance scan pointer to skip alpha channel
      #endif
    }
    else
    {
      // Source format is (A)RGB      
      nBandCount = 3;
      nBandSpace = 1;

      if(format == dmPixelFormat32bppARGB && mAlphaBand)
      {
        nBandCount   = 4;
        #ifdef DM_IS_LITTLE_ENDIAN
          panBanMap[0] = 3; //B
          panBanMap[1] = 2; //G
          panBanMap[2] = 1; //R
          panBanMap[3] = mAlphaBand;          
        #else
          panBanMap[0] = mAlphaBand;
          panBanMap[1] = 1;
          panBanMap[2] = 2;
          panBanMap[3] = 3;
        #endif
      }
      else
      {
        if(format == dmPixelFormat32bppARGB)
           FillAlphaChannel(pScan0,imData.Width,imData.Height,nLineSpace);          
        #ifdef DM_IS_LITTLE_ENDIAN
          panBanMap[0] = 3;
          panBanMap[1] = 2;
          panBanMap[2] = 1;
        #else
          pScan0 += 1; // Advance scan pointer to skip alpha channel
        #endif        
      }
    }
  }
  else // Destination format is scalar
  if(dmIsPixelFormatScalar(format))
  {
    nBandCount = 1;
    nBandSpace = imData.Stride * imData.Height;

    // Get a specific band if required or band 1 otherwise
    int band = (int)((ELockBandMask & lockModes) >> 16);
    if(band == 0) band = 1;
    else if(band > GDALGetRasterCount(mDS))
      return CCI_ERROR_OUT_OF_RANGE;

    panBanMap[0] = band;
  }
  else
    return CCI_ERROR_INVALID_ARG;

  // Do gdal IO
  CPLErr err = GDALDatasetRasterIO(mDS,GF_Read,
                                   mLockRect.Left(),
                                   mLockRect.Top(),
                                   mLockRect.Width(),
                                   mLockRect.Height(),
                                   pScan0,
                                   imData.Width,
                                   imData.Height,
                                   dataType, nBandCount, panBanMap,
                                   nPixelSpace, nLineSpace, nBandSpace);

  // Do we need to map any color map ?
  if(hColorTable) 
    MapColorTable(hColorTable,format,imData);
  
  return (err != CE_None) ? CCI_ERROR_FAILURE : CCI_OK;
}

/* [noscript] void lockBitsRect (in dmRectRef rect, in EPixelFormat format, in dmImageDataRef imData, in unsigned long lockModes); */
CCI_IMETHODIMP gdalSurface::LockBitsRect(dm_rect & rect, EPixelFormat format, dmImageData & imData,
                                         dm_uint32 lockModes)
{
  CCI_ENSURE_TRUE(mDS,CCI_ERROR_NOT_INITIALIZED);

  if(mLock != 0)
     return CCI_ERROR_FAILURE;

  if(lockModes == 0)
     return CCI_ERROR_INVALID_ARG;

  if((lockModes & ELockWrite)!=0 && mAccess!=GA_Update) {
    dmLOG_ERROR("Cannot lock write a read-only opened resource !!");
    return CCI_ERROR_FAILURE;
  }

  mLockRect = rect;

  // Clip user ROI
  if(!mLockRect.Clip(dmRect(0,0,Width(),Height())))
     return CCI_ERROR_OUT_OF_RANGE;

  cci_result rv;
  
  if((lockModes & ELockUserBuffer)==0)
  {
    if(mRequireUserBuffer)
    {
      // This will initialise imData
      rv = CreateUserBuffer(mLockRect,format,imData);
      if(CCI_FAILED(rv))
         return rv;

      // Read data if required
      if((lockModes & ELockRead)!= 0)
         rv = ReadData(imData.PixelFormat,imData,lockModes);
    }
    else
    {
      if(format==dmPixelFormatUndefined)
        format = mImageBuffer->PixelFormat();
      else
        if(format==dmPixelFormat32bppARGB && mImageBuffer->PixelFormat()!=dmPixelFormat24bppRGB)
           return CCI_ERROR_INVALID_ARG;
      else
        if(format != mImageBuffer->PixelFormat())
          return CCI_ERROR_INVALID_ARG;
      
      dmImageData srcData;
      mImageBuffer->GetImageData(srcData);

      size_t byteStart = mLockRect.Left() * (dmGetPixelFormatBits(format) >> 3);

      imData.PixelFormat = format;
      imData.Width       = mLockRect.Width();
      imData.Height      = mLockRect.Height();
      imData.Stride      = srcData.Stride;
      imData.Scan0       = 
          static_cast<dm_byte*>(srcData.Scan0) + (srcData.Stride * mLockRect.Top()) + byteStart;
      
      rv = CCI_OK;
    }
  }
  else
  {
    // Read data if required
    if((lockModes & ELockRead)!= 0)
       rv = ReadData(format,imData,lockModes);
    else
       rv = CCI_OK; // Nothing more to do
  }

  mLock = lockModes;
  return rv;
}


/* [noscript] void lockBits (in EPixelFormat format, in dmImageDataRef imData, in unsigned long lockModes); */
CCI_IMETHODIMP gdalSurface::LockBits(EPixelFormat format, dmImageData & imData, dm_uint32 lockModes)
{
  dmRect rect(0,0,Width(),Height());
  return LockBitsRect(rect,format,imData,lockModes);
}

/* [noscript] void unlockBits (in dmImageDataRef imData); */
CCI_IMETHODIMP gdalSurface::UnlockBits(dmImageData & imData)
{
  if(mLock==0)
     return CCI_ERROR_UNEXPECTED;

  cci_result rv = CCI_OK;

  if((mLock & ELockWrite)!=0)
  {
    if((mLock & ELockUserBuffer)!=0 || mRequireUserBuffer)
    {
      EPixelFormat format   = imData.PixelFormat;
      GDALDataType dataType = resolveDataType(format);

      if(dataType == GDT_Unknown)
      {
        dmLOG_ERROR("Unknown data type !");
        return CCI_ERROR_FAILURE;
      }

      int nPixelSpace = dmGetPixelFormatBits(format) >> 3;
      int nLineSpace  = imData.Stride;

      dm_byte* pScan0 =  static_cast<dm_byte*>(imData.Scan0);

      int nBandSpace;
      int nBandCount;
      int panBanMap[4] = { 1,2,3,4 };

      if(format == dmPixelFormat24bppRGB || format == dmPixelFormat32bppARGB)
      {
        // Ensure that we are in compatible format
        if(dmIsPixelFormatScalar(mFormat)) {
          rv = CCI_ERROR_INVALID_ARG;
          goto done;
        }

        nBandCount = 3; // RGB
        nBandSpace = 1;

        // Just write all bands here
        if(format == dmPixelFormat32bppARGB && mAlphaBand)
        {
            nBandCount   = 4;
            #ifdef DM_IS_LITTLE_ENDIAN
              panBanMap[0] = 3;
              panBanMap[1] = 2;
              panBanMap[2] = 1;
              panBanMap[3] = mAlphaBand;
            #else
              panBanMap[0] = mAlphaBand;
              panBanMap[1] = 1;
              panBanMap[2] = 2;
              panBanMap[3] = 3;
            #endif
        }
        else
        {
          #ifdef DM_IS_LITTLE_ENDIAN
            panBanMap[0] = 3;
            panBanMap[1] = 2;
            panBanMap[2] = 1;            
          #else
            pScan0 += 1; // Advance scan pointer to skip alpha channel
          #endif
        }
      }
      else
      if(dmIsPixelFormatScalar(format))
      {
        // Get a specific band if required or band 1 otherwise
        nBandCount = 1;
        nBandSpace = imData.Stride * imData.Height;

        int band = (int)((ELockBandMask & mLock) >> 16);
        if(band == 0)
           band = 1;

        panBanMap[0] = band;
      }
      else {
        rv = CCI_ERROR_INVALID_ARG;
        goto done;
      }

      // Do gdal IO
      CPLErr err = GDALDatasetRasterIO(mDS,GF_Write,
                                       mLockRect.Left(),
                                       mLockRect.Top(),
                                       mLockRect.Width(),
                                       mLockRect.Height(),
                                       pScan0,
                                       imData.Width,
                                       imData.Height,
                                       dataType, nBandCount, panBanMap,
                                       nPixelSpace, nLineSpace, nBandSpace);

      rv = (err != CE_None) ? CCI_ERROR_FAILURE : CCI_OK;
    }
  }

  // Unlock
done:
  mLock = 0;
  mLockRect.Clear();

  return rv;
}

/* readonly attribute boolean isLocked; */
CCI_IMETHODIMP gdalSurface::GetIsLocked(bool *aIsLocked)
{
  *aIsLocked = IsLocked();
  return CCI_OK;
}

/* readonly attribute boolean isWritable; */
CCI_IMETHODIMP gdalSurface::GetIsWritable(bool *aIsWritable)
{
  *aIsWritable = IsWritable();
  return CCI_OK;
}

/* [noscript] cciIColorTable getColorTable(); */
CCI_IMETHODIMP gdalSurface::GetColorTable( cciIColorTable * *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(_retval);

  // Get specific band or default to band 1
  int band = (int)((ELockBandMask & mLock) >> 16);

  if(band == 0)
     band = 1;

  if(band > GDALGetRasterCount(mDS))
     return CCI_ERROR_INVALID_ARG;

  GDALRasterBandH hBand = GDALGetRasterBand(mDS,band);
  CCI_ENSURE_TRUE(hBand,CCI_ERROR_UNEXPECTED);

  if(GDALGetRasterColorInterpretation(hBand)==GCI_PaletteIndex)
  {
    GDALColorTableH hColorTable = GDALCloneColorTable(GDALGetRasterColorTable(hBand));

    return CCI_NewGDALColorTable(hColorTable,_retval);
  }

  return CCI_ERROR_NOT_AVAILABLE;
}

/* [noscript] void setColorTable( in cciIColorTable colorTable); */
CCI_IMETHODIMP gdalSurface::SetColorTable( cciIColorTable *colorTable )
{
  CCI_ENSURE_ARG_POINTER(colorTable);

  // Ensure that surface is writable
  if(mAccess!=GA_Update)
     return CCI_ERROR_FAILURE;

  // Get specific band or default to band 1
  int band = (int)((ELockBandMask & mLock) >> 16);

  if(band == 0)
     band = 1;

  if(band > GDALGetRasterCount(mDS))
     return CCI_ERROR_INVALID_ARG;

  GDALRasterBandH hBand = GDALGetRasterBand(mDS,band);
  CCI_ENSURE_TRUE(hBand,CCI_ERROR_UNEXPECTED);

  CPLErr err = GDALSetRasterColorInterpretation(hBand,GCI_PaletteIndex);
  if(err != CE_None)
     return CCI_ERROR_FAILURE;

  GDALColorTableH hColorTable;

  // Copy data from src colortable
  cci_result rv = gdalColorTable::copyColorTable(colorTable,hColorTable);
  CCI_ENSURE_SUCCESS(rv,CCI_ERROR_FAILURE);
  
  err = GDALSetRasterColorTable(hBand,hColorTable);

  GDALDestroyColorTable(hColorTable);

  if(err != CE_None)
     return CCI_ERROR_FAILURE;

  return CCI_OK;
}


/* void flush (); */
CCI_IMETHODIMP gdalSurface::Flush()
{
  CCI_ENSURE_TRUE(mDS,CCI_ERROR_NOT_INITIALIZED);
  
  GDALFlushCache(mDS);
  return CCI_OK;
}

//==================================
// cciIRemoteSurfaceSurface
//==================================

/* attribute cciIInterfaceRequestor callbacks; */
CCI_IMETHODIMP gdalSurface::GetCallbacks(cciIInterfaceRequestor * *aCallbacks)
{
  CCI_ENSURE_ARG_POINTER(aCallbacks);

  *aCallbacks = mCallBacks;
  CCI_IF_ADDREF(*aCallbacks);
  return CCI_OK;
}
CCI_IMETHODIMP gdalSurface::SetCallbacks(cciIInterfaceRequestor *aCallbacks)
{
  mCallBacks = aCallbacks;
  return CCI_OK;
}


/* readonly attribute cciISurfaceDriver driver; */
CCI_IMETHODIMP gdalSurface::GetDriver(cciISurfaceDriver * *aDriver)
{
  CCI_ENSURE_TRUE(mDS,CCI_ERROR_NOT_INITIALIZED);

  GDALDriverH hDrv = GetGDALDriver();
  if(hDrv)
     return CCI_NewGDALDriver(hDrv,aDriver);

  return CCI_ERROR_FAILURE;
}


/* readonly attribute dm_uint32 ioFlags; */
CCI_IMETHODIMP gdalSurface::GetIoFlags(dm_uint32 *aIoFlags)
{
  CCI_ENSURE_ARG_POINTER(aIoFlags);
  *aIoFlags = mAccess;
  return CCI_OK;
}


CCI_IMETHODIMP gdalSurface::Close()
{
  if(mDS)
  {
    cci_result rv = CCI_OK;

    if(mOutputDriver)
    {
       //TODO set progress data from callbacks
       GDALDatasetH hDS = GDALCreateCopy(mOutputDriver,mLocation.get(),mDS,false,
                                         const_cast<char**>(mCreateOpts),
                                         dm_null,dm_null);
       if(hDS)
          GDALClose(hDS);
       else
          rv = CCI_ERROR_FAILURE;
    }

    GDALClose(mDS);
    mDS = dm_null;

    if(mCreateOpts) {
       CSLDestroy( mCreateOpts );
       mCreateOpts = dm_null;
    }

    return rv;
  }

  return CCI_ERROR_NOT_INITIALIZED;
}

/* void createCopy (in string newLocation, in string options); */
CCI_IMETHODIMP gdalSurface::CreateCopy(const char * newLocation, const char * options)
{
  CCI_ENSURE_ARG_POINTER(newLocation);

  GDALDriverH hDriver = GetGDALDriver();
  if(!hDriver)
     return CCI_ERROR_NOT_AVAILABLE;

  char** createOpts = dm_null;

  if(options)
     createOpts = CSLTokenizeStringComplex(options,",",false,false);

  //TODO set progress data from callbacks
  GDALDatasetH newDS = GDALCreateCopy(hDriver,newLocation,mDS,false,createOpts,
                                      dm_null,dm_null);

  cci_result rv;

  if(newDS)
  {
    GDALClose(newDS);
    rv = CCI_OK;
  }
  else
     rv = CCI_ERROR_FAILURE;

  if(createOpts)
     CSLDestroy( createOpts );

  return rv;
}

/* void imageIO (in cciImage image, in long srcX, in long srcY, in unsigned long lockModes, [optional] in boolean alpha); */
CCI_IMETHODIMP gdalSurface::ImageIO(cciImage image, dm_int32 srcX, dm_int32 srcY, dm_uint32 lockModes, bool alpha)
{
  dmImage* img = CCI_IF_NATIVE(image);
  CCI_ENSURE_ARG_POINTER(img);  
  
  dmRect rect(0,0,Width(),Height());
  dmRect srcRect(srcX,srcY,img->Width(),img->Height());
  
  if(!rect.Clip(srcRect))
    return CCI_OK; //Nothing to do
  
  dmImageData _Data;
  img->GetImageData(_Data);
  if(_Data.PixelFormat == dmPixelFormat24bppRGB && alpha)
     _Data.PixelFormat = dmPixelFormat32bppARGB;
  
  cci_result rv = LockBitsRect(rect,_Data.PixelFormat,_Data,lockModes|cciISurface::ELockUserBuffer);
  
  if(CCI_SUCCEEDED(rv))
     UnlockBits(_Data);
  
  return rv; 
}

//==================================
// cciIMetaDataContainer
//==================================

class cciMetaDataIterator : public cciIUTF8StringIterator,
                            public cciIUTF8StringEnumerator,
                            public cciIStringEnumerator

{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IUTF8STRINGITERATOR

  // have to declare cciIStringEnumerator manually, because of
  // overlapping method names
  CCI_IMETHOD GetNext(dmAString& );
  CCI_IMETHOD GetNext(dmACString&);
  
  CCI_IMETHOD_(bool) HasMore(void);
  
  cciMetaDataIterator(char **papszMetadata, cciISupports* owner)
  : mCurrent(papszMetadata)
  , mOwner(owner)
  {}

  char* *mCurrent;
  cci_Ptr<cciISupports> mOwner;

private:
  ~cciMetaDataIterator() {}
};

CCI_IMPL_ISUPPORTS3(cciMetaDataIterator,
                    cciIUTF8StringIterator,
                    cciIUTF8StringEnumerator,
                    cciIStringEnumerator)

CCI_IMETHODIMP_(bool)
cciMetaDataIterator::HasMore()
{
  return (*mCurrent != dm_null);
}

/* boolean hasMore (); */
CCI_IMETHODIMP 
cciMetaDataIterator::HasMore(bool *_retval CCI_OUTPARAM)
{
  *_retval = HasMore();
  return CCI_OK;
}


/* void getNext ([shared, retval] out string value); */
CCI_IMETHODIMP 
cciMetaDataIterator::GetNext(const char * *value CCI_OUTPARAM)
{
  if(*mCurrent==dm_null)
     return CCI_ERROR_FAILURE;

  *value = *mCurrent++;
  return CCI_OK;
}


CCI_IMETHODIMP
cciMetaDataIterator::GetNext(dmACString& _retval)
{
  if(*mCurrent==dm_null)
     return CCI_ERROR_FAILURE;

  _retval.Assign(*mCurrent++);
  return CCI_OK;
}

CCI_IMETHODIMP
cciMetaDataIterator::GetNext(dmAString& _retval)
{
  if(*mCurrent==dm_null)
     return CCI_ERROR_FAILURE;

  _retval.Assign(*mCurrent++);
  return CCI_OK;
}
/* void getMetadataItem (in string aName, in string aDomain, [shared, retval] out string aValue); */
CCI_IMETHODIMP gdalSurface::GetMetadataItem(const char * aName, const char * aDomain, const char * *aValue CCI_OUTPARAM)
{
  CCI_ENSURE_TRUE(mDS,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(aName);

  const char* pszValue = GDALGetMetadataItem(mDS,aName,aDomain);
  if(!pszValue)
     return CCI_ERROR_NOT_AVAILABLE;

  *aValue = pszValue;
  return CCI_OK;
}

/* void setMetadataItem (in string aName, in wstring aValue, in string aDomain); */
CCI_IMETHODIMP gdalSurface::SetMetadataItem(const char * aName, const char * aValue, const char * aDomain)
{
  CCI_ENSURE_TRUE(mDS,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(aName);

  CPLErr err = GDALSetMetadataItem(mDS,aName,aValue,aDomain);
  if(err != CE_None)
     return CCI_ERROR_FAILURE;

  return CCI_OK;
}

/* cciIUTF8StringIterator getMetadata (in string aDomain); */
CCI_IMETHODIMP gdalSurface::GetMetadata(const char * aDomain, cciIUTF8StringIterator * *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_TRUE(mDS,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(_retval);

  char **papszMetadata = GDALGetMetadata(mDS,aDomain);
  if(!papszMetadata)
     return CCI_ERROR_NOT_AVAILABLE;

  *_retval = new cciMetaDataIterator(papszMetadata,CCI_ISUPPORTS_CAST(cciIMetaDataContainer*,this));
  CCI_ADDREF(*_retval);
  return CCI_OK;
}

//==================================
// gdalMetadata
//==================================

CCI_IMETHODIMP_(char* *)
gdalSurface::GetMetaData(const char * aDomain)
{
  if(mDS)
     return GDALGetMetadata(mDS,aDomain);
  
  return dm_null;
}

CCI_IMETHODIMP gdalSurface::SetMetaData(const char * aDomain, char* *data)
{
  CCI_ENSURE_TRUE(mDS,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(data);

  CPLErr err = GDALSetMetadata(mDS,data,aDomain);
  if(err != CE_None)
     return CCI_ERROR_FAILURE;

  return CCI_OK;
}



//=====================================
// Utilities
//=====================================
cci_result gdalSurface::Create(GDALDatasetH aDataset, dm_uint32 ioFlags, GDALDriverH tmpDriver, gdalSurface** result)
{
  gdalSurface* pSurface = new gdalSurface();
  if(!pSurface)
     return CCI_ERROR_OUT_OF_MEMORY;
  
  cci_result rv = pSurface->Init(aDataset,ioFlags,tmpDriver);
  if(CCI_FAILED(rv)) {
    delete pSurface;
  } else {
    CCI_ADDREF(*result = pSurface);
  }
  
  return rv;
}

cci_result gdalSurface::Create(GDALDriverH aDriver,const char * location,
                               dm_uint32 width, dm_uint32 height,
                               EPixelFormat format,
                               const char *options,
                               gdalSurface** result)
{
  gdalSurface* pSurface = new gdalSurface();
  if(!pSurface)
     return CCI_ERROR_OUT_OF_MEMORY;
  cci_result rv = pSurface->Init(aDriver,location,width,height,
                                 format,options);
  if(CCI_FAILED(rv)) {
    delete pSurface;
  } else {
    CCI_ADDREF(*result = pSurface);
  }
 
  return rv;
}

cci_result gdalSurface::Create(GDALDriverH aDriver,const char * location, dmImageData& imData, const char *options,
                               gdalSurface** result)
{
  gdalSurface* pSurface = new gdalSurface();
  if(!pSurface)
     return CCI_ERROR_OUT_OF_MEMORY;
  
  cci_result rv = pSurface->Init(aDriver,location,imData,options);
  if(CCI_FAILED(rv)) {
    delete pSurface;
  } else {
    CCI_ADDREF(*result = pSurface);
  }
 
  return rv;
}
