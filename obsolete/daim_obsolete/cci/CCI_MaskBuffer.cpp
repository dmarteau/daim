
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
#include "daim_kernel.h"
#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_MaskBuffer.h"

#include "daim_kernel/operators/dmOperators.h"

#define CCIC_NAME  CCIC_MaskBuffer
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_MaskBuffer_Version
#define CCI_REVISION  1
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//-------------------------------------------------------------------
#define CCI_INTERNAL_MODULE
#include "daim_modules/CCI_Base.h"

#include <deque>
//-------------------------------------------------------------------
#define BUFFER_DFLT_SIZE 16
#define CHECK_HEAP(pos)  (pos < _This->_RoiHeap.size())

#define _NOINDEX_ ~0UL

CCI_DECL_DATA( MaskBuffer )
{
  std::deque<dmRegion>  _RoiList;
  std::vector<dmRegion> _RoiHeap;
  dmRegion              _RoiInfo;
};
//--------------------------------------------------------------------
static dmRegion* MaskBuffer_DoMaskOperation( 
       dmRegion& dst,const dmRegion& src, int op )
{
  switch(op) {
    case dmTk::Math::AddPixels  :
    case dmTk::Math::OrPixels   : dst.AddRoi(src); break;
    case dmTk::Math::SubPixels  : dst.SubRoi(src); break;
    case dmTk::Math::XorPixels  : dst.XorRoi(src); break;
    case dmTk::Math::AndPixels  : dst.AndRoi(src); break;
    case dmTk::Math::NSubPixels : {
      dmRegion _tmp = src;
      _tmp.SubRoi(dst);
      dst = _tmp;
    } break;

    default : dst = src;
  } 
  return &dst; 
} 
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static inline CCI_CONSTRUCTOR( MaskBuffer )
{
  CCI_CONSTRUCT_SUPER()

  CCI_INSTANCE( MaskBuffer )
  CCI_INIT_MEMBER(_RoiList)
  CCI_INIT_MEMBER(_RoiHeap)
  CCI_INIT_MEMBER(_RoiInfo)

  _This->_RoiHeap.resize(BUFFER_DFLT_SIZE);

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static inline CCI_DESTRUCTOR( MaskBuffer )
{
  CCI_INSTANCE( MaskBuffer )
  CCI_DESTROY_MEMBER( _RoiList )
  CCI_DESTROY_MEMBER( _RoiHeap )
  CCI_DESTROY_MEMBER( _RoiInfo )
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( MaskBuffer )
  CCI_ATTR_GETVAL_L(MaskBuffer,HeapSize   ,  _This->_RoiHeap.size() )
  CCI_ATTR_GETVAL_L(MaskBuffer,MaskSize   ,  _This->_RoiInfo.Area() )
  CCI_ATTR_GETVAL_P(MaskBuffer,DefaultMask, &_This->_RoiInfo )
  CCI_ATTR_GETVAL_P(MaskBuffer,TopMask    ,  _This->_RoiList.empty()?NULL:&_This->_RoiList.back()  )
  CCI_ATTR_GETVAL_P(MaskBuffer,BottomMask ,  _This->_RoiList.empty()?NULL:&_This->_RoiList.front() )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( MaskBuffer )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method : StoreMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( MaskBuffer, StoreMask )
{
  CCI_INSTANCE( MaskBuffer )
 
  if(CHECK_HEAP(CCI_P(index)))
  {
    const dmRegion* mask = CCI_P(mask);
    if(mask==NULL) mask = &_This->_RoiInfo;
    MaskBuffer_DoMaskOperation(_This->_RoiHeap[CCI_P(index)],*mask,CCI_P(op));
    CCI_RETURN_OK()
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : CallMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( MaskBuffer, CallMask )
{
  CCI_INSTANCE( MaskBuffer )

  if(CHECK_HEAP(CCI_P(index))) 
  {
    dmRegion* mask = CCI_P(mask);
    if(mask==NULL) mask = &_This->_RoiInfo;
    MaskBuffer_DoMaskOperation(*mask,_This->_RoiHeap[CCI_P(index)],CCI_P(op));
    CCI_RETURN_OK()
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : ClearMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( MaskBuffer, ClearMask )
{
  CCI_INSTANCE( MaskBuffer )

  if(CHECK_HEAP(CCI_P(index))) {
    _This->_RoiHeap[CCI_P(index)].KillRoi();
    CCI_RETURN_OK()
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : SetHeapSize
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( MaskBuffer, SetHeapSize )
{
  CCI_INSTANCE( MaskBuffer )

  if( _This->_RoiHeap.size() < CCI_P(size) )
      _This->_RoiHeap.resize(CCI_P(size));

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method : GetMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( MaskBuffer, GetMask )
{
  CCI_INSTANCE( MaskBuffer )

  if(CCI_P(mask)!=NULL) {
    *CCI_P(mask) = _This->_RoiInfo;
    CCI_RETURN_OK()
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : SetMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( MaskBuffer, SetMask )
{
  CCI_INSTANCE( MaskBuffer )

  if(CCI_P(mask)!=NULL) {
    MaskBuffer_DoMaskOperation(_This->_RoiInfo,*CCI_P(mask),CCI_P(op)); 
    CCI_RETURN_OK()
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : KillMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( MaskBuffer, KillMask )
{
  CCI_INSTANCE( MaskBuffer )

  _This->_RoiInfo.KillRoi(); 
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method : PushMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( MaskBuffer, PushMask )
{
  CCI_INSTANCE( MaskBuffer )

  const dmRegion* mask = CCI_P(mask);
  if(mask==NULL) mask = &_This->_RoiInfo;
  _This->_RoiList.push_back(*mask);

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method : QueueMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( MaskBuffer, QueueMask )
{
  CCI_INSTANCE( MaskBuffer )

  const dmRegion* mask = CCI_P(mask);
  if(mask==NULL) mask = &_This->_RoiInfo;
  _This->_RoiList.push_front(*mask);

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method : PopMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( MaskBuffer, PopMask )
{
  CCI_INSTANCE( MaskBuffer )

  if(!_This->_RoiList.empty()) {
    _This->_RoiList.pop_back();
    CCI_RETURN_OK();
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : SwapMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( MaskBuffer, SwapMask )
{
  CCI_INSTANCE( MaskBuffer )

  dm_uint   index   = CCI_P(index);
  dmRegion* mask_1  = CCI_P(mask);
  dmRegion* mask_2  = NULL;

  if(mask_1 == NULL) 
    mask_1 = &_This->_RoiInfo;

  if(index != _NOINDEX_) {
    if(CHECK_HEAP(index))
      mask_2 = &_This->_RoiHeap[index];
  } 
  else if(!_This->_RoiList.empty())
    mask_2 = &_This->_RoiList.back();

  if(mask_1!=NULL && mask_2!=NULL) {
    dmRegion _exch = *mask_1;
    *mask_1 = *mask_2;
    *mask_2 = _exch;
    CCI_RETURN_OK();
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method : IsEmptyMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( MaskBuffer, IsEmptyMask )
{
  CCI_INSTANCE( MaskBuffer )

  dm_uint index = CCI_P(index);
  if(index != _NOINDEX_) {
    if(CHECK_HEAP(index) && !_This->_RoiHeap[index].IsEmptyRoi())
      CCI_RETURN_OK()
  } else if(!_This->_RoiInfo.IsEmptyRoi())
      CCI_RETURN_OK()
  
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : PopMaskOp
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( MaskBuffer, PopMaskOp )
{
  CCI_INSTANCE( MaskBuffer )

  if(!_This->_RoiList.empty())
  {
    dmRegion* mask = CCI_P(mask);
    if(mask==NULL) mask = &_This->_RoiInfo;
    MaskBuffer_DoMaskOperation(*mask,_This->_RoiList.back(),CCI_P(op));
    _This->_RoiList.pop_back();
    CCI_RETURN_OK();
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : RotateMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( MaskBuffer, RotateMask )
{
  CCI_INSTANCE( MaskBuffer )

  if(!_This->_RoiList.empty())
  {
    dmRegion* mask = CCI_P(mask);
    if(mask==NULL) mask = &_This->_RoiInfo;

    if(!mask->IsEmptyRoi()) {
       _This->_RoiList.push_front(*mask);
       *mask = _This->_RoiList.back();
       _This->_RoiList.pop_back();
      CCI_RETURN_OK();
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : ClearStack
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( MaskBuffer, ClearStack )
{
  CCI_INSTANCE( MaskBuffer )
  _This->_RoiList.clear();
  CCI_RETURN_OK();
}
//---------------------------------------------------------------------
// Method : IsEmptyStack
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( MaskBuffer, IsEmptyStack )
{
  CCI_INSTANCE( MaskBuffer )
  if(_This->_RoiList.empty())
    CCI_RETURN_OK();

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : GetBuffer
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( MaskBuffer, GetBuffer )
{
  CCI_INSTANCE( MaskBuffer )
  if( CHECK_HEAP(CCI_P(index)) ) {
    CCI_RETVAL_P(mask) = &_This->_RoiHeap[CCI_P(index)];
    CCI_RETURN_OK();
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : GetMaskAttributes
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( MaskBuffer, GetMaskAttributes )
{
  CCI_INSTANCE( MaskBuffer )
  
  dmRegion* _Mask;
  
  if(CCI_P(index) != dm_noindex) {
    if( CHECK_HEAP(CCI_P(index)) )
       _Mask = &_This->_RoiHeap[CCI_P(index)];
  } else
       _Mask = &_This->_RoiInfo;
     
  if(_Mask) 
  {
    if(!_Mask->IsEmptyRoi()) {
       CCI_RETVAL_P(flags) = (_Mask->IsRectRoi()  ? 
                               CCIV_SimpleRegion :
                               CCIV_ComplexRegion); 
                                
       const dmRect& r =  _Mask->Rectangle(); 
       CCI_RETVAL_P(left)   = r.Left();
       CCI_RETVAL_P(top)    = r.Top();
       CCI_RETVAL_P(right)  = r.Right();
       CCI_RETVAL_P(bottom) = r.Bottom();
    } else 
       CCI_RETVAL_P(flags) = 0;
      
    CCI_RETURN_OK();  
  }
  
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( MaskBuffer )
  CCI_REGISTER_METHOD_PP(MaskBuffer,StoreMask    )
  CCI_REGISTER_METHOD_PP(MaskBuffer,CallMask     )
  CCI_REGISTER_METHOD_PP(MaskBuffer,ClearMask    )
  CCI_REGISTER_METHOD_PP(MaskBuffer,SetHeapSize  )
  CCI_REGISTER_METHOD_PP(MaskBuffer,GetMask      )
  CCI_REGISTER_METHOD_PP(MaskBuffer,SetMask      )
  CCI_REGISTER_METHOD_NP(MaskBuffer,KillMask     )
  CCI_REGISTER_METHOD_PP(MaskBuffer,PushMask     )
  CCI_REGISTER_METHOD_PP(MaskBuffer,QueueMask    )
  CCI_REGISTER_METHOD_NP(MaskBuffer,PopMask      )
  CCI_REGISTER_METHOD_PP(MaskBuffer,SwapMask     )
  CCI_REGISTER_METHOD_PP(MaskBuffer,PopMaskOp    )
  CCI_REGISTER_METHOD_PP(MaskBuffer,RotateMask   )
  CCI_REGISTER_METHOD_NP(MaskBuffer,ClearStack   )
  CCI_REGISTER_METHOD_NP(MaskBuffer,IsEmptyStack )
  CCI_REGISTER_METHOD_PP(MaskBuffer,GetBuffer    )
  CCI_REGISTER_METHOD_PP(MaskBuffer,GetMaskAttributes)
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( MaskBuffer )
{
  CCI_INVOKE_FACTORY(MaskBuffer);
}
//---------------------------------------------------------------------
