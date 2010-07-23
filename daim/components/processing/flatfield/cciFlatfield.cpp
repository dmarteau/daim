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
// File         : cciFlatfield.cpp
// Date         : 11 juil. 2010
// Author       : David Marteau
//--------------------------------------------------------
#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciIImageContainer.h"
#include "cciImageUtils.h"
#include "cciIImageMath.h"
#include "cciIFlatfield.h"

#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "templates/processing/dmArithmetics.h"


/* Header file */
class cciFlatfield : public cciIFlatfield
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IFLATFIELD

  cciFlatfield();

private:
  ~cciFlatfield();

protected:
  cci_Ptr<cciIImageList>  mFields;
  cci_Ptr<cciIColorSpace> mColorspace;
  cci_Ptr<cciIImageMath>  mImageMath;
  
  cci_result EnsureImageMath();
};

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciFlatfield, cciIFlatfield)

cciFlatfield::cciFlatfield()
{
  /* member initializers and constructor code */
}

cciFlatfield::~cciFlatfield()
{
  /* destructor code */
}

cci_result cciFlatfield::EnsureImageMath()
{
  if(!mImageMath) {
    cci_result rv;
    mImageMath = do_CreateInstance("@daim.org/processing/imagemath;1",&rv);
    return rv;
  }

  return CCI_OK;
}

/* void setFlatField (in cciImage image, in dm_real offset, in dm_real norm, [optional] in string colorspace); */
CCI_IMETHODIMP cciFlatfield::SetFlatField(cciImage image, dm_real offset, dm_real norm, 
                                          const char * colorspace)
{
  dmImage* img = CCI_IF_NATIVE(image);
  CCI_ENSURE_ARG_POINTER(img);  
  
  cci_result rv;

  if(!mFields) {
    rv = CCI_NewImageList(getter_AddRefs(mFields));
    CCI_ENSURE_SUCCESS(rv,CCI_ERROR_FAILURE);
  }
  
  mFields->Clear();  
  
  // Handle RGB image
  if(!dmIsPixelFormatScalar(img->PixelFormat()))
  {
    if(!colorspace)
        colorspace = "RGB";
    
    // split image  
    rv = CCI_NewColorSpace(colorspace,getter_AddRefs(mColorspace));
    if(CCI_FAILED(rv))
       return rv;
    
    mColorspace->SetChannelFmt(dmPixelFormat32bppFloat);
    mColorspace->SetImageList(mFields);
    
    rv = mColorspace->Split(native_Wrapper(img),dm_null);
    if(CCI_FAILED(rv))
       return rv;

    mColorspace->SetImageList(dm_null);
  }
  else
  {
    mFields->SetSize(1);
    rv = mFields->StoreBuffer(0,img,dm_null,dmPixelFormat32bppFloat);
    if(CCI_FAILED(rv))
       return rv;    
  }

  dm_uint32 _NumBuffers = 0;
          
  mFields->GetSize(&_NumBuffers);
  for(dm_uint32 i=0;i < _NumBuffers; ++i)
  {
    img = mFields->GetNativeBuffer(i);
    CCI_ENSURE_TRUE(img,CCI_ERROR_UNEXPECTED);
       
    daim::image<dm_float>& _img = dmIImage<dmPixelFormat32bppFloat>::Cast(img)->Gen();

    dm_real Mean = norm; 
    if(Mean<=0.) // Need to compute a mean value
    {
      if(offset>=0) 
      {
        dmRegion Rgn;
        
        // Create a region associated to values greater than offset
        daim::create_roi(_img,std::bind2nd(std::greater<dm_float>(),
                         static_cast<dm_float>(offset)),Rgn);

        if(!Rgn.IsEmptyRoi())
           Mean = daim::accumulate(Rgn,_img,0.0) / Rgn.Area();
           
      } else
        Mean = daim::accumulate(_img.rect(),_img,0.0) / _img.size();
    }
  
    if(Mean > 0)
    {
      if(offset>=0)  // Replace low values by mean/norm value
        daim::replace_if(_img,std::bind2nd(std::less_equal<dm_float>(),
                         static_cast<dm_float>(offset)),Mean);             
      
      daim::mul_image(_img.rect(),_img,static_cast<dm_float>(1.0/Mean));
     
      rv = CCI_OK;
    } else {
      dmLOG_ERROR("Zero level image !");
      rv = CCI_ERROR_FAILURE;
      break;
    }
  }

  return rv;
}

/* void apply (in cciImage image, in unsigned long flags); */
CCI_IMETHODIMP cciFlatfield::Apply(cciImage image, dm_uint32 flags)
{
  CCI_ENSURE_TRUE(mFields,CCI_ERROR_NOT_INITIALIZED);

  dmImage* img = CCI_IF_NATIVE(image);
  CCI_ENSURE_ARG_POINTER(img);

  cci_result rv = EnsureImageMath();
  if(CCI_FAILED(rv))
     return rv;
  
  /// Handle RGB image
  if(!dmIsPixelFormatScalar(img->PixelFormat()))
  {
    CCI_ENSURE_TRUE(mColorspace,CCI_ERROR_FAILURE);
    
    rv = mColorspace->Split(native_Wrapper(img),dm_null);
    if(CCI_FAILED(rv))
       return rv;
    
    cci_Ptr<cciIImageList> lhs;
    mColorspace->GetImageList(getter_AddRefs(lhs));
    
    // Apply image math to all image list
    rv = mImageMath->DoImageListMath(lhs,mFields,dm_null,dmTk::Math::DivPixels,dm_null);
    if(CCI_FAILED(rv))
       return rv;

    rv = mColorspace->Merge(native_Wrapper(img)); // Merge back         
  }
  else 
  {
    dmImage* rhs = mFields->GetNativeBuffer(0);
    
    rv = mImageMath->DoImageMath(native_Wrapper(img),native_Wrapper(rhs),dm_null,0,0,
                                 dmTk::Math::DivPixels,dm_null);
  }  
  
  
  return rv;  
}

/* void clear (); */
CCI_IMETHODIMP cciFlatfield::Clear()
{
  mFields     = dm_null;
  mColorspace = dm_null;
  return CCI_OK;
}


//=====================================
// Module
//=====================================

#include "cciIGenericFactory.h"

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciFlatfield)

static const cciModuleComponentInfo components[] = {
    { CCI_FLATFIELD_CLASSNAME,
      CCI_FLATFIELD_CID,
      CCI_FLATFIELD_CONTRACTID,
      cciFlatfieldConstructor
    },
};

CCI_IMPL_GETMODULE(FlatfieldModule, components)


