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
// File         : cciLacunarity.cpp
// Date         : 4 mars 2010
// Author       : David Marteau
//--------------------------------------------------------

#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciIRegionContainer.h"
#include "cciIImageContainer.h"
#include "cciIImageList.h"
#include "cciILacunarity.h"
#include "cciDaimCID.h"

#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "common/dmUserLib.h"


#include "operators/dmAccumulator.h"
#include "templates/processing/dmArithmetics.h"
#include "templates/processing/dmLinearFilters.h"

#define _NOINDEX_ ~0L

#define _IMG_RES 0
#define _IMG_FRC 1
#define _IMG_SRC 2
#define _IMG_ROI 3

#define _IMG_NUM_IMAGES 5

#define FRACT_FMT  dmPixelFormat32bppFloat

namespace fractal
{
  typedef dmIImage<FRACT_FMT> image_type; 

  inline image_type* cast_image( dmImage* p ) {
    return dmIImage<FRACT_FMT>::Cast(p);
  }

  struct multiplies 
  {
    float operator()(const float& w, const float& r ) {
       static float _eps = std::numeric_limits<float>::epsilon();
       return (r>_eps ? w / r : 0.f);
     }  
  };
}


/* Header file */
class cciLacunarity : public cciILacunarity
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_ILACUNARITY

  cciLacunarity();

private:
  ~cciLacunarity();

protected:
  bool                mInited;
  dmRegion               mSrcRoi;
  dmRegion               mDstRoi;
  cci_Ptr<cciIImageList> mBuffers;
};

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciLacunarity, cciILacunarity)

cciLacunarity::cciLacunarity()
: mInited(false)
{
  /* member initializers and constructor code */
}

cciLacunarity::~cciLacunarity()
{
  /* destructor code */
}

/* void init (in cciImage image, in cciRegion rgn, in dm_uint32 flags); */
CCI_IMETHODIMP cciLacunarity::Init(cciImage _image, cciRegion _rgn, dm_uint32 flags)
{
  cci_result rv;
    
  dmImage*  image = CCI_IF_NATIVE(_image);
  dmRegion* rgn   = CCI_IF_NATIVE(_rgn);
  
  CCI_ENSURE_ARG_POINTER(image);
  
  mSrcRoi = rgn ? *rgn : image->Rect();
  
  if(!mBuffers) {
    mBuffers = do_CreateInstance(CCI_IMAGELIST_CONTRACTID,&rv);
    if(CCI_SUCCEEDED(rv))
       rv = mBuffers->SetSize(_IMG_NUM_IMAGES);
   
    CCI_ENSURE_SUCCESS(rv,rv);
  }

  mInited = true;
  
  //===============================
  // Store source image ROI in floating point format
  //===============================

  dmRect _Rect = mSrcRoi.Rectangle();
  
  mBuffers->StoreBuffer(_IMG_SRC,image,&_Rect,FRACT_FMT);
  mSrcRoi.OffsetRoi();

  //===============================
  // Compute fractional map values
  //===============================
  if((flags && CCIV_Lacunarity_Fractional)!=0 && !mSrcRoi.IsRectRoi()) 
  {
    // Compute fractional part of the roi by creating a 
    // binary image and applying a mean filter on it

    mBuffers->CreateBuffer(_IMG_ROI,FRACT_FMT,_Rect.Width(),_Rect.Height());
    fractal::image_type *imroi = fractal::cast_image(mBuffers->GetNativeBuffer(_IMG_ROI));
    if(!imroi)
      return CCI_ERROR_FAILURE;
       
    imroi->Fill(0.0);
    imroi->Fill(mSrcRoi,1.0);    

    // Clear image outside ROI

    fractal::image_type *src = fractal::cast_image(mBuffers->GetNativeBuffer(_IMG_SRC));
    if(!src)
      return CCI_ERROR_FAILURE;
    
    mDstRoi = mSrcRoi;
    mDstRoi.XorRoi(mSrcRoi.Rectangle());
    src->Fill(mDstRoi,0.0);
  }
  return rv;
}

/* double lacunarity (in unsigned long size, in dm_uint32 flags); */
CCI_IMETHODIMP cciLacunarity::Lacunarity(dm_uint32 size, dm_uint32 flags, double *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_TRUE(mInited,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_MIN(size,0);
  
  fractal::image_type *src = fractal::cast_image(mBuffers->GetNativeBuffer(_IMG_SRC));

  mDstRoi = mSrcRoi;
  dmRegion& _Roi = mDstRoi;

  cci_result rv;
  
  //===============================
  // Check the special case when size is greater or equal
  // to the roi size
  //===============================
  if(size >= _Roi.Rectangle().Width() ||
    size  >= _Roi.Rectangle().Height()) 
  {
    // In this case there is only one box and thus 
    // variance is 0, so lacunarity is 1
    *_retval = 1.0;
    
    rv = CCI_OK;
  }
  //===============================
  // Case size == 1
  //===============================
  else if(size == 1) 
  {
    dmRect _Rect = _Roi.Rectangle();

    mBuffers->StoreBuffer(_IMG_RES,src,&_Rect,FRACT_FMT);
    fractal::image_type *res = fractal::cast_image(mBuffers->GetNativeBuffer(_IMG_RES));
    if(!res)
      return CCI_ERROR_FAILURE;
 
    dm_real M1,M2,N = _Roi.Area();

    M1 = daim::accumulate(_Roi,src->Gen(),0.0f)/N; // Accumulate 

    daim::sqr_image(_Roi,res->Gen());              // Compute square values
    M2 = daim::accumulate(_Roi,res->Gen(),0.0f)/N; // Accumulate again
        
    *_retval = M2 / (M1*M1);
    
    rv = CCI_OK;
  }
  //===============================
  // Gliding box algorithm
  //===============================
  else
  {
    //===============================
    // Compute ROI
    //===============================
    dmMaskDescription _Mask(size,size);

    _Roi.ClipToRect( _Mask.GetMask(_Roi.Rectangle()) );

    dmRect  _Rect = _Roi.Rectangle();
    dmPoint _Origin(0,0);

    daim::filter::kernel<int> _kernel;
    _kernel.fromMaskDescription(_Mask);

    mBuffers->CreateBuffer(_IMG_RES,FRACT_FMT,_Rect.Width(),_Rect.Height());
    fractal::image_type *res = fractal::cast_image(mBuffers->GetNativeBuffer(_IMG_RES));
    if(!res)
      return CCI_ERROR_OUT_OF_MEMORY;

    res->Fill(0);

    //===============================
    // Compute weight for each box, that is proportional to the mean value
    // of pixels inside box, for this we use the optimized algorithm.
    //===============================

    // Compute weights
    daim::RoiOperation(
         daim::filter::getMeanFunctor(_kernel,
           daim::filter::accumulator_base<float,float,float>()),
           src->Gen(),res->Gen(),_Roi,_Origin);  

    //===============================
    // Compute fractional values
    // The idea is to correct the values
    // of computed weights when the box lies outside
    // the ROI. 
    //===============================
    if((flags & CCIV_Lacunarity_Fractional)!=0 && !_Roi.IsRectRoi()) 
    {
      mBuffers->CreateBuffer(_IMG_FRC,FRACT_FMT,_Rect.Width(),_Rect.Height());

      fractal::image_type *roi = fractal::cast_image(mBuffers->GetNativeBuffer(_IMG_ROI));
      fractal::image_type *frc = fractal::cast_image(mBuffers->GetNativeBuffer(_IMG_FRC));

      frc->Fill(0);

       // Get coefficient by computing the mean values
       daim::RoiOperation(
           daim::filter::getMeanFunctor(_kernel,
             daim::filter::accumulator_base<float,float,float>()),
             roi->Gen(),frc->Gen(),_Roi,_Origin);  


       // Multiply weight by coefficients
       daim::combine(frc->Rect(),_Origin,roi->Gen(),res->Gen(),
                     fractal::multiplies());
    }

    _Roi.OffsetRoi();

    dm_real M1,M2,N = _Roi.Area();

    M1 = daim::accumulate(_Roi,res->Gen(),0.0f)/N; // Accumulate 

    daim::sqr_image(_Roi,res->Gen());  // Compute square values
    M2 = daim::accumulate(_Roi,res->Gen(),0.0f)/N; // Accumulate again

    //===============================
    // Compute lacunarity
    // NOTE: lacunarity is defined to be Variance/Mean^2 + 1 of the number
    // of sites (i.e the weight) per box. Here we have used the formula
    // Var = M2 - M1^2 (where Mp is the moment of order p), which lead
    // to the following expression M2/M1^2 for the lacunarity
    //===============================
    *_retval = M2 / (M1*M1);
    
    rv = CCI_OK;
  }

  return rv;
}

/* void clear(); */
CCI_IMETHODIMP cciLacunarity::Clear()
{
  if(mBuffers)
     mBuffers->Clear(); 
    
  mInited = false;
  
  return CCI_OK;
}

//=====================================
// Module
//=====================================

#include "cciIGenericFactory.h"

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciLacunarity)

static const cciModuleComponentInfo components[] = {
    {  CCI_LACUNARITY_CLASSNAME,
       CCI_LACUNARITY_CID,
       CCI_LACUNARITY_CONTRACTID,
       cciLacunarityConstructor
    },
};

CCI_IMPL_GETMODULE(LacunarityModule, components)

