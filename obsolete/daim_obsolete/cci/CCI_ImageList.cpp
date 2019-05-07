
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

/* $ChangeLog  v1.1, 31 août 2006 16:16:27 , David
 *
 * Removed duplicate allocation in CreateBuffer !
 */

/* $ChangeLog  v1.2, 15 sept. 2006 10:43:56 , David
 *
 * Catch exceptions in 'CreateBuffer'
 */

#define dmUseCCI
#define dmUseKernelImage
#include "daim_kernel.h"
#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_ImageList.h"

#define CCIC_NAME  CCIC_ImageList
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_ImageList_Version
#define CCI_REVISION  3
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))

//-------------------------------------------------------------------
#define CCI_INTERNAL_MODULE
#include "daim_modules/CCI_Base.h"
//-------------------------------------------------------------------
#define BUFFER_DFLT_SIZE 16
#define BUFFER_MAX_SIZE  4096
#define CHECK_SIZE(pos) (pos < _This->buffers.size())

#define _NOINDEX_ dm_noindex

CCI_DECL_DATA( ImageList )
{
  std::vector< dmLink<dmImage> > buffers;
};
//--------------------------------------------------------------------
static bool ImageList_StoreBuffer( CCI_INSTANCE_PTR(ImageList) _This, 
                                   dm_uint index, 
                                   const dmImage* image,
                                   const dm_rect* clip_rect,
                                   EPixelFormat format )
{
  dmDEBUG_ASSERT( image != NULL     );

  dmRect rect(image->Rect());

  if(clip_rect) 
    rect.Clip(*clip_rect);

  dmImageDescriptor* _Desc = NULL;
  if(format!=dmPixelFormatUndefined)
    _Desc = dmGetDescriptor(format);

  if(_Desc == NULL)
     _Desc = image->TypeDescriptor();

  if(!rect.IsEmpty()) 
  {
    if(index!=_NOINDEX_) {
      dmLink<dmImage>& _buff = _This->buffers[index];
      if( _buff.IsNull()  == false  && 
          _buff.Pointer() != image  && // prevent copy onto itself
          _buff->PixelFormat()==_Desc->PixelFormat() &&
          _buff->Width()   == rect.Width()  &&
          _buff->Height()  == rect.Height()  ) 

       _buff->GetCopy(*image,rect); 

      else 
       _This->buffers[index] = image->CreateCopy(*_Desc,rect);
    } else 
       _This->buffers.push_back(image->CreateCopy(*_Desc,rect));
    
    return true;
  }
  return false; 
}
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static inline CCI_CONSTRUCTOR( ImageList )
{
  CCI_CONSTRUCT_SUPER()

  CCI_INSTANCE( ImageList )
  CCI_INIT_MEMBER(buffers)

  _This->buffers.resize(BUFFER_DFLT_SIZE);

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static inline CCI_DESTRUCTOR( ImageList )
{
  CCI_INSTANCE( ImageList )
  CCI_DESTROY_MEMBER( buffers )
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( ImageList )
  CCI_ATTR_GET(ImageList,Size  , CCIA_RETVAL = _This->buffers.size() )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( ImageList )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method : StoreBuffer
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageList, StoreBuffer )
{
  CCI_INSTANCE( ImageList )

  const dmImage* image = CCI_P(image);
  if(image!=NULL && (CCI_P(index)==_NOINDEX_ || CHECK_SIZE(CCI_P(index)) )) 
  {
    if(ImageList_StoreBuffer(_This,CCI_P(index),image,CCI_P(rect),
                             static_cast<EPixelFormat>(CCI_P(format))))
       CCI_RETURN_OK()
    else
      CCI_RETURN_FAIL()
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : IsEmpty
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageList, IsEmpty )
{
  CCI_INSTANCE( ImageList )

  if( CHECK_SIZE(CCI_P(index)) ) {
    CCI_RETVAL_P(empty) = _BOOL(!_This->buffers[CCI_P(index)].IsNull());
    CCI_RETURN_OK()
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : ReleaseBuffer
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageList, ReleaseBuffer )
{
  CCI_INSTANCE( ImageList )

  if(CHECK_SIZE(CCI_P(index)) ) {
    _This->buffers[CCI_P(index)].Release();
    CCI_RETURN_OK()
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : GetBuffer
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageList, GetBuffer )
{
  CCI_INSTANCE( ImageList )

  CCI_RETVAL_P(image) = NULL;
  if(CHECK_SIZE(CCI_P(index)) ) {

    CCI_RETVAL_P(image) = _This->buffers[CCI_P(index)];
    CCI_RETURN_OK()
  }
  
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : Resize
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageList, Resize )
{
  CCI_INSTANCE( ImageList )

  if(CCI_P(size) < BUFFER_MAX_SIZE) 
  {
    dm_uint size = CCI_P(size);
    if(size<1) size = 1;
    if(size != _This->buffers.size())
      _This->buffers.resize(size);

    CCI_RETURN_OK()
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : CreateBuffer
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageList, CreateBuffer )
{
  CCI_INSTANCE( ImageList )

  if(CHECK_SIZE(CCI_P(index)) ) 
  {
   dmImageDescriptor* pDesc = dmGetDescriptor(static_cast<EPixelFormat>(CCI_P(format)));
   if(pDesc) 
   {
     // Check if we can reuse a buffer
     // This buffer should not been held by someone else
     dmLink<dmImage> _buff = _This->buffers[CCI_P(index)];
     if( _buff.IsNull()       == false  && 
         _buff.RefCount()     == 1 && 
         _buff->PixelFormat() == pDesc->PixelFormat() &&
         _buff->Width()       == CCI_P(width) &&
         _buff->Height()      == CCI_P(height)  ) 
     {
        // Ok return that buffer
        CCI_RETURN_OK()
     } 
     else
     {
       
       dmTRY
       
       _buff = pDesc->CreateImage(CCI_P(width),CCI_P(height));

       dmCATCH(_E)
         CCI_SET_ERROR_MSG(_E.Text().Get());
         CCI_RETURN_FAIL()
       dmDONE

       if(!_buff.IsNull()) {
         _This->buffers[CCI_P(index)] = _buff;
         CCI_RETURN_OK()
       } else
         CCI_SET_ERROR_MSG(_TXT("Failed to create image !"));
     }
   }
   else 
    CCI_SET_ERROR_MSG(_TXT("Invalid image format !"));    
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : ReleaseAll
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( ImageList, ReleaseAll )
{
  CCI_INSTANCE( ImageList )

  for(int i=_This->buffers.size();--i>=0;) 
    _This->buffers[i].Release();

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method : MoveBuffer
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageList, MoveBuffer )
{
  CCI_INSTANCE( ImageList )

  if(CHECK_SIZE(CCI_P(from)) && CHECK_SIZE(CCI_P(to)) ) 
  {
    _This->buffers[CCI_P(to)] = _This->buffers[CCI_P(from)];
    CCI_RETURN_OK()
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : SwapBuffers
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageList, SwapBuffers )
{
  CCI_INSTANCE( ImageList )

  dm_uint first  = CCI_P(first);
  dm_uint second = CCI_P(second);

  if(CHECK_SIZE(first) && CHECK_SIZE(second) ) 
  {
    dmLink<dmImage> _tmp = _This->buffers[second];
    _This->buffers[second] = _This->buffers[first];
    _This->buffers[first]  = _tmp;
    CCI_RETURN_OK()
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : CloneBuffer
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageList, CloneBuffer )
{
  CCI_INSTANCE( ImageList )

  dm_uint src = CCI_P(src);
  dm_uint dst = CCI_P(dst);

  if(CHECK_SIZE(src) && CHECK_SIZE(dst)) 
  {
    dmLink<dmImage> _src = _This->buffers[src];
    if(!_src.IsNull()) {
      ImageList_StoreBuffer(_This,CCI_P(dst),_src,NULL,dmPixelFormatUndefined);

      // _This->buffers[dst] = _src->CreateCopy();
    }
    else
      _This->buffers[dst].Release();

    CCI_RETURN_OK()
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : SetBufferFormat
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageList, SetBufferFormat )
{
  CCI_INSTANCE( ImageList )

  dm_uint index = CCI_P(index);

  dmImageDescriptor* _Desc = dmGetDescriptor(static_cast<EPixelFormat>(CCI_P(format)));

  if(_Desc!=NULL) 
  {
    if(index==_NOINDEX_) {
      for(size_t i=0;i<_This->buffers.size();++i) {
        dmLink<dmImage>& _src = _This->buffers[i];
        if(!_src.IsNull() && (_src->PixelFormat()!=_Desc->PixelFormat()))
          _This->buffers[i] = _src->CreateCopy(*_Desc);
      }
      CCI_RETURN_OK()
    }
    else
    if(CHECK_SIZE(index)) 
    {
      dmLink<dmImage>& _src = _This->buffers[index];
      if(!_src.IsNull() && (_src->PixelFormat()!=_Desc->PixelFormat()))
          _This->buffers[index] = _src->CreateCopy(*_Desc);

      CCI_RETURN_OK()
    }
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : GetBufferFormat
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageList, GetBufferFormat )
{
  CCI_INSTANCE( ImageList )

  dm_uint index = CCI_P(index);

  if(CHECK_SIZE(index)) 
  {
    dmLink<dmImage>& _Img = _This->buffers[index];
    if(!_Img.IsNull())
      CCI_RETVAL_P(format) = _Img->PixelFormat();
    else      
      CCI_RETVAL_P(format) = dmPixelFormatUndefined;

    CCI_RETURN_OK();   
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : SpliceBuffers
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageList, SpliceBuffers )
{
  CCI_INSTANCE( ImageList )

  dmIImageList _Source;
  if(_Source.QueryInterface(CCI_P(source)))
  {
    CCI_INSTANCE_PTR(ImageList) _Src = CCI_GetInstData(CCI_DECL_DATA_(ImageList),
                                       _THIS_CLASS,_Source); 
    dm_uint from  = CCI_P(from);
    dm_uint to    = CCI_P(to);
    dm_uint count = CCI_P(count);

    if(from < _Src->buffers.size()) 
    {
      if(count==_NOINDEX_) 
         count = _Src->buffers.size() - from;

      dm_uint last_from = from + count;
      if(last_from > _Src->buffers.size())
         last_from = _Src->buffers.size();

      if(to + count > _This->buffers.size())
         _This->buffers.resize(to + count);

      for(;from < last_from;++from,++to) {
          _This->buffers[to] = _Src->buffers[from];
          _Src->buffers[from].Release();
      }

      CCI_RETURN_OK()
    }
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : CopyBuffers
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageList, CopyBuffers )
{
  CCI_INSTANCE( ImageList )

  dmIImageList _Source;
  if(_Source.QueryInterface(CCI_P(source)))
  {
    CCI_INSTANCE_PTR(ImageList) _Src = CCI_GetInstData(CCI_DECL_DATA_(ImageList),
                                       _THIS_CLASS,_Source); 
    dm_uint from  = CCI_P(from);
    dm_uint to    = CCI_P(to);
    dm_uint count = CCI_P(count);

    if(from < _Src->buffers.size()) 
    {
      if(count==_NOINDEX_) 
        count = _Src->buffers.size() - from;

      dm_uint last_from = from + count;
      if(last_from > _Src->buffers.size())
        last_from = _Src->buffers.size();

      if(to + count > _This->buffers.size())
        _This->buffers.resize(to + count);

      for(;from < last_from;++from,++to) {
         dmLink<dmImage>& _src = _This->buffers[from];
         if(!_src.IsNull()) 
            _This->buffers[to] = _Src->buffers[from]->CreateCopy();
      }

      CCI_RETURN_OK()

    }
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : SetCapacity
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageList, SetCapacity )
{
  CCI_INSTANCE( ImageList )

  dm_uint capacity =  CCI_P(capacity);
  if(_This->buffers.size() < capacity) 
  {
    if(capacity > BUFFER_MAX_SIZE) 
      CCI_RETURN_FAIL()

    _This->buffers.resize(capacity);
  }
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method : SetBufferData
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageList, SetBufferData )
{
  CCI_INSTANCE( ImageList )

  CCI_ENSURE_PARAM( CCI_P(scan0)  !=  NULL );
  CCI_ENSURE_PARAM( CCI_P(width)  !=  0    );
  CCI_ENSURE_PARAM( CCI_P(height) !=  0    );
 
  if(CCI_P(index)==_NOINDEX_ || CHECK_SIZE(CCI_P(index)) )
  {
    dmLink<dmImage> image;

    dmImageData imagedata;
    imagedata.PixelFormat = static_cast<EPixelFormat>(CCI_P(format));
    imagedata.Width       = CCI_P(width);
    imagedata.Height      = CCI_P(height);
    imagedata.Stride      = CCI_P(stride);
    imagedata.Scan0       = CCI_P(scan0);

    dmImageDescriptor* _descriptor = dmGetDescriptor(imagedata.PixelFormat);
    if(_descriptor)
    {
      image = _descriptor->CreateImage(imagedata);

      if(ImageList_StoreBuffer(_This,CCI_P(index),image,NULL,
                               static_cast<EPixelFormat>(CCI_P(format))))
         CCI_RETURN_OK()

      CCI_RETURN_FAIL()

    } else
      CCI_SET_ERROR_MSG(_TXT("Invalid pixel format !"));
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : GetBufferData
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageList, GetBufferData )
{
  CCI_INSTANCE( ImageList )

  dm_uint index = CCI_P(index);

  if(CHECK_SIZE(index)) 
  {
    dmLink<dmImage>& _Img = _This->buffers[index];
    if(!_Img.IsNull()) {
     dmImageData imagedata;
     if(_Img->GetImageData(imagedata)) {
       CCI_P(format) = imagedata.PixelFormat;
       CCI_P(width)  = imagedata.Width;
       CCI_P(height) = imagedata.Height;
       CCI_P(stride) = imagedata.Stride; 
       CCI_P(scan0)  = imagedata.Scan0; 
       CCI_RETURN_OK()
     }
   }
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : GetBufferA
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageList, GetBufferA )
{
  CCI_INSTANCE( ImageList )

  if( CCI_P(link)!=NULL && CHECK_SIZE(CCI_P(index)) )
  {
    switch(CCI_P(me))
    {
      case CCI_ME_GET:
        *CCI_P(link) = _This->buffers[CCI_P(index)];
         CCI_RETURN_OK()

      case CCI_ME_SET:
        _This->buffers[CCI_P(index)] = *CCI_P(link);
        CCI_RETURN_OK()
    }
  }
  
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( ImageList )
  CCI_REGISTER_METHOD_PP( ImageList,  StoreBuffer     )   
  CCI_REGISTER_METHOD_PP( ImageList,  IsEmpty         )
  CCI_REGISTER_METHOD_PP( ImageList,  ReleaseBuffer   )
  CCI_REGISTER_METHOD_PP( ImageList,  GetBuffer       )
  CCI_REGISTER_METHOD_PP( ImageList,  Resize          )
  CCI_REGISTER_METHOD_PP( ImageList,  CreateBuffer    )
  CCI_REGISTER_METHOD_NP( ImageList,  ReleaseAll      )
  CCI_REGISTER_METHOD_PP( ImageList,  MoveBuffer      )
  CCI_REGISTER_METHOD_PP( ImageList,  SwapBuffers     )
  CCI_REGISTER_METHOD_PP( ImageList,  CloneBuffer     )
  CCI_REGISTER_METHOD_PP( ImageList,  SetBufferFormat )
  CCI_REGISTER_METHOD_PP( ImageList,  GetBufferFormat )
  CCI_REGISTER_METHOD_PP( ImageList,  SpliceBuffers   )
  CCI_REGISTER_METHOD_PP( ImageList,  CopyBuffers     )
  CCI_REGISTER_METHOD_PP( ImageList,  SetCapacity     )
  CCI_REGISTER_METHOD_PP( ImageList,  SetBufferData   )
  CCI_REGISTER_METHOD_PP( ImageList,  GetBufferData   )
  CCI_REGISTER_METHOD_PP( ImageList,  GetBufferA      )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( ImageList )
{
  CCI_INVOKE_FACTORY(ImageList);
}
//---------------------------------------------------------------------
