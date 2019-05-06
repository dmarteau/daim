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
// File         : cciImageMath.cpp
// Date         : 11 déc. 2008
// Author       : David Marteau
//--------------------------------------------------------


#include "cciCOMPtr.h"
#include "cciInterfaceRequestorUtils.h"
#include "cciIFilterContext.h"
#include "cciIImageList.h"
#include "cciIImageMath.h"
#include "cciIRegionContainer.h"
#include "cciIImageContainer.h"

#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "common/dmUserLib.h"
#include "operators/dmAccumulator.h"

/* Header file */
class cciImageMath final : public cciIImageMath
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IIMAGEMATH

  cciImageMath();

private:
  ~cciImageMath();

protected:
  /* additional members */
};

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciImageMath, cciIImageMath)

cciImageMath::cciImageMath()
{
  /* member initializers and constructor code */
}

cciImageMath::~cciImageMath()
{
  /* destructor code */
}

//---------------------------------------------------------------------
// Defined in daim_kernel library
bool __dmKernel _dmDoImageMath2( dmBufferParameters& , dm_int );
bool __dmKernel _dmDoImageMath5( dm_int op, dmImage& lhs,
                                 const dmImage& rhs ,
                                 const dmRegion&, const dmPoint& );


static cci_result _DoImageMath(dmImage *lhsImage,
                               dmImage *rhsImage,
                               dmRegion& rgn,
                               dmPoint&  loc,
                               dm_int32 operation,
                               cciIFilterContext* fctxt)
{
  if(dmIsPixelFormatScalar(lhsImage->PixelFormat())  &&
     dmIsPixelFormatScalar(rhsImage->PixelFormat()))
  {
    switch(operation)
    {
      case dmTk::Math::MulPixels :
        dmMultiplyImage(*lhsImage,*rhsImage,rgn,loc);
        return CCI_OK;
      case dmTk::Math::DivPixels :
        dmDivideImage(*lhsImage,*rhsImage,rgn,loc);
        return CCI_OK;
        break;
    }
  }

  if(lhsImage->PixelFormat()!=rhsImage->PixelFormat())
  {
     if(!fctxt)
         return CCI_ERROR_FAILURE;

     dmImageBuffer* buffer = fctxt->NativeBuffer();

     // This will ensure that images will be of the same type
     buffer->CreateBuffer(*lhsImage,rgn);
     lhsImage->GetCopy(*rhsImage,rgn,loc);

     dmBufferParameters _params(*buffer,*lhsImage,rgn);
     _dmDoImageMath2(_params,operation);
  } else
     _dmDoImageMath5(operation,*lhsImage,*rhsImage,rgn,loc);

  return CCI_OK;
}
//============================
// cciIImageMath
//============================


/* void doImageMath (in cciImage lhsImage, in cciImage rhsImage, in cciRegion roi, in dm_int32 x, in dm_int32 y, in dm_int32 operation, in cciISupports context); */
CCI_IMETHODIMP cciImageMath::DoImageMath(cciImage lhsImage, cciImage rhsImage,
                                         cciRegion roi, dm_int32 x, dm_int32 y,
                                         dm_int32 operation,
                                         cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(lhsImage);
  CCI_ENSURE_ARG_POINTER(rhsImage);

  dmRegion rgn = roi ? *CCI_NATIVE(roi) : CCI_NATIVE(rhsImage)->Rect();
  dmPoint  loc(x,y);

  return _DoImageMath(CCI_NATIVE(lhsImage),CCI_NATIVE(rhsImage),rgn,loc,operation,filterCtxt);
}

/* void accumulate (in cciIImageList imagelist, in cciImage dstImage, in cciRegion roi, in bool normalize, in cciISupports context); */
CCI_IMETHODIMP cciImageMath::Accumulate(cciIImageList *imagelist, cciImage dstImage, cciRegion roi,
                                        bool normalize, cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(imagelist);

  dm_uint32 count = 0;

  imagelist->GetSize(&count);
  if(count>0)
  {
    dmRegion      rgn;
    dmImage*      rhsImage;
    dmAccumulator accumulator;

    // Set the destination rectangle
    if(dstImage==dm_null)
    {
      CCI_ENSURE_ARG_POINTER(filterCtxt);

      if(roi)
         rgn = *CCI_NATIVE(roi);

      if(rgn.IsEmptyRoi())
      {                                // The ROI is empty, get the total size from
        for(dm_uint i=0;i<count;++i)   // buffers size
        {
          rhsImage = imagelist->GetNativeBuffer(i);
          if(rhsImage!=NULL)
            rgn.AddCoordinates(rhsImage->Rect());
        }
      }

      dmImageBuffer* buffer = filterCtxt->NativeBuffer();

      // Create the buffer that will hold the result
      buffer->CreateBuffer(*imagelist->GetNativeBuffer(0)->TypeDescriptor(),rgn);
      accumulator.Initialize(buffer->Buffer(),rgn);
    }
    else
    {
      rgn = roi ? *CCI_NATIVE(roi) : CCI_NATIVE(dstImage)->Rect();
      // If the image is not null we should have a non empty ROI
      accumulator.Initialize(CCI_NATIVE(dstImage),rgn);
    }

    // Accumulate

    for(dm_uint i=0;i<count;++i)
    {
      rhsImage = imagelist->GetNativeBuffer(i);
      accumulator.Add( rhsImage );
    }

    accumulator.Finalize(normalize!=false);
  }

  // We always return OK because having no mages in list is a no-op
  return CCI_OK;
}

/* void doImageListMath (in cciIImageList lhsimages, in cciIImageList rhsimages, in cciRegion roi, in dm_int32 operation, in cciISupports context); */
CCI_IMETHODIMP cciImageMath::DoImageListMath(cciIImageList *lhsimages, cciIImageList *rhsimages, cciRegion roi,
                                             dm_int32 operation,
                                             cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(lhsimages);
  CCI_ENSURE_ARG_POINTER(rhsimages);

  dm_uint32 lhsCount,rhsCount;
  lhsimages->GetSize(&lhsCount);
  rhsimages->GetSize(&rhsCount);

  // Loop over all images in the list
  dm_uint count = dm_min(lhsCount,rhsCount);
  if(count>0)
  {
    cci_result rv;

    dmImage *lhs_image;
    dmImage *rhs_image;

    dmRegion rgn;
    dmPoint  loc;
    if(roi) {
       rgn = *CCI_NATIVE(roi);
       loc = rgn.Rectangle().TopLeft();
    }

    lhsimages->SetSize(count);

    for(dm_uint i=0;i<count;++i)
    {
      lhs_image = lhsimages->GetNativeBuffer(i);
      rhs_image = rhsimages->GetNativeBuffer(i);
      if(lhs_image==NULL)
      {
        if(rhs_image)
          lhsimages->StoreBuffer(i,rhs_image,dm_null,dmPixelFormatUndefined);

        rv = CCI_OK;
      }
      else if(rhs_image)
      {
        if(rgn.IsEmptyRoi()) {
           rgn.SetRectRoi(rhs_image->Rect());
           loc = rgn.Rectangle().TopLeft();
        }

        rv = _DoImageMath(lhs_image,rhs_image,rgn,loc,operation,filterCtxt);
        if(CCI_FAILED(rv))
           break;
      }
    }

    return rv;
  }

  return CCI_OK;
}

// defined in daim_kernel library
// defined in daim_kernel library
bool __dmKernel _dmDoImageMath4( dmImage& lhs , const dmRegion& r, dm_real a, dm_real b );

/* void addMul (in cciImage image, in cciRegion roi, in dm_real addval, in dm_real mulval); */
CCI_IMETHODIMP cciImageMath::AddMul(cciImage image, cciRegion roi, dm_real addval, dm_real mulval)
{
  CCI_ENSURE_ARG_POINTER(image);

  dmRegion rgn = roi ? *CCI_NATIVE(roi) : CCI_NATIVE(image)->Rect();

  if(_dmDoImageMath4(*CCI_NATIVE(image),rgn,mulval,addval))
     return CCI_OK;

  return CCI_ERROR_FAILURE;
}

/* void blend (in cciImage srcImage, in cciImage dstImage, in dm_real percent, in cciRegion roi, in dm_int32 x, in dm_int32 y ); */
CCI_IMETHODIMP cciImageMath::Blend(cciImage srcImage, cciImage dstImage, dm_real percent,
                                   cciRegion roi, dm_int32 x, dm_int32 y )
{
  CCI_ENSURE_ARG_POINTER(srcImage);
  CCI_ENSURE_ARG_POINTER(dstImage);

  dmImage* srcNative = CCI_NATIVE(srcImage);
  dmImage* dstNative = CCI_NATIVE(dstImage);

  dmRegion mask = roi ? *CCI_NATIVE(roi) : srcNative->Rect();
  dmPoint  offs(x,y);

  if(percent==1) { // Need just to copy the image
    dstNative->GetCopy(*srcNative,mask,offs);
    return CCI_OK;
  }

  if(srcNative->PixelFormat()!=dstNative->PixelFormat())
  {
     dmLink<dmImage> copyOfSrc = srcNative->CreateCopy(*dstNative->TypeDescriptor(),mask.Rectangle());
     mask.OffsetRoi();
     if(dmBlendImage(percent,*copyOfSrc,*dstNative,mask,offs))
        return CCI_OK;
  } else {
     if(dmBlendImage(percent,*srcNative,*dstNative,mask,offs))
       return CCI_OK;
  }

  return CCI_ERROR_FAILURE;
}



//=====================================
// Module
//=====================================

#include "cciIGenericFactory.h"

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciImageMath)

static const cciModuleComponentInfo components[] = {
    {  CCI_IMAGEMATH_CLASSNAME,
       CCI_IMAGEMATH_CID,
       CCI_IMAGEMATH_CONTRACTID,
       cciImageMathConstructor
    },
};

CCI_IMPL_GETMODULE(ImageMathModule, components)


