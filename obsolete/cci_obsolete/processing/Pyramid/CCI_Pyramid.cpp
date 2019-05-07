
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

#include "daim_modules/processing/CCI_Filters.h"
#include "daim_modules/processing/CCI_Pyramid.h"
#include "daim_modules/processing/CCI_KernelFamilies.h"

//------------------------------------------------------------------
#define CCIC_NAME CCIC_Pyramid
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Pyramid_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"

#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"
#include "common/dmUserLib.h"

//--------------------------------------------------------------------
CCI_DECL_DATA( Pyramid )
{
  dm_uint       Factor;
  dmString      Kernel;
  dmImageBuffer Buffer;

  dmIKernelFamilies KernelMngr;
};
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( Pyramid )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( Pyramid)

  CCI_INIT_MEMBER(Kernel)
  CCI_INIT_MEMBER(KernelMngr)
  CCI_INIT_MEMBER(Buffer)

  _This->Kernel.Assign(_TXT("Smooth/Gauss5x5"));
  _This->Factor = 2;
  _This->KernelMngr.LoadKernels();   // Get the kernel manager

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( Pyramid )
{
  CCI_INSTANCE( Pyramid )

  _This->KernelMngr.Release();

  CCI_DESTROY_MEMBER(Buffer)
  CCI_DESTROY_MEMBER(KernelMngr);
  CCI_DESTROY_MEMBER(Kernel)
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Pyramid )
  CCI_ATTR_GET( Pyramid,Kernel  , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->Kernel.CStr())  )
  CCI_ATTR_GET( Pyramid,Factor  , CCIA_RETVAL = _This->Factor )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Pyramid )
  CCI_ATTR_GET( Pyramid,Kernel  , _This->Kernel = CCIA_TRANSLATE_DATA(char_t*))
  CCI_ATTR_SET( Pyramid,Factor  , _This->Factor = CCIA_DATA )
CCI_END_SETTER()
//---------------------------------------------------------------------
static CCI_METHOD Pyramid_Generate( CCI_INSTANCE_PTR(Pyramid) _This,
                                    dmIImageList& _IImages, dm_int limit,
                                    dmKernelFamily* family, dmImageBuffer* buffer )
{
   dmImage *src,*dst;
   dmRegion srcRgn;

   src = _IImages.GetBuffer(0);
   if(src==NULL) CCI_RETURN_INVALID_PARAM()

   dm_int steps = 1;
   if(limit <= 0) limit = 10000;

   dmPoint factors(_This->Factor,_This->Factor);
   
   // Compute the number of steps (levels)
   // in the pyramid

   dmPoint size      = src->Dimensions();
   dm_int  sizelimit = 4*_This->Factor;
   while( (size.x > sizelimit) && (size.y > sizelimit) && 
          (steps  < limit))
   {
     size.x /= factors.x;
     size.y /= factors.y; 
     steps++;
   }

   _IImages.Resize(steps);
 
   // Downsampling
     
   size  = src->Dimensions();

   // Loop over the number of steps
   for(dm_int i=1;i<steps;++i) 
   {
     size.x /= factors.x;
     size.y /= factors.y; 
     _IImages.CreateBuffer(i,src->PixelFormat(),size.x,size.y);
     
     dst = _IImages.GetBuffer(i); 
     srcRgn.SetRectRoi(src->Rect());
   
     if(dst!=NULL) 
     {
       dmBufferParameters _Params(*buffer,*src,srcRgn);
       dmDownSampling     _Filter(*family,*dst,factors);

       if(!_Filter.Apply(_Params))
          CCI_RETURN_FAIL()

       src = dst;

     } else
        CCI_RETURN_FAIL();
   }
   CCI_RETURN_OK()  
}
//---------------------------------------------------------------------
// Method: ApplyKernel
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Pyramid, Generate )
{
  CCI_INSTANCE( Pyramid )
   
  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImageBuffer* buffer = _IParams.GetImageBuffer(); 
    dmASSERT( buffer != NULL );

    dmKernelFamily* family = _This->KernelMngr.FindFamily(_This->Kernel.CStr());      
    if(family) 
    {
      // Get an image list
      dmIImageList _IImages;
      if( _IImages.QueryInterface(_IParams.GetImageList()) )
        return Pyramid_Generate(_This,_IImages,CCI_P(limit),family,buffer);
   
    } else
      CCI_SET_ERROR_MSG(
       dmString::FormatString(dmString::npos,_TXT("Cannot find kernel family %s !"),
                               _This->Kernel.CStr()).CStr());
  } else 
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: DownSample
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Pyramid, DownSample )
{
  CCI_INSTANCE( Pyramid )
   
  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImageBuffer* buffer = _IParams.GetImageBuffer(); 
    dmImage*       src    = _IParams.GetImage();

    if(src==NULL) CCI_RETURN_INVALID_PARAM()

    dmASSERT( buffer != NULL );

    dmKernelFamily* family = _This->KernelMngr.FindFamily(_This->Kernel.CStr());      
    if(family) 
    {
       // Get an image list
       dmIImageList _IImages;
       if(_IImages.QueryInterface(_IParams.GetImageList()))
       {        
          // Downsampling            
          dmPoint size  = src->Dimensions();
          dmPoint factors(_This->Factor,_This->Factor);
          dm_uint index = CCI_P(index);

          size.x /= factors.x;
          size.y /= factors.y; 
          _IImages.CreateBuffer(index,src->PixelFormat(),size.x,size.y);
            
          dmImage *dst = _IImages.GetBuffer(index); 
          
          if(dst!=NULL) 
          {
            dmRegion srcRgn;
            srcRgn.SetRectRoi(src->Rect());

            dmBufferParameters _Params(*buffer,*src,srcRgn);
            dmDownSampling     _Filter(*family,*dst,factors);

            if(_Filter.Apply(_Params))
              CCI_RETURN_OK()  
          }
      }
    } else
        CCI_SET_ERROR_MSG(
         dmString::FormatString(dmString::npos,_TXT("Cannot find kernel family %s !"),
                                _This->Kernel.CStr()).CStr());
  } else 
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Reconstruct
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Pyramid, Reconstruct )
{
  CCI_INSTANCE( Pyramid )
   
  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params))) 
  {
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmASSERT( buffer != NULL );

    dmIImageList _DstImages;

    // If we have an image list, use it
    if(CCI_P(imagelist)!=NULL) {
      if(!_DstImages.QueryInterface(CCI_P(imagelist)))
        CCI_RETURN_INVALID_PARAM()
    }


    dmKernelFamily* family = _This->KernelMngr.FindFamily(_This->Kernel.CStr());      
    if(family!=NULL) 
    {
      dmIImageList _SrcImages;
      if(_SrcImages.QueryInterface(_IParams.GetImageList()))
      {
        dmImage* src = _SrcImages.GetBuffer(CCI_P(from));
        if(src!=NULL)
        { 
          dm_uint fromIndex = CCI_P(from);
          dm_uint toIndex   = CCI_P(to);

          if(toIndex >= fromIndex )         
          {
            if(_DstImages.IsValid())
              _DstImages.StoreBuffer(toIndex,src);
            else {
              dmImage* image = _IParams.GetImage();
              if(image) image->GetCopy(*src);
              else      buffer->CreateBuffer(*src);
            }
            CCI_RETURN_OK()              
          } 
          else
          {
            dmPoint size = src->Dimensions();
            dmPoint factors(_This->Factor,_This->Factor);
            dmRegion srcRgn; // Needed in dmBufferParameters

            dmPoint endSize(size);
            for(dm_uint i=toIndex;i<fromIndex;++i) {
              endSize.x = (endSize.x - 1) * factors.x + 1;
              endSize.y = (endSize.y - 1) * factors.y + 1;
            }

             dmImage* dst = NULL;

            // Preallocate a buffer large enough to hold the full final image
            // The idea is to use that buffer as a source and a destination
            // copy will not overlap as long as dmUpSampling use his
            // parameter \a buffer as temporary

            dmRect dst_rect(0,0,endSize.x,endSize.y);

            if(_DstImages.IsValid()) {
              _DstImages.CreateBuffer(toIndex,src->PixelFormat(),endSize.x,endSize.y);
              dst = _DstImages.GetBuffer(toIndex);
            } else {
              _This->Buffer.CreateBuffer(*src->TypeDescriptor(),dst_rect);
              dst = _This->Buffer.Buffer();
            }

            if(dst == NULL ) {
              CCI_SET_ERROR_MSG(_TXT("Cannot allocate image !"));
              CCI_RETURN_FAIL(); 
            }

            // Copy the source image into dest
            dst->GetCopy(*src); 

            // Up sampling
            dmBufferParameters params(*buffer,*dst,srcRgn);
            for(;toIndex < fromIndex;++toIndex) 
            {
              // The size of the input image
              srcRgn.SetRectRoi(dmRect(0,0,size.x,size.y));

              dmUpSampling _Filter(*family,*dst,srcRgn.Rectangle(),factors);

              if(!_Filter.Apply(params))
                 CCI_RETURN_FAIL();

              size.x = (size.x - 1) * factors.x + 1;
              size.y = (size.y - 1) * factors.y + 1;
            }

            // Copy back the image if no image list was available
            if(!_DstImages.IsValid()) 
            {
              dmImage*  image = _IParams.GetImage();
              if(image) image->GetCopy(*dst,dst_rect);
              else      buffer->CreateBuffer(*dst,dst_rect);
            }

            CCI_RETURN_OK()
          } 
        } // src == NULL
      }
    } else
      CCI_SET_ERROR_MSG(
       dmString::FormatString(dmString::npos,_TXT("Cannot find kernel family %s !"),
                              _This->Kernel.CStr()).CStr());
  } else 
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Laplacian
//---------------------------------------------------------------------
bool __dmKernel _dmDoImageMath5( dm_int op, dmImage& lhs, 
                                 const dmImage& rhs , 
                                 const dmRegion&, const dmPoint& );


static CCI_IMPL_METHOD( Pyramid, Laplacian )
{
  CCI_INSTANCE( Pyramid )
   
  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImageBuffer* buffer = _IParams.GetImageBuffer(); 
    dmASSERT( buffer != NULL );

    dmKernelFamily* family = _This->KernelMngr.FindFamily(_This->Kernel.CStr());      
    if(family) 
    {
      // Get an image list
      dmIImageList _IImages;
      if( _IImages.QueryInterface(_IParams.GetImageList()) ) {
        dm_param result = Pyramid_Generate(_This,_IImages,CCI_P(limit),family,buffer);
        if(CCI_SUCCEEDED(result)) 
        {
          dmPoint  size;
          dmPoint  factors(_This->Factor,_This->Factor);
          dmRegion srcRgn; // Needed in dmBufferParameters

          // The gaussian pyramid is in our image list
          dm_uint count = _IImages.Size();
          dmImage *src,*dst,*tmp;

          dst = _IImages.GetBuffer(0);

          _This->Buffer.CreateBuffer(*dst->TypeDescriptor(),dst->Rect());
          tmp = _This->Buffer.Buffer();

          dmBufferParameters params(*buffer,*tmp,srcRgn);

          for(dm_uint i=1;i<count;++i) 
          {
            src = _IImages.GetBuffer(i);
            tmp->GetCopy(*src); 

            dmUpSampling _Filter(*family,*tmp,src->Rect(),factors);

            if(!_Filter.Apply(params))
               CCI_RETURN_FAIL();

            // Substract the result from dest                       
            _dmDoImageMath5(dmTk::Math::NSubPixels,*dst,*tmp, 
                            dst->Rect(),dst->Rect().TopLeft());
            dst = src;

          }
        }
      }
    } else
      CCI_SET_ERROR_MSG(
       dmString::FormatString(dmString::npos,_TXT("Cannot find kernel family %s !"),
                               _This->Kernel.CStr()).CStr());
  } else 
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Pyramid )
  CCI_REGISTER_METHOD_PP( Pyramid, Generate    )
  CCI_REGISTER_METHOD_PP( Pyramid, DownSample  )
  CCI_REGISTER_METHOD_PP( Pyramid, Reconstruct )
  CCI_REGISTER_METHOD_PP( Pyramid, Laplacian   )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Pyramid )
{
  CCI_INVOKE_FACTORY(Pyramid)
}
//---------------------------------------------------------------------
