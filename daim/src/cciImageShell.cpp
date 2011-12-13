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
#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"
#include "cciImageUtils.h"
#include "cciIFilterContext.h"
#include "cciIImageFilter.h"
#include "cciIImageContainer.h"
#include "cciIRegionContainer.h"
#include "cciIImageMath.h"
#include "cciISurface.h"
#include "cciScriptableImage.h"
#include "cciImageShell.h"

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciImageShell, cciIImageShell)

cciImageShell::cciImageShell()
{
  /* member initializers and constructor code */
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

//===============================================
// cciIImageShell
//===============================================

/* attribute cciIImage image; */
CCI_IMETHODIMP cciImageShell::GetImage(cciIImage * *aImage)
{
  CCI_IF_ADDREF(*aImage = mImage);
  return CCI_OK;
}
CCI_IMETHODIMP cciImageShell::SetImage(cciIImage *aImage)
{
  mImage = aImage;
  return CCI_OK;
}

/* void create (in unsigned long width, in unsigned long height, in EPixelFormat format ); */
CCI_IMETHODIMP cciImageShell::Create(dm_uint32 width, dm_uint32 height, EPixelFormat format )
{
  mImage = dm_null;  
  cci_result rv = CCI_NewImage(width,height,format,getter_AddRefs(mImage));
  return rv;
}

/* void destroy(); */
CCI_IMETHODIMP cciImageShell::Destroy()
{
  mImage = dm_null;

  if(mFilterContext) {
     mFilterContext->ClearBuffer();
     mFilterContext->SetRange(0,0);
  }
  
  return CCI_OK;
}

/* void loadImage (in string path ); */
CCI_IMETHODIMP cciImageShell::LoadImage(const char * path )
{
  CCI_ENSURE_ARG_POINTER(path);

  mImage = dm_null;
  
  cci_result rv = CCI_LoadImage(dm_null,path,getter_AddRefs(mImage));

  return rv;
}

/* void saveImage (in string path, in string type, in string options); */
CCI_IMETHODIMP cciImageShell::SaveImage(const char * path, const char * type, const char* options)
{
  CCI_ENSURE_TRUE(mImage,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(path);
  CCI_ENSURE_ARG_POINTER(type);

  cci_result rv;

  bool compat = false;
  if(mDriverCache)
     mDriverCache->IsCompatibleDriver(type,&compat);

  if(!compat)
  {
    rv = CCI_GetLoader(dm_null,type,true,getter_AddRefs(mDriverCache));
    if(CCI_FAILED(rv))
       return rv;
  }

  rv = CCI_SaveImage(mDriverCache,path,mImage,options);
  
  return rv;
}

/* void copy (in cciImage srcImage, in cciRegion roi, in dm_int32 dstX, in dm_int32 dstY, in dm_uint32 mode); */
CCI_IMETHODIMP cciImageShell::Copy(cciImage srcImage, cciRegion roi, dm_int32 dstX, dm_int32 dstY, dm_uint32 mode)
{
  CCI_ENSURE_TRUE(mImage,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(srcImage);

  if(mode)
  {
    cci_result rv = EnsureImageMath();
    if(CCI_FAILED(rv))
       return rv;

    return mImageMath->DoImageMath(mImage,srcImage,roi,dstX,dstY,mode,mFilterContext);
  }
  else
  {
    mImage->GetCopy(srcImage,roi,dstX,dstY);
    return CCI_OK;
  }
}

/* void applyFilter (in cciIImageFilter filter, in cciRegion rgn); */
CCI_IMETHODIMP cciImageShell::ApplyFilter(cciIImageFilter *filter, cciRegion rgn)
{
  CCI_ENSURE_TRUE(mImage,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(filter);

  cci_result rv = EnsureFilterContext();
  if(CCI_FAILED(rv))
     return rv;

  return filter->ApplyFilter(mImage,rgn,mFilterContext);
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
  CCI_ENSURE_TRUE(mImage,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(context);

  if(mFilterContext)
  {
    dm_real minrange,maxrange;
    mFilterContext->GetMaxRange(&maxrange);
    mFilterContext->GetMinRange(&minrange);
    context->SetRange(minrange,maxrange);
  }

  return context->CreateBuffer(mImage,rgn,0,0,0);
}

/* void restoreFilterContext (in cciIFilterContext context); */
CCI_IMETHODIMP cciImageShell::RestoreFilterContext(cciIFilterContext *context)
{
  CCI_ENSURE_TRUE(mImage,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(context);

  cci_result rv = EnsureFilterContext();
  if(CCI_SUCCEEDED(rv))
  {
    dm_real minrange,maxrange;
    context->GetMaxRange(&maxrange);
    context->GetMinRange(&minrange);
    mFilterContext->SetRange(minrange,maxrange);
  }

  return context->RestoreBuffer(mImage);
}
