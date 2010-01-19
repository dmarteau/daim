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
#include "cciIStringEnumerator.h"
#include "cciIInterfaceRequestor.h"
#include "cciIColorTable.h"

#include "gdaldrv.h"
#include "daim_kernel.h"

#define CPL_SUPRESS_CPLUSPLUS
#include "cpl_string.h"

#include "gdalSurface.h"
#include "gdalColorTable.h"

/* gdalSurface Implementation */
CCI_IMPL_ISUPPORTS4(gdalSurface, cciISurface,
                                 cciIRemoteSurface,
                                 cciIScriptableSurface,
                                 cciIMetaDataContainer)

gdalSurface::gdalSurface()
: mDS(dm_null)
, mOutputDriver(dm_null)
, mTmpDriver(dm_null)
, mAccess(GA_ReadOnly)
, mAlphaBand(0)
, mFormat(dmPixelFormatUndefined)
, mRequireUserBuffer(DM_FALSE)
, mCreateOpts(dm_null)
, mLock(0)
{
}

gdalSurface::~gdalSurface()
{
  Close();
}

// Static method
cci_result gdalSurface::CreateInMemorySurface( const dmImageData & imData, const dm_uint8* alphaBits, 
                                               dm_int32 alphaStride ,GDALDatasetH& hDS )
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

  if(imData.PixelFormat==dmPixelFormat24bppRGB) {
    nBands     = 3;
    BandStride = 1;
  }

  dmCString buf = dmCString::FormatString(256,
                                "MEM:::DATAPOINTER=%ld,PIXELS=%ld,LINES=%ld,"
                                "BANDS=%ld,DATATYPE=%s,PIXELOFFSET=%ld,"
                                "LINEOFFSET=%ld,BANDOFFSET=%ld",
                                 imData.Scan0,
                                 imData.Width,
                                 imData.Height,
                                 nBands,pszType,
                                 PixelStride,
                                 LineStride,
                                 BandStride);

  hDS = GDALOpen(buf.get(),GA_Update);
  if(!hDS)
     return CCI_ERROR_FAILURE;
  
  cci_result rv = CCI_OK;
  
  if(alphaBits)
  {
    if((static_cast<dm_uint32>(alphaStride) >=  imData.Width * sizeof(dm_uint8)))
    {
      char **papszOptions = dm_null;

      papszOptions = CSLAppendPrintf(papszOptions,"DATAPOINTER=%ld",alphaBits);
      papszOptions = CSLAppendPrintf(papszOptions,"PIXELOFFSET=%ld",1);
      papszOptions = CSLAppendPrintf(papszOptions,"LINEOFFSET=%ld" ,alphaStride);

      CPLErr err = GDALAddBand(hDS,GDT_Byte,papszOptions);

      CSLDestroy( papszOptions );
      if(err != CE_None) 
         rv = CCI_ERROR_FAILURE;
      else
         GDALSetRasterColorInterpretation(GDALGetRasterBand(hDS,nBands+1),GCI_AlphaBand);
    }
    else 
    {
      dmLOG_WARN("Invalid alpha channel format, dropping alpha channel !");
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
     GDALGetRasterColorInterpretation(GDALGetRasterBand(aDS,3))==GCI_BlueBand) {
     mFormat = dmPixelFormat24bppRGB;
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

  // Check for alpha band
  if(GDALGetRasterColorInterpretation(GDALGetRasterBand(aDS,nBands))==GCI_AlphaBand)
     mAlphaBand = nBands;

  mDS                = aDS;
  mOutputDriver      = dm_null;
  mTmpDriver         = tmpDriver;
  mRequireUserBuffer = DM_TRUE;
  mAccess            = static_cast<GDALAccess>(ioFlags);

  return CCI_OK;
}

// Init from image data
cci_result gdalSurface::Init(GDALDriverH aDriver,const char * location,
                             dmImageData& imData, dm_uint8* alphaBits, dm_int32 alphaStride,
                             const char *options)
{
  if(aDriver && GDALGetMetadataItem(aDriver,GDAL_DCAP_CREATECOPY,NULL) == NULL)
     return CCI_ERROR_NOT_AVAILABLE;

  GDALDatasetH hDS = dm_null;

  // Wrap image data
  dmImageDescriptor* pDesc = dmGetDescriptor(mFormat);
  if(pDesc) {
    mImageBuffer = pDesc->CreateImage(imData);
    if(!mImageBuffer)
       return CCI_ERROR_OUT_OF_MEMORY;
    
    if(alphaBits && (static_cast<dm_uint32>(alphaStride) >= imData.Width * sizeof(dm_uint8)))
    { 
      dmImageData alphaData;
      alphaData.PixelFormat = dmPixelFormat8bppIndexed;
      alphaData.Scan0       = alphaBits;
      alphaData.Stride      = alphaStride;
      alphaData.Width       = imData.Width;
      alphaData.Height      = imData.Height;
    
      mAlphaBuffer = dmCreateImage<dmPixelFormat8bppIndexed>(alphaData);
    }
  } else {
    return CCI_ERROR_UNEXPECTED;
  }
  
  cci_result rv = CreateInMemorySurface(imData,alphaBits,alphaStride,hDS);
  if(CCI_FAILED(rv))
     return rv;

  // Clone options;
  if(options)
     mCreateOpts = CSLTokenizeStringComplex(options,",",DM_FALSE,DM_FALSE);

  mDS                = hDS;
  mLocation          = location;
  mOutputDriver      = aDriver;
  mFormat            = imData.PixelFormat;
  mRequireUserBuffer = DM_FALSE;
  mAccess            = GA_Update;

  return rv;
}

// Init from a driver
cci_result gdalSurface::Init(GDALDriverH aDriver,const char * location,
                             dm_uint32 width, dm_uint32 height,
                             EPixelFormat format, dm_bool hasAlpha,
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

  if(GDALGetMetadataItem(mOutputDriver,GDAL_DCAP_CREATE,NULL ) == NULL)
     return CCI_ERROR_NOT_AVAILABLE;
    
  char** createOpts = dm_null;

  if(options)
     createOpts = CSLTokenizeStringComplex(options,",",DM_FALSE,DM_FALSE);

  int nBands = 1;
  if(format==dmPixelFormat24bppRGB)
     nBands = 3;

  hDS = GDALCreate(mOutputDriver,location,width,height,nBands,dataType,createOpts);
  if(createOpts)
     CSLDestroy(createOpts);

  if(!hDS)
     return CCI_ERROR_FAILURE;

  // Set alpha channel
  if(hasAlpha) 
     GDALSetRasterColorInterpretation( GDALGetRasterBand(hDS,++nBands),
                                       GCI_AlphaBand );

  mDS                = hDS;
  mLocation          = location;
  mFormat            = format;
  mOutputDriver      = dm_null;
  mRequireUserBuffer = DM_TRUE;
  mAccess            = GA_Update;

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
  return GDALGetRasterCount(mDS);;
}

/* [noscript,notxpcom] EPixelFormat PixelFormat (); */
CCI_IMETHODIMP_(EPixelFormat) gdalSurface::PixelFormat()
{
  return mFormat;
}

/* [noscript,notxpcom] dm_bool HasAlpha (); */
CCI_IMETHODIMP_(dm_bool) gdalSurface::HasAlpha()
{
  return mAlphaBand!=0 ? DM_FALSE : DM_TRUE;;
}

/* [noscript,notxpcom] dm_bool HasSamplingCapabilities (); */
CCI_IMETHODIMP_(dm_bool) gdalSurface::HasSamplingCapabilities()
{
  return dm_true;
}

/* [noscript,notxpcom] dm_bool HasConvertCapabilities(); */
CCI_IMETHODIMP_(dm_bool) gdalSurface::HasConvertCapabilities()
{
  return dm_true;
}

/* [noscript,notxpcom] boolean IsLocked (); */
CCI_IMETHODIMP_(dm_bool) gdalSurface::IsLocked()
{
  return mLock != 0;
}

/* [noscript,notxpcom] boolean IsWritable (); */
CCI_IMETHODIMP_(dm_bool) gdalSurface::IsWritable()
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

/* readonly attribute dm_bool hasAlpha; */
CCI_IMETHODIMP gdalSurface::GetHasAlpha(dm_bool *aHasAlpha)
{
  *aHasAlpha = HasAlpha();
  return CCI_OK;
}

cci_result gdalSurface::CreateUserBuffer(dmRect & rect, EPixelFormat format, dmImageData& imData,
                                         dm_bool wantAlpha)
{
  EPixelFormat bufferFmt = format;
  if(bufferFmt==dmPixelFormatUndefined)
  {
    bufferFmt = mFormat;
    if(wantAlpha && !dmIsPixelFormatScalar(bufferFmt))
       bufferFmt = dmPixelFormat8bppIndexed;
  }
  else
  {
    if(wantAlpha && !dmIsPixelFormatScalar(bufferFmt))
       return CCI_ERROR_INVALID_ARG;
  }

  if(mImageBuffer.IsNull() || mImageBuffer->PixelFormat()!=bufferFmt||
     mImageBuffer->Width()  != rect.Width() ||
     mImageBuffer->Height() != rect.Height())
  {
    dmImageDescriptor* pDesc = dmGetDescriptor(bufferFmt);
    if(pDesc)
    {
      mImageBuffer = pDesc->CreateImage(mLockRect.Width(),mLockRect.Height());

      if(!mImageBuffer)
         return CCI_ERROR_OUT_OF_MEMORY;

      mImageBuffer->GetImageData(imData);
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
        entry = &colors[pixel.red];
        pixel.red   = entry->c1;
        pixel.green = entry->c2;
        pixel.blue  = entry->c3;
      }
      pixbuf += stride;    
    }    
  }
}



static bool IsLittleEndian()
{
  static const dm_uint32 i32 = 0x000000ff;
  return *(dm_byte*)(&i32) == 0xff;
}

cci_result gdalSurface::ReadData(EPixelFormat format, dmImageData& imData, dm_bool wantAlpha,
                                 dm_uint32 lockModes)
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
  
  if( wantAlpha )
  {
    // Ensure that we have a scalar format
    if(!dmIsPixelFormatScalar(format))
    {
      return CCI_ERROR_INVALID_ARG;      
    }

    nBandCount   = 1;
    nBandSpace   = imData.Stride * imData.Height;
    panBanMap[0] = mAlphaBand;
  }
  else
  // We are putting data into rgb format 
  if(format == dmPixelFormat24bppRGB || format == dmPixelFormat32bppARGB)
  {
    if(dmIsPixelFormatScalar(mFormat))
    {
      nBandCount = 3;
      nBandSpace = 1;

      // Assign all channels to same band
      panBanMap[0] = panBanMap[1] = panBanMap[2] = 1;
      
      GDALRasterBandH hBand = GDALGetRasterBand(mDS,1);
      if(GDALGetRasterColorInterpretation(hBand)==GCI_PaletteIndex)
         hColorTable = GDALGetRasterColorTable(hBand);

      // Just read all bands here
      if(format == dmPixelFormat32bppARGB)
      {
        bool le = IsLittleEndian();
        // Check for endianness

        if(mAlphaBand)
        {
          nBandCount = 4;
          if(le) {
            panBanMap[3] = mAlphaBand;
          } else {
            panBanMap[0] = mAlphaBand;
            panBanMap[1] = panBanMap[2] = panBanMap[3] = 1;
          }
        }
        else
        {
          FillAlphaChannel(pScan0,imData.Width,imData.Height,nLineSpace);
          if(!le)
             pScan0 += 1; // Advance scan pointer to skip alpha channel
        }
      } 
    }
    else
    {
      nBandCount = 3;
      nBandSpace = 1;

      // Just read all bands here
      if(format == dmPixelFormat32bppARGB)
      {
        bool le = IsLittleEndian();
        // Check for endianness

        if(mAlphaBand)
        {
          nBandCount   = 4;
          if(le) {
            panBanMap[0] = 3;
            panBanMap[1] = 2;
            panBanMap[2] = 1;
            panBanMap[3] = mAlphaBand;
          } else {
            panBanMap[0] = mAlphaBand;
            panBanMap[1] = 1;
            panBanMap[2] = 2;
            panBanMap[3] = 3;
          }
        }
        else
        {
          FillAlphaChannel(pScan0,imData.Width,imData.Height,nLineSpace);
          if(le) {
            panBanMap[0] = 3;
            panBanMap[1] = 2;
            panBanMap[2] = 1;
          } else
            pScan0 += 1; // Advance scan pointer to skip alpha channel
        }
      }
    }
  }
  else
  if(dmIsPixelFormatScalar(format))
  {
    // Get a specific band if required or band 1 otherwise
    nBandCount = 1;
    nBandSpace = imData.Stride * imData.Height;

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

  dm_bool wantAlpha = (lockModes & ELockAlpha);

  // Check that alpha request is valid
  if(wantAlpha && !mAlphaBand)
     return CCI_ERROR_NOT_AVAILABLE;

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
      rv = CreateUserBuffer(mLockRect,format,imData,wantAlpha);
      if(CCI_FAILED(rv))
         return rv;

      // Read data if required
      if((lockModes & ELockRead)!= 0)
         rv = ReadData(imData.PixelFormat,imData,wantAlpha,lockModes);
    }
    else
    {
      dmImage* srcImage = wantAlpha ? mAlphaBuffer : mImageBuffer;

      if(format!=dmPixelFormatUndefined && format != srcImage->PixelFormat())
        return CCI_ERROR_INVALID_ARG;

      dmImageData srcData;
      srcImage->GetImageData(srcData);

      size_t byteStart = mLockRect.Left() * (dmGetPixelFormatBits(format) >> 3);

      imData.Width       = mLockRect.Width();
      imData.Height      = mLockRect.Height();
      imData.Stride      = srcData.Stride;
      imData.Scan0       = static_cast<dm_byte*>(srcData.Scan0) + (srcData.Stride * mLockRect.Top()) + byteStart;
      imData.PixelFormat = srcImage->PixelFormat();

      rv = CCI_OK;
    }
  }
  else
  {
    // Read data if required
    if((lockModes & ELockRead)!= 0)
       rv = ReadData(format,imData,wantAlpha,lockModes);
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

      if(mLock & ELockAlpha)
      {
        // Ensure that we have a scalar format
        if(!dmIsPixelFormatScalar(format) || format != dmPixelFormat32bppARGB) {
           rv = CCI_ERROR_INVALID_ARG;
           goto done;
        }

        nBandCount   = 1;
        nBandSpace   = imData.Stride * imData.Height;
        panBanMap[0] = mAlphaBand;
      }
      else
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
        if(format == dmPixelFormat32bppARGB)
        {
          bool le = IsLittleEndian();
          // Check for endianness

          if(mAlphaBand)
          {
            nBandCount   = 4;
            if(le) {
              panBanMap[0] = 3;
              panBanMap[1] = 2;
              panBanMap[2] = 1;
              panBanMap[3] = mAlphaBand;
            } else {
              panBanMap[0] = mAlphaBand;
              panBanMap[1] = 1;
              panBanMap[2] = 2;
              panBanMap[3] = 3;
            }
          }
          else
          {
            if(le) {
              panBanMap[0] = 3;
              panBanMap[1] = 2;
              panBanMap[2] = 1;
            } else
              pScan0 += 1; // Advance scan pointer to skip alpha channel
          }
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
CCI_IMETHODIMP gdalSurface::GetIsLocked(dm_bool *aIsLocked)
{
  *aIsLocked = IsLocked();
  return CCI_OK;
}

/* readonly attribute boolean isWritable; */
CCI_IMETHODIMP gdalSurface::GetIsWritable(dm_bool *aIsWritable)
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

  GDALColorTableH hColorTable = GDALCreateColorTable(GPI_RGB);
  CCI_ENSURE_TRUE(hColorTable,CCI_ERROR_OUT_OF_MEMORY);

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
       GDALDatasetH hDS = GDALCreateCopy(mOutputDriver,mLocation.get(),mDS,DM_FALSE,
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
     createOpts = CSLTokenizeStringComplex(options,",",DM_FALSE,DM_FALSE);

  //TODO set progress data from callbacks
  GDALDatasetH newDS = GDALCreateCopy(hDriver,newLocation,mDS,DM_FALSE,createOpts,
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

//==================================
// cciIMetaDataContainer
//==================================

class cciMetaDataEnumerator : public cciIStringEnumerator,
                              public cciIUTF8StringEnumerator
{
public:
  CCI_DECL_ISUPPORTS
  // have to declare cciIStringEnumerator manually, because of
  // overlapping method names
  CCI_IMETHOD GetNext(dmAString& aResult);
  CCI_DECL_IUTF8STRINGENUMERATOR

  cciMetaDataEnumerator(char **papszMetadata, cciISupports* owner)
  : mCurrent(papszMetadata)
  , mOwner(owner)
  {}

  char* *mCurrent;
  cci_Ptr<cciISupports> mOwner;

private:
  ~cciMetaDataEnumerator() {}
};

CCI_IMPL_ISUPPORTS2(cciMetaDataEnumerator, 
                    cciIUTF8StringEnumerator, 
                    cciIStringEnumerator)

CCI_IMETHODIMP_(dm_bool)
cciMetaDataEnumerator::HasMore()
{
  return (*mCurrent != dm_null);
}

CCI_IMETHODIMP
cciMetaDataEnumerator::GetNext(dmAString& _retval)
{
  if(*mCurrent==dm_null)
     return CCI_ERROR_FAILURE;

  _retval.Assign(*mCurrent++);
  return CCI_OK;
}

CCI_IMETHODIMP
cciMetaDataEnumerator::GetNext(dmACString& _retval)
{
  if(*mCurrent==dm_null)
     return CCI_ERROR_FAILURE;

  _retval.Assign(*mCurrent++);
  return CCI_OK;
}


/* AString getMetadataItem (in string aName, in string aDomain); */
CCI_IMETHODIMP gdalSurface::GetMetadataItem(const char * aName, const char * aDomain, dmAString & _retval CCI_OUTPARAM)
{
  CCI_ENSURE_TRUE(mDS,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(aName);

  const char* pszValue = GDALGetMetadataItem(mDS,aName,aDomain);
  if(!pszValue)
     return CCI_ERROR_NOT_AVAILABLE;

  _retval.Assign(pszValue);

  return CCI_OK;
}

/* void setMetadataItem (in string aName, in wstring aValue, in string aDomain); */
CCI_IMETHODIMP gdalSurface::SetMetadataItem(const char * aName, const dm_unichar * aValue, const char * aDomain)
{
  CCI_ENSURE_TRUE(mDS,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(aName);

  dmCString asciiValue(aValue);

  CPLErr err = GDALSetMetadataItem(mDS,aName,asciiValue.get(),aDomain);
  if(err != CE_None)
     return CCI_ERROR_FAILURE;

  return CCI_OK;
}


/* void copyMetaData (in cciIMetaDataContainer destContainer, in string aDomain); */
CCI_IMETHODIMP gdalSurface::CopyMetaData(cciIMetaDataContainer *destContainer, const char * aDomain)
{
  CCI_ENSURE_TRUE(mDS,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(destContainer);

  return destContainer->SetMetaData(aDomain,GDALGetMetadata(mDS,aDomain));
}

/* [noscript] void setMetaData (in string aDomain, [array, size_is (count)] in charPtr data); */
CCI_IMETHODIMP gdalSurface::SetMetaData(const char * aDomain, char **data)
{
  CCI_ENSURE_TRUE(mDS,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(data);

  CPLErr err = GDALSetMetadata(mDS,data,aDomain);
  if(err != CE_None)
     return CCI_ERROR_FAILURE;

  return CCI_OK;
}

/* [noscript] void readMetaData (in cciWriteMetaDataFun writer, in voidPtr closure, in string aDomain); */
CCI_IMETHODIMP gdalSurface::ReadMetaData(cciWriteMetaDataFun writer, void *closure, const char * aDomain)
{
  CCI_ENSURE_TRUE(mDS,CCI_ERROR_NOT_INITIALIZED);
  char **papszMetadata = GDALGetMetadata(mDS,aDomain);
  if(!papszMetadata)
     return CCI_ERROR_NOT_AVAILABLE;

  return writer(this,closure,aDomain,papszMetadata);
}

/* cciIUTF8StringEnumerator getMetadata (in string aDomain); */
CCI_IMETHODIMP gdalSurface::GetMetadata(const char * aDomain, cciIUTF8StringEnumerator * *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_TRUE(mDS,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(_retval);

  char **papszMetadata = GDALGetMetadata(mDS,aDomain);
  if(!papszMetadata)
     return CCI_ERROR_NOT_AVAILABLE;

  *_retval = new cciMetaDataEnumerator(papszMetadata,CCI_ISUPPORTS_CAST(cciIMetaDataContainer*,this));
  CCI_ADDREF(*_retval);
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
                               EPixelFormat format, dm_bool hasAlpha,
                               const char *options,
                               gdalSurface** result)
{
  gdalSurface* pSurface = new gdalSurface();
  if(!pSurface)
     return CCI_ERROR_OUT_OF_MEMORY;
  cci_result rv = pSurface->Init(aDriver,location,width,height,
                                 format,hasAlpha,options);
  if(CCI_FAILED(rv)) {
    delete pSurface;
  } else {
    CCI_ADDREF(*result = pSurface);
  }
 
  return rv;
}

cci_result gdalSurface::Create(GDALDriverH aDriver,const char * location,
                               dmImageData& imData, dm_uint8* alphaBits, dm_int32 alphaStride,
                               const char *options,
                               gdalSurface** result)
{
  gdalSurface* pSurface = new gdalSurface();
  if(!pSurface)
     return CCI_ERROR_OUT_OF_MEMORY;
  
  cci_result rv = pSurface->Init(aDriver,location,imData,alphaBits,alphaStride,options);
  if(CCI_FAILED(rv)) {
    delete pSurface;
  } else {
    CCI_ADDREF(*result = pSurface);
  }
 
  return rv;
}
