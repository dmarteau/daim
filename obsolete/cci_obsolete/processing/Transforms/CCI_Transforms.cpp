
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

/* $ChangeLog  v1.1, 25 août 2006 00:54:39 , David
 *
 * Added Crop, Motif, Flip, Rotate90
 */

#define dmUseCCI
#define dmUseUtilitiesExtra
#define dmUseKernelBuffer
#define dmUseKernelImage
#include "daim_kernel.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Parameters.h"
#include "daim_modules/CCI_ImageList.h"

#include "daim_modules/processing/CCI_Transforms.h"

//------------------------------------------------------------------
#define CCIC_NAME CCIC_Transforms
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Transforms_Version
#define CCI_REVISION  1
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2005"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "daim_modules/CCI_Interface.h"

#include "common/dmUserLib.h"

//--------------------------------------------------------------------
CCI_DECL_DATA( Transforms )
{
  dm_uint dummy;
};
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( Transforms )
{
  CCI_CONSTRUCT_SUPER()
  CCI_UNUSUED_INSTANCE( Transforms)

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( Transforms )
{
  CCI_UNUSUED_INSTANCE( Transforms )

  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( Transforms )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( Transforms )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method: Strech
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Transforms, Stretch )
{
  CCI_UNUSUED_INSTANCE( Transforms )

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    dmRect _Rect;

    if(img==NULL) {
       img  = buffer->Buffer();
      _Rect = buffer->BufferRect();
    } else
      _Rect = rgn->Rectangle();
    
    if(img) 
    {   
      dmIImageList _ImageList;
      if(_ImageList.QueryInterface(_IParams.GetImageList()))
      {
        dmImage* dst =  _ImageList.GetBuffer(CCI_P(index));
        if(dst != NULL)
        {
          dmInterpolationType _Mode = static_cast<dmInterpolationType>(CCI_P(mode));

          if(dmLinearStretch(_Mode,*img,_Rect,*dst))
             CCI_RETURN_OK()
                    
        } else 
          CCI_SET_ERROR_MSG(_TXT("Invalid buffer !"));
      }
    } else 
      CCI_SET_ERROR_MSG(_TXT("No image !"));
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method: Rotate
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Transforms, Rotate )
{
  CCI_UNUSUED_INSTANCE( Transforms )

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    dmRect _Rect;

    if(img==NULL) {
       img  = buffer->Buffer();
      _Rect = buffer->BufferRect();
    } else
      _Rect = rgn->Rectangle();
    
    if(img) 
    {   
      dmIImageList _ImageList;
      if(_ImageList.QueryInterface(_IParams.GetImageList()))
      {
        dmImage* dst =  _ImageList.GetBuffer(CCI_P(index));
        if(dst != NULL)
        {
          if(dmRotateImage(CCI_P(angle),*img,_Rect,*dst))
             CCI_RETURN_OK()
                    
        } else 
          CCI_SET_ERROR_MSG(_TXT("Invalid buffer !"));
      }
    } else 
      CCI_SET_ERROR_MSG(_TXT("No image !"));
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method: Crop
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Transforms, Crop )
{
  CCI_UNUSUED_INSTANCE( Transforms )

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage* img = _IParams.GetImage();
    
    dmRect   _Rect;

    if(img==NULL) {      // Get Image from buffer
       dmImageBuffer* buffer = _IParams.GetImageBuffer();
       img  = buffer->Buffer();
      _Rect = buffer->BufferRect();
    } else
      _Rect = img->Rect();
    
    if(img) 
    {   
      dmIImageList _ImageList;
      if(_ImageList.QueryInterface(_IParams.GetImageList()))
      {
        dmImage* dst =  _ImageList.GetBuffer(CCI_P(index));
        if(dst != NULL)
        {
          _Rect.Resize(-CCI_P(left),-CCI_P(right),-CCI_P(top),-CCI_P(bottom));
          if(!_Rect.IsEmpty())
          {
            dst->GetCopy( *img, _Rect );           
            CCI_RETURN_OK()
          }
        } else 
          CCI_SET_ERROR_MSG(_TXT("Invalid buffer !"));
      }
    } else 
      CCI_SET_ERROR_MSG(_TXT("No image !"));
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method: Mosaic
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Transforms, Motif )
{
  CCI_UNUSUED_INSTANCE( Transforms )

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*  img = _IParams.GetImage();
    
    dmRect   _Rect;

    if(img==NULL) {      // Get Image from buffer
       dmImageBuffer* buffer = _IParams.GetImageBuffer();
       img  = buffer->Buffer();
      _Rect = buffer->BufferRect();
    } else
      _Rect = img->Rect();
    
    if(img) 
    {   
      dmIImageList _ImageList;
      if(_ImageList.QueryInterface(_IParams.GetImageList()))
      {
        dmImage* dst =  _ImageList.GetBuffer(CCI_P(index));
        if(dst != NULL)
        {
          dmRect _SrcRect(CCI_P(left),CCI_P(right),CCI_P(top),CCI_P(bottom));
          
          if(CCI_P(periodic)) 
            dmMakePeriodic( *img, _SrcRect,_Rect);          
          else
            dmCreateMotif( *img, _SrcRect,_Rect);     
         
          CCI_RETURN_OK()
 
        } else 
          CCI_SET_ERROR_MSG(_TXT("Invalid buffer !"));
      }
    } else 
      CCI_SET_ERROR_MSG(_TXT("No image !"));
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method: Flip
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Transforms, Flip )
{
  CCI_UNUSUED_INSTANCE( Transforms )

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*   img = _IParams.GetImage();
    dmRegion*  roi = _IParams.GetRoi();
    
    dmRect   _Rect;

    if(img==NULL) {      // Get Image from buffer
       dmImageBuffer* buffer = _IParams.GetImageBuffer();
       img  = buffer->Buffer();
      _Rect = buffer->BufferRect();
    } else
      _Rect = roi->Rectangle();
    
    if(img) 
    {   
      dmIImageList _ImageList;
      if(_ImageList.QueryInterface(_IParams.GetImageList()))
      {
        if(_ImageList.CreateBuffer(CCI_P(index),
                                   img->PixelFormat(), 
                                   _Rect.Width(),
                                   _Rect.Height()))
        {                        
          dmImage* dst = _ImageList.GetBuffer(CCI_P(index));
          if(dst != NULL)
          {          
            if(dmFlipCopy(*img,*dst,_Rect,dmPoint(0,0),CCI_P(way)))                    
               CCI_RETURN_OK()
 
            CCI_RETURN_FAIL()
 
          } else 
            CCI_SET_ERROR_MSG(_TXT("Invalid buffer !"));
        } else 
          CCI_SET_ERROR_MSG(_TXT("Cannot create buffer !"));
      }
    } else 
      CCI_SET_ERROR_MSG(_TXT("No image !"));
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method: Rotate90
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Transforms, Rotate90 )
{
  CCI_UNUSUED_INSTANCE( Transforms )

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*   img = _IParams.GetImage();
    dmRegion*  roi = _IParams.GetRoi();
    
    dmRect   _Rect;

    if(img==NULL) {      // Get Image from buffer
       dmImageBuffer* buffer = _IParams.GetImageBuffer();
       img  = buffer->Buffer();
      _Rect = buffer->BufferRect();
    } else
      _Rect = roi->Rectangle();
    
    if(img) 
    {
      dmIImageList _ImageList;
      if(_ImageList.QueryInterface(_IParams.GetImageList()))
      {
        if(_ImageList.CreateBuffer(CCI_P(index),
                                   img->PixelFormat(), 
                                   _Rect.Height(),
                                   _Rect.Width()))
        {                        
          dmImage* dst = _ImageList.GetBuffer(CCI_P(index));
          if(dst != NULL)
          {
            if(dmRotateCopy(*img,*dst,_Rect,dmPoint(0,0),
                               CCI_P(clockwise)?dmTk::RotateRight:
                                                dmTk::RotateLeft))                    
                CCI_RETURN_OK()
 
            CCI_RETURN_FAIL()

          } else 
            CCI_SET_ERROR_MSG(_TXT("Invalid buffer !"));
        } else 
          CCI_SET_ERROR_MSG(_TXT("Cannot create buffer !"));
      }
    } else 
      CCI_SET_ERROR_MSG(_TXT("No image !"));
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Transforms )
    CCI_REGISTER_METHOD_PP( Transforms, Stretch  )
    CCI_REGISTER_METHOD_PP( Transforms, Rotate   )
    CCI_REGISTER_METHOD_PP( Transforms, Crop     )
    CCI_REGISTER_METHOD_PP( Transforms, Motif    )
    CCI_REGISTER_METHOD_PP( Transforms, Flip     )
    CCI_REGISTER_METHOD_PP( Transforms, Rotate90 )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Transforms )
{
  CCI_INVOKE_FACTORY(Transforms)
}
//---------------------------------------------------------------------
