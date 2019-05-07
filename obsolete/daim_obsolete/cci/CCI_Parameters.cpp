
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

/* $ChangeLog  v1.2, 27 aout 2006 02:00:54 , David
 *
 * Added CreateMask()
 */

/* $ChangeLog  v1.3, 22 sept. 2006 00:39:25 , David
 *
 * Use auto pointer dmLink<> for storing image pointer
 */

/* $ChangeLog  v1.4, 25 sept. 2006 17:57:05 , David
 *
 * Added ReleaseImageBuffer()
 */

/* $ChangeLog  v1.4, 25 sept. 2006 23:10:11 , David
 *
 * MaskToRoi now set the roi as a user roi
 */

#define dmUseCCI
#define dmUseKernelImage
#define dmUseKernelBuffer

#include "daim_kernel.h"
#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Parameters.h"
#include "daim_modules/CCI_ImageList.h"
#include "daim_modules/CCI_MaskBuffer.h"

#include "daim_modules/processing/CCI_Colorspaces.h"
#include "daim_modules/processing/CCI_RGBFunctor.h"

#define CCIC_NAME  CCIC_Parameters
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Parameters_Version
#define CCI_REVISION  3
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//-------------------------------------------------------------------
#define CCI_INTERNAL_MODULE
#include "daim_modules/CCI_Base.h"
#include "daim/daim_base.h"

#include "common/dmUserLib.h"
//-------------------------------------------------------------------

#define _NOINDEX_ dm_noindex

CCI_DECL_DATA( Parameters )
{
  dmString      url;
  dmRegion      roi;
  dmRegion*     mask;
  
  dmLink<dmImage> image;

  dmIMaskBuffer maskbuffer;
  dmIImageList  imagelist;
  dmIColorSpace colorspace;
  
  dmLink<dmImageBuffer> imagebuffer;
  dm_bool               userRoi;
  dm_uint               channel;
  
};
//--------------------------------------------------------------------
dmImage* Parameters_Image( CCI_INSTANCE_PTR(Parameters) _This, dmImage* image )
{
  if(image) 
  {
    _This->image = dmNewLink<dmImage>(image);
    _This->roi.ClipToRect(_This->image->Rect());
    
    if(_This->mask) 
       _This->mask->ClipToRect(_This->image->Rect());
  } 
  return _This->image;
}
//--------------------------------------------------------------------
void Parameters_ClearImageList( CCI_INSTANCE_PTR(Parameters) _This )
{
  if(_This->imagelist.IsValid()) {
     _This->imagelist.Release();
     _This->imagelist.Detach();
  }
}
//--------------------------------------------------------------------
cci_Object* Parameters_ImageList( CCI_INSTANCE_PTR(Parameters) _This, cci_Object* imagelist )
{
  if(imagelist) {
     Parameters_ClearImageList(_This);
     if(_This->imagelist.QueryInterface(imagelist))
        _This->imagelist.AddRef();
  }
  else if(!_This->imagelist.IsValid())
  {
    if(!_This->url.Empty()) {
      // Addref is done in DM_GetModule
      imagelist = reinterpret_cast<cci_Object*>(
                 DM_GetModule(_This->url.Get(),CCI_GET_CLSID(ImageList),TGZ_DEFAULT));
                 
      _This->imagelist.Attach(imagelist);             
    } else {
      _This->imagelist.Create();
    }
  }
  return _This->imagelist;
}
//--------------------------------------------------------------------
cci_Object* Parameters_MaskBuffer( CCI_INSTANCE_PTR(Parameters) _This, cci_Object* maskbuffer)
{
  if(maskbuffer) // Set mask buffer
  {
    // Clear mask pointer (may become invalide) 
    _This->mask = NULL;
    
     // Release current maskbuffer
    _This->maskbuffer.Release();
    _This->maskbuffer.Detach();
    
    // Assign new maskbuffer
    if(_This->maskbuffer.QueryInterface(maskbuffer))
       _This->maskbuffer.AddRef();
  }
  else if(!_This->maskbuffer.IsValid()) 
  {
    // Get mask buffer
    if(!_This->url.Empty()) {
        maskbuffer = reinterpret_cast<cci_Object*>(
          DM_GetModule(_This->url.Get(),CCI_GET_CLSID(MaskBuffer),TGZ_DEFAULT));          
       
        // AddRef is done by DM_GetModule
        _This->maskbuffer.Attach(maskbuffer);  
          
    } else {
      // Create local mask buffer
      _This->maskbuffer.Create();
    }
  }
  // Big trouble, should always return a valid maskbuffer 
  dmASSERT( _This->maskbuffer.IsValid() );
  return _This->maskbuffer;
}
//--------------------------------------------------------------------
dmImageBuffer* Parameters_ImageBuffer( CCI_INSTANCE_PTR(Parameters) _This )
{
  if(_This->imagebuffer.IsNull())
     _This->imagebuffer = dmAutoLink<dmImageBuffer>(new dmImageBuffer);

  return _This->imagebuffer;
}
//--------------------------------------------------------------------
dmRegion* Parameters_ImageRoi( CCI_INSTANCE_PTR(Parameters) _This, dmRegion* roi )
{
  if(roi) {
    _This->roi     = *roi;    // Set roi
    _This->userRoi = dm_true;
  } else {
    // Asking to return a roi
    // if our roi is empty, set it to the image rectangle
    
    // XXXDavid
    // if this is not a user ROI set it also  to the image rectangle
    // This prevent setting a roi wich is adjusted from a previous image
    
    if(!_This->image.IsNull() && (!_This->userRoi || _This->roi.IsEmptyRoi()))
        _This->roi.SetRectRoi(_This->image->Rect());
  }
  // Always return a roi that is compatible 
  // with the image parameter
  if(!_This->image.IsNull()) 
      _This->roi.ClipToRect(_This->image->Rect());

  return &_This->roi;
}
//--------------------------------------------------------------------
dmRegion* Parameters_ImageMask( CCI_INSTANCE_PTR(Parameters) _This, dmRegion* mask )
{
  if(Parameters_MaskBuffer(_This,NULL))
  {
    dmIMaskBuffer& _buffer = _This->maskbuffer;
    
    if(mask!=NULL) {
      _buffer.SetMask(mask);
      _This->mask = _buffer.DefaultMask();
      if(!_This->image.IsNull())
          _This->mask->ClipToRect(_This->image->Rect()); 
    } else
      _This->mask = _buffer.DefaultMask();
  }

  return _This->mask;
}
//--------------------------------------------------------------------
cci_Object* Parameters_ColorSpace( CCI_INSTANCE_PTR(Parameters) _This, cci_Object* colorspace )
{ 
  if(colorspace!=NULL)
  {
    // Release current colorspace
    _This->colorspace.Release();
    _This->colorspace.Detach();
                    
    if(_This->colorspace.QueryInterface(colorspace))
       _This->colorspace.AddRef();
  }
  // Create a default RGB colorspace
  else if(!_This->colorspace.IsValid()) 
  { 
    dmIColorSpaceRGB _RGBColorSpace;
    
    _RGBColorSpace.Create();
    _This->colorspace.Attach(_RGBColorSpace);
    
    _RGBColorSpace.Detach();
  }
  
  if(!_This->colorspace.IsValid())
     dmLOG("ERROR: Invalid colorspace in CCI_Parameters !!\n");
       
  return _This->colorspace;
}
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static inline CCI_CONSTRUCTOR( Parameters )
{
  CCI_CONSTRUCT_SUPER()

  CCI_INSTANCE( Parameters )
  CCI_INIT_MEMBER(url)
  CCI_INIT_MEMBER(roi)
  CCI_INIT_MEMBER(imagebuffer)
  CCI_INIT_MEMBER(image)

  CCI_INIT_MEMBER(maskbuffer)
  CCI_INIT_MEMBER(imagelist)
  CCI_INIT_MEMBER(colorspace)

  //_This->url.Assign(_TXT("daim://"));

  _This->mask     = NULL;
  _This->channel  = CCIV_Parameters_AllChannels;

  _This->userRoi  = dm_false;

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static inline CCI_DESTRUCTOR( Parameters )
{
  CCI_INSTANCE( Parameters )

  _This->image     .Release();
  _This->imagelist .Release();
  _This->maskbuffer.Release();
  _This->colorspace.Release();

  CCI_DESTROY_MEMBER(colorspace)
  CCI_DESTROY_MEMBER(imagelist)
  CCI_DESTROY_MEMBER(maskbuffer)

  CCI_DESTROY_MEMBER( url  )
  CCI_DESTROY_MEMBER( roi  )
  CCI_DESTROY_MEMBER( imagebuffer )
  CCI_DESTROY_MEMBER( image )
  
  CCI_DESTROY_SUPER()
}


//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Parameters )
  CCI_ATTR_GET(Parameters,Image       , CCIA_RETVAL = CCIA_TRANSLATE_VAL(Parameters_Image      (_This,NULL)) )
  CCI_ATTR_GET(Parameters,Mask        , CCIA_RETVAL = CCIA_TRANSLATE_VAL(Parameters_ImageMask  (_This,NULL)) )
  CCI_ATTR_GET(Parameters,Roi         , CCIA_RETVAL = CCIA_TRANSLATE_VAL(Parameters_ImageRoi   (_This,NULL)) )
  CCI_ATTR_GET(Parameters,ImageList   , CCIA_RETVAL = CCIA_TRANSLATE_VAL(Parameters_ImageList  (_This,NULL)) )
  CCI_ATTR_GET(Parameters,MaskBuffer  , CCIA_RETVAL = CCIA_TRANSLATE_VAL(Parameters_MaskBuffer (_This,NULL)) )
  CCI_ATTR_GET(Parameters,ImageBuffer , CCIA_RETVAL = CCIA_TRANSLATE_VAL(Parameters_ImageBuffer(_This)) )
  CCI_ATTR_GET(Parameters,ColorSpace  , CCIA_RETVAL = CCIA_TRANSLATE_VAL(Parameters_ColorSpace (_This,NULL)) )
  CCI_ATTR_GET(Parameters,Channel     , CCIA_RETVAL = _This->channel )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Parameters )
  CCI_ATTR_SET(Parameters,Image       ,
    if(CCIA_DATA!=0) Parameters_Image(_This,CCIA_TRANSLATE_DATA(dmImage*));
    else {
      _This->image.Release();
    }
  )
  CCI_ATTR_SET(Parameters,ImageList   , 
    if(CCIA_DATA!=dm_null) Parameters_ImageList( _This,CCIA_TRANSLATE_DATA(cci_Object*));
    else Parameters_ClearImageList(_This);
  )  
  CCI_ATTR_SET(Parameters,MaskBuffer  , Parameters_MaskBuffer(_This,CCIA_TRANSLATE_DATA(cci_Object*)))
  CCI_ATTR_SET(Parameters,Mask        , Parameters_ImageMask (_This,CCIA_TRANSLATE_DATA(dmRegion*)  ))
  CCI_ATTR_SET(Parameters,Roi         , Parameters_ImageRoi  (_This,CCIA_TRANSLATE_DATA(dmRegion*)  ))
  CCI_ATTR_SET(Parameters,ColorSpace  , Parameters_ColorSpace(_This,CCIA_TRANSLATE_DATA(cci_Object*)))
  CCI_ATTR_SET(Parameters,Channel     , _This->channel = CCIA_DATA )
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method : UseNameSpace
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Parameters, UseNameSpace )
{
  CCI_INSTANCE( Parameters )

  if(!EMPTY_STRING(CCI_P(url))) {
    _This->url.Assign(CCI_P(url));
    CCI_RETURN_OK()
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : MaskToROI
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Parameters, MaskToRoi )
{
  CCI_INSTANCE( Parameters )
  Parameters_ImageRoi(_This, Parameters_ImageMask(_This,NULL));

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method : KillROI
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Parameters, KillRoi )
{
  CCI_INSTANCE( Parameters )

  _This->roi.KillRoi();
  _This->userRoi = dm_false;
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method : HasMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Parameters, HasMask )
{
  CCI_INSTANCE( Parameters )

  if(_This->mask && !_This->mask->IsEmptyRoi()) 
    CCI_RETURN_OK()

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : CreateRoi
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Parameters, CreateRoi )
{
  CCI_INSTANCE( Parameters )

  dm_ptr p = CCI_P(points);
  if(p!=NULL) 
  {
    dmRegion roi;
    switch(CCI_P(type)) 
    {
      case dmRectangle_t: roi.SetRoi(dmRect   (*static_cast<dm_rect*>   (p))); break;
      case dmEllipse_t  : roi.SetRoi(dmEllipse(*static_cast<dm_ellipse*>(p))); break;
      case dmCircle_t   : roi.SetRoi(dmCircle (*static_cast<dm_circle*> (p))); break;
      case dmLine_t     : roi.SetRoi(dmLine   (*static_cast<dm_line*>   (p))); break;
      case dmPoly_t     : 
        if(CCI_P(count)>0) {
          dmPoly poly(static_cast<dm_point*>(p),CCI_P(count));
          roi.SetRoi(poly);
          break;
        } 
      default:
        CCI_RETURN_INVALID_PARAM()
    }
 
    if(!roi.IsEmptyRoi()) {
      Parameters_ImageRoi(_This,&roi);
      CCI_RETURN_OK()    
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: RoiToMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Parameters, RoiToMask )
{
  CCI_INSTANCE( Parameters )

  dmRegion* pmask = Parameters_ImageMask(_This,NULL);
  if(pmask) {
    *pmask = _This->roi;
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()

}
//---------------------------------------------------------------------
// Method: InvertMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Parameters, InvertMask )
{
  CCI_INSTANCE( Parameters )

  dmRegion* pmask = Parameters_ImageMask(_This,NULL);
  if(pmask && !_This->image.IsNull()) {
     pmask->XorCoordinates(_This->image->Rect());
     CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : StoreBuffer
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Parameters, StoreBuffer )
{
  CCI_INSTANCE( Parameters )

  if(!_This->image.IsNull()) 
  {
    dmIImageList _ImageList;
    if(_ImageList.QueryInterface(Parameters_ImageList(_This,NULL)) &&
       _ImageList.StoreBuffer(CCI_P(index),_This->image))

      CCI_RETURN_OK()    
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : SelectBuffer
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Parameters, SelectBuffer )
{
  CCI_INSTANCE( Parameters )

  if(CCI_P(index) == _NOINDEX_)
  {
    // Try to get image from info
    _This->image.Release();
    if(Parameters_Image(_This,NULL)!=NULL)
       CCI_RETURN_OK();
  } 
  else 
  {
    dmIImageList _ImageList;
    if(_ImageList.QueryInterface(Parameters_ImageList(_This,NULL)))
    {
      dmLink<dmImage> image; 
      _ImageList.GetBufferA(CCI_P(index),image);
      if(!image.IsNull()) 
      {
        _This->image = image;
        _This->roi.ClipToRect(_This->image->Rect());
    
        if(_This->mask) 
           _This->mask->ClipToRect(_This->image->Rect());

        CCI_RETURN_OK();
      } else 
        _This->image.Release();
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : Fill
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Parameters, Fill )
{
  CCI_INSTANCE( Parameters )

  if(!_This->image.IsNull()) {
    dmRegion* roi = Parameters_ImageRoi(_This,NULL);
    _This->image->FillArea(*roi,CCI_P(foreground)!=dm_false);
    CCI_RETURN_OK()    
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : StoreMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Parameters, StoreMask )
{
  CCI_INSTANCE( Parameters )

  dmIMaskBuffer _buffer;
  if(_buffer.QueryInterface(Parameters_MaskBuffer(_This,NULL)) &&
     _buffer.StoreMask(NULL,CCI_P(index),CCI_P(op)))
       CCI_RETURN_OK()

  CCI_RETURN_FAIL()  
}
//---------------------------------------------------------------------
// Method : CallMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Parameters, CallMask )
{
  CCI_INSTANCE( Parameters )

  dmIMaskBuffer _buffer;
  if(_buffer.QueryInterface(Parameters_MaskBuffer(_This,NULL)) &&
     _buffer.CallMask(NULL,CCI_P(index),CCI_P(op))) {

     CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()  
}
//---------------------------------------------------------------------
// Method : ClearMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Parameters, ClearMask )
{
  CCI_INSTANCE( Parameters )

  dmIMaskBuffer _buffer;
  if(_buffer.QueryInterface(Parameters_MaskBuffer(_This,NULL)) &&
     _buffer.ClearMask(CCI_P(index)))
       CCI_RETURN_OK()

  CCI_RETURN_FAIL()  
}
//---------------------------------------------------------------------
// Method : OutlineROI
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Parameters, OutlineRoi )
{
  CCI_INSTANCE( Parameters )

  dmRegion outline;
  _This->roi.GetOutLine(outline);
  _This->roi = outline;
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method : StoreSelection
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Parameters, StoreSelection )
{
  CCI_INSTANCE( Parameters )

  if(!_This->image.IsNull()) 
  {
    dmRegion* thisRoi = Parameters_ImageRoi(_This,NULL);
    if(thisRoi) 
    {
      dm_rect rect = thisRoi->Rectangle();

      dmIImageList _ImageList;
      if(_ImageList.QueryInterface(Parameters_ImageList(_This,NULL)) &&
         _ImageList.StoreBuffer(CCI_P(index),_This->image,&rect))

      CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : PushMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Parameters, PushMask )
{
  CCI_INSTANCE( Parameters )

  if(Parameters_MaskBuffer(_This,NULL) &&
     _This->maskbuffer.PushMask(NULL))
       CCI_RETURN_OK()

  CCI_RETURN_FAIL()  
}
//---------------------------------------------------------------------
// Method : QueueMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Parameters, QueueMask )
{
  CCI_INSTANCE( Parameters )

  if(Parameters_MaskBuffer(_This,NULL) &&
     _This->maskbuffer.QueueMask(NULL))
       CCI_RETURN_OK()

  CCI_RETURN_FAIL()  
}
//---------------------------------------------------------------------
// Method : PopMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Parameters, PopMask )
{
  CCI_INSTANCE( Parameters )

  if(Parameters_MaskBuffer(_This,NULL) &&
     _This->maskbuffer.PopMask())
       CCI_RETURN_OK()

  CCI_RETURN_FAIL()  
}
//---------------------------------------------------------------------
// Method : SwapMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Parameters, SwapMask )
{
  CCI_INSTANCE( Parameters )

  if(Parameters_MaskBuffer(_This,NULL) &&
     _This->maskbuffer.SwapMask(CCI_P(index)))
       CCI_RETURN_OK()

  CCI_RETURN_FAIL()  
}
//---------------------------------------------------------------------
// Method : PopMaskOp
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Parameters, PopMaskOp )
{
  CCI_INSTANCE( Parameters )

  if(Parameters_MaskBuffer(_This,NULL) &&
     _This->maskbuffer.PopMaskOp(CCI_P(op)))
       CCI_RETURN_OK()

  CCI_RETURN_FAIL()  
}
//---------------------------------------------------------------------
// Method : RotateMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Parameters, RotateMask )
{
  CCI_INSTANCE( Parameters )

  if(Parameters_MaskBuffer(_This,NULL) &&
     _This->maskbuffer.RotateMask(NULL))
       CCI_RETURN_OK()

  CCI_RETURN_FAIL()  
}
//---------------------------------------------------------------------
// Method : GetImageSize
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Parameters, GetImageSize )
{
  CCI_INSTANCE( Parameters )

  dm_int       _Width  = CCI_P(width);
  dm_int       _Height = CCI_P(height);
  EPixelFormat _Format = static_cast<EPixelFormat>(CCI_P(format));

  if(_Width < 0 || _Height < 0 || _Format==0 ) 
  {
    if(!_This->image.IsNull()) 
    {
      dmRegion* roi = Parameters_ImageRoi(_This,NULL);
      if(_Width  <  0) _Width  = roi->Rectangle().Width();
      if(_Height <  0) _Height = roi->Rectangle().Height();
      if(_Format == 0) _Format = _This->image->PixelFormat();
    } else
      CCI_RETURN_FAIL()
  }

  dm_uint _Bytes = dmGetPixelFormatBits(_Format) >> 3;

  CCI_RETVAL_P(size) = dm_align32( _Width * _Bytes ) * _Height;

  CCI_RETURN_OK()  
}
//---------------------------------------------------------------------
// Method : CreateImageBuffer
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Parameters, CreateImageBuffer )
{
  CCI_INSTANCE( Parameters )

  if(!_This->image.IsNull())
  {
    dmImageBuffer* Buffer = Parameters_ImageBuffer(_This);
    dmRegion*      roi    = Parameters_ImageRoi(_This,NULL);

    if(CCI_P(padding) > 0) 
    {
      dmMaskDescription  mask(CCI_P(padding),CCI_P(padding));

      Buffer->CreateBuffer(*_This->image,*roi,mask);
      Buffer->SetBufferLimit(_This->image->Rect(),
             static_cast<dmEBndryLimitType>(CCI_P(padding_mode)));
    } else 
      Buffer->CreateBuffer(*_This->image,*roi);

    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()  
}
//---------------------------------------------------------------------
// Method : RestoreImageBuffer
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Parameters, RestoreImageBuffer )
{
  CCI_INSTANCE( Parameters )

  if(!_This->image.IsNull())
  {
    dmImageBuffer* Buffer = Parameters_ImageBuffer(_This);
    Buffer->BufferToImage(*_This->image);

    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : ClearImageBuffer
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Parameters, ClearImageBuffer )
{
  CCI_INSTANCE( Parameters )

  dmImageBuffer* Buffer = Parameters_ImageBuffer(_This);
  if(Buffer) {
    Buffer->Clear();
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : ReleaseImageBuffer
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Parameters, ReleaseImageBuffer )
{
  CCI_INSTANCE( Parameters )

  dmImageBuffer* Buffer = Parameters_ImageBuffer(_This);
  if(Buffer) {
    Buffer->ReleaseBuffer();
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : KillMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Parameters, KillMask )
{
  CCI_INSTANCE( Parameters )
  
  if(Parameters_MaskBuffer(_This,NULL) &&
     _This->maskbuffer.KillMask())
       CCI_RETURN_OK()

  CCI_RETURN_FAIL()
}
//------------------------------------------------------------------------

#define CHECK_ERROR( err, ob, me ) {\
    const char_t* txt = dmColorSpaceFunctorHelper::GetErrorText(err);\
    if(txt) CCI_SET_ERROR(ob,me,txt);                                \
  }

#define CAST_ERROR( err ) static_cast<dm_uint>(err)

#define RETURN_ERROR_CHECK(err)                          \
        if(err!=dmFunctorErrors::E_NO_ERROR) {           \
          CHECK_ERROR(err,_THIS_OBJECT,CCI_P(Id));       \
          CCI_RETURN_FAIL()                              \
        }                                                \
        CCI_RETURN_OK()

//---------------------------------------------------------------------
// Method: FillChannel    
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Parameters, FillChannel )
{
  CCI_INSTANCE(Parameters)

  dm_uint _LastError = CAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmImage*  img = Parameters_Image(_This,NULL); 

  if(img!=NULL) 
  {  
    dmImageBuffer* buffer  = Parameters_ImageBuffer(_This);
    dmRegion*      rgn     = Parameters_ImageRoi(_This,NULL);

    dmFillScalar _Filter(CCI_P(value),CCI_P(alpha));

    dmIParameters _IParams;
    _IParams.Attach(_THIS_OBJECT);

    dmColorSpaceFunctor<dmFillScalar> _Functor(_Filter);
    if(!_Functor.Apply(_IParams,buffer,img,rgn,true)) {
        _LastError = CAST_ERROR(_Functor.ReturnedError);
    }

  } else 
    _LastError = CAST_ERROR(dmFunctorErrors::E_NO_IMAGE);

  RETURN_ERROR_CHECK(_LastError)
}
//---------------------------------------------------------------------
// Method: InvertPixels   
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Parameters, InvertPixels )
{
  CCI_INSTANCE(Parameters)

  dm_uint _LastError = CAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmImage* img = Parameters_Image(_This,NULL); 

  if(img!=NULL) 
  {  
    dmImageBuffer* buffer  = Parameters_ImageBuffer(_This);
    dmRegion*      rgn     = Parameters_ImageRoi(_This,NULL);

    dmIParameters _IParams;
    _IParams.Attach(_THIS_OBJECT);

    dmInvertMap _Filter(CCI_P(minrange),CCI_P(maxrange));

    dmColorSpaceFunctor<dmInvertMap> _Functor(_Filter);
    if(!_Functor.Apply(_IParams,buffer,img,rgn,true))
        _LastError = _Functor.ReturnedError;
        
  } else 
    _LastError = CAST_ERROR(dmFunctorErrors::E_NO_IMAGE);

  RETURN_ERROR_CHECK(_LastError)
} 
//---------------------------------------------------------------------
// Method: InvertRoi   
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Parameters, InvertRoi )
{
  CCI_INSTANCE(Parameters)

  if(!_This->image.IsNull()) 
  {
    dmRegion& roi  = _This->roi;
    dmRect rect = _This->image->Rect();
    roi.XorCoordinates(rect);

    CCI_RETURN_OK()
  }
  
  CCI_RETURN_FAIL()
} 
//---------------------------------------------------------------------
// Method: GetImageData
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Parameters, GetImageData )
{
  CCI_INSTANCE( Parameters )
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
// Method: CreateMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Parameters, CreateMask )
{
  CCI_INSTANCE( Parameters )
  
  dmImage *image = _This->image;
  
  if(image)
  {
    if(Parameters_MaskBuffer(_This,NULL))
    {      
      dmRegion  mask;
      dmRegion* roi = Parameters_ImageRoi(_This,NULL); 

      if(!dmIsPixelFormatScalar(image->PixelFormat())) 
      {
        dmImageBuffer* buffer = Parameters_ImageBuffer(_This);

        if(_This->channel == CCIV_Parameters_AllChannels) 
        {
           // Convert image to grayscale in buffer
        	 dmImageDescriptor* _Descriptor = dmImageDescriptor::GetDescriptor( dmPixelFormat8bppIndexed );
        	 dmASSERT( _Descriptor != dm_null );
           buffer->CreateBuffer(*image,*_Descriptor,*roi);
        } 
        else 
        {
        	// Extract the corresponding channel
        	Parameters_ColorSpace(_This,NULL); 
          if(!_This->colorspace.Extract(_THIS_OBJECT,_This->channel,-1))
             CCI_RETURN_FAIL()
          
          if(dmCreateRegionRoi(*buffer->Buffer(),mask, 
                                CCI_P(predicat),CCI_P(value),
                                buffer->BufferRgn())) 
          {
          	mask.Translate(buffer->BufferSrc());
          	goto _Success;
          }
        }           
      } else {
        if(dmCreateRegionRoi(*image,mask, 
                              CCI_P(predicat),CCI_P(value),
                              *roi))
        {
          goto _Success;
        }
        
       CCI_RETURN_FAIL()
        
       _Success:
         if(CCI_P(index)!=dm_noindex)
           _This->maskbuffer.StoreMask(&mask,CCI_P(index),CCI_P(op));
         else
           _This->maskbuffer.SetMask(&mask,CCI_P(op));

         CCI_RETURN_OK()
      }
    }
  }
  CCI_RETURN_FAIL()
}  
//---------------------------------------------------------------------
// Method: GetRectSelection
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Parameters, GetRectSelection )
{
  CCI_INSTANCE( Parameters )
 
  dmRegion* roi = Parameters_ImageRoi(_This,NULL); 
  if(roi)
  {
  	const dmRect& _Rect = roi->Rectangle();
    CCI_P(left)   = _Rect.Left();
    CCI_P(top)    = _Rect.Top();
    CCI_P(right)  = _Rect.Right();
    CCI_P(bottom) = _Rect.Bottom();
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: FillRGB  
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Parameters, FillRGB )
{
  CCI_INSTANCE(Parameters)

  dm_uint _LastError = CAST_ERROR(dmFunctorErrors::E_NO_ERROR);
  
  dmImage*  img = Parameters_Image(_This,NULL); 

  if(img!=NULL) 
  {  
    dmRegion* rgn = Parameters_ImageRoi(_This,NULL);

    if(dmFillRGB::SetValues( *img,*rgn,
                             CCI_P(red),CCI_P(green),CCI_P(blue),
                             CCI_P(alpha))) {

       CCI_RETURN_OK()
    }
  } else 
    _LastError = CAST_ERROR(dmFunctorErrors::E_NO_IMAGE);

  RETURN_ERROR_CHECK(_LastError)
}
//---------------------------------------------------------------------
// Method : GetImageA
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Parameters, GetImageA )
{
  CCI_INSTANCE( Parameters )

  if( CCI_P(link)!=NULL )
  {
    switch(CCI_P(me))
    {
      case CCI_ME_GET:
        *CCI_P(link) = _This->image;
         CCI_RETURN_OK()

      case CCI_ME_SET:
        _This->image = *CCI_P(link);
        CCI_RETURN_OK()
    }
  }
  
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Parameters )

  CCI_REGISTER_METHOD_PP( Parameters,  UseNameSpace   )
  CCI_REGISTER_METHOD_NP( Parameters,  MaskToRoi      )
  CCI_REGISTER_METHOD_NP( Parameters,  KillRoi        )
  CCI_REGISTER_METHOD_NP( Parameters,  HasMask        )
  CCI_REGISTER_METHOD_PP( Parameters,  CreateRoi      )
  CCI_REGISTER_METHOD_PP( Parameters,  StoreBuffer    )
  CCI_REGISTER_METHOD_PP( Parameters,  SelectBuffer   )
  CCI_REGISTER_METHOD_NP( Parameters,  RoiToMask      )
  CCI_REGISTER_METHOD_NP( Parameters,  InvertMask     )
  CCI_REGISTER_METHOD_PP( Parameters,  Fill           )
  CCI_REGISTER_METHOD_PP( Parameters,  StoreMask      )
  CCI_REGISTER_METHOD_PP( Parameters,  CallMask       )
  CCI_REGISTER_METHOD_PP( Parameters,  ClearMask      )
  CCI_REGISTER_METHOD_NP( Parameters,  OutlineRoi     )
  CCI_REGISTER_METHOD_PP( Parameters,  StoreSelection )

  CCI_REGISTER_METHOD_NP( Parameters,  PushMask       )
  CCI_REGISTER_METHOD_NP( Parameters,  QueueMask      )
  CCI_REGISTER_METHOD_NP( Parameters,  PopMask        )
  CCI_REGISTER_METHOD_PP( Parameters,  SwapMask       )
  CCI_REGISTER_METHOD_PP( Parameters,  PopMaskOp      )
  CCI_REGISTER_METHOD_NP( Parameters,  RotateMask     )

  CCI_REGISTER_METHOD_PP( Parameters,  GetImageSize   )

  CCI_REGISTER_METHOD_PP( Parameters,  CreateImageBuffer  )
  CCI_REGISTER_METHOD_NP( Parameters,  RestoreImageBuffer )
  CCI_REGISTER_METHOD_NP( Parameters,  ClearImageBuffer   )
  CCI_REGISTER_METHOD_NP( Parameters,  ReleaseImageBuffer )
  
  CCI_REGISTER_METHOD_NP( Parameters,  KillMask           )

  CCI_REGISTER_METHOD_PP( Parameters,  FillChannel  )
  CCI_REGISTER_METHOD_PP( Parameters,  InvertPixels )
  CCI_REGISTER_METHOD_NP( Parameters,  InvertRoi    )

  CCI_REGISTER_METHOD_PP( Parameters,  GetImageData    )
  CCI_REGISTER_METHOD_PP( Parameters,  CreateMask      )
  CCI_REGISTER_METHOD_PP( Parameters,  GetRectSelection)

  CCI_REGISTER_METHOD_PP( Parameters,  FillRGB  )

  CCI_REGISTER_METHOD_PP( Parameters,  GetImageA  )

CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Parameters )
{
  CCI_INVOKE_FACTORY(Parameters);
}
//---------------------------------------------------------------------
