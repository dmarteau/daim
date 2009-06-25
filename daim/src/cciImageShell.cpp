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
// File         : cciImageShell.cpp
// Date         : 12 déc. 2008
// Author       : David Marteau
//--------------------------------------------------------
#define dmUseKernelImageTemplates
#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"
#include "cciImageUtils.h"
#include "cciIFilterContext.h"
#include "cciIImageFilter.h"
#include "cciIImageList.h"
#include "cciIImageContainer.h"
#include "cciIRegionContainer.h"
#include "cciIImageMath.h"
#include "cciITransforms.h"
#include "cciISurface.h"
#include "cciIMetaDataContainer.h"
#include "cciIRemoteSurface.h"
#include "cciILoaderService.h"
#include "cciISurfaceDriver.h"

#include "cciImageShell.h"

#include "daim_kernel.h"
#include "cciIImageListContainer.h"

/* Implementation file */
CCI_IMPL_ISUPPORTS2(cciImageShell, cciIImageShell,
                                   cciIImageContainer)

cciImageShell::cciImageShell()
: mLock(DM_FALSE)
, mLockAlpha(DM_FALSE)
, mPreserveMetaData(DM_TRUE)
{
  /* member initializers and constructor code */
}

cciImageShell::cciImageShell(const cciImageShell* aSrc)
: mLock(DM_FALSE)
, mLockAlpha(DM_FALSE)
, mPreserveMetaData(DM_TRUE)
{
  if(!aSrc->mImage.IsNull())
  {
    mImage = aSrc->mImage->CreateCopy();

    // Copy the alpha channel
    if(aSrc->mAlpha.IsNull())
       mAlpha = aSrc->mImage->CreateCopy();
  }
}

cciImageShell::~cciImageShell()
{
}

cci_result cciImageShell::EnsureFilterContext()
{
  if(!mFilterContext)
     return CCI_NewFilterContext(getter_AddRefs(mFilterContext));

  return CCI_OK;
}

cci_result cciImageShell::EnsureImageMath()
{
  if(!mImageMath)
  {
    cci_result rv = EnsureFilterContext();
    if(CCI_SUCCEEDED(rv))
       mImageMath = do_GetService("@daim.org/processing/imagemath;1",&rv);
    return rv;
  }

  return CCI_OK;
}

void cciImageShell::ClearOpenedResources()
{
  mCurrentSurface = dm_null;
  mColorTable     = dm_null;
}

//===============================================
// cciIImageContainer
//===============================================

/* [noscript,notxpcom] dmImagePtr getNative (); */
CCI_IMETHODIMP_(dmImage *) cciImageShell::GetNative()
{
   return mImage;
}

//===============================================
// cciIImageShell
//===============================================


static cci_result GetImageDataRect( dmImageData & imData, dm_rect& rect )
{
  dmRect _DstRect = rect;

  if(!_DstRect.Clip(0,0,imData.Width-1,imData.Height-1))
     return CCI_ERROR_OUT_OF_RANGE;

  int PixelStride = dmGetPixelFormatBits(imData.PixelFormat) >> 3;

  imData.Width  = _DstRect.Width();
  imData.Height = _DstRect.Height();
  imData.Scan0  = static_cast<dm_byte*>(imData.Scan0)  +
                  (imData.Stride * _DstRect.Top()) + (_DstRect.Left() * PixelStride);

  return CCI_OK;
}

/* [noscript] void lock (in dmImageDataRef imData, in dmRectPtr rect); */
CCI_IMETHODIMP cciImageShell::Lock(dmImageData & imData, dm_rect *rect)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_FALSE(mLock,CCI_ERROR_FAILURE);

  mLock = DM_TRUE;
  mImage->GetImageData(imData);

  if(rect)
     return GetImageDataRect(imData,*rect);

  return CCI_OK;
}

/* [noscript] void unLock (); */
CCI_IMETHODIMP cciImageShell::UnLock()
{
  CCI_ENSURE_TRUE(mLock,CCI_ERROR_FAILURE);

  mLock = DM_FALSE;
  return CCI_OK;
}

/* [noscript] void lockAlpha (in dmImageDataRef imData, in dmRectPtr rect); */
CCI_IMETHODIMP cciImageShell::LockAlpha(dmImageData & imData, dm_rect *rect)
{
  CCI_ENSURE_FALSE(mAlpha.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_FALSE(mLockAlpha,CCI_ERROR_FAILURE);

  mLockAlpha = DM_TRUE;
  mAlpha->GetImageData(imData);

  if(rect)
     return GetImageDataRect(imData,*rect);

  return CCI_OK;
}

/* [noscript] void unLockAlpha (); */
CCI_IMETHODIMP cciImageShell::UnLockAlpha()
{
  CCI_ENSURE_TRUE(mLockAlpha,CCI_ERROR_FAILURE);

  mLockAlpha = DM_FALSE;
  return CCI_OK;
}

/* readonly attribute unsigned long width; */
CCI_IMETHODIMP cciImageShell::GetWidth(dm_uint32 *aWidth)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(aWidth);

  *aWidth = mImage->Width();
  return CCI_OK;
}

/* readonly attribute unsigned long height; */
CCI_IMETHODIMP cciImageShell::GetHeight(dm_uint32 *aHeight)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(aHeight);

  *aHeight = mImage->Height();
  return CCI_OK;
}

/* readonly attribute EPixelFormat pixelFormat; */
CCI_IMETHODIMP cciImageShell::GetPixelFormat(EPixelFormat *aPixelFormat)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(aPixelFormat);

  *aPixelFormat = mImage->PixelFormat();
  return CCI_OK;
}

/* readonly attribute boolean hasAlpha; */
CCI_IMETHODIMP cciImageShell::GetHasAlpha(dm_bool *aHasAlpha)
{
  CCI_ENSURE_ARG_POINTER(aHasAlpha);

  *aHasAlpha = mAlpha.IsNull()?DM_FALSE:DM_TRUE;
  return CCI_OK;
}

/* void create (in unsigned long width, in unsigned long height, in EPixelFormat format, in dm_bool createAlpha); */
CCI_IMETHODIMP cciImageShell::Create(dm_uint32 width, dm_uint32 height, EPixelFormat format, dm_bool createAlpha)
{
  CCI_ENSURE_FALSE(mLock,CCI_ERROR_FAILURE);

  ClearOpenedResources();

  dmImageDescriptor* desc = dmGetDescriptor(format);
  if(desc)
  {
    mImage = desc->CreateImage(width,height);
    if(!mImage.IsNull())
    {
      if(createAlpha) {
         mImage = dmCreateImage<dmPixelFormat8bppIndexed>(width,height);
         if(mImage.IsNull())
            return CCI_ERROR_OUT_OF_MEMORY;
      }
       return CCI_OK;
    } else
      return CCI_ERROR_OUT_OF_MEMORY;
  }

  return CCI_ERROR_FAILURE;
}

/* void initFromImageList (in cciIImageList imagelist, in unsigned long index, in boolean share); */
CCI_IMETHODIMP cciImageShell::LoadBuffer(cciIImageList *imagelist, dm_uint32 index, dm_bool share)
{
  CCI_ENSURE_FALSE(mLock,CCI_ERROR_FAILURE);
  CCI_ENSURE_ARG_POINTER(imagelist);

  ClearOpenedResources();

  cci_result rv;

  cci_Ptr<cciIImageListContainer> aListContainer = do_QueryInterface(imagelist,&rv);
  if(CCI_FAILED(rv))
     return rv;

  dmLink<dmImage> src;

  rv = aListContainer->GetImageLink(src,index);
  if(CCI_SUCCEEDED(rv))
  {
    if(share)
      mImage = src;
    else
      mImage = src->CreateCopy();
  }

  return rv;
}


/* void loadSurfaceBits (in cciISurface surface, in cciRegion rgn, in boolean getAlpha); */
CCI_IMETHODIMP cciImageShell::LoadSurfaceBits(cciISurface *surface, cciRegion rgn, dm_bool getAlpha)
{
  CCI_ENSURE_ARG_POINTER(surface);
  CCI_ENSURE_FALSE(mLock,CCI_ERROR_FAILURE);
  CCI_ENSURE_FALSE(mLockAlpha,CCI_ERROR_FAILURE);

  dmRegion* nativeRgn = rgn ? rgn->GetNative() : dm_null;

  ClearOpenedResources();

  cci_result rv;

  EPixelFormat surfaceFormat = surface->PixelFormat();

  if(nativeRgn) {
    dmRect& r = nativeRgn->Rectangle();
    rv = Create(r.Width(),r.Height(),surfaceFormat,getAlpha);
  }
  else {
    rv = Create(surface->Width(),surface->Height(),surfaceFormat,getAlpha);
  }

  if(CCI_FAILED(rv))
     return rv;

  dmImageData _Data;
  mImage->GetImageData(_Data);

  dm_uint32 lock = cciISurface::ELockRead|cciISurface::ELockUserBuffer;

  // Copy data in our buffer
  if(nativeRgn) rv = surface->LockBitsRect(nativeRgn->Rectangle(),_Data.PixelFormat,_Data,lock);
  else          rv = surface->LockBits(_Data.PixelFormat,_Data,lock);

  // Try to get a possible color table
  if(mImage->PixelFormat()==dmPixelFormat8bppIndexed)
     surface->GetColorTable(getter_AddRefs(mColorTable));

  surface->UnlockBits(_Data);

  if(CCI_SUCCEEDED(rv) && getAlpha && surface->HasAlpha())
  {
    mAlpha->GetImageData(_Data);

    if(nativeRgn) rv = surface->LockBitsRect(nativeRgn->Rectangle(),_Data.PixelFormat,_Data,lock|cciISurface::ELockAlpha);
    else          rv = surface->LockBits(_Data.PixelFormat,_Data,lock|cciISurface::ELockAlpha);

    surface->UnlockBits(_Data);
  }

  return rv;
}

/* void writeSurfaceBits (in cciISurface surface, in cciRegion rgn, in boolean getAlpha); */
CCI_IMETHODIMP cciImageShell::WriteSurfaceBits(cciISurface *surface, cciRegion rgn, dm_bool getAlpha)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(surface);

  dmRegion* nativeRgn = rgn ? rgn->GetNative() : dm_null;

  dmRect srcRect = mImage->Rect();

  if(nativeRgn && !srcRect.Clip(nativeRgn->Rectangle()))
     return CCI_OK; //Nothing to do

  dmImageData _Data;
  mImage->GetImageData(_Data);

  dm_uint32 lock = cciISurface::ELockWrite|cciISurface::ELockUserBuffer;

  // Copy image data in surface
  cci_result rv = surface->LockBitsRect(srcRect,_Data.PixelFormat,_Data,lock);
  CCI_ENSURE_SUCCESS(rv,rv);

  // Set color table if any
  if(mColorTable && mImage->PixelFormat()==dmPixelFormat8bppIndexed)
     surface->SetColorTable(mColorTable);

  rv = surface->UnlockBits(_Data);

  if(CCI_SUCCEEDED(rv) && getAlpha && !mAlpha.IsNull())
  {
    mAlpha->GetImageData(_Data);

    rv = surface->LockBitsRect(srcRect,_Data.PixelFormat,_Data,lock|cciISurface::ELockAlpha);
    CCI_ENSURE_SUCCESS(rv,rv);

    rv = surface->UnlockBits(_Data);
  }

  return rv;
}


/* void loadImage (in string path, in boolean getAlpha ); */
CCI_IMETHODIMP cciImageShell::LoadImage(const char * path, dm_bool getAlpha )
{
  CCI_ENSURE_ARG_POINTER(path);

  ClearOpenedResources();

  cci_result rv;

  cci_Ptr<cciILoaderService> ldrSrvc = do_GetService("@daim.org/contrib/loader-service;1",&rv);
  if(CCI_FAILED(rv))
     return rv;

  cci_Ptr<cciISurface> surface;

  rv = ldrSrvc->OpenSurface(path,cciIDriverProxy::IO_READONLY,getter_AddRefs(surface));
  if(CCI_FAILED(rv))
     return rv;

  rv = LoadSurfaceBits(surface,dm_null,getAlpha);

  if(CCI_SUCCEEDED(rv))
     mCurrentSurface = surface;

  return rv;
}

/* void saveImage (in string path, in string type, in string options); */
CCI_IMETHODIMP cciImageShell::SaveImage(const char * path, const char * type, const char* options)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(path);
  CCI_ENSURE_ARG_POINTER(type);

  cci_result rv;

  dm_bool compat = DM_FALSE;
  if(mDriverCache)
     mDriverCache->IsCompatibleDriver(type,&compat);

  if(!compat)
  {
    cci_Ptr<cciILoaderService> ldrSrvc = do_GetService("@daim.org/contrib/loader-service;1",&rv);
    if(CCI_FAILED(rv))
       return rv;

    // Get the corresponding driver
    rv = ldrSrvc->GetDriver(type,DM_TRUE,getter_AddRefs(mDriverCache));
    if(CCI_FAILED(rv))
       return rv;
  }

  dmImageData _Data;
  mImage->GetImageData(_Data);

  cci_Ptr<cciIMetaDataContainer> _MetaData;

  if(mPreserveMetaData && mCurrentSurface)
    _MetaData = do_QueryInterface(mCurrentSurface);

  if(mColorTable && mImage->PixelFormat()==dmPixelFormat8bppIndexed)
  {
    // Create a temporary surface for setting the color table
    cci_Ptr<cciIRemoteSurface> tmpSurface;

    rv = mDriverCache->CreateTmpSurfaceFromImageBits(_Data,_MetaData,getter_AddRefs(tmpSurface));
    if(CCI_SUCCEEDED(rv))
    {
      tmpSurface->SetColorTable(mColorTable);

      rv = tmpSurface->CreateCopy(path,options);
    }
  }
  else
    rv = mDriverCache->SaveImageBits(path,_Data,_MetaData,options);

  return rv;
}


/* void clear (); */
CCI_IMETHODIMP cciImageShell::Clear()
{
  mImage.Release();
  mAlpha.Release();

  if(mFilterContext) {
     mFilterContext->ClearBuffer();
     mFilterContext->SetRange(0,0);
  }

  return CCI_OK;
}

/* readonly attribute cciISurface currentSurface; */
CCI_IMETHODIMP cciImageShell::GetCurrentSurface(cciISurface * *aCurrentSurface)
{
  if(mCurrentSurface) {
     CCI_ADDREF( *aCurrentSurface = mCurrentSurface );
     return CCI_OK;
  }

  return CCI_ERROR_NOT_AVAILABLE;
}

/* attribute boolean preserveMetaData; */
CCI_IMETHODIMP cciImageShell::GetPreserveMetaData(dm_bool *aPreserveMetaData)
{
  CCI_ENSURE_ARG_POINTER(aPreserveMetaData);

  *aPreserveMetaData = mPreserveMetaData;
  return CCI_OK;
}
CCI_IMETHODIMP cciImageShell::SetPreserveMetaData(dm_bool aPreserveMetaData)
{
  mPreserveMetaData = aPreserveMetaData;
  return CCI_OK;
}

/* cciIImageShell clone (); */
CCI_IMETHODIMP cciImageShell::Clone(cciIImageShell * *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);

  cciImageShell* cloned = new cciImageShell(this);
  if(!cloned)
      return CCI_ERROR_OUT_OF_MEMORY;

  if(mPreserveMetaData)
     cloned->mCurrentSurface = mCurrentSurface;

  CCI_ADDREF( *_retval = cloned);
  return CCI_OK;
}

/* cciIImageShell createCopy (in cciRegion rgn, in EPixelFormat format); */
CCI_IMETHODIMP cciImageShell::CreateCopy(cciRegion rgn, EPixelFormat format, cciIImageShell * *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);

  cciImageShell* cloned = new cciImageShell();
  if(!cloned)
      return CCI_ERROR_OUT_OF_MEMORY;

  if(format == dmPixelFormatUndefined)
     format  = mImage->PixelFormat();

  dmRect rect = rgn ? rgn->GetNative()->Rectangle() : mImage->Rect();

  cloned->mImage = dmCreateCopy(mImage,rect,format);

  if(cloned->mImage.IsNull())
     goto out;

  if(!mAlpha.IsNull())
  {
    cloned->mAlpha = mAlpha->CreateCopy(rect);
    if(cloned->mAlpha.IsNull())
       goto out;
  }

  if(mPreserveMetaData)
     cloned->mCurrentSurface = mCurrentSurface;

  CCI_ADDREF( *_retval = cloned);
  return CCI_OK;

out:
  cloned->mImage.Release();
  delete cloned;

  return CCI_ERROR_FAILURE;
}

/* void resample (in cciImage dest, in cciRegion rgn, in unsigned long mode, in dm_bool getAlpha); */
CCI_IMETHODIMP cciImageShell::Resample(cciImage dest, cciRegion rgn, dm_uint32 mode, dm_bool getAlpha)
{
  CCI_ENSURE_ARG_POINTER(dest);
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);

  dmImage* nativeImg = dest->GetNative();
  CCI_PRECONDITION(nativeImg,"null image");

  cci_result rv;
  cci_Ptr<cciITransforms> tr = do_GetService("@daim.org/processing/transforms;1",&rv);
  if(CCI_FAILED(rv))
     return rv;

  if(getAlpha)
  {
    if(mAlpha.IsNull())
      return CCI_ERROR_NOT_AVAILABLE;

    rv = tr->Strech(native_Wrapper(mAlpha),rgn,mode,dest);
  } else
    rv = tr->Strech(native_Wrapper(mImage),rgn,mode,dest);

  return rv;
}

/* void getCopy (in cciImage srcImage, in cciRegion roi, in dm_int32 dstX, in dm_int32 dstY, in dm_uint32 mode); */
CCI_IMETHODIMP cciImageShell::GetCopy(cciImage srcImage, cciRegion roi, dm_int32 dstX, dm_int32 dstY, dm_uint32 mode)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(srcImage);

  if(mode)
  {
    cci_result rv = EnsureImageMath();
    if(CCI_FAILED(rv))
       return rv;

    return mImageMath->DoImageMath(native_Wrapper(mImage),srcImage,roi,dstX,dstY,mode,mFilterContext);
  }
  else
  {
    dmRegion rgn = roi ? *CCI_NATIVE(roi) : CCI_NATIVE(srcImage)->Rect();
    mImage->GetCopy(*CCI_NATIVE(srcImage),rgn,dmPoint(dstX,dstY));
    return CCI_OK;
  }
}

/* void blend (in cciImage srcImage, in dm_real percent, in cciRegion roi, in dm_int32 dstX, in dm_int32 dstY); */
CCI_IMETHODIMP cciImageShell::Blend(cciImage srcImage, dm_real percent, cciRegion roi, dm_int32 dstX, dm_int32 dstY)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(srcImage);

  cci_result rv = EnsureImageMath();
  if(CCI_FAILED(rv))
     return rv;

  return mImageMath->Blend(srcImage,native_Wrapper(mImage),percent,roi,dstX,dstY,mFilterContext);
}


/* void copyToBuffer (in cciIImageList imagelist, in unsigned long index, in cciRegion rgn, in EPixelFormat format); */
CCI_IMETHODIMP cciImageShell::CopyToBuffer(cciIImageList *imagelist, dm_uint32 index, cciRegion rgn, EPixelFormat format)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(imagelist);

  dmRect rect = rgn ? CCI_NATIVE(rgn)->Rectangle() : mImage->Rect();

  return imagelist->StoreBuffer(index,mImage,&rect,format);
}


/* void moveToBuffer (in cciIImageList imagelist, in unsigned long index); */
CCI_IMETHODIMP cciImageShell::MoveToBuffer(cciIImageList *imagelist, dm_uint32 index)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(imagelist);

  cci_result rv;

  cci_Ptr<cciIImageListContainer> aListContainer = do_QueryInterface(imagelist,&rv);
  if(CCI_SUCCEEDED(rv))
     rv = aListContainer->SetImageLink(mImage,index);

  return rv;
}


/* void applyFilter (in cciIImageFilter filter, in cciRegion rgn); */
CCI_IMETHODIMP cciImageShell::ApplyFilter(cciIImageFilter *filter, cciRegion rgn)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_FALSE(mLock,CCI_ERROR_FAILURE);
  CCI_ENSURE_ARG_POINTER(filter);

  cci_result rv = EnsureFilterContext();
  if(CCI_FAILED(rv))
     return rv;

  return filter->ApplyFilter(native_Wrapper(mImage),rgn,mFilterContext);
}

/* readonly attribute cciIFilterContext filterContext; */
CCI_IMETHODIMP cciImageShell::GetFilterContext(cciIFilterContext * *aFilterContext)
{
  cci_result rv = EnsureFilterContext();

  CCI_IF_ADDREF( *aFilterContext = mFilterContext );
  return rv;
}

/* void saveFilterContext (in cciIFilterContext context, in cciRegion rgn); */
CCI_IMETHODIMP cciImageShell::SaveFilterContext(cciIFilterContext *context, cciRegion rgn)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(context);

  if(mFilterContext)
  {
    dm_real minrange,maxrange;
    mFilterContext->GetMaxRange(&maxrange);
    mFilterContext->GetMinRange(&minrange);
    context->SetRange(minrange,maxrange);
  }

  return context->CreateBuffer(native_Wrapper(mImage),rgn,0,0,0);
}

/* void restoreFilterContext (in cciIFilterContext context); */
CCI_IMETHODIMP cciImageShell::RestoreFilterContext(cciIFilterContext *context)
{
  CCI_ENSURE_FALSE(mImage.IsNull(),CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(context);

  cci_result rv = EnsureFilterContext();
  if(CCI_SUCCEEDED(rv))
  {
    dm_real minrange,maxrange;
    context->GetMaxRange(&maxrange);
    context->GetMinRange(&minrange);
    mFilterContext->SetRange(minrange,maxrange);
  }

  return context->RestoreBuffer(native_Wrapper(mImage));
}
