
/* :::BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2006 David Marteau
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
#define dmUseKernelImage
#define dmUseKernelBuffer
#include "daim_kernel.h"
#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Parameters.h"
#include "daim_modules/CCI_ImageHandle.h"

#define CCIC_NAME  CCIC_ImageHandle
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_ImageHandle_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2006"))
//-------------------------------------------------------------------
#define CCI_INTERNAL_MODULE

#include "daim_modules/CCI_Base.h"
//-----------------------------------------
CCI_DECL_DATA( ImageHandle )
{
  dmLink<dmImage> image;
  dm_uint         width;
  dm_uint         height;
  dm_uint         format;
  dm_uint         bands;
};
//--------------------------------------------------------------------
typedef CCI_INSTANCE_PTR(ImageHandle) _Instance_Ptr;

static dm_param ImageHandle_Init( _Instance_Ptr _This, dmLink<dmImage>& target )
{
  if(!_This->image.IsNull()) {
       dmLOG("CCI:ERROR: Handle already initialized !");
       CCI_RETURN_FAIL()
  }

  _This->image = target;
  if(!_This->image.IsNull()) 
  {
    _This->width  =  _This->image->Width(); 
    _This->height =  _This->image->Height(); 
    _This->format =  _This->image->PixelFormat(); 
    switch(_This->image->PixelFormat())
    {
       case dmPixelFormat24bppRGB :  _This->bands = 3; break;
       default:                      _This->bands = 1; break;
    }
  }
  CCI_RETURN_OK()
}
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static inline CCI_CONSTRUCTOR( ImageHandle )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( ImageHandle )
  
  _This->width  = 0;
  _This->height = 0;
  _This->format = 0;
  _This->bands  = 0;
  
  CCI_INIT_MEMBER(image);  
  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static inline CCI_DESTRUCTOR( ImageHandle )
{
  CCI_INSTANCE( ImageHandle )
  CCI_DESTROY_MEMBER(image);
  CCI_DESTROY_SUPER()  
}
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( ImageHandle )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( ImageHandle )
{
  CCI_RETURN_INVOKE_SUPER()
}
*/

//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( ImageHandle )
  CCI_ATTR_GET(ImageHandle, Width , CCIA_RETVAL = _This->width  )
  CCI_ATTR_GET(ImageHandle, Height, CCIA_RETVAL = _This->height )
  CCI_ATTR_GET(ImageHandle, Bands , CCIA_RETVAL = _This->format )
  CCI_ATTR_GET(ImageHandle, Format, CCIA_RETVAL = _This->format )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( ImageHandle )
  CCIA_DISCARD(ImageHandle, Width)
  CCIA_DISCARD(ImageHandle, Height)
  CCIA_DISCARD(ImageHandle, Bands)
  CCIA_DISCARD(ImageHandle, Format)
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method: Init
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageHandle, Init )
{
  CCI_INSTANCE( ImageHandle )
  
  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmLink<dmImage> target;
    if(_IParams.GetImageA(target)) 
       return ImageHandle_Init(_This,target);
  }
  
  CCI_RETURN_INVALID_PARAM()  
}
//---------------------------------------------------------------------
// Method: GetImageData
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageHandle, GetImageData )
{
  CCI_INSTANCE( ImageHandle )
  if(!_This->image.IsNull()) 
  {
    dmImageData imagedata;
    if(_This->image->GetImageData(imagedata)) {
      CCI_P(format) = imagedata.PixelFormat;
      CCI_P(width)  = imagedata.Width;
      CCI_P(height) = imagedata.Height;
      CCI_P(stride) = imagedata.Stride; 
      CCI_P(scan0)  = imagedata.Scan0; 
      CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL()
} 
//---------------------------------------------------------------------
// Method : GetImageA
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageHandle, GetImageA )
{
  CCI_INSTANCE( ImageHandle )

  if( CCI_P(link)!=NULL )
  {
    switch(CCI_P(me))
    {
      case CCI_ME_GET:
        *CCI_P(link) = _This->image;
         CCI_RETURN_OK()

      case CCI_ME_SET:
        return ImageHandle_Init(_This,*CCI_P(link));
    }
  }
  
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( ImageHandle )
  CCI_REGISTER_METHOD_PP( ImageHandle,  Init         )
  CCI_REGISTER_METHOD_PP( ImageHandle,  GetImageData )
  CCI_REGISTER_METHOD_PP( ImageHandle,  GetImageA    )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( ImageHandle )
{
  CCI_INVOKE_FACTORY(ImageHandle);
}
//---------------------------------------------------------------------
