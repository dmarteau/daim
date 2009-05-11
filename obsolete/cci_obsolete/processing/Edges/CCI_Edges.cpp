
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

#include "daim_modules/processing/CCI_Edges.h"
#include "daim_modules/processing/CCI_Filters.h"
#include "daim_modules/processing/CCI_RGBFunctor.h"

//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_Edges
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Edges_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "common/dmUserLib.h"

#include "templates/processing/dmPolar.h"

//---------------------------------------------------------------------
CCI_DECL_DATA( Edges )
{
  dm_uint    LastError;
  dmIFilters Filters;
};

typedef CCI_INSTANCE_PTR(Edges) _Instance_Ptr;
//------------------------------------------------------------------------
static bool CHECK_ERROR( dm_param err, cci_Object* ob, dm_param me ) 
{
  char_t* txt = NULL;
  switch(err) {
    case dmFunctorErrors::E_BAD_IMAGE : txt = _TXT("Invalid image type for this operation!"); break;
    case dmFunctorErrors::E_NO_IMAGE  : txt = _TXT("Cannot get image !")                    ; break;
    case dmFunctorErrors::E_BAD_PARAM : txt = _TXT("Invalid parameter !")                   ; break;

    case dmFunctorErrors::E_BAD_COLORSPACE: txt = _TXT("Invalid colorspace !")              ; break;
    case dmFunctorErrors::E_SPLIT_ERROR:    txt = _TXT("Cannot spplit image !")             ; break;

    case dmFunctorErrors::E_NO_KERNEL  : break;
    case dmFunctorErrors::E_NO_ERROR   : return false;
  };
  if(txt) CCI_SET_ERROR(ob,me,txt);
  return true;  
} 

#define CHECK_LAST_ERROR(me) CHECK_ERROR(_This->LastError,ob,me)
#define SET_LAST_ERROR(err) _This->LastError = static_cast<dm_uint>(err)
#define RETURN_ERROR_CHECK() \
        if(_This->LastError!=dmFunctorErrors::E_NO_ERROR) {     \
          CHECK_ERROR(_This->LastError,_THIS_OBJECT,CCI_P(Id)); \
          CCI_RETURN_FAIL()                                     \
        }                                                       \
        CCI_RETURN_OK()

//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( Edges )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE(Edges)

  CCI_INIT_MEMBER(Filters);

  _This->LastError = dmFunctorErrors::E_NO_ERROR;

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( Edges )
{
  CCI_INSTANCE(Edges)

  if(_This->Filters.IsValid()) 
      _This->Filters.Release();

  CCI_DESTROY_MEMBER(Filters);
  CCI_DESTROY_SUPER()
}
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Edges )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Edges )
CCI_END_SETTER()
*/
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( Edges )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( Edges )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method Canny    
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Edges, Canny )
{
  CCI_INSTANCE(Edges)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    if(img) 
    {      
      dmASSERT( buffer != NULL );
   
      dmCannyFilter _Filter(CCI_P(inner)==dm_true);

      dmColorSpaceFunctor<dmCannyFilter> _Functor(_Filter);
      if(!_Functor.Apply(_IParams,buffer,img,rgn,true))
        SET_LAST_ERROR(_Functor.ReturnedError);

    } else 
      SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);

  RETURN_ERROR_CHECK()
}
//---------------------------------------------------------------------
// Method: Gradient    
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Edges, Gradient )
{
  CCI_INSTANCE(Edges)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    if(img) 
    {      
      dmASSERT( buffer != NULL );

      dmGradientFilter _Filter;

      dmColorSpaceFunctor<dmGradientFilter> _Functor(_Filter);
      if(!_Functor.Apply(_IParams,buffer,img,rgn,true))
        SET_LAST_ERROR(_Functor.ReturnedError);

    } else 
      SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);

  RETURN_ERROR_CHECK()
}   
//---------------------------------------------------------------------
// Method: NonMaximalSuppression    
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Edges, NonMaximalSuppression )
{
  CCI_INSTANCE(Edges)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    if(img) 
    {      
      dmASSERT( buffer != NULL );

      dmNonMaxSuppress _Filter;

      dmColorSpaceFunctor<dmNonMaxSuppress> _Functor(_Filter);
      if(!_Functor.Apply(_IParams,buffer,img,rgn,true))
        SET_LAST_ERROR(_Functor.ReturnedError);
    } else 
      SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);

  RETURN_ERROR_CHECK()
}   
//---------------------------------------------------------------------
// Method Nagao    
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Edges, Nagao )
{
  CCI_INSTANCE(Edges)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    if(img) 
    {      
      dmASSERT( buffer != NULL );

      dmNagaoFilter _Filter;

      dmColorSpaceFunctor<dmNagaoFilter> _Functor(_Filter);
      if(!_Functor.Apply(_IParams,buffer,img,rgn,true))
        SET_LAST_ERROR(_Functor.ReturnedError);

    } else 
      SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);

  RETURN_ERROR_CHECK()
}   
//---------------------------------------------------------------------
// Method: Phase    
//---------------------------------------------------------------------
#define PHASE_FMT dmPixelFormat32bppFloat

static CCI_IMPL_METHOD( Edges, Phase )
{
  CCI_INSTANCE(Edges)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    if(img) 
    {
      dmASSERT( buffer != NULL );

      // Get the image list for storing results
      dmIImageList _IList;
      if(_IList.QueryInterface(_IParams.GetImageList()))
      {
        // Create Filter object
        if(!_This->Filters.IsValid()) {
           if(!_This->Filters.Create())
             CCI_RETURN_FAIL()
        }

        // Resize _ImageList
        _IList.SetCapacity(4);
     
        dmRect rect = rgn->Rectangle();
  
        // Create results images in buffer
        _IList.CreateBuffer(0,PHASE_FMT,rect.Width(),rect.Height());
        _IList.StoreBuffer (1,img,&rect,PHASE_FMT);
        _IList.StoreBuffer (2,img,&rect,PHASE_FMT);
        
        // Modify the roi accordingly
        rgn->Translate(-rect.Left(),-rect.Top());

        dmIImage<PHASE_FMT>* result = dmIImage<PHASE_FMT>::Cast(_IList.GetBuffer(0));
        dmIImage<PHASE_FMT>* ximage = dmIImage<PHASE_FMT>::Cast(_IList.GetBuffer(1));
        dmIImage<PHASE_FMT>* yimage = dmIImage<PHASE_FMT>::Cast(_IList.GetBuffer(2));

        if(result==NULL || ximage==NULL || yimage==NULL) {
            CCI_SET_ERROR_MSG(_TXT("Failed to create images !"));
            CCI_RETURN_FAIL()
        }

        // Compute x image 
        _IParams.SetImage(ximage);
        if(!_This->Filters.ApplyKernel(_IParams,CCI_P(xkernel),1,0))
             CCI_RETURN_FAIL()

        // Compute y image 
        _IParams.SetImage(yimage);
        if(!_This->Filters.ApplyKernel(_IParams,CCI_P(ykernel),1,0))
             CCI_RETURN_FAIL()

       daim::transform(*rgn,dmPoint(0,0),
                       ximage->Gen(),yimage->Gen(),result->Gen(),
                       daim::polar::phase<dm_float,daim::integer_false>::functor());
       

        rgn->Translate(rect.Left(),rect.Top());  // Restore roi
        _IParams.SetImage(img);                  // Restore image

      } else {
        CCI_SET_ERROR_MSG(_TXT("Cannot get imagelist !"));
        CCI_RETURN_FAIL()
      }
    } else 
      SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);

  RETURN_ERROR_CHECK()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Edges )  	
  CCI_REGISTER_METHOD_PP( Edges, Canny    )
  CCI_REGISTER_METHOD_PP( Edges, Gradient )
  CCI_REGISTER_METHOD_PP( Edges, NonMaximalSuppression   )
  CCI_REGISTER_METHOD_PP( Edges, Nagao      )
  CCI_REGISTER_METHOD_PP( Edges, Phase   )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Edges )
{
  CCI_INVOKE_FACTORY( Edges )
}
//---------------------------------------------------------------------
