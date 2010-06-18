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
// File         : cciThreshold.cpp
// Date         : 11 déc. 2008
// Author       : David Marteau
//--------------------------------------------------------


#include "cciCOMPtr.h"
#include "cciInterfaceRequestorUtils.h"
#include "cciIFilterContext.h"
#include "cciIColorSpace.h"
#include "cciIThreshold.h"
#include "cciIRegionContainer.h"
#include "cciIImageContainer.h"

#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "common/dmUserLib.h"

class cciThreshold : public cciIThreshold
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_ITHRESHOLD

  cciThreshold();

private:
  ~cciThreshold();

protected:
  /* additional members */
};

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciThreshold, cciIThreshold)

cciThreshold::cciThreshold()
{
  /* member initializers and constructor code */
}

cciThreshold::~cciThreshold()
{
  /* destructor code */
}

//================================
// cciIThreshold
//================================

/* void threshold (in cciImage image, in cciRegion roi, in cciRegion dstRgn, in dm_real rmin, in dm_real rmax, in cciISupports context); */
CCI_IMETHODIMP cciThreshold::Threshold(cciImage image, cciRegion roi, cciRegion dstRgn,
                                       dm_real rmin, dm_real rmax, cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(dstRgn);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  if(!dmIsPixelFormatScalar(CCI_NATIVE(image)->PixelFormat())) {
     dmLOG_ERROR("Cannot threshold on multi channel image, use RGBThreshold() instead !");
     return CCI_ERROR_INVALID_ARG;
  }

  dmRegion rgn = CCI_NATIVE_ROI(roi,CCI_NATIVE(image)->Rect());

  dmThreshold _Filter(rmin,rmax,*CCI_NATIVE(dstRgn));

  return dmApplyFilter(_Filter,*filterCtxt->NativeBuffer(),*CCI_NATIVE(image),rgn,true)?
         CCI_OK : CCI_ERROR_FAILURE;

}

/* void RGBThreshold (in cciImage image, in cciRegion roi, in cciRegion dstRgn, in dm_uint16 red, in dm_uint16 green, in dm_uint16 blue, in dm_real rradius, in dm_real gradius, in dm_real bradius); */
CCI_IMETHODIMP cciThreshold::RGBThreshold(cciImage image, cciRegion roi, cciRegion dstRgn, dm_uint16 red, dm_uint16 green, dm_uint16 blue, dm_real rradius, dm_real gradius, dm_real bradius)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(dstRgn);

  dmRegion rgn = CCI_NATIVE_ROI(roi,CCI_NATIVE(image)->Rect());

  dmRGBColor _Color;
  _Color.red   = static_cast<dm_uint8>(red);
  _Color.green = static_cast<dm_uint8>(green);
  _Color.blue  = static_cast<dm_uint8>(blue);

  dm_float rr = static_cast<dm_float>(daim::max(rradius,0.5));
  dm_float rg = static_cast<dm_float>(daim::max(gradius,0.5));
  dm_float rb = static_cast<dm_float>(daim::max(bradius,0.5));

  dmRegion _Result;

  if(dmRGBThreshold(*CCI_NATIVE(image),rgn,_Color,rr,rg,rb,*CCI_NATIVE(dstRgn)))
     return CCI_OK;

  return CCI_ERROR_FAILURE;
}

/* void hysteresisThreshold (in cciImage image, in cciRegion roi, in cciRegion dstRgn, in dm_real rmin, in dm_real rmax, in dm_uint32 connect, in cciISupports context); */
CCI_IMETHODIMP cciThreshold::HysteresisThreshold(cciImage image, cciRegion roi, cciRegion dstRgn, dm_real rmin, dm_real rmax, dm_uint32 connect,
                                                 cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(dstRgn);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  if(!dmIsPixelFormatScalar(CCI_NATIVE(image)->PixelFormat())) {
     dmLOG_ERROR("Cannot hysteresis threshold on multi channel image !");
     return CCI_ERROR_INVALID_ARG;
  }

  dmRegion rgn = CCI_NATIVE_ROI(roi,CCI_NATIVE(image)->Rect());

  dmHysteresisThreshold _Filter(rmin,rmax,connect,*CCI_NATIVE(dstRgn));

  return dmApplyFilter(_Filter,*filterCtxt->NativeBuffer(),*CCI_NATIVE(image),rgn,true)?
         CCI_OK : CCI_ERROR_FAILURE;

}

//=====================================
// Module
//=====================================

#include "cciIGenericFactory.h"

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciThreshold)

static const cciModuleComponentInfo components[] = {
    {  CCI_THRESHOLD_CLASSNAME,
       CCI_THRESHOLD_CID,
       CCI_THRESHOLD_CONTRACTID,
       cciThresholdConstructor
    },
};

CCI_IMPL_GETMODULE(ThresholdModule, components)



