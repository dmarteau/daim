
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

#define dmUseCCI
#define dmUseKernelImageTemplates
#define dmUseKernelBuffer
#include "daim_kernel.h"

#include "templates/processing/dmArithmetics.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_CommandHandler.h"
#include "daim_modules/CCI_PropertyList.h"
#include "daim_modules/CCI_Parameters.h"

#include "CCI_GREYCStoration.h"

//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_GREYCStoration
#define CCIC_SUPER CCIC_CommandHandler
#define CCIC_SUPER_VERSION CCI_CommandHandler_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_GREYCStoration_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2006"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"

//--------------------------------------------------------------------
CCI_DECL_DATA( GREYCStoration )
{
  dm_real   dt;           // Regularization strength for one iteration (>=0);
  dm_uint   nb_iter;      // Number of regularization iterations (>0);
  dm_real   p;            // Contour preservation for regularization (>=0);
  dm_real   anisotropy;   // Regularization anisotropy (0<=a<=1);
  dm_real   alpha;        // Noise scale(>=0);
  dm_real   sigma;        // Geometry regularity (>=0);
  bool      fast_approx;  // Use fast approximation for regularization (0 or 1);
  dm_real   gauss_prec;   // Precision of the gaussian function for regularization (>0);
  dm_real   dl;           // Spatial integration step for regularization (0<=dl<=1);
  dm_real   da;           // Angular integration step for regulatization (0<=da<=90);
  dm_uint   interp;       // Interpolation type (0=Nearest-neighbor, 1=Linear, 2=Runge-Kutta);
  
  dm_real   sdt;          // Sharpening strength for one iteration (>=0)");
  dm_real   sp;           // Sharpening edge threshold (>=0)");
  
  dm_uint   progress;     // Progress indicator
  dm_uint   iter;         // Iteration indicator
  dm_bool   stopped;
  
  dm_uint   inpaint;      // Inpainting init (0=black, 1=white, 2=noise, 3=unchanged, 4=interpol);
  
};

typedef CCI_INSTANCE_PTR(GREYCStoration) _Instance_Ptr;

void GREYCStoration_Dump( const char_t* msg, _Instance_Ptr _This ) 
{
  dmLOG("* GREYCStoration (%s) :\n",msg);
  
  dmLOG(" dt          = %lf\n",_This->dt);
  dmLOG(" nb_iter     = %u\n" ,_This->nb_iter);
  dmLOG(" p           = %lf\n",_This->p);
  dmLOG(" anisotropy  = %lf\n",_This->anisotropy);
  dmLOG(" alpha       = %lf\n",_This->alpha);
  dmLOG(" sigma       = %lf\n",_This->sigma);
  dmLOG(" fast_approx = %s\n",_This->fast_approx?"true":"false");
  dmLOG(" gauss_prec  = %lf\n",_This->gauss_prec);
  dmLOG(" dl          = %lf\n",_This->dl);
  dmLOG(" da          = %lf\n",_This->da);
  dmLOG(" interp      = %u\n" ,_This->interp);  
  dmLOG(" sdt         = %lf\n",_This->sdt);
  dmLOG(" sp          = %lf\n",_This->sp);
  dmLOG(" progress    = %u\n" ,_This->progress);
  dmLOG(" stopped     = %s\n" ,_This->stopped?"true":"false");
  dmLOG(" inpaint     = %u\n" ,_This->inpaint);
}

#include "dmGreyCStoration.cpp"

//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( GREYCStoration )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE(GREYCStoration)

  // Set up default values

  _This->dt          = 50.0;
  _This->nb_iter     = 1;
  _This->p           = 0.9;
  _This->anisotropy  = 0.2;
  _This->alpha       = 0.5;
  _This->sigma       = 1.0;
  _This->fast_approx = true;
  _This->gauss_prec  = 2.0;
  _This->dl          = 0.8;
  _This->da          = 30.0;
  _This->interp      = 0;

  _This->sdt         = 0.0;
  _This->sp          = 0.5;

  _This->progress    = 0;
  _This->iter        = 0;
  
  _This->stopped     = dm_false;
  
  _This->inpaint     = 4;

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( GREYCStoration )
{
  CCI_UNUSUED_INSTANCE(GREYCStoration)
  CCI_DESTROY_SUPER()
}

//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( GREYCStoration )
   CCI_ATTR_GET( GREYCStoration, Progress, CCIA_RETVAL = _This->progress )
   CCI_ATTR_GET( GREYCStoration, Iter    , CCIA_RETVAL = _This->iter )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( GREYCStoration )
   CCI_ATTR_SET( GREYCStoration, Progress, _This->progress = CCIA_DATA )
   CCI_ATTR_SET( GREYCStoration, Iter    , _This->iter     = CCIA_DATA )
CCI_END_SETTER()

/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( GREYCStoration )
{
  CCI_UNUSUED_INSTANCE(GREYCStoration)
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( GREYCStoration )
{
  CCI_UNUSUED_INSTANCE(GREYCStoration)
  CCI_RETURN_INVOKE_SUPER()
}
*/
//---------------------------------------------------------------------
// Utility macros
//---------------------------------------------------------------------
#define IF_EXECCOMMAND( cmd ) \
    if(EQUAL_STRING(cmd_name,cmd))

#define GET_PARAM( param, Type ) \
   _This->param = _IProps.GetProperty##Type(key,NULL,_TXT(#param) ,_This->param);
    
#define GET_PARAM_ASSERT( param, assertion, Type ) \
   _This->param = _IProps.GetProperty##Type(key,NULL,_TXT(#param) ,_This->param); \
   if(!assertion) CCI_RETURN_INVALID_PARAM();  
   
#define SET_PARAM( param, Type ) \
   _IProps.SetProperty##Type(key,NULL,_TXT(#param) ,_This->param);

//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( CommandHandler , ExecCommand )
{
  CCI_INSTANCE(GREYCStoration)

  const char_t* cmd_name = CCI_P( command );  
     
  IF_EXECCOMMAND("setoptions") 
  {
    dmIPropertyList _IProps;
    if(_IProps.QueryInterface(CCI_P(param)))
    {
      dm_param key  = CCI_P(argint);

      GET_PARAM_ASSERT(dt,(_This->dt >= 0)    ,Real);
      GET_PARAM_ASSERT(nb_iter,(_This->nb_iter > 0),Long);
      GET_PARAM_ASSERT(p,(_This->p >= 0)     ,Real);
      GET_PARAM_ASSERT(anisotropy,(_This->anisotropy >= 0 && _This->anisotropy <= 1),Real);
      GET_PARAM_ASSERT(alpha,(_This->alpha >= 0) ,Real);
      GET_PARAM_ASSERT(sigma,(_This->alpha >= 0) ,Real);
      GET_PARAM_ASSERT(gauss_prec,(_This->gauss_prec > 0) ,Real);
      GET_PARAM_ASSERT(dl,(_This->dl >= 0 && _This->dl <= 1)  ,Real);
      GET_PARAM_ASSERT(da,(_This->da >= 0 && _This->da <= 90) ,Real);
      GET_PARAM_ASSERT(sdt,(_This->sdt >= 0) ,Real);
      GET_PARAM_ASSERT(sp,(_This->sp  >= 0) ,Real);
      GET_PARAM_ASSERT(interp,(_This->interp >=0 && _This->interp<=2)   ,Long);
      GET_PARAM(fast_approx ,Bool); 

      #ifdef _DEBUG
        GREYCStoration_Dump(cmd_name,_This);
      #endif

      CCI_RETURN_OK() 
    }
  }


  IF_EXECCOMMAND("getoptions") 
  {
    dmIPropertyList _IProps;
    if(_IProps.QueryInterface(CCI_P(param)))
    {
      dm_param key  = CCI_P(argint);

      SET_PARAM(dt,Real);
      SET_PARAM(nb_iter,Long);
      SET_PARAM(p,Real);
      SET_PARAM(anisotropy,Real);
      SET_PARAM(alpha,Real);
      SET_PARAM(sigma,Real);
      SET_PARAM(gauss_prec,Real);
      SET_PARAM(dl,Real);
      SET_PARAM(da,Real);
      SET_PARAM(sdt,Real);
      SET_PARAM(sp,Real);
      SET_PARAM(interp,Long);
      SET_PARAM(fast_approx ,Bool); 

      CCI_RETURN_OK() 
    }
  }

  IF_EXECCOMMAND("restore") 
  {
     dmIParameters _IParams;
     if(_IParams.QueryInterface(CCI_P(param))) 
     {
        dmImage*   _Img = _IParams.GetImage();
        dmRegion*  _Roi = _IParams.GetRoi();
        
        if(_Img) 
        {
          dmINotify _Progress;
          _Progress.Attach(_THIS_OBJECT);
        
          _This->stopped = dm_false;
        
          if(dmGREYCStoration(*_Img,*_Roi,NULL,_Progress,_This,GREYCStoration::restore))
             CCI_RETURN_OK() 
        } else
          dmLOG("GREYCStoration: missing image for %s\n",CCI_P(command));
     } else
       dmLOG("GREYCStoration: invalid parameter for %s\n",CCI_P(command));
     CCI_RETURN_FAIL() 
  }

  IF_EXECCOMMAND("inpaint") 
  {
     dmIParameters _IParams;
     if(_IParams.QueryInterface(CCI_P(param))) 
     {
        dmImage*   _Img  = _IParams.GetImage();
        dmRegion*  _Roi  = _IParams.GetRoi();
        dmRegion*  _Mask = _IParams.GetMask();
        
        if(_Mask==NULL || _Mask->IsEmptyRoi())
           CCI_RETURN_INVALID_PARAM()
        
        if(_Img) 
        {
          dmINotify _Progress;
          _Progress.Attach(_THIS_OBJECT);
        
          _This->stopped = dm_false;
          _This->inpaint = CCI_P(argint);
        
          if(dmGREYCStoration(*_Img,*_Roi,_Mask,_Progress,_This,GREYCStoration::inpaint))
             CCI_RETURN_OK() 
        } else
          dmLOG("GREYCStoration: missing image for %s\n",CCI_P(command));
     } else
       dmLOG("GREYCStoration: invalid parameter for %s\n",CCI_P(command));
     CCI_RETURN_FAIL() 
  }

  IF_EXECCOMMAND("stop") 
  {
    if(_This->progress > 0) {
       _This->stopped = dm_true;
    }
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( CommandHandler , IsCommandSupported )
{
  CCI_UNUSUED_INSTANCE(GREYCStoration)
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( GREYCStoration )
  CCI_REGISTER_METHOD_PP( CommandHandler , ExecCommand        )
  CCI_REGISTER_METHOD_PP( CommandHandler , IsCommandSupported )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( GREYCStoration )
{   
  CCI_INVOKE_FACTORY(GREYCStoration)
}
//---------------------------------------------------------------------
