
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
#define dmUseKernelImageTemplates
#define dmUseKernelBuffer
#define dmUseKernelImage
#include "daim_kernel.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_ImageList.h"
#include "daim_modules/CCI_Parameters.h"

#include "daim_modules/processing/CCI_Colorspace.h"
#include "daim_modules/processing/CCI_FlatField.h"
#include "daim_modules/processing/CCI_ImageMath.h"

#include "templates/processing/dmArithmetics.h"
//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_FlatField
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR "David Marteau"
#define CCI_VERSION   CCI_FlatField_Version
#define CCI_REVISION  1
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2005"))
//------------------------------------------------------------------------
#define SET_ERROR( ob,me,txt ) CCI_SetError(ECCI_ERROR,me,CCI_ObjectClass(ob),ob,txt)
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "common/dmUserLib.h"

#define _NOINDEX_ ~0UL

CCI_DECL_DATA( FlatField )
{
  dmIImageList  mFields;
  dmIImageMath  mMaths;
  dm_uint       mWidth;
  dm_uint       mHeight;
  dm_uint       mFormat;  
} mData;

typedef CCI_INSTANCE_PTR(FlatField) _Instance_Ptr;
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( FlatField )
{
  CCI_GENERIC_CONSTRUCT(FlatField)

  _This->mWidth   = 0;
  _This->mHeight  = 0;
  _This->mFormat  = 0;
  _This->mFields.Create();
  
  CCI_RETURN_THIS()
}
//--------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( FlatField )
{
  CCI_INSTANCE(FlatField)

  _This->mMaths.Release();
  _This->mFields.Release();
  
  CCI_DESTROY_DATA()     
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( FlatField ) // Use CCIA_RETVAL
  CCI_ATTR_GET( FlatField,Format, CCIA_RETVAL = _This->mFormat  )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( FlatField ) // Use CCIA_DATA
  CCIA_DISCARD( FlatField, Format );
CCI_END_SETTER()
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( FlatField )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( FlatField )
{
  CCI_RETURN_INVOKE_SUPER()
}
*/
//---------------------------------------------------------------------
// Method     : SetFlatField
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( FlatField , SetFlatField )
{  
  CCI_INSTANCE(FlatField)   

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {    
    dmImage*  img   = _IParams.GetImage();
    dmImage* _Image = img;

    if(img) 
    {
      _This->mWidth  = img->Width(); 
      _This->mHeight = img->Height();
      
      dm_uint i,_NumBuffers = _This->mFields.Size();
      for(i=0;i < _NumBuffers; ++i)
         _This->mFields.ReleaseBuffer(i);
        
      _This->mFormat = 0;
       
      //------------------------------------------
      // Handle RGB image
      //------------------------------------------

      if(!dmIsPixelFormatScalar(img->PixelFormat()))
      {
        dmIColorSpace _ColorSpace;
        if(_ColorSpace.QueryInterface(_IParams.GetColorSpace()))
        {          
          dm_uint  numchannels = _ColorSpace.NumChannels();
                    
          _This->mFields.Resize(numchannels);
          
          if(_ColorSpace.SplitImage(img)) 
          {
            _NumBuffers = _This->mFields.Size();
            for(i=0;i < _NumBuffers; ++i) {
              _This->mFields.StoreBuffer(i,_ColorSpace.GetBuffer(i),
                                         dm_null,dmPixelFormat32bppFloat);
            }
          } 
        } else {
          CCI_SET_ERROR_MSG(_TXT("Cannot get colorspace !"));
          CCI_RETURN_FAIL()
        }
      } 
      else
      {
        _This->mFields.Resize(1);
        _This->mFields.StoreBuffer(0,img,dm_null,dmPixelFormat32bppFloat);  
      }  
    
      _NumBuffers = _This->mFields.Size();
      for(i=0;i < _NumBuffers; ++i)
      {
        img = _This->mFields.GetBuffer(i);
        if(!img) {
          CCI_SET_ERROR_MSG(_TXT("Null image in list !"));
          CCI_RETURN_FAIL()
        }
           
        daim::image<dm_float>& _img =  dmIImage<dmPixelFormat32bppFloat>::Cast(img)->Gen();

        dm_real Mean = CCI_P(norm); 
        if(Mean<=0.) // Need to compute a mean value
        {
          if(CCI_P(offset)>=0) 
          {
            dmRegion Rgn;
            
            // Create a region associated to values greater than offset
            daim::create_roi(_img,std::bind2nd(std::greater<dm_float>(),
                             static_cast<dm_float>(CCI_P(offset))),Rgn);

            if(!Rgn.IsEmptyRoi())
               Mean = daim::accumulate(Rgn,_img,0.0) / Rgn.Area();
               
          } else
            Mean = daim::accumulate(_img.rect(),_img,0.0) / _img.size();
        }
      
        if(Mean > 0)
        {
          if(CCI_P(offset)>=0)  // Replace low values by mean/norm value
            daim::replace_if(_img,std::bind2nd(std::less_equal<dm_float>(),
                             static_cast<dm_float>(CCI_P(offset))),Mean);             
          
          daim::mul_image(_img.rect(),_img,static_cast<dm_float>(1.0/Mean));
          
        } else {
          CCI_SET_ERROR_MSG(_TXT("Zero level image !"));
          CCI_RETURN_FAIL()
        }        
      }
      
      _This->mFormat = _Image->PixelFormat();
      
      CCI_RETURN_OK()
      
    } else
      CCI_SET_ERROR_MSG(_TXT("No image !"));
  
  } else {
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));
    CCI_RETURN_INVALID_PARAM()
  } 

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method     : Apply
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( FlatField , Apply )
{  
  CCI_INSTANCE(FlatField)   
  
  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
   dmImage*  img = _IParams.GetImage();
    
   if(img) 
   {
     // Create the math object if needed
     dmIImageMath& _IMath = _This->mMaths;
     if(!_IMath.IsValid())
         _IMath.Create();

      dmIImageList  _savedList;
      if(_savedList.QueryInterface(_IParams.GetImageList()))
         _savedList.AddRef();

     _IParams.SetImageList(_This->mFields); 
     _IParams.SetImage    (img); // Restore image (XXX BUG CCI_Parameters) 

     /// Handle RGB image
     if(!dmIsPixelFormatScalar(img->PixelFormat()))
     {
       dmIColorSpace _ColorSpace;
       if(_ColorSpace.QueryInterface(_IParams.GetColorSpace()))
       {          
         dm_uint  numchannels = _ColorSpace.NumChannels();
          
         _ColorSpace.SetChannelFmt(dmPixelFormat32bppFloat);
          
         if(_ColorSpace.SplitImage(img)) 
         {
            // Apply image math to all image list
            _IMath.DoImageListMath(_IParams,dmTk::Math::DivPixels,_ColorSpace);            
            _ColorSpace.Merge(_IParams);        // Merge back         
         } 
       } 
     }
     else 
     {
       _IMath.DoImageMath(_IParams,dmTk::Math::DivPixels,0);         
     }  

     // restore imageList
     _IParams.SetImageList(_savedList);  // Restore imagelist    
     _IParams.SetImage    (img); // Restore image (XXX BUG CCI_Parameters) 

     if(_savedList.IsValid())
        _savedList.Release();

     CCI_RETURN_OK()
                 
   } else
      CCI_SET_ERROR_MSG(_TXT("No image !"));
  
 } else {
   CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));
   CCI_RETURN_INVALID_PARAM()
 } 

 CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method     : Clear
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( FlatField , Clear )
{  
   CCI_INSTANCE(FlatField)   

   dm_uint i,_NumBuffers = _This->mFields.Size();
   for(i=0;i < _NumBuffers; ++i)
      _This->mFields.ReleaseBuffer(i);

   CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( FlatField )
  CCI_REGISTER_METHOD_PP(FlatField,SetFlatField)
  CCI_REGISTER_METHOD_PP(FlatField,Apply       )
  CCI_REGISTER_METHOD_NP(FlatField,Clear       )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( FlatField )
{
  //DM_INIT_API()
  CCI_INVOKE_FACTORY(FlatField)
}
//---------------------------------------------------------------------
