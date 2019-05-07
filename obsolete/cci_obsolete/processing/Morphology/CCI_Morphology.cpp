
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

#include "daim_modules/processing/CCI_Morphology.h"
#include "daim_modules/processing/CCI_KernelFamilies.h"

//------------------------------------------------------------------
#define CCIC_NAME CCIC_Morphology
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Morphology_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "daim_modules/CCI_Interface.h"

#include "common/dmUserLib.h"

//--------------------------------------------------------------------
CCI_DECL_DATA( Morphology )
{
  dmIKernelFamilies KernelMngr;
};
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( Morphology )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( Morphology)

  CCI_INIT_MEMBER(KernelMngr)

  _This->KernelMngr.LoadKernels();  // Get the kernel manager

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( Morphology )
{
  CCI_INSTANCE( Morphology )

  _This->KernelMngr.Release();

  CCI_DESTROY_MEMBER(KernelMngr);
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( Morphology )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( Morphology )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method: ApplyKernel
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Morphology, ApplyKernel )
{
  CCI_INSTANCE( Morphology )

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
          dmBufferParameters _Params(*buffer,*img,*rgn);
          dmMorphology       _Filter(*family,CCI_P(action),CCI_P(iter));

          if(_Filter.Apply(_Params))
          {
            if(CCI_P(operation))
              dmArithmetics(_Params,CCI_P(operation));
 
            CCI_RETURN_OK()
          }
       }
       else
         CCI_SET_ERROR_MSG(
           dmString::FormatString(dmString::npos,_TXT("Cannot find kernel family %s !"),CCI_P(kernel)).CStr()
         );
 
      } else 
        CCI_SET_ERROR_MSG(_TXT("No image !"));
    } else 
      CCI_SET_ERROR_MSG(_TXT("Invalid masks parameters !"));    
  } else 
    CCI_SET_ERROR_MSG(_TXT("Kernels missing !"));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Morphology )
    CCI_REGISTER_METHOD_PP( Morphology, ApplyKernel )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Morphology )
{
  CCI_INVOKE_FACTORY(Morphology)
}
//---------------------------------------------------------------------
