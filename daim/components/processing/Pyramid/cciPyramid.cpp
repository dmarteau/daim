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
// File         : cciPyramid.cpp
// Date         : 25 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciCOMPtr.h"
#include "cciInterfaceRequestorUtils.h"
#include "cciIFilterContext.h"
#include "cciIImageList.h"
#include "cciIPyramid.h"
#include "cciIKernelService.h"
#include "cciIImageFilter.h"
#include "cciIRegionContainer.h"
#include "cciIImageContainer.h"
#include "cciIKernelFamilyContainer.h"

#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "common/dmUserLib.h"


/* Header file */
class cciPyramid : public cciIPyramid
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IPYRAMID

  cciPyramid();

private:
  ~cciPyramid();

protected:
  dm_uint32      mFactor;
  dmKernelFamily mKernel;

  cci_result EnsureKernel();
  cci_result doGenerate( dmImage* image, const dmRect& srcRect,
                         cciIImageList* _IImages, dm_uint32 sizelimit, dm_uint32 limit,
                         dm_int32 prevIndex,
                         cciIFilterContext* );

};

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciPyramid, cciIPyramid)

cciPyramid::cciPyramid()
: mFactor(2)
{
  /* member initializers and constructor code */
}

cciPyramid::~cciPyramid()
{
  /* destructor code */
}

cci_result cciPyramid::EnsureKernel()
{
  cci_result rv;
  if(mKernel.Empty())
     rv = CCI_GetKernelFamily("LowPass/Gauss5x5",mKernel);
  else
    rv = CCI_OK;

  if(CCI_FAILED(rv))
     dmLOG_ERROR("Cannot get kernel for pyramid operation !");

  return rv;
}

//---------------------------------------------------------------------
cci_result cciPyramid::doGenerate( dmImage* image, const dmRect& srcRect,
                                   cciIImageList* _IImages, dm_uint32 sizelimit, dm_uint32 limit,
                                   dm_int32 prevIndex,
                                   cciIFilterContext* context )
{
   dmImageBuffer& buffer = *context->NativeBuffer();

   dmImage *src,*dst;
   dmRegion srcRgn;

   src = image; //_IImages->GetNativeBuffer(0);
   if(src==NULL)
      return CCI_ERROR_FAILURE;

   dm_uint32 steps = 1;

   if(limit == 0)
      limit = DM_UINT32_MAX; // Don't set limit on steps

   if(sizelimit == DM_UINT32_MAX||sizelimit < 4*mFactor)
      sizelimit = 4*mFactor;

   dmPoint factors(mFactor,mFactor);

   // Compute the number of steps (levels)
   // in the pyramid

   dmPoint size = srcRect.Dimensions(); // = src->Dimensions();

   size.x /= factors.x;
   size.y /= factors.y;

   while( (static_cast<dm_uint>(size.x) >= sizelimit) &&
          (static_cast<dm_uint>(size.y) >= sizelimit) &&
          (steps  <= limit))
   {
     size.x /= factors.x;
     size.y /= factors.y;
     steps++;
   }

   // Make room for images
   _IImages->SetSize(steps+prevIndex);

   // Downsampling

   size = srcRect.Dimensions(); //src->Dimensions();

   // Set source region
   srcRgn.SetRectRoi(srcRect);

   EPixelFormat format = src->PixelFormat();

   // Holder for RGB source channel
   dmLink<dmImage> srcChannel;
   if(format==dmPixelFormat24bppRGB)
      srcChannel = dmCreateImage<dmPixelFormat8bppIndexed>(src->Width(),src->Height());

   // Loop over the number of steps
   for(dm_uint32 i=1;i<steps;++i)
   {
     size.x /= factors.x;
     size.y /= factors.y;

     #ifdef _DEBUG
       dmLOG("cciIPyramid::doGenerate %d x %d , (%d)",size.x,size.y,i);
     #endif

     // Create new buffer to hold result
     cci_result rv = _IImages->CreateBuffer(i+prevIndex,format,size.x,size.y);
     if(CCI_FAILED(rv))
        return rv;

     dst = _IImages->GetNativeBuffer(i+prevIndex);
     dmASSERT(dst != NULL);

     // Handle rgb format
     if(format==dmPixelFormat24bppRGB)
     {
       // Create new dest channel
       dmLink<dmImage> dstChannel = dmCreateImage<dmPixelFormat8bppIndexed>(size.x,size.y);

       for(dm_uint chan=1;chan<=3;++chan)
       {
         // Extract src channel
         if(!dmCopyRGBData(*src,*srcChannel,chan))
             return CCI_ERROR_FAILURE;

         dmBufferParameters _Params(buffer ,*srcChannel,srcRgn);
         dmDownSampling     _Filter(mKernel,*dstChannel,factors);

         if(!_Filter.Apply(_Params))
            return CCI_ERROR_FAILURE;

         // Merge channel into dest
         if(!dmCopyRGBData(*dstChannel,*dst,chan))
             return CCI_ERROR_FAILURE;
       }

       srcChannel = dstChannel; // do not realloc memory for src channel
     }
     else
     {
       dmBufferParameters _Params(buffer ,*src,srcRgn);
       dmDownSampling     _Filter(mKernel,*dst,factors);

       if(!_Filter.Apply(_Params))
          return CCI_ERROR_FAILURE;
     }

     src = dst;
     srcRgn.SetRectRoi(src->Rect());
   }

   return CCI_OK;
}
//====================================
// cciIPyramid
//====================================

/* void generate (in cciImage image, in cciRegion roi, in cciIImageList imagelist, in unsigned long sizelimit, in cciISupports context); */
CCI_IMETHODIMP cciPyramid::Generate(cciImage image, cciRegion roi, cciIImageList *imagelist,
                                    dm_uint32 sizelimit, cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(imagelist);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  dmRect srcRect = roi ? CCI_NATIVE(roi)->Rectangle() : CCI_NATIVE(image)->Rect();

  cci_result rv = EnsureKernel();
  if(CCI_FAILED(rv))
     return rv;

  rv = doGenerate(CCI_NATIVE(image),srcRect,imagelist,sizelimit,DM_UINT32_MAX,0,filterCtxt);

  return rv;
}

/* void reconstruct (in cciIImageList srcImages, in cciIImageList dstImages, in unsigned long from, in unsigned long to, in cciISupports context); */
CCI_IMETHODIMP cciPyramid::Reconstruct(cciIImageList *srcImages, cciIImageList *dstImages,
                                       dm_uint32 from, dm_uint32 to,
                                       cciIFilterContext *filterCtxt)
{
  //FIXME handle RGB images

  CCI_ENSURE_ARG_POINTER(srcImages);
  CCI_ENSURE_ARG_POINTER(dstImages);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  dmImage* src = srcImages->GetNativeBuffer(from);
  if(!src)
     return CCI_ERROR_INVALID_ARG;

  cci_result rv = EnsureKernel();
  if(CCI_FAILED(rv))
     return rv;

  if(to >= from )
  {
    return dstImages->StoreBuffer(to,src,dm_null,static_cast<EPixelFormat>(0));
  }
  else
  {
    dmPoint size = src->Dimensions();
    dmPoint factors(mFactor,mFactor);
    dmRegion srcRgn; // Needed in dmBufferParameters

    dmPoint endSize(size);
    for(dm_uint i=to;i<from;++i) {
      endSize.x = (endSize.x - 1) * factors.x + 1;
      endSize.y = (endSize.y - 1) * factors.y + 1;
    }

    // Preallocate a buffer large enough to hold the full final image
    // The idea is to use that buffer as a source and a destination
    // copy will not overlap as long as dmUpSampling use his
    // parameter \a buffer as temporary

    dmRect dst_rect(0,0,endSize.x,endSize.y);

    rv = dstImages->CreateBuffer(to,src->PixelFormat(),endSize.x,endSize.y);
    if(CCI_FAILED(rv))
       return rv;

    dmImage* dst = dstImages->GetNativeBuffer(to);
    dmDEBUG_ASSERT(dst != NULL);

    // Copy the source image into dest
    dst->GetCopy(*src);

    // Up sampling
    dmBufferParameters params(*filterCtxt->NativeBuffer(),*dst,srcRgn);
    for(;to < from;++to)
    {
      // The size of the input image
      srcRgn.SetRectRoi(dmRect(0,0,size.x,size.y));

      dmUpSampling _Filter(mKernel,*dst,srcRgn.Rectangle(),factors);

      if(!_Filter.Apply(params))
         return CCI_ERROR_FAILURE;

      size.x = (size.x - 1) * factors.x + 1;
      size.y = (size.y - 1) * factors.y + 1;
    }
  }

  return rv;
}

/* void downSample (in cciImage image, in cciRegion roi, in cciIImageList imagelist, in unsigned long index, in cciISupports context); */
CCI_IMETHODIMP cciPyramid::DownSample(cciImage image, cciRegion roi,
                                      cciIImageList *imagelist, dm_uint32 index,
                                      cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(imagelist);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  dmRect srcRect = roi ? CCI_NATIVE(roi)->Rectangle() : CCI_NATIVE(image)->Rect();

  cci_result rv = EnsureKernel();
  if(CCI_FAILED(rv))
     return rv;

  rv = doGenerate(CCI_NATIVE(image),srcRect,imagelist,DM_UINT32_MAX,1,index-1,filterCtxt);

  return rv;

}

/* attribute cciKernelFamily kernel; */
CCI_IMETHODIMP cciPyramid::GetKernel(cciKernelFamily* aKernel)
{
  return CCI_NewKernelFamily(mKernel,aKernel);
}
CCI_IMETHODIMP cciPyramid::SetKernel(cciKernelFamily aKernel)
{
  mKernel = *CCI_NATIVE(aKernel);
  return CCI_OK;
}

/* attribute dm_uint32 factor; */
CCI_IMETHODIMP cciPyramid::GetFactor(dm_uint32 *aFactor)
{
  CCI_ENSURE_ARG_POINTER(aFactor);
  *aFactor = mFactor;
  return CCI_OK;
}
CCI_IMETHODIMP cciPyramid::SetFactor(dm_uint32 aFactor)
{
  mFactor = aFactor;
  return CCI_OK;
}


//=====================================
// Module
//=====================================

#include "cciIGenericFactory.h"

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciPyramid)

static const cciModuleComponentInfo components[] = {
    {  CCI_PYRAMID_CLASSNAME,
       CCI_PYRAMID_CID,
       CCI_PYRAMID_CONTRACTID,
       cciPyramidConstructor
    },
};

CCI_IMPL_GETMODULE(PyramidModule, components)

