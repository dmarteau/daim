
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
#define dmUseKernelImageTemplates
#define dmUseUtilitiesExtra
#define dmUseKernelBuffer
#include "daim_kernel.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Parameters.h"
#include "daim_modules/CCI_ImageList.h"

#include "daim_modules/processing/CCI_Fractals.h"

//------------------------------------------------------------------
#define CCIC_NAME CCIC_Fractals
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Fractals_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2005"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "common/dmUserLib.h"

#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"

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

#define ENSURE_IMAGE_PTR(img) \
   if(img==NULL) {\
     CCI_SET_ERROR_MSG("Image allocation failure !");\
     CCI_RETURN_FAIL();\
   }

namespace fractal
{
  typedef dmIImage<FRACT_FMT> image_type; 

  inline image_type* cast_image( dmImage* p ) {
    return dmIImage<FRACT_FMT>::Cast(p);
  }

  struct multiplies 
  {
     dm_float operator()(const dm_float& w, const dm_float& r ) {
       static dm_float _eps = std::numeric_limits<float>::epsilon();
       return (r>_eps ? w / r : 0.f);
     }  
  };
}

//--------------------------------------------------------------------
CCI_DECL_DATA(Fractals)
{
  dmRegion                   _SrcRoi;
  dmRegion                   _DstRoi;
  dmIInterface<dmIImageList> _Buffers;
};
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( Fractals )
{
  CCI_GENERIC_CONSTRUCT(Fractals)

  _This->_Buffers.Resize(_IMG_NUM_IMAGES);

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( Fractals )
{
  CCI_UNUSUED_INSTANCE(Fractals)
  CCI_DESTROY_SUPER()
}
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Fractals )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Fractals )
CCI_END_SETTER()
*/
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( Fractals )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( Fractals )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method: Setup
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Fractals, Setup )
{
  CCI_INSTANCE(Fractals)

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*  image = _IParams.GetImage();
    dmRegion* rgn   = _IParams.GetRoi();

    dmASSERT( rgn != NULL );

    if(image) 
    {  
      _This->_SrcRoi  = *rgn;

      dmRect _Rect = _This->_SrcRoi.Rectangle();

      //===============================
      // Store source image ROI in floating point format
      //===============================

      _This->_Buffers.StoreBuffer (_IMG_SRC,image,&_Rect,FRACT_FMT);
      _This->_SrcRoi.OffsetRoi();

      //===============================
      // Compute fractional map values
      //===============================
      if((CCI_P(flags) & CCIV_Lacunarity_Fractional)!=0 && !_This->_SrcRoi.IsRectRoi()) 
      {
        // Compute fractional part of the roi by creating a 
        // binary image and applying a mean filter on it

        _This->_Buffers.CreateBuffer(_IMG_ROI,FRACT_FMT,_Rect.Width(),_Rect.Height());
        fractal::image_type *imroi = fractal::cast_image(_This->_Buffers.GetBuffer(_IMG_ROI));

        ENSURE_IMAGE_PTR(imroi)
        imroi->Fill(0.0);
        imroi->Fill(_This->_SrcRoi,1.0);    

        // Clear image outside ROI

        fractal::image_type *src = fractal::cast_image(_This->_Buffers.GetBuffer(_IMG_SRC));
        ENSURE_IMAGE_PTR(src)
        
        _This->_DstRoi = _This->_SrcRoi;
        _This->_DstRoi.XorRoi(_This->_SrcRoi.Rectangle());
        src->Fill(_This->_DstRoi,0.0);

        CCI_RETURN_OK()
      }
    } else 
      CCI_SET_ERROR_MSG(_TXT("No image !"));
  } else
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));   

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Lacunarity
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Fractals, Lacunarity )
{
  CCI_INSTANCE(Fractals)

  if(CCI_P(size)<=0)
    CCI_RETURN_INVALID_PARAM()

  fractal::image_type *src = fractal::cast_image(_This->_Buffers.GetBuffer(_IMG_SRC));
  ENSURE_IMAGE_PTR(src)

  _This->_DstRoi = _This->_SrcRoi;

  dmRegion& _Roi = _This->_DstRoi;

  //===============================
  // Check the special case when size is greater or equal
  // to the roi size
  //===============================
  if(CCI_P(size) >= _Roi.Rectangle().Width() ||
     CCI_P(size) >= _Roi.Rectangle().Height()) 
  {
    // In this case there is only one box and thus 
    // variance is 0, so lacunarity is 1
    CCI_RETVAL_P(result) = 1.0;
    CCI_RETURN_OK()
  } 
  //===============================
  // Case size == 1
  //===============================
  else if(CCI_P(size) == 1) 
  {
    dmRect _Rect = _Roi.Rectangle();

    _This->_Buffers.StoreBuffer (_IMG_RES,src,&_Rect,FRACT_FMT);
    fractal::image_type *res = fractal::cast_image(_This->_Buffers.GetBuffer(_IMG_RES));

    ENSURE_IMAGE_PTR(res)

    dm_real M1,M2,N = _Roi.Area();

    M1 = daim::accumulate(_Roi,src->Gen(),0.0f)/N; // Accumulate 

    daim::sqr_image(_Roi,res->Gen());              // Compute square values
    M2 = daim::accumulate(_Roi,res->Gen(),0.0f)/N; // Accumulate again
        
    CCI_RETVAL_P(result) = M2 / (M1*M1);
    CCI_RETURN_OK()
  }
  //===============================
  // Gliding box algorithm
  //===============================
  else
  {
    //===============================
    // Compute ROI
    //===============================
    dmMaskDescription _Mask(CCI_P(size),CCI_P(size));

    _Roi.ClipToRect( _Mask.GetMask(_Roi.Rectangle()) );

    dmRect  _Rect = _Roi.Rectangle();
    dmPoint _Origin(0,0);

    daim::filter::kernel<int> _kernel;
    _kernel.fromMaskDescription(_Mask);

    _This->_Buffers.CreateBuffer(_IMG_RES,FRACT_FMT,_Rect.Width(),_Rect.Height());
    fractal::image_type *res = fractal::cast_image(_This->_Buffers.GetBuffer(_IMG_RES));

    ENSURE_IMAGE_PTR(res)

    res->Fill(0);

    //===============================
    // Compute weight for each box, that is proportional to the mean value
    // of pixels inside box, for this we use the optimized algorithm.
    //===============================

    // Compute weights
    daim::RoiOperation(
         daim::filter::getMeanFunctor(_kernel,
           daim::filter::accumulator_base<dm_float,dm_float,dm_float>()),
           src->Gen(),res->Gen(),_Roi,_Origin);  

    //===============================
    // Compute fractional values
    // The idea is to correct the values
    // of computed weights when the box lies outside
    // the ROI. 
    //===============================
    if((CCI_P(flags) & CCIV_Lacunarity_Fractional)!=0 && !_Roi.IsRectRoi()) 
    {
      _This->_Buffers.CreateBuffer(_IMG_FRC,FRACT_FMT,_Rect.Width(),_Rect.Height());

      fractal::image_type *roi = fractal::cast_image(_This->_Buffers.GetBuffer(_IMG_ROI));
      fractal::image_type *frc = fractal::cast_image(_This->_Buffers.GetBuffer(_IMG_FRC));

      frc->Fill(0);

       // Get coefficient by computing the mean values
       daim::RoiOperation(
           daim::filter::getMeanFunctor(_kernel,
             daim::filter::accumulator_base<dm_float,dm_float,dm_float>()),
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
    CCI_RETVAL_P(result) = M2 / (M1*M1);

    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Fractals )
  CCI_REGISTER_METHOD_PP( Fractals, Setup      )
  CCI_REGISTER_METHOD_PP( Fractals, Lacunarity )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Fractals )
{
  CCI_INVOKE_FACTORY( Fractals )
}
//---------------------------------------------------------------------
