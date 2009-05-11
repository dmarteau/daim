
/* ::: BEGIN LICENSE BLOCK:::
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
 *  ::: END LICENSE BLOCK::: */

#define dmUseCCI
#define dmUseUtilitiesExtra
#define dmUseKernelBuffer
#define dmUseKernelImageTemplates
#include "daim_kernel.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_ImageList.h"
#include "daim_modules/CCI_MaskBuffer.h"
#include "daim_modules/CCI_Parameters.h"

#include "daim_modules/processing/CCI_BinaryMath.h"
#include "daim_modules/processing/CCI_KernelFamilies.h"

//------------------------------------------------------------------
#define CCIC_NAME CCIC_BinaryMath
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_BinaryMath_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "daim/daim_interfaces.h"

#include "templates/processing/dmBinaryMorphology.h"
#include "common/dmUserLib.h"

#include "daim/daim_base.h"

#include "daim_modules/CCI_Interface.h"

#define _NOINDEX_ dm_noindex
//--------------------------------------------------------------------
CCI_DECL_DATA( BinaryMath )
{
  dmIKernelFamilies KernelMngr;
};
//--------------------------------------------------------------------
//  NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( BinaryMath )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE(BinaryMath)

  CCI_INIT_MEMBER(KernelMngr);

  _This->KernelMngr.LoadKernels();  // Get the kernel manager

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
//  DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( BinaryMath )
{
  CCI_INSTANCE(BinaryMath)

  _This->KernelMngr.Release();

  CCI_DESTROY_MEMBER(KernelMngr);
  CCI_DESTROY_SUPER()
}
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( BinaryMath )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( BinaryMath )
CCI_END_SETTER()
*/
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( BinaryMath )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( BinaryMath )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method: ApplyKernel
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( BinaryMath, ApplyKernel )
{
  CCI_INSTANCE(BinaryMath)

  if(CCI_P(kernel) != NULL)
  {
    dmIParameters _IParams;
    if(_IParams.QueryInterface(CCI_P(params)))
    {
      dmRegion* rgn  = _IParams.GetMask();
      if(rgn && !rgn->IsEmptyRoi())
      {
        dmKernelFamily* family = _This->KernelMngr.FindFamily(CCI_P(kernel));
        if(family)
        {
          dmRegion dest;
          if(CCI_P(conditional)!=_NOINDEX_)
          {
             // Get the conditional mask from mask buffer parameters
            dmIMaskBuffer _IMasks;
            if(_IMasks.QueryInterface(_IParams.GetMaskBuffer()))
              _IMasks.CallMask(&dest,CCI_P(conditional));

          }

          if(CCI_P(iter)<=0) {  // Idempotence
            // if there is no conditional region we check the roi
            if((CCI_P(action)==dmTk::Math::Thickening ||
                CCI_P(action)==dmTk::Math::Dilation) && dest.IsEmptyRoi())
            {
              dest = *_IParams.GetRoi();
              if(dest.IsEmptyRoi()) {
                CCI_SET_ERROR_MSG(_TXT("Cannot perform operation without roi or conditional mask !"));
                CCI_RETURN_FAIL() // Cannot do that operation !
              }
            }
          }

          //----------------------------------------------
          // Handle borders for thinning/erosion
          //----------------------------------------------

          dmRegion outer;

          //XXX Must check when roi dest and mask
          // are same regions, this seems to lead to strange (and buggy)
          // results

          if(dest.IsEmptyRoi())
          {
            dest = *_IParams.GetRoi();
            if(!dest.IsEmptyRoi())
            {
              if(CCI_P(action)==dmTk::Math::Thinning ||
                 CCI_P(action)==dmTk::Math::Erosion  ||
                 CCI_P(action)==dmTk::Math::Closing)
              {
                 dmRect r = dest.Rectangle();
                 daim::handle_region_borders(*rgn,r,*family);
                 dest = *rgn;
                 dest.SubCoordinates(r);
              }
              else
              {
                outer = *rgn;
                outer.SubRoi(dest);
              }
            }
          }

          switch(CCI_P(action)) {
            case dmTk::Math::Erosion    : daim::binary_erosion   (*rgn,dest,*family,CCI_P(iter));break;
            case dmTk::Math::Dilation   : daim::binary_dilation  (*rgn,dest,*family,CCI_P(iter));break;
            case dmTk::Math::Thinning   : daim::binary_thinning  (*rgn,dest,*family,CCI_P(iter));break;
            case dmTk::Math::Thickening : daim::binary_thickening(*rgn,dest,*family,CCI_P(iter));break;
            case dmTk::Math::Closing    : daim::binary_closing   (*rgn,dest,*family,CCI_P(iter));break;
            case dmTk::Math::Opening    : daim::binary_opening   (*rgn,dest,*family,CCI_P(iter));break;
            case dmTk::Math::HitOrMiss  : daim::binary_hitormiss (*rgn,dest,*family,CCI_P(iter));break;
          }


          if(!outer.IsEmptyRoi())
              dest.AddRoi(outer);

          *rgn = dest;
          CCI_RETURN_OK()

        } else
          CCI_SET_ERROR_MSG(
            dmString::FormatString(dmString::npos,_TXT("Cannot find kernel family %s !"),CCI_P(kernel)).CStr()
          );
      } else
        CCI_SET_ERROR_MSG(_TXT("Empty region !"));
    } else
      CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));
  } else
    CCI_SET_ERROR_MSG(_TXT("Kernel missing !"));

  CCI_RETURN_FAIL()
}

//---------------------------------------------------------------------
// Method: DoSeparation
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( BinaryMath, DoSeparation )
{
  CCI_INSTANCE(BinaryMath)

  if(CCI_P(kernel) != NULL)
  {
    dmIParameters _IParams;
    if(_IParams.QueryInterface(CCI_P(params)))
    {
      dmRegion* rgn  = _IParams.GetMask();
      if(rgn && !rgn->IsEmptyRoi())
      {
        dmKernelFamily* family = _This->KernelMngr.FindFamily(CCI_P(kernel));

        if(family)
        {
          dmKernelFamily* _closing = _This->KernelMngr.FindFamily(_TXT("Morpho8c/C(8)"));
          if(_closing==NULL)
            dmLOG(_TXT("WARNING: The \"Separate\" operation can results in over-segmentation !\n"));

          //----------------------------------------------
          // We have to handle borders by extending
          // border objects over the size of the kernel
          //----------------------------------------------
          dmRegion* roi = _IParams.GetRoi();
          if(roi && !roi->IsEmptyRoi())
             daim::handle_region_borders(*rgn,roi->Rectangle(),*family);

          // Get seeds in buffer
          if(CCI_P(seeds)!=_NOINDEX_)
          {
            dmIMaskBuffer _IMasks;
            if(_IMasks.QueryInterface(_IParams.GetMaskBuffer()))
            {
              const dmRegion* seeds = _IMasks.GetBuffer(CCI_P(seeds));
              if(seeds && !seeds->IsEmptyRoi())
                daim::separate_particules(*rgn,*rgn,*family,seeds,_closing);
              else seeds = NULL;

              if(seeds==NULL) {
                CCI_SET_ERROR_MSG(_TXT("Invalid Seeds region !"));
                CCI_RETURN_FAIL()
              }
            }
          } else
            daim::separate_particules(*rgn,*rgn,*family,NULL,_closing);

          if(roi && !roi->IsEmptyRoi())
            rgn->AndRoi(*roi);

          CCI_RETURN_OK()

        } else
          CCI_SET_ERROR_MSG(
            dmString::FormatString(dmString::npos,_TXT("Cannot find kernel family %s !"),CCI_P(kernel)).CStr()
          );
      } else
        CCI_SET_ERROR_MSG(_TXT("Empty mask !"));
    } else
      CCI_SET_ERROR_MSG(_TXT("Invalid masks parameters !"));
  } else
    CCI_SET_ERROR_MSG(_TXT("Kernel missing !"));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: DistanceFunc
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( BinaryMath, DistanceFunc )
{
  CCI_INSTANCE(BinaryMath)

  if(CCI_P(kernel) != NULL)
  {
    dmIParameters _IParams;
    if(_IParams.QueryInterface(CCI_P(params)))
    {
      dmRegion* rgn  = _IParams.GetMask();
      if(rgn && !rgn->IsEmptyRoi())
      {
        dmKernelFamily* family = _This->KernelMngr.FindFamily(CCI_P(kernel));
        if(family)
        {
          // Get image list from parameters
          dmIImageList _ImageList;
          if(_ImageList.QueryInterface(_IParams.GetImageList()))
          {
             dmRect r;
             const dmRegion* roi = _IParams.GetRoi();
             if(roi && !roi->IsEmptyRoi())
               r = roi->Rectangle();
             else
               r = rgn->Rectangle();

             if(_ImageList.CreateBuffer(CCI_P(index),dmPixelFormat16bppGrayScale,
                                          r.Width(),r.Height()))
             {
                dmImage* _map = _ImageList.GetBuffer(CCI_P(index));
                if(_map) {
                  daim::distance_map(*rgn,*family,
                    dmIImage<dmPixelFormat16bppGrayScale>::Cast(_map)->Gen());

                  CCI_RETURN_OK()
                }
             } else
                CCI_SET_ERROR_MSG(_TXT("Cannot create image map !"));
          } else
            CCI_SET_ERROR_MSG(_TXT("Cannot get image list !"));
        } else
          CCI_SET_ERROR_MSG(
            dmString::FormatString(dmString::npos,_TXT("Cannot find kernel family %s !"),CCI_P(kernel)).CStr()
          );
      } else
        CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));
    } else
      CCI_SET_ERROR_MSG(_TXT("Empty mask !"));
  } else
    CCI_SET_ERROR_MSG(_TXT("Kernel missing !"));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: UltimateErosion
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( BinaryMath, UltimateErosion )
{
  CCI_INSTANCE(BinaryMath)

  if(CCI_P(kernel) != NULL)
  {
    dmIParameters _IParams;
    if(_IParams.QueryInterface(CCI_P(params)))
    {
      dmRegion* rgn  = _IParams.GetMask();
      if(rgn && !rgn->IsEmptyRoi())
      {
        dmKernelFamily* family = _This->KernelMngr.FindFamily(CCI_P(kernel));
        if(family)
        {
          // Check for ROI
          dmRegion* roi = _IParams.GetRoi();
          if(!roi->IsEmptyRoi())
          {
            dmRegion dest = *rgn;

            dest.AndRoi(*roi);
            daim::ultimate_erosion(dest,dest,*family);

            rgn->SubRoi(*roi);
            rgn->AddRoi(dest);

          } else
            daim::ultimate_erosion(*rgn,*rgn,*family);

          _IParams.SetMask(rgn);
          CCI_RETURN_OK()

        } else
          CCI_SET_ERROR_MSG(
            dmString::FormatString(dmString::npos,_TXT("Cannot find kernel family %s !"),CCI_P(kernel)).CStr()
          );
      } else
        CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));
    } else
      CCI_SET_ERROR_MSG(_TXT("Empty mask !"));
  } else
    CCI_SET_ERROR_MSG(_TXT("Kernel missing !"));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: UltimateDilation
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( BinaryMath, UltimateDilation )
{
  CCI_INSTANCE(BinaryMath)

  if(CCI_P(kernel) != NULL)
  {
    dmIParameters _IParams;
    if(_IParams.QueryInterface(CCI_P(params)))
    {
      dmRegion* rgn  = _IParams.GetMask();
      if(rgn && !rgn->IsEmptyRoi())
      {
        dmKernelFamily* family = _This->KernelMngr.FindFamily(CCI_P(kernel));
        if(family)
        {
          dmRegion cond,dest;

          if(CCI_P(conditional)!=_NOINDEX_)
          {
             // Get the conditional mask from mask buffer parameters
            dmIMaskBuffer _IMasks;
            if(_IMasks.QueryInterface(_IParams.GetMaskBuffer()))
              _IMasks.CallMask(&cond,CCI_P(conditional));

          }

          if(cond.IsEmptyRoi()) {
            cond = *_IParams.GetRoi();
            if(cond.IsEmptyRoi()) {
              CCI_SET_ERROR_MSG(_TXT("Cannot perform operation without roi or conditional mask !"));
              CCI_RETURN_FAIL() // Cannot do that operation !
            }
          }

          daim::ultimate_dilation(*rgn,dest,*family,&cond);

          _IParams.SetMask(&dest);
          CCI_RETURN_OK()

        } else
          CCI_SET_ERROR_MSG(
            dmString::FormatString(dmString::npos,_TXT("Cannot find kernel family %s !"),CCI_P(kernel)).CStr()
          );
      } else
        CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));
    } else
      CCI_SET_ERROR_MSG(_TXT("Empty mask !"));
  } else
    CCI_SET_ERROR_MSG(_TXT("Kernel missing !"));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: LocalMax
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( BinaryMath, LocalMax )
{
  CCI_UNUSUED_INSTANCE(BinaryMath)

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*  image = _IParams.GetImage();
    dmRegion* roi   = _IParams.GetRoi();

    if(image)
    {
      dmRegion      _Result;
      dmBufferParameters _Params(*_IParams.GetImageBuffer(),*image,*roi);
      if(dmLocalMax(_Params,_Result))
      {
        _IParams.SetMask(&_Result);
        CCI_RETURN_OK()
      }
    } else
      CCI_SET_ERROR_MSG(_TXT("Missing image !"));
  } else
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Reconstruct
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( BinaryMath, Reconstruct )
{
  CCI_UNUSUED_INSTANCE(BinaryMath)

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmRegion* rgn  = _IParams.GetMask();
    if(rgn && !rgn->IsEmptyRoi())
    {
      // On recupère les germes dans le tableau de masques
      dmRegion dest;

      if(CCI_P(seeds)!=_NOINDEX_) {
        dmIMaskBuffer _IMasks;
        if(_IMasks.QueryInterface(_IParams.GetMaskBuffer()))
           _IMasks.CallMask(&dest,CCI_P(seeds));
      }

      if(dest.IsEmptyRoi()) {
        CCI_SET_ERROR_MSG(_TXT("Cannot perform operation without seeds !"));
        CCI_RETURN_FAIL() // Cannot do that operation !
      }

      daim::reconstruct(*rgn,dest,(CCI_P(connect)>=8?daim::connect8:daim::connect4));

      _IParams.SetMask(&dest);
      CCI_RETURN_OK()

    } else
      CCI_SET_ERROR_MSG(_TXT("Empty mask !"));
  } else
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Watershed
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( BinaryMath, Watershed )
{
  CCI_UNUSUED_INSTANCE(BinaryMath)

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    const dmRegion* seeds = NULL;

    dmImage*  img = _IParams.GetImage();
    dmRegion* roi = _IParams.GetRoi();

    if(img==NULL) {
      CCI_SET_ERROR_MSG(_TXT("No image !"));
      CCI_RETURN_FAIL()
    }

    // Get seeds in buffer
    if(CCI_P(seeds)!=_NOINDEX_)
    {
      dmIMaskBuffer _IMasks;
      if(_IMasks.QueryInterface(_IParams.GetMaskBuffer())) {
         const dmRegion* seeds = _IMasks.GetBuffer(CCI_P(seeds));
         if(seeds == NULL) {
           CCI_SET_ERROR_MSG(_TXT("Invalid Seeds region !"));
           CCI_RETURN_FAIL()
         }
      }
      if(seeds->IsEmptyRoi())
        seeds = NULL;
    }

    // Create a 8 bit image
    dmLink<dmImage> _src = dmCreateCopy(img,roi->Rectangle(),
                                        dmPixelFormat8bppIndexed);

    dmRegion dest = *roi;
    dest.XorCoordinates(roi->Rectangle());
    dest.OffsetRoi();

    _src->ClearArea(dest);

    dest.KillRoi();

    daim::watershed_simple(dmIImage<dmPixelFormat8bppIndexed>::Cast(_src)->Gen(),dest,seeds);
    _IParams.SetMask(&dest);

    CCI_RETURN_OK()

  } else
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( BinaryMath )
  CCI_REGISTER_METHOD_PP( BinaryMath, ApplyKernel      )
  CCI_REGISTER_METHOD_PP( BinaryMath, DoSeparation     )
  CCI_REGISTER_METHOD_PP( BinaryMath, DistanceFunc     )
  CCI_REGISTER_METHOD_PP( BinaryMath, UltimateErosion  )
  CCI_REGISTER_METHOD_PP( BinaryMath, UltimateDilation )
  CCI_REGISTER_METHOD_PP( BinaryMath, LocalMax         )
  CCI_REGISTER_METHOD_PP( BinaryMath, Reconstruct      )
  CCI_REGISTER_METHOD_PP( BinaryMath, Watershed        )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( BinaryMath )
{
  DM_INIT_API()
  CCI_INVOKE_FACTORY( BinaryMath )
}
//---------------------------------------------------------------------
