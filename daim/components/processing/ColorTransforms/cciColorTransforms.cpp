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
// File         : cciColorTransforms.cpp
// Date         : 25 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciCOMPtr.h"
#include "cciInterfaceRequestorUtils.h"
#include "cciIFilterContext.h"
#include "cciIColorTransforms.h"
#include "cciIColorUtils.h"
#include "cciIMatrix.h"
#include "cciIRegionContainer.h"
#include "cciIImageContainer.h"

#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "common/dmUserLib.h"

#include "templates/processing/dmArithmetics.h"
#include "templates/rgb/dmRGBArithmetics.h"


/* Header file */
class cciColorTransforms : public cciIColorTransforms,
                           public cciIColorUtils
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IIMAGEFILTER
  CCI_DECL_ICOLORTRANSFORMS
  CCI_DECL_ICOLORUTILS

  cciColorTransforms();

private:
  ~cciColorTransforms();

protected:
  cci_Ptr<cciIMatrix> mMatrix;

};

/* Implementation file */
CCI_IMPL_ISUPPORTS3(cciColorTransforms, cciIColorTransforms,
                                        cciIImageFilter,
                                        cciIColorUtils)

//==================================
// cciColorTransforms
//==================================

cciColorTransforms::cciColorTransforms()
{
  /* member initializers and constructor code */
}

cciColorTransforms::~cciColorTransforms()
{
  /* destructor code */
}

//==================================
// cciIColorTransforms
//==================================

/* attribute cciIMatrix transformMatrix; */
CCI_IMETHODIMP cciColorTransforms::GetTransformMatrix(cciIMatrix * *aTransformMatrix)
{
  CCI_ENSURE_ARG_POINTER(aTransformMatrix);

  CCI_ADDREF(*aTransformMatrix = mMatrix);
  return CCI_OK;
}
CCI_IMETHODIMP cciColorTransforms::SetTransformMatrix(cciIMatrix *aTransformMatrix)
{
  mMatrix = aTransformMatrix;
  return CCI_OK;
}

//==================================
// cciIColorUtils
//==================================

/* void fillChannel (in cciImage image, in cciRegion roi, in dm_real value, in dm_real alpha ); */
CCI_IMETHODIMP cciColorTransforms::Fill(cciImage image, cciRegion roi, dm_real value, dm_real alpha )
{
  CCI_ENSURE_ARG_POINTER(image);

  dmRegion rgn = roi ? *CCI_NATIVE(roi) : CCI_NATIVE(image)->Rect();

  return dmFillScalar::SetValue(*CCI_NATIVE(image),rgn,value,alpha) ? CCI_OK : CCI_ERROR_FAILURE;
}

/* void fillRGBA (in cciImage image, in cciRegion roi, in dm_uint16 red, in dm_uint16 green, in dm_uint16 blue, in dm_real alpha); */
CCI_IMETHODIMP cciColorTransforms::FillRGBA(cciImage image, cciRegion roi, dm_uint16 red, dm_uint16 green, dm_uint16 blue, dm_real alpha)
{
  CCI_ENSURE_ARG_POINTER(image);

  dmRegion rgn = roi ? *CCI_NATIVE(roi) : CCI_NATIVE(image)->Rect();

  if(dmFillRGB::SetValues(*CCI_NATIVE(image),rgn,red,green,blue,alpha))
    return CCI_OK;

  return CCI_ERROR_FAILURE;
}

/* void invert(in cciImage image, in cciRegion roi, in cciISupports context); */
CCI_IMETHODIMP cciColorTransforms::Invert(cciImage image, cciRegion roi, cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  dm_real minRange = 0,maxRange = 0;

  filterCtxt->GetMinRange(&minRange);
  filterCtxt->GetMaxRange(&maxRange);

  dmRegion rgn = roi ? *CCI_NATIVE(roi) : CCI_NATIVE(image)->Rect();
  dmInvertMap _Filter(minRange,maxRange);

  return dmApplyFilter(_Filter,*filterCtxt->NativeBuffer(),*CCI_NATIVE(image),rgn,true)?
         CCI_OK : CCI_ERROR_FAILURE;
}

/* void doRGBColorTransform (in cciImage image, in cciRegion roi, in cciIMatrix matrix); */
CCI_IMETHODIMP cciColorTransforms::DoRGBColorTransform(cciImage image, cciRegion roi, cciIMatrix *matrix)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(matrix);

  // We need at least 9 entries in the matrix
  if(matrix->Size()!=9)
     return CCI_ERROR_FAILURE;

  dmImage* native =  CCI_NATIVE(image);
  dmRegion rgn    = roi ? *CCI_NATIVE(roi) : native->Rect();

  // Apply only on RGB images
  dmIImage<dmPixelFormat24bppRGB>* pSrc = dmIImage<dmPixelFormat24bppRGB>::Cast(native);
  if(pSrc)
  {
    daim::rgb_color_transform(rgn,pSrc->Gen(),matrix->Data());
    return CCI_OK;
  }

  return CCI_ERROR_INVALID_ARG;
}

//=========================================
// cciIImageFilter
//=========================================

/* attribute cciMaskDescription mask; */
CCI_IMETHODIMP cciColorTransforms::GetMask(dmMaskDescription& aMask)
{
  aMask = dmMaskDescription(0,0,1,1);
  return CCI_OK;
}
CCI_IMETHODIMP cciColorTransforms::SetMask(dmMaskDescription& aMask)
{
  return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void applyFilter (in cciImage image, in cciRegion roi, in cciISupports context); */
CCI_IMETHODIMP cciColorTransforms::ApplyFilter(cciImage image, cciRegion roi, cciIFilterContext *filterCtxt)
{
  return DoRGBColorTransform(image,roi,mMatrix);
}


//=====================================
// Module
//=====================================

#include "cciIGenericFactory.h"

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciColorTransforms)

static const cciModuleComponentInfo components[] = {
    {  CCI_COLORTRANSFORMS_CLASSNAME,
       CCI_COLORTRANSFORMS_CID,
       CCI_COLORTRANSFORMS_CONTRACTID,
       cciColorTransformsConstructor
    },
    {  CCI_COLORTRANSFORMS_CLASSNAME,
       CCI_COLORTRANSFORMS_CID,
       CCI_COLORUTILS_CONTRACTID,
       cciColorTransformsConstructor
    },
};

CCI_IMPL_GETMODULE(ColorTransformsModule, components)



