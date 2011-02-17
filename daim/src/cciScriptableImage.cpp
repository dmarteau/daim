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
// File         : cciScriptableImage.cpp
// Date         : 4 févr. 2011
// Author       : David Marteau
//--------------------------------------------------------

#define dmUseKernelImageTemplates
#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"
#include "cciIImageContainer.h"
#include "cciIRegionContainer.h"
#include "cciITransforms.h"
#include "cciISurface.h"

#include "cciScriptableImage.h"

#include "daim_kernel.h"
#include "templates/processing/dmDensityMap.h"
#include "common/dmUserLib.h"

#include "cciIImageListContainer.h"

/* Implementation file */
//CCI_IMPL_ISUPPORTS3(cciScriptableImage, cciIImage, cciImageWrapper, cciIImageContainer)

// cciIImageContainer is an ambigous class of cciIImage and cciImageWrapper

CCI_IMPL_ADDREF(cciScriptableImage)  
CCI_IMPL_RELEASE(cciScriptableImage)   
CCI_IMPL_ALLOCATOR(cciScriptableImage)
//CCI_IMPL_QUERY_INTERFACE3(_class, _i1, _i2, _i3)
CCI_INTERFACE_TABLE_HEAD(cciScriptableImage)
CCI_INTERFACE_TABLE_BEGIN
  CCI_INTERFACE_TABLE_ENTRY(cciScriptableImage, cciIImage)
  CCI_INTERFACE_TABLE_ENTRY(cciScriptableImage, cciImageWrapper)
  CCI_INTERFACE_TABLE_ENTRY_AMBIGUOUS(cciScriptableImage, cciIImageContainer, cciImageWrapper)
  CCI_INTERFACE_TABLE_ENTRY_AMBIGUOUS(cciScriptableImage, cciISupports, cciIImage)
CCI_INTERFACE_TABLE_END
CCI_INTERFACE_TABLE_TAIL


cciScriptableImage::cciScriptableImage()
: mLock(DM_FALSE)
, mEnableAlpha(DM_FALSE)
{
}

cciScriptableImage::cciScriptableImage( dmLink<dmImage>& imglink )
: mLock(DM_FALSE)
, mEnableAlpha(DM_FALSE)
{
  mImage = imglink;
}

/* void init (in unsigned long width, in unsigned long height, in EPixelFormat format); */
cciScriptableImage::cciScriptableImage(dm_uint32 width, dm_uint32 height, EPixelFormat format)
: mLock(DM_FALSE)
, mEnableAlpha(DM_FALSE)
{
  Initialize(width,height,format);
}

cciScriptableImage::cciScriptableImage( dmImageData& imData )
{
  Initialize(imData);
}

cciScriptableImage::~cciScriptableImage()
{
}

cci_result cciScriptableImage::Initialize( dm_uint32 width, dm_uint32 height, EPixelFormat format )
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_ALREADY_INITIALIZED);
  
  dmImageDescriptor* desc = dmGetDescriptor(format);
  if(desc) {
     mImage = desc->CreateImage(width,height);
  }
  
  if(mImage.IsNull())
     return CCI_ERROR_OUT_OF_MEMORY;
  
  // Enable alpha if argb specified
  if(format == dmPixelFormat32bppARGB)
     mEnableAlpha = DM_TRUE;
  
  return CCI_OK;
}

cci_result cciScriptableImage::Initialize( dmImageData& imData )
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_ALREADY_INITIALIZED);

  dmImageDescriptor* desc = dmGetDescriptor(imData.PixelFormat);
  if(desc) {
     mImage = desc->CreateImage(imData);
  }

  if(mImage.IsNull())
     return CCI_ERROR_OUT_OF_MEMORY;

  // Enable alpha if argb specified
  if(imData.PixelFormat == dmPixelFormat32bppARGB)
     mEnableAlpha = DM_TRUE;

  return CCI_OK;
}

//===============================================
// cciIImage
//===============================================

static inline void 
GetImageData( dmImage* img, dmImageData& imData, dm_bool alpha )
{
  img->GetImageData(imData);
  if(imData.PixelFormat == dmPixelFormat24bppRGB && alpha)
     imData.PixelFormat = dmPixelFormat32bppARGB;
}


/* [noscript] void lock (in dmImageDataRef imData, in dmRectPtr rect); */
CCI_IMETHODIMP cciScriptableImage::Lock(dmImageData & imData, dm_rect *rect)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_FALSE(mLock,CCI_ERROR_FAILURE);

  mLock = DM_TRUE;
  
  ::GetImageData(mImage,imData,mEnableAlpha);

  if(rect) 
  {
    dmRect _DstRect = *rect;

    if(!_DstRect.Clip(0,0,imData.Width-1,imData.Height-1))
       return CCI_ERROR_OUT_OF_RANGE;

    int PixelStride = dmGetPixelFormatBits(imData.PixelFormat) >> 3;

    imData.Width  = _DstRect.Width();
    imData.Height = _DstRect.Height();
    imData.Scan0  = static_cast<dm_byte*>(imData.Scan0)  +
                    (imData.Stride * _DstRect.Top()) + (_DstRect.Left() * PixelStride);
  }
  return CCI_OK;
}

/* [noscript] void unlock (); */
CCI_IMETHODIMP cciScriptableImage::Unlock()
{
  CCI_ENSURE_TRUE(mLock,CCI_ERROR_FAILURE);

  mLock = DM_FALSE;
  return CCI_OK;
}

/* readonly attribute unsigned long width; */
CCI_IMETHODIMP cciScriptableImage::GetWidth(dm_uint32 *aWidth)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(aWidth);

  *aWidth = mImage->Width();
  return CCI_OK;
}

/* readonly attribute unsigned long height; */
CCI_IMETHODIMP cciScriptableImage::GetHeight(dm_uint32 *aHeight)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(aHeight);

  *aHeight = mImage->Height();
  return CCI_OK;
}

/* readonly attribute EPixelFormat pixelFormat; */
CCI_IMETHODIMP cciScriptableImage::GetPixelFormat(EPixelFormat *aPixelFormat)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(aPixelFormat);

  *aPixelFormat = mImage->PixelFormat();
  return CCI_OK;
}

/* attribute boolean enableAlpha; */
CCI_IMETHODIMP cciScriptableImage::GetEnableAlpha(dm_bool *aEnableAlpha)
{
  *aEnableAlpha = mEnableAlpha;
  return CCI_OK;
}
CCI_IMETHODIMP cciScriptableImage::SetEnableAlpha(dm_bool aEnableAlpha)
{
  mEnableAlpha = aEnableAlpha;
  return CCI_OK;
}


/* void loadSurfaceBits (in cciISurface surface, in cciRegion rgn); */
CCI_IMETHODIMP cciScriptableImage::LoadSurfaceBits(cciISurface *surface, cciRegion rgn)
{
  CCI_ENSURE_ARG_POINTER(surface);
  CCI_ENSURE_FALSE(mLock,CCI_ERROR_FAILURE);

  // Image must be initialized first
  if(mImage.IsNull()) 
     return CCI_ERROR_NOT_INITIALIZED;
  
  dmRegion* nativeRgn = CCI_IF_NATIVE(rgn);
  cci_result rv;

  dm_uint32 width  = mImage->Width();
  dm_uint32 height = mImage->Height();

  EPixelFormat format        = mImage->PixelFormat();  
  EPixelFormat surfaceFormat = surface->PixelFormat();
  
  if(!surface->HasConvertCapabilities()) 
    format = surfaceFormat;

  if(!surface->HasSamplingCapabilities()) {
    width  = surface->Width();
    height = surface->Height();
  }
    
  dmImageData _Data;
  dm_uint32 lock = cciISurface::ELockRead|cciISurface::ELockUserBuffer;
    
  if(format != mImage->PixelFormat()
    ||width != mImage->Width()
    ||height!= mImage->Height())
  {
    // Create a new image to hold surface data
    dmLink<dmImage> dstImage;
    dmImageDescriptor* desc = dmGetDescriptor(format);
    if(desc)
       dstImage = desc->CreateImage(width,height);
    
    if(dstImage.IsNull())
       return CCI_ERROR_OUT_OF_MEMORY; 
    
    ::GetImageData(dstImage,_Data,mEnableAlpha);
     
    // Copy data in our buffer
    if(nativeRgn) rv = surface->LockBitsRect(nativeRgn->Rectangle(),_Data.PixelFormat,_Data,lock);
    else          rv = surface->LockBits(_Data.PixelFormat,_Data,lock);
    surface->UnlockBits(_Data);

    if(CCI_FAILED(rv))
      return CCI_ERROR_FAILURE;
    
    if(width!=mImage->Width()||height!=mImage->Height()) 
    {
      cci_Ptr<cciITransforms> tr = do_GetService("@daim.org/processing/transforms;1",&rv);
      CCI_ENSURE_SUCCESS(rv,rv);

      dmLink<dmImage> tmp;

      if(format!=mImage->PixelFormat()) 
      {
        // We need to resample the data in a temporary image
        tmp = desc->CreateImage(mImage->Width(),mImage->Height());
        rv = tr->Strech(native_Wrapper(dstImage),dm_null,dmBilinear,native_Wrapper(tmp));
        
        // Copy back data to shell image (conversion is done automagically)
        if(CCI_SUCCEEDED(rv))
           mImage->GetCopy(*tmp);
        
      } else {
        // Resample directly into shell image
        rv = tr->Strech(native_Wrapper(dstImage),dm_null,dmBilinear,native_Wrapper(mImage));
      }
      
      dstImage.Release();
    }
  }
  else
  {
    // Sample data directly from surface

    ::GetImageData(mImage,_Data,mEnableAlpha);

    // Copy data in our buffer
    if(nativeRgn) rv = surface->LockBitsRect(nativeRgn->Rectangle(),_Data.PixelFormat,_Data,lock);
    else          rv = surface->LockBits(_Data.PixelFormat,_Data,lock);
    surface->UnlockBits(_Data);
  }

  return rv;
}

/* void writeSurfaceBits (in cciISurface surface, in cciRegion rgn); */
CCI_IMETHODIMP cciScriptableImage::WriteSurfaceBits(cciISurface *surface, cciRegion rgn)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(surface);

  dmRegion* nativeRgn = CCI_IF_NATIVE(rgn);

  dmRect dstRect(0,0,surface->Width(),surface->Height());

  if(nativeRgn && !dstRect.Clip(nativeRgn->Rectangle()))
     return CCI_OK; //Nothing to do

  dmImageData _Data;
  ::GetImageData(mImage,_Data,mEnableAlpha);
  
  dm_uint32 lock = cciISurface::ELockWrite|cciISurface::ELockUserBuffer;
 
  // Copy image data in surface
  cci_result rv = surface->LockBitsRect(dstRect,_Data.PixelFormat,_Data,lock);
  CCI_ENSURE_SUCCESS(rv,rv);

  // Set color table if any
  if(mColorTable && mImage->PixelFormat()==dmPixelFormat8bppIndexed)
     surface->SetColorTable(mColorTable);
    
  rv = surface->UnlockBits(_Data);
  
  return rv;
}

/* attribute cciIColorTable colorTable; */
CCI_IMETHODIMP cciScriptableImage::GetColorTable(cciIColorTable * *aColorTable)
{
  if(!mColorTable)
     return CCI_ERROR_NOT_AVAILABLE;
  
  CCI_ADDREF( *aColorTable = mColorTable );
  return CCI_OK;
}
CCI_IMETHODIMP cciScriptableImage::SetColorTable(cciIColorTable *aColorTable)
{
  mColorTable = aColorTable;
  return CCI_OK;
}

/* void applyColorTable ([optional] in cciRegion rgn); */
CCI_IMETHODIMP cciScriptableImage::ApplyColorTable(cciRegion rgn)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  
  if(mColorTable && mImage->PixelFormat() == dmPixelFormat24bppRGB)
  {
    dm_uint32 c,colors[dmLUT8_MAX_COLORS];
    
    cci_result rv = mColorTable->GetColorEntries(reinterpret_cast<dm_uint32**>(&colors),dmLUT8_MAX_COLORS);
    if(CCI_FAILED(rv))
       return CCI_ERROR_FAILURE;
    
    dmRGBColorTable rgb_table;    

    //XXX Find what to do with alpha value
    for(int i=0;i<dmLUT8_MAX_COLORS;++i)
    {
      c = colors[i];
      rgb_table[i].r = DM_GETRVALUE(c);
      rgb_table[i].g = DM_GETGVALUE(c);
      rgb_table[i].b = DM_GETBVALUE(c);
    }

    dmRegion roi = rgn ? *CCI_NATIVE(rgn) : mImage->Rect();
    daim::apply_map(dmIImage<dmPixelFormat24bppRGB>::Cast(mImage)->Gen(),roi,rgb_table);
  }  
  
  return CCI_OK;
}

/* void fill (in boolean foregroundValue, [optional] in cciRegion rgn); */
CCI_IMETHODIMP cciScriptableImage::Fill(dm_bool foregroundValue, cciRegion rgn)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  
  dmRegion* nativeRgn = CCI_IF_NATIVE(rgn);
  if(nativeRgn)
     mImage->FillArea(*nativeRgn,foregroundValue);
  else
     mImage->FillArea(mImage->Rect(),foregroundValue);    
  
  return CCI_OK;
}

/* void getCopy (in cciImage srcImage, in cciRegion roi, in dm_int32 dstX, in dm_int32 dstY); */
CCI_IMETHODIMP cciScriptableImage::GetCopy(cciImage srcImage, cciRegion roi, dm_int32 dstX, dm_int32 dstY)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(srcImage);

  dmRegion rgn = roi ? *CCI_NATIVE(roi) : CCI_NATIVE(srcImage)->Rect();
  mImage->GetCopy(*CCI_NATIVE(srcImage),rgn,dmPoint(dstX,dstY));
 
  return CCI_OK;
}

/* cciIImage createCopy (in EPixelFormat format, [optional] in cciRegion rgn); */
CCI_IMETHODIMP cciScriptableImage::CreateCopy(EPixelFormat format, cciRegion rgn, cciIImage * *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  
  cciScriptableImage* copyOf = new cciScriptableImage();
  if(!copyOf)
      return CCI_ERROR_OUT_OF_MEMORY;
  
  dmRegion* nativeRgn = CCI_IF_NATIVE(rgn);
  dmRect rect = nativeRgn ? nativeRgn->Rectangle() : mImage->Rect();
  
  copyOf->mImage = dmCreateCopy(mImage,rect,format);
  
  if(copyOf->mImage.IsNull())
     goto error;

  // Clone color table
  if(mColorTable) 
  {
    mColorTable->Clone(getter_AddRefs(copyOf->mColorTable));
    // Apply color table  if needed
    if( dmIsPixelFormatScalar(mImage->PixelFormat())
    && ((format == dmPixelFormat24bppRGB)||(format == dmPixelFormat32bppARGB)))
      copyOf->ApplyColorTable(dm_null); 
  }

  CCI_ADDREF( *_retval = copyOf);
  return CCI_OK;

error:
  copyOf->mImage.Release();
  delete copyOf;

  return CCI_ERROR_FAILURE;
}

/* cciIImage clone (); */
CCI_IMETHODIMP cciScriptableImage::Clone(cciIImage * *_retval CCI_OUTPARAM)
{
  return CreateCopy(dmPixelFormatUndefined,dm_null,_retval);
}

