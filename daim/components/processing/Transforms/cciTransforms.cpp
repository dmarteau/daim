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
// File         : cciTransforms.cpp
// Date         : 15 déc. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciCOMPtr.h"
#include "cciInterfaceRequestorUtils.h"
#include "cciIFilterContext.h"
#include "cciITransforms.h"
#include "cciIRegionContainer.h"
#include "cciIImageContainer.h"

#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "common/dmUserLib.h"

#include "math.h"

/* Header file */
class cciTransforms : public cciITransforms
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_ITRANSFORMS

  cciTransforms();

private:
  ~cciTransforms();

protected:
  /* additional members */
};

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciTransforms, cciITransforms)

cciTransforms::cciTransforms()
{
  /* member initializers and constructor code */
}

cciTransforms::~cciTransforms()
{
  /* destructor code */
}

/* void strech (in cciImage image, in cciRegion roi, in unsigned long mode, in cciImage dest); */
CCI_IMETHODIMP cciTransforms::Strech(cciImage image, cciRegion roi, dm_uint32 mode, cciImage dest)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(dest);

  dmInterpolationType _Mode = static_cast<dmInterpolationType>(mode);

  dmRect srcRect = roi ? CCI_NATIVE(roi)->Rectangle() : CCI_NATIVE(image)->Rect();

  if(dmLinearStretch(_Mode,*CCI_NATIVE(image),srcRect,*CCI_NATIVE(dest)))
     return CCI_OK;

  return CCI_ERROR_FAILURE;
}

/* void rotate (in cciImage image, in cciRegion roi, in dm_real angle, in cciImage dest); */
CCI_IMETHODIMP cciTransforms::Rotate(cciImage image, cciRegion roi, dm_real angle, cciImage dest)
{
  dmImage* srcImg = CCI_IF_NATIVE(image);
  dmImage* dstImg = CCI_IF_NATIVE(dest);

  CCI_ENSURE_ARG_POINTER(srcImg);
  CCI_ENSURE_ARG_POINTER(dstImg);

  dmRect rect = roi ? CCI_NATIVE(roi)->Rectangle() : srcImg->Rect();

  if(dmRotateImage(angle,*srcImg,rect,*dstImg))
     return CCI_OK;

  return CCI_ERROR_FAILURE;
}

/* void createMotif (in cciImage image, in cciRegion roi, in boolean periodic, in cciImage dest); */
CCI_IMETHODIMP cciTransforms::CreateMotif(cciImage image, cciRegion roi, dm_bool periodic, cciImage dest)
{
  dmImage*  srcImg = CCI_IF_NATIVE(image);
  dmImage*  dstImg = CCI_IF_NATIVE(dest);

  CCI_ENSURE_ARG_POINTER(srcImg);
  CCI_ENSURE_ARG_POINTER(dstImg);
  CCI_ENSURE_ARG_POINTER(roi);

  dmRegion* rgn = CCI_NATIVE(roi);
  if(!rgn || rgn->IsEmptyRoi())
    return CCI_ERROR_ILLEGAL_VALUE;
  
  if(dstImg!=srcImg) {
     dstImg->GetCopy(*srcImg,*rgn);
     rgn->OffsetRoi();
  }
  
  dmRect srcRect = rgn->Rectangle();
  
  if(periodic)
     dmMakePeriodic(*dstImg,srcRect,dstImg->Rect());
  else
     dmCreateMotif(*dstImg ,srcRect,dstImg->Rect());

  return CCI_OK;
}

/* void flip (in cciImage image, in cciRegion roi, in unsigned long way, in cciImage dest); */
CCI_IMETHODIMP cciTransforms::Flip(cciImage image, cciRegion roi, dm_uint32 way, cciImage dest)
{
  dmImage*  srcImg = CCI_IF_NATIVE(image);
  dmImage*  dstImg = CCI_IF_NATIVE(dest);

  CCI_ENSURE_ARG_POINTER(srcImg);
  CCI_ENSURE_ARG_POINTER(dstImg);

  dmRect  srcRect = roi ? CCI_NATIVE(roi)->Rectangle() : srcImg->Rect();

  if(dmFlipCopy(*srcImg,*dstImg,srcRect,dmPoint(0,0),way))
     return CCI_OK;

  return CCI_ERROR_FAILURE;
}

/* void rotate90 (in cciImage image, in cciRegion roi, in boolean clockwise, in cciImage dest); */
CCI_IMETHODIMP cciTransforms::Rotate90(cciImage image, cciRegion roi, dm_bool clockwise, cciImage dest)
{
  dmImage*  srcImg = CCI_IF_NATIVE(image);
  dmImage*  dstImg = CCI_IF_NATIVE(dest);

  CCI_ENSURE_ARG_POINTER(srcImg);
  CCI_ENSURE_ARG_POINTER(dstImg);

  dmRect srcRect = roi ? CCI_NATIVE(roi)->Rectangle() : srcImg->Rect();

  if(dmRotateCopy(*srcImg,*dstImg,srcRect,dmPoint(0,0),clockwise?dmTk::RotateRight: dmTk::RotateLeft))
     return CCI_OK;

  return CCI_ERROR_FAILURE;
}


//=====================================
// Module
//=====================================

#include "cciIGenericFactory.h"

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciTransforms)

static const cciModuleComponentInfo components[] = {
    {  CCI_TRANSFORMS_CLASSNAME,
       CCI_TRANSFORMS_CID,
       CCI_TRANSFORMS_CONTRACTID,
       cciTransformsConstructor
    },
};

CCI_IMPL_GETMODULE(TransformsModule, components)


