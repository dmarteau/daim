
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
#include "daim_modules/processing/CCI_ImageStat.h"

//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_ImageStat
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR "David Marteau"
#define CCI_VERSION   CCI_ImageStat_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2005"))
//------------------------------------------------------------------------
#define SET_ERROR( ob,me,txt ) CCI_SetError(ECCI_ERROR,me,CCI_ObjectClass(ob),ob,txt)
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "common/dmUserLib.h"

#include <math.h>

CCI_DECL_DATA( ImageStat )
{
  dm_uint mDummy;
} mData;

//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( ImageStat )
{
  CCI_GENERIC_CONSTRUCT(ImageStat)

  CCI_RETURN_THIS()
}
//--------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( ImageStat )
{
  CCI_INSTANCE(ImageStat)

  CCI_DESTROY_DATA()     
  CCI_DESTROY_SUPER()
}
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( ImageStat ) // Use CCIA_RETVAL
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( ImageStat ) // Use CCIA_DATA
CCI_END_SETTER()
*/
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( ImageStat )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( ImageStat )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method     : Moment
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageStat , Moment )
{  
  CCI_INSTANCE(ImageStat)   

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*  img = _IParams.GetImage();
    dmRegion* rgn = _IParams.GetRoi();
    if(img) 
    {
      dm_real _Value = 0.0;
      if(dmImageStat(*img,*rgn,CCI_P(order),CCI_P(central),_Value)) {
         CCI_RETVAL_P(value) = _Value;  
         CCI_RETURN_OK()
      }
    } else
      CCI_SET_ERROR_MSG(_TXT("No image !"));
  
  } else {
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));
    CCI_RETURN_INVALID_PARAM()
  } 

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method     : GetStat
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageStat , GetStat )
{  
  CCI_INSTANCE(ImageStat)   

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*  img = _IParams.GetImage();
    dmRegion* rgn = _IParams.GetRoi();
    if(img) 
    {
      const char_t* _Type = CCI_P(type);

      dm_real N;

      if(dmImageStat(*img,*rgn,0,0.,N) && N > 1)
      {
        dm_uint order = 4;
        dm_real q[4];
        q[0]=q[1]=q[2]=q[3]=0.0;

        dmImageStats(*img,*rgn,1,0.,q);
        dm_real m = q[0]/N;
                
        if(EQUAL_STRING(_Type,"mean")) {
          CCI_RETVAL_P(value) = m;
          CCI_RETURN_OK()
        }

        if(EQUAL_STRING(_Type,"variance")) {
          dmImageStats(*img,*rgn,2,m,q);
          CCI_P(value) = (q[1] - (q[0]/N))/(N-1);
          CCI_RETURN_OK()
        }

        if(EQUAL_STRING(_Type,"skewness")) {
          dmImageStats(*img,*rgn,3,m,q);
          dm_real var  = (q[1] - (q[0]/N))/(N-1);
          dm_real sdev = sqrt(var);
          CCI_P(value) = q[3] / (N*var*sdev);
          CCI_RETURN_OK()
        }

        if(EQUAL_STRING(_Type,"kurtosis")) {
          dmImageStats(*img,*rgn,4,m,q);
          dm_real var   = (q[1] - (q[0]/N))/(N-1);
          CCI_P(value) = q[4] / (N*var*var);
          CCI_RETURN_OK()
        }

      }
    } else
      CCI_SET_ERROR_MSG(_TXT("No image !"));
  
  } else {
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));
    CCI_RETURN_INVALID_PARAM()
  } 

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( ImageStat )
  CCI_REGISTER_METHOD_PP(ImageStat,Moment   )
  CCI_REGISTER_METHOD_PP(ImageStat,GetStat  )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( ImageStat )
{
  //DM_INIT_API()
  CCI_INVOKE_FACTORY(ImageStat)
}
//---------------------------------------------------------------------
