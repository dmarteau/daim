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

  dmRegion rgn = roi ? CCI_NATIVE(roi)->Rectangle() : CCI_NATIVE(image)->Rect();

  if(dmLinearStretch(_Mode,*CCI_NATIVE(image),rgn.Rectangle(),*CCI_NATIVE(dest)))
     return CCI_OK;

  return CCI_ERROR_FAILURE;
}

/* void rotate (in cciImage image, in cciRegion roi, in dm_real angle, in boolean crop, in cciISupports context); */
CCI_IMETHODIMP cciTransforms::Rotate(cciImage image, cciRegion roi, dm_real angle, dm_bool crop,
                                     cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  dmImageBuffer* buffer    = filterCtxt->NativeBuffer();
  dmImage*       nativeImg = CCI_NATIVE(image);

  dmRect rect   = roi ? CCI_NATIVE(roi)->Rectangle() : nativeImg->Rect();

  dm_int32 width  = rect.Width();
  dm_int32 height = rect.Height();

  if(!crop)
  {
    // Compute new width and height for the image
    width  = daim::round(daim::abs(width * cos(angle)) + daim::abs(height * sin(angle)));
    height = daim::round(daim::abs(width * sin(angle)) + daim::abs(height * cos(angle)));
  }

  buffer->CreateBuffer(*nativeImg->TypeDescriptor(),dmRect(0,0,width,height));

  if(dmRotateImage(angle,*nativeImg,rect,*buffer->Buffer()))
     return CCI_OK;

  return CCI_ERROR_FAILURE;

}

/* void createMotif (in cciImage image, in cciRegion roi, in boolean periodic, in cciISupports context); */
CCI_IMETHODIMP cciTransforms::CreateMotif(cciImage image, cciRegion roi, dm_bool periodic,
                                          cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(roi);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  dmImageBuffer* buffer = filterCtxt->NativeBuffer();

  buffer->CreateBuffer(*CCI_NATIVE(image),false);

  dmLink<dmImage> dest = buffer->Buffer();

  dest->GetCopy(*CCI_NATIVE(image),CCI_NATIVE(roi)->Rectangle());

  if(periodic)
     dmMakePeriodic(*dest, CCI_NATIVE(roi)->Rectangle(),dest->Rect());
  else
     dmCreateMotif(*dest, CCI_NATIVE(roi)->Rectangle(),dest->Rect());

  return CCI_OK;
}

/* void flip (in cciImage image, in cciRegion roi, in unsigned long way, in cciISupports context); */
CCI_IMETHODIMP cciTransforms::Flip(cciImage image, cciRegion roi, dm_uint32 way, cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  dmImageBuffer* buffer    = filterCtxt->NativeBuffer();
  dmImage*       nativeImg = CCI_NATIVE(image);
  dmRect         srcRect   = roi ? CCI_NATIVE(roi)->Rectangle() : nativeImg->Rect();

  buffer->CreateBuffer(*nativeImg->TypeDescriptor(),dmRect(0,0,srcRect.Height(),srcRect.Width()));

  if(dmFlipCopy(*nativeImg,*buffer->Buffer(),srcRect,dmPoint(0,0),way))
     return CCI_OK;

  return CCI_ERROR_FAILURE;
}

/* void rotate90 (in cciImage image, in cciRegion roi, in boolean clockwise, in cciISupports context); */
CCI_IMETHODIMP cciTransforms::Rotate90(cciImage image, cciRegion roi, dm_bool clockwise,
                                       cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  dmImageBuffer* buffer    = filterCtxt->NativeBuffer();
  dmImage*       nativeImg = CCI_NATIVE(image);
  dmRect         srcRect   = roi ? CCI_NATIVE(roi)->Rectangle() : nativeImg->Rect();

  buffer->CreateBuffer(*nativeImg->TypeDescriptor(),dmRect(0,0,srcRect.Height(),srcRect.Width()));

  if(dmRotateCopy(*nativeImg,*buffer->Buffer(),srcRect,dmPoint(0,0),
                   clockwise?dmTk::RotateRight:
                             dmTk::RotateLeft))
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


