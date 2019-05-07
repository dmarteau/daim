
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
#define dmUseKernelImage
#include "daim_kernel.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Parameters.h"
#include "daim_modules/CCI_ImageList.h"

#include "daim_modules/processing/CCI_Filters.h"
#include "daim_modules/processing/CCI_KernelFamilies.h"

#include "daim_modules/processing/CCI_RGBFunctor.h"

//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_Filters
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Filters_Version
#define CCI_REVISION  1
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------

/*  $Rev 1.1, 19 août 2006 16:15:44, David
 *
 * Now use optimized median filter for 8 bits and 16 bits
 * images.
 */

#include "daim_modules/CCI_Base.h"
#include "daim_modules/CCI_Interface.h"
#include "common/dmUserLib.h"

#include <vector>
//---------------------------------------------------------------------
CCI_DECL_DATA( Filters )
{
  dmMaskDescription Mask;
  dm_uint           LastError;
  dmIKernelFamilies KernelMngr;
};

typedef CCI_INSTANCE_PTR(Filters) _Instance_Ptr;
//------------------------------------------------------------------------
static bool CHECK_ERROR( dm_param err, cci_Object* ob, dm_param me ) 
{
  char_t* txt = NULL;
  switch(err) {
    case dmFunctorErrors::E_BAD_IMAGE : txt = _TXT("Invalid image type for this operation!"); break;
    case dmFunctorErrors::E_NO_IMAGE  : txt = _TXT("Cannot get image !");  break;
    case dmFunctorErrors::E_BAD_PARAM : txt = _TXT("Invalid parameter !"); break;

    case dmFunctorErrors::E_BAD_COLORSPACE: txt = _TXT("Invalid colorspace !");  break;
    case dmFunctorErrors::E_SPLIT_ERROR:    txt = _TXT("Cannot spplit image !"); break;

    case dmFunctorErrors::E_NO_KERNEL  : break;
    case dmFunctorErrors::E_NO_ERROR   : return false;
  };
  if(txt) CCI_SET_ERROR(ob,me,txt);
  return true;  
} 

#define CHECK_LAST_ERROR(me) CHECK_ERROR(_This->LastError,ob,me)
#define SET_LAST_ERROR(err) _This->LastError = err
#define RETURN_ERROR_CHECK() \
        if(_This->LastError!=dmFunctorErrors::E_NO_ERROR) {     \
          CHECK_ERROR(_This->LastError,_THIS_OBJECT,CCI_P(Id)); \
          CCI_RETURN_FAIL()                                     \
        }                                                       \
        CCI_RETURN_OK()

//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( Filters )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE(Filters)

  CCI_INIT_MEMBER(KernelMngr)

  _This->LastError = dmFunctorErrors::E_NO_ERROR;
  _This->KernelMngr.LoadKernels();   // Get the kernel manager

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( Filters )
{
  CCI_INSTANCE(Filters)

  _This->KernelMngr.Release();

  CCI_DESTROY_MEMBER(KernelMngr);
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Filters )
  CCI_ATTR_GET( Filters,MaskLength, CCIA_RETVAL = _This->Mask.Size() )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Filters )
  CCIA_DISCARD( Filters,MaskLength )
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method MaskSize   
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Filters, MaskSize )
{
  CCI_INSTANCE(Filters)
  switch(CCI_P(flag)) {
    case CCI_ME_SET : _This->Mask = dmMaskDescription(CCI_P(width),CCI_P(height)); break;
    case CCI_ME_GET :{
      CCI_RETVAL_P(height) = _This->Mask.Height();
      CCI_RETVAL_P(width)  = _This->Mask.Width();
    } break;
  }
  CCI_RETURN_OK()
}    
//---------------------------------------------------------------------
// Method MaskRef
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Filters, MaskRef )
{
  CCI_INSTANCE(Filters)
  switch(CCI_P(flag)) {
    case CCI_ME_SET : _This->Mask.SetOrigin(CCI_P(ox),CCI_P(oy)); break;
    case CCI_ME_GET :{
      CCI_RETVAL_P(ox) = _This->Mask.Ox();
      CCI_RETVAL_P(oy) = _This->Mask.Oy();
    } break;
  }
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method MFilter    
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Filters, MFilter )
{
  CCI_INSTANCE(Filters)

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

      dmBufferParameters _Params(*buffer,*img,*rgn);

      //__dm_DebugBreak__

      dmFilterType _ftype;
      switch(CCI_P(type)) {
        case CCIV_Filters_Min   : _ftype = dmFilterMin    ; break;
        case CCIV_Filters_Max   : _ftype = dmFilterMax    ; break;
        case CCIV_Filters_Middle: _ftype = dmFilterMiddle ; break;
        case CCIV_Filters_Mean  : _ftype = dmFilterMean   ; break;
        case CCIV_Filters_Median: _ftype = dmFilterMedian ; break;
/*
        //------------------------------
        // Special case for Median
        //------------------------------
        case CCIV_Filters_Median: 
        {
           dmRFilter _RFilter(_This->Mask,static_cast<int>((_This->Mask.Size()+1)/2));

           dmColorSpaceFunctor<dmRFilter> _RFunctor(_RFilter);

           if(!_RFunctor.Apply(_IParams,buffer,img,rgn,true))
             SET_LAST_ERROR(_RFunctor.ReturnedError);
          
          RETURN_ERROR_CHECK();
        } break;
*/
        default : 
           CCI_SET_ERROR_MSG(_TXT("Invalid filter type !"));
           CCI_RETURN_FAIL()
       }

      dmMFilter _MFilter(_This->Mask,_ftype);

      dmColorSpaceFunctor<dmMFilter> _MFunctor(_MFilter);
      if(!_MFunctor.Apply(_IParams,buffer,img,rgn,true))
          SET_LAST_ERROR(_MFunctor.ReturnedError);

    } else SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else  SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);
  RETURN_ERROR_CHECK()
}    
//---------------------------------------------------------------------
// Method RFilter    
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Filters, RFilter )
{
  CCI_INSTANCE(Filters)

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
      
      if(CCI_P(rank)>=_This->Mask.Size()) {
        CCI_SET_ERROR_MSG(_TXT("Rank out of range !"));
        CCI_RETURN_FAIL()
      }

      dmRFilter _Filter(_This->Mask,CCI_P(rank));

      dmColorSpaceFunctor<dmRFilter> _Functor(_Filter);
      if(!_Functor.Apply(_IParams,buffer,img,rgn,true))
          SET_LAST_ERROR(_Functor.ReturnedError);

    } else SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else  SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);
  RETURN_ERROR_CHECK()
}    
//---------------------------------------------------------------------
// Method LFilter    
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Filters, LFilter )
{
  CCI_INSTANCE(Filters)

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    if(img) 
    {      
      dmASSERT( buffer != NULL );
      
      if(CCI_P(count) < _This->Mask.Size()) 
        CCI_RETURN_INVALID_PARAM()

      dmLFilter _Filter(_This->Mask,CCI_P(coeffs));

      dmColorSpaceFunctor<dmLFilter> _Functor(_Filter);
      if(!_Functor.Apply(_IParams,buffer,img,rgn,true))
          SET_LAST_ERROR(_Functor.ReturnedError);

    } else SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);
  RETURN_ERROR_CHECK()
}   

//---------------------------------------------------------------------
// Method ApplyKernel
//---------------------------------------------------------------------
// Defined in daim_kernel library
bool __dmKernel _dmDoImageMath2( dmBufferParameters& , dm_int );
//---------------------------------------------------------------------
class dmConvolutionOperator : public dmConvolution
{
   public:
     int _Math;

     dmConvolutionOperator(  dmKernelFamily& aFamily, int Iter, int Math ) 
     : dmConvolution(aFamily,Iter),_Math(Math) {}

   bool Apply( dmBufferParameters& params ) {
      if(dmConvolution::Apply(params)) {
        if(_Math) _dmDoImageMath2(params,_Math);
        return true;        
      }
      return false;
   }
};
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Filters, ApplyKernel )
{
  CCI_INSTANCE(Filters)

  if(CCI_P(kernel) != NULL )
  {
    dmIParameters _IParams;
    if(_IParams.QueryInterface(CCI_P(params)))
    {
      dmImage*       img    = _IParams.GetImage();
      dmImageBuffer* buffer = _IParams.GetImageBuffer();
      dmRegion*      rgn    = _IParams.GetRoi();

      if(img) 
      {   
        dmASSERT( buffer != NULL );

        dmKernelFamily* family = _This->KernelMngr.FindFamily(CCI_P(kernel));      
        if(family) 
        {
          dmConvolutionOperator _Convolution(*family,CCI_P(iter),CCI_P(oper));
 
          dmColorSpaceFunctor<dmConvolutionOperator> _Functor(_Convolution);
          if(!_Functor.Apply(_IParams,buffer,img,rgn,true))
             SET_LAST_ERROR(_Functor.ReturnedError);
          else
            CCI_RETURN_OK()
        } 
        else 
        {
          CCI_SET_ERROR_MSG(
            dmString::FormatString(dmString::npos,_TXT("Cannot find kernel family %s !"),
                                   CCI_P(kernel)).CStr()
          );
          SET_LAST_ERROR(dmFunctorErrors::E_NO_KERNEL);
        }
      } else SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
    } else SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);
  } else {
    CCI_SET_ERROR_MSG(_TXT("Kernel missing !"));
    SET_LAST_ERROR(dmFunctorErrors::E_NO_KERNEL);
  }
  RETURN_ERROR_CHECK()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Filters )  	
  CCI_REGISTER_METHOD_PP( Filters, ApplyKernel)
  CCI_REGISTER_METHOD_PP( Filters, MaskSize   )     
  CCI_REGISTER_METHOD_PP( Filters, MaskRef    )
  CCI_REGISTER_METHOD_PP( Filters, MFilter    )
  CCI_REGISTER_METHOD_PP( Filters, RFilter    )
  CCI_REGISTER_METHOD_PP( Filters, LFilter    )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Filters )
{
  CCI_INVOKE_FACTORY( Filters )
}
//---------------------------------------------------------------------
