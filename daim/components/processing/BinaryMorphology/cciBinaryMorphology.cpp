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
// File         : cciBinaryMorphology.cpp
// Date         : 19 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciCOMPtr.h"
#include "cciInterfaceRequestorUtils.h"
#include "cciIBinaryMorphology.h"
#include "cciIKernelService.h"
#include "cciIFilterContext.h"
#include "cciIImageList.h"
#include "cciIRegionContainer.h"
#include "cciIImageContainer.h"
#include "cciIKernelFamilyContainer.h"

// We need kernel templates

#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "common/dmUserLib.h"
#include "templates/processing/dmBinaryMorphology.h"

class cciBinaryMorphology : public cciIBinaryMorphology
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IBINARYMORPHOLOGY

  cciBinaryMorphology();

private:
  ~cciBinaryMorphology();

protected:
  dmKernelFamily mClosing; // Kernel used for separation
  dmRegion       mRoi;
};


//=====================================
// Implementation
//=====================================

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciBinaryMorphology, cciIBinaryMorphology)

cciBinaryMorphology::cciBinaryMorphology()
{
  /* member initializers and constructor code */
}

cciBinaryMorphology::~cciBinaryMorphology()
{
  /* destructor code */
}

//=====================================
// cciIBinaryMorphology
//=====================================

/* [noscript,notxpcom] dmRegionRef nativeRoi (); */
CCI_IMETHODIMP_(dmRegion &) cciBinaryMorphology::NativeRoi()
{
  return mRoi;
}

/* attribute cciRegion roi; */
CCI_IMETHODIMP cciBinaryMorphology::GetRoi(cciRegion *aRoi)
{
  return CCI_ERROR_NOT_IMPLEMENTED;
}
CCI_IMETHODIMP cciBinaryMorphology::SetRoi(cciRegion aRoi)
{
  CCI_ENSURE_ARG_POINTER(aRoi);
  mRoi = *CCI_NATIVE(aRoi);
  return CCI_OK;
}

/* void applyKernel (in cciRegion rgn, in cciKernelFamily family, in dm_int32 action, in dm_int32 iter, in dm_int32 operation, in cciRegion conditional); */
CCI_IMETHODIMP cciBinaryMorphology::ApplyKernel(cciRegion rgn, cciKernelFamily family, dm_int32 action,
                                                dm_int32 iter, dm_int32 operation,
                                                cciRegion conditional)
{
  CCI_ENSURE_ARG_POINTER(rgn);
  CCI_ENSURE_ARG_POINTER(family);

  dmKernelFamily* nativeFamily = CCI_NATIVE(family);
  CCI_PRECONDITION(nativeFamily,"null family");

  dmRegion* nativeRgn = CCI_NATIVE(rgn);
  CCI_PRECONDITION(nativeRgn,"null region");

  if(nativeRgn->IsEmptyRoi())
     return CCI_OK; // Nothing to do

  dmRegion dest;

  if(conditional)
     dest = *CCI_NATIVE(conditional);

  if(iter<=0)   // Idempotence
  {
    // if there is no conditional region we check the roi
    if((action==dmTk::Math::Thickening ||
        action==dmTk::Math::Dilation) && dest.IsEmptyRoi())
    {
      dest = mRoi;
      if(dest.IsEmptyRoi()) {
        dmLOG_ERROR("Cannot perform operation without conditional mask !");
        return CCI_ERROR_FAILURE; // Cannot do that operation !
      }
    }
  }

  //----------------------------------------------
  // Handle borders for thinning/erosion
  // We want to preserve border connectivity.
  //----------------------------------------------

  dmRegion  outer;

  if(dest.IsEmptyRoi())
  {
    dest = mRoi;
    if(!dest.IsEmptyRoi())
    {
      if(action==dmTk::Math::Thinning ||
         action==dmTk::Math::Erosion  ||
         action==dmTk::Math::Closing)
      {
         dmRect r = dest.Rectangle();
         daim::handle_region_borders(*nativeRgn,r,*nativeFamily);
         dest = *nativeRgn;
         dest.SubCoordinates(r);
      }
      else
      {
        outer = *nativeRgn;
        outer.SubRoi(dest);
      }
    }
  }

  switch(action) {
    case dmTk::Math::Erosion    : daim::binary_erosion   (*nativeRgn,dest,*nativeFamily,iter);break;
    case dmTk::Math::Dilation   : daim::binary_dilation  (*nativeRgn,dest,*nativeFamily,iter);break;
    case dmTk::Math::Thinning   : daim::binary_thinning  (*nativeRgn,dest,*nativeFamily,iter);break;
    case dmTk::Math::Thickening : daim::binary_thickening(*nativeRgn,dest,*nativeFamily,iter);break;
    case dmTk::Math::Closing    : daim::binary_closing   (*nativeRgn,dest,*nativeFamily,iter);break;
    case dmTk::Math::Opening    : daim::binary_opening   (*nativeRgn,dest,*nativeFamily,iter);break;
    case dmTk::Math::HitOrMiss  : daim::binary_hitormiss (*nativeRgn,dest,*nativeFamily,iter);break;
  }

  if(!outer.IsEmptyRoi())
      dest.AddRoi(outer);

  *nativeRgn = dest;
  return CCI_OK;
}

/* void doSeparation (in cciRegion rgn, in cciKernelFamily family, in cciRegion seeds); */
CCI_IMETHODIMP cciBinaryMorphology::DoSeparation(cciRegion rgn, cciKernelFamily family,
                                                 cciRegion seeds)
{
  CCI_ENSURE_ARG_POINTER(rgn);
  CCI_ENSURE_ARG_POINTER(family);

  dmKernelFamily* nativeFamily = CCI_NATIVE(family);
  CCI_PRECONDITION(nativeFamily,"null family");

  dmRegion* nativeRgn = CCI_NATIVE(rgn);
  CCI_PRECONDITION(nativeRgn,"null region");

  if(nativeRgn->IsEmptyRoi())
     return CCI_OK; // Nothing to do

  // Use closing operation to limit over segmentation
  if(mClosing.Empty())
  {
    if(!CCI_SUCCEEDED(CCI_GetKernelFamily("Morpho8c/C(8)",mClosing)))
      dmLOG_WARN("WARNING: The \"Separate\" operation can results in over-segmentation !");
  }
  //----------------------------------------------
  // We have to handle borders by extending
  // border objects over the size of the kernel
  //----------------------------------------------
  if(!mRoi.IsEmptyRoi())
     daim::handle_region_borders(*nativeRgn,mRoi.Rectangle(),*nativeFamily);

  // Get seeds in buffer
  if(seeds)
  {
    if(!CCI_NATIVE(seeds)->IsEmptyRoi())
      daim::separate_particules(*nativeRgn,*nativeRgn,*nativeFamily,CCI_NATIVE(seeds),&mClosing);
    else {
      dmLOG_ERROR("Invalid Seeds region !");
      return CCI_ERROR_FAILURE;
    }
  } else
    daim::separate_particules(*nativeRgn,*nativeRgn,*nativeFamily,NULL,&mClosing);

  if(!mRoi.IsEmptyRoi())
     nativeRgn->AndRoi(mRoi);

  return CCI_OK;
}

/* void distanceFunc (in cciRegion rgn, in cciKernelFamily family, in cciIImageList buffers, in dm_uint32 index); */
CCI_IMETHODIMP cciBinaryMorphology::DistanceFunc(cciRegion rgn,
                                                 cciKernelFamily family,
                                                 cciIImageList *buffers,
                                                 dm_uint32 index)
{
  CCI_ENSURE_ARG_POINTER(rgn);
  CCI_ENSURE_ARG_POINTER(family);
  CCI_ENSURE_ARG_POINTER(buffers);

  dmKernelFamily* nativeFamily = CCI_NATIVE(family);
  CCI_PRECONDITION(nativeFamily,"null family");

  dmRegion* nativeRgn = CCI_NATIVE(rgn);
  CCI_PRECONDITION(nativeRgn,"null region");

  if(nativeRgn->IsEmptyRoi())
     return CCI_ERROR_INVALID_ARG;

  dmRect r;
  if(mRoi.IsEmptyRoi())
   r = mRoi.Rectangle();
  else
   r = nativeRgn->Rectangle();

  cci_result rv = buffers->CreateBuffer(index,dmPixelFormat16bppGrayScale,
                                        r.Width(),r.Height());
  if(CCI_SUCCEEDED(rv))
  {
    dmImage* _map = buffers->GetNativeBuffer(index);
    dmDEBUG_ASSERT(_map != dm_null);
    daim::distance_map(*nativeRgn,*nativeFamily,
          dmIImage<dmPixelFormat16bppGrayScale>::Cast(_map)->Gen());

    rv = CCI_OK;
  }

  return rv;
}

/* void ultimateErosion (in cciRegion rgn, in cciKernelFamily family); */
CCI_IMETHODIMP cciBinaryMorphology::UltimateErosion(cciRegion rgn, cciKernelFamily family)
{
  CCI_ENSURE_ARG_POINTER(rgn);
  CCI_ENSURE_ARG_POINTER(family);

  dmKernelFamily* nativeFamily = CCI_NATIVE(family);
  CCI_PRECONDITION(nativeFamily,"null family");

  dmRegion* nativeRgn = CCI_NATIVE(rgn);
  CCI_PRECONDITION(nativeRgn,"null region");

  if(nativeRgn->IsEmptyRoi())
    return CCI_OK;  // Nothing to do;

  if(!mRoi.IsEmptyRoi())
  {
    dmRegion dest = *nativeRgn;

    dest.AndRoi(mRoi);
    daim::ultimate_erosion(dest,dest,*nativeFamily);

    nativeRgn->SubRoi(mRoi);
    nativeRgn->AddRoi(dest);

  } else
    daim::ultimate_erosion(*nativeRgn,*nativeRgn,*nativeFamily);

  return CCI_OK;
}

/* void ultimateDilation (in cciRegion rgn, in cciKernelFamily kernel, in cciRegion cond); */
CCI_IMETHODIMP cciBinaryMorphology::UltimateDilation(cciRegion rgn, cciKernelFamily family, cciRegion cond)
{
  CCI_ENSURE_ARG_POINTER(rgn);
  CCI_ENSURE_ARG_POINTER(family);

  dmKernelFamily* nativeFamily = CCI_NATIVE(family);
  CCI_PRECONDITION(nativeFamily,"null family");

  dmRegion* nativeRgn = CCI_NATIVE(rgn);
  CCI_PRECONDITION(nativeRgn,"null region");

  if(nativeRgn->IsEmptyRoi())
    return CCI_OK;  // Nothing to do;

  dmRegion dest,_cond = cond ? *CCI_NATIVE(cond) : mRoi;

  if(_cond.IsEmptyRoi()) {
    dmLOG_ERROR("Cannot perform operation without roi or conditional mask !");
    return CCI_ERROR_FAILURE;
  }

  daim::ultimate_dilation(*nativeRgn,dest,*nativeFamily,&_cond);

  *nativeRgn = dest;
  return CCI_OK;
}

/* void localMax (in cciImage image, in cciISupports context, in cciRegion result); */
CCI_IMETHODIMP cciBinaryMorphology::LocalMax(cciImage image, cciIFilterContext *filterCtxt, cciRegion result)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(result);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  dmBufferParameters _Params(*filterCtxt->NativeBuffer(),*CCI_NATIVE(image),mRoi);
  if(dmLocalMax(_Params,*CCI_NATIVE(result)))
     return CCI_OK;

  return  CCI_ERROR_FAILURE;
}

/* void reconstruct (in cciRegion mask, in short connect, in cciRegion seeds); */
CCI_IMETHODIMP cciBinaryMorphology::Reconstruct(cciRegion mask, dm_int16 connect,
                                                cciRegion seeds)
{
  CCI_ENSURE_ARG_POINTER(mask);
  CCI_ENSURE_ARG_POINTER(seeds);

  dmRegion* nativeMask = CCI_NATIVE(mask);
  CCI_PRECONDITION(nativeMask,"null region");

  dmRegion dest = *CCI_NATIVE(seeds);
  if(dest.IsEmptyRoi()) {
    dmLOG_ERROR("Cannot perform operation without seeds !");
    return CCI_ERROR_FAILURE;
  }

  daim::reconstruct(*nativeMask,dest,(connect>=8?daim::connect8:daim::connect4));

  *nativeMask = dest;
  return CCI_OK;
}

/* void watershed (in cciImage image, in cciRegion seeds, in cciRegion result); */
CCI_IMETHODIMP cciBinaryMorphology::Watershed(cciImage image, cciRegion seeds, cciRegion result )
{
  CCI_ENSURE_ARG_POINTER(result);
  CCI_ENSURE_ARG_POINTER(image);

  dmRegion* nativeResult = CCI_NATIVE(result);
  CCI_PRECONDITION(nativeResult,"null region");

  dmRegion* nativeSeeds = seeds ? CCI_NATIVE(seeds) : dm_null;
  if(nativeSeeds && nativeSeeds->IsEmptyRoi())
    nativeSeeds = NULL;

  // Create a 8 bit image
  dmRegion roi = mRoi;

  if(roi.IsEmptyRoi())
     roi = CCI_NATIVE(image)->Rect();

  dmLink<dmImage> _src = dmCreateCopy(CCI_NATIVE(image),mRoi.Rectangle(),
                                      dmPixelFormat8bppIndexed);
  *nativeResult = roi;
  nativeResult->XorCoordinates(roi.Rectangle());
  nativeResult->OffsetRoi();

  _src->ClearArea(*nativeResult);

  nativeResult->KillRoi();

  daim::watershed_simple(dmIImage<dmPixelFormat8bppIndexed>::Cast(_src)->Gen(),
                         *nativeResult,nativeSeeds);
  return CCI_OK;
}

//=====================================
// Module
//=====================================

#include "cciIGenericFactory.h"

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciBinaryMorphology)

static const cciModuleComponentInfo components[] = {
    {  CCI_BINARYMORPHOLOGY_CLASSNAME,
       CCI_BINARYMORPHOLOGY_CID,
       CCI_BINARYMORPHOLOGY_CONTRACTID,
       cciBinaryMorphologyConstructor
    },
};

CCI_IMPL_GETMODULE(BinaryMorphologyModule, components)

