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
// File         : cciSharpenFilter.cpp
// Date         : 17 déc. 2008
// Author       : David Marteau
//--------------------------------------------------------

#define dmUseKernelImageTemplates
#include "cciCOMPtr.h"
#include "cciInterfaceRequestorUtils.h"
#include "cciSharpenFilter.h"
#include "cciIFilterContext.h"
#include "cciImageUtils.h"
#include "cciIColorSpace.h"
#include "cciIRegionContainer.h"
#include "cciIImageContainer.h"

#include "daim_kernel.h"
#include "common/dmUserLib.h"

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciSharpenFilter, cciISharpenFilter)

cciSharpenFilter::cciSharpenFilter()
: mStrength(0.4)
, mRadius(3)
{
}

cciSharpenFilter::~cciSharpenFilter()
{
}

//=========================================
// cciISharpenFilter
//=========================================

/* attribute unsigned long radius; */
CCI_IMETHODIMP cciSharpenFilter::GetRadius(dm_uint32 *aRadius)
{
  CCI_ENSURE_ARG_POINTER(aRadius);

  *aRadius = mRadius;
  return CCI_OK;
}
CCI_IMETHODIMP cciSharpenFilter::SetRadius(dm_uint32 aRadius)
{
  mRadius = aRadius;
  return CCI_OK;
}

/* attribute double strength; */
CCI_IMETHODIMP cciSharpenFilter::GetStrength(double *aStrength)
{
  CCI_ENSURE_ARG_POINTER(aStrength);

  *aStrength = mStrength;
  return CCI_OK;
}
CCI_IMETHODIMP cciSharpenFilter::SetStrength(double aStrength)
{
  if(aStrength >= 1)
     aStrength = 0.99;
  else if(aStrength < 0)
    return CCI_ERROR_FAILURE;

  mStrength = aStrength;
  return CCI_OK;
}

// Defined in daim_kernel library
bool __dmKernel _dmDoImageMath2( dmBufferParameters& , dm_int );
bool __dmKernel _dmDoImageMath4( dmImage& lhs , const dmRegion& r, dm_real a, dm_real b );
//---------------------------------------------------------------------
class dmSharpenOperator : public dmMFilter
{
public:

  double _Strength;

  dmSharpenOperator( dmMaskDescription& aMask, double aStrength )
  : dmMFilter(aMask,dmFilterMean),_Strength(aStrength) {}

  bool Apply( dmBufferParameters& params ) {
    if(dmMFilter::Apply(params))
    {
      return (_dmDoImageMath4(params.thisImage,params.thisRegion,_Strength,0) &&
              _dmDoImageMath2(params,dmTk::Math::SubPixels));
    }
    return false;
  }
};
/* [noscript] void doSharpen (in cciImage image, in cciRegion rgn, in unsigned long radius, in double strength, in cciISupports context); */
CCI_IMETHODIMP cciSharpenFilter::DoSharpen(dmImage *image, dmRegion *rgn, dm_uint32 radius, double strength,
                                           cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  if(strength <= 0)
     return CCI_OK;  // Nothing to do, strength == 0 is identity

  dmRegion roi = rgn ? *rgn : image->Rect();

  dmMaskDescription mask(radius);

  dmSharpenOperator _Filter(mask,strength);

  if(image->PixelFormat()==dmPixelFormat24bppRGB)
  {
    cci_Ptr<cciIColorSpace> colorspace;
    cci_result rv = CCI_NewColorSpace("HSV",getter_AddRefs(colorspace));
    if(CCI_FAILED(rv))
       return rv;

    rv = colorspace->Split(native_Wrapper(image),dm_null);
    if(CCI_SUCCEEDED(rv))
    {
      dmImage* srcImage = colorspace->GetChannel(3);
      if(!srcImage)
         return CCI_ERROR_UNEXPECTED;

      dmBufferParameters _Params(*filterCtxt->NativeBuffer(),*srcImage,roi);

      if(_Filter.Apply(_Params) && _dmDoImageMath4(*srcImage,roi,1.0/(1.0 - strength),0))
      {
        return colorspace->Merge(native_Wrapper(image));
      }
    }
  }
  else
  {
    dmBufferParameters _Params(*filterCtxt->NativeBuffer(),*image,roi);

    if(_Filter.Apply(_Params) && _dmDoImageMath4(*image,roi,1.0/(1.0 - strength),0))
      return CCI_OK;

  }
  return CCI_ERROR_FAILURE;
}

//=========================================
// cciIImageFilter
//=========================================

/* attribute cciMaskDescription mask; */
CCI_IMETHODIMP cciSharpenFilter::GetMask(dmMaskDescription& aMask)
{
  aMask = dmMaskDescription(mRadius);
  return CCI_OK;
}
CCI_IMETHODIMP cciSharpenFilter::SetMask(dmMaskDescription& aMask)
{
  return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void applyFilter (in cciImage image, in cciRegion roi, in cciISupports context); */
CCI_IMETHODIMP cciSharpenFilter::ApplyFilter(cciImage image, cciRegion roi, cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);

  dmRegion* nativeRgn = roi ? CCI_NATIVE(roi) : dm_null;

  return DoSharpen(CCI_NATIVE(image),nativeRgn,mRadius,mStrength,filterCtxt);
}
