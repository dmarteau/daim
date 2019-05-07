#ifndef CCI_MaskBuffer_h
#define CCI_MaskBuffer_h

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

//--------------------------------------------------------
// File         : CCI_MaskBuffer.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_MaskBuffer  _TXT("maskbuffer.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( MaskBuffer, Notify )
  
  CCI_MODULE_UUID( MaskBuffer, 21ec35fd-3acc-47cc-8a7f-ba39ecfecd90)

  CCI_MODULE_VERSION( MaskBuffer, 1 ) 

  /* Methods */
  CCI_DECL_METHOD_PP(MaskBuffer, 1, StoreMask  , _in CONST dmRegion* mask _in dm_uint index _in dm_int op )
  CCI_DECL_METHOD_PP(MaskBuffer, 2, CallMask   , _in dmRegion* mask _in dm_uint index _in dm_int op )
  CCI_DECL_METHOD_PP(MaskBuffer, 3, ClearMask  , _in dm_uint index )
  CCI_DECL_METHOD_PP(MaskBuffer, 4, SetHeapSize, _in dm_uint size  )

  CCI_DECL_METHOD_PP(MaskBuffer, 7, GetMask,  _in dmRegion* mask )
  CCI_DECL_METHOD_PP(MaskBuffer, 8, SetMask,  _in CONST dmRegion* mask _in dm_int op )
  CCI_DECL_METHOD_NP(MaskBuffer, 9, KillMask  )

  CCI_DECL_METHOD_PP(MaskBuffer,10, PushMask  , _in CONST dmRegion* mask )
  CCI_DECL_METHOD_PP(MaskBuffer,11, QueueMask , _in CONST dmRegion* mask )
  CCI_DECL_METHOD_NP(MaskBuffer,12, PopMask   )
  CCI_DECL_METHOD_PP(MaskBuffer,13, SwapMask  , _in dmRegion* mask _in dm_uint index )

  CCI_DECL_METHOD_PP(MaskBuffer,14, IsEmptyMask , _in dm_uint index )

  CCI_DECL_METHOD_PP(MaskBuffer,15, PopMaskOp  , _in dmRegion* mask _in dm_int op )
  CCI_DECL_METHOD_PP(MaskBuffer,16, RotateMask , _in dmRegion* mask  )
  CCI_DECL_METHOD_NP(MaskBuffer,17, ClearStack   )
  CCI_DECL_METHOD_NP(MaskBuffer,18, IsEmptyStack )

  CCI_DECL_METHOD_PP(MaskBuffer,19, GetBuffer      , _in dm_uint index _out dmRegion* mask )
  CCI_DECL_METHOD_PP(MaskBuffer,20, GetMaskAttributes ,
      _in dm_uint  index
      _in dm_int   left
      _in dm_int   top 
      _in dm_int   right
      _in dm_int   bottom 
      _in dm_uint  flags )

  /* Attributs */
  CCI_DECL_ATTR(MaskBuffer, 1,  TopMask    , dmRegion* ,[..G.] )
  CCI_DECL_ATTR(MaskBuffer, 2,  BottomMask , dmRegion* ,[..G.] )
  CCI_DECL_ATTR(MaskBuffer, 3,  HeapSize   , dm_uint   ,[..G.] )
  CCI_DECL_ATTR(MaskBuffer, 4,  MaskSize   , dm_uint   ,[..G.] )
  CCI_DECL_ATTR(MaskBuffer, 5,  DefaultMask, dmRegion* ,[..G.] )

  #define CCIV_SimpleRegion  0x01
  #define CCIV_ComplexRegion 0x02

CCI_END_MODULE_DECL( MaskBuffer )

#endif // __doxygen
//----------------------------------------------------------------

#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Mask buffer and calculator
 * 
 *  This interface expose methods for a container for storing/retrieving
 *  masks (i.e regions) as for perfotming calulation on the stored elements.
 *  The object will hold a "Current mask" which will be used for calculation
 *  For most of the methods exposed, the \a mask parameter can be null, in this
 *  case, the default mask will be used unless specified otherwise. 
 *
 *  The \a op parameter specifies which arithmetic operation should be performed
 *  between the source mask and the destination mask.
 */
//------------------------------------------------------------------------------------
class dmIMaskBuffer : public dmINotify
{    
  public:
   dmDECLARE_GENERIC_IINTERFACE(MaskBuffer)

  //----------------------------------------------------------------
  /*! \brief Store a \e copy of the mask at buffer index \a index. */
  //----------------------------------------------------------------
  bool StoreMask( const dmRegion* mask, dm_uint index,  dm_int op = 0 ) {
    CCI_RETURN_IMETHOD_PP_OK(MaskBuffer,StoreMask,_in mask _in index _in op )
  }
  //----------------------------------------------------------------
  /*! \brief Retrieve a copy of the mask at buffer index \a index. */
  //----------------------------------------------------------------
  bool CallMask( dmRegion* mask, dm_uint index, dm_int op = 0 ) {
    CCI_RETURN_IMETHOD_PP_OK(MaskBuffer,CallMask, _in mask _in index _in op )
  }
  //----------------------------------------------------------------
  /*! \brief Clear the mask at buffer index \a index. */
  //----------------------------------------------------------------
  bool ClearMask( dm_uint index ) {
    CCI_RETURN_IMETHOD_PP_OK(MaskBuffer,ClearMask,_in index)
  }

  // Obsolete, for compatibilities
  bool SetHeapSize( dm_uint size  ) {
    CCI_RETURN_IMETHOD_PP_OK(MaskBuffer,SetHeapSize, _in size)
  }
  
  //----------------------------------------------------------------
  /*! \brief Set the size of the mask array. */
  //----------------------------------------------------------------
  bool SetArraySize( dm_uint size  ) {
    CCI_RETURN_IMETHOD_PP_OK(MaskBuffer,SetHeapSize, _in size)
  }

  //----------------------------------------------------------------
  /*! \brief Retrieve a copy of the default mask */
  //----------------------------------------------------------------
  bool GetMask( dmRegion* mask ) { 
    CCI_RETURN_IMETHOD_PP_OK(MaskBuffer,GetMask, _in mask )
  }
  //----------------------------------------------------------------
  /*! \brief Set the default mask */
  //----------------------------------------------------------------
  bool SetMask( const dmRegion* mask, dm_int op = 0) {
    CCI_RETURN_IMETHOD_PP_OK(MaskBuffer,SetMask, _in mask _in op )
  }
  //----------------------------------------------------------------
  /*! \brief Clear the default mask */
  //----------------------------------------------------------------
  bool KillMask() { CCI_RETURN_IMETHOD_NP_OK(MaskBuffer,KillMask ) }

  //----------------------------------------------------------------
  /*! \brief Push a mask at the top of the stack */
  //----------------------------------------------------------------
  bool PushMask( const dmRegion* mask ) {
    CCI_RETURN_IMETHOD_PP_OK(MaskBuffer,PushMask, _in mask )
  }
  //----------------------------------------------------------------
  /*! \brief Queue a mask at the bottom of the stack */
  //----------------------------------------------------------------
  bool QueueMask( const dmRegion* mask ) {
    CCI_RETURN_IMETHOD_PP_OK(MaskBuffer,QueueMask, _in mask )
  }
  //----------------------------------------------------------------
  /*! \brief Pop the stack. */
  //----------------------------------------------------------------
  bool PopMask() { CCI_RETURN_IMETHOD_NP_OK(MaskBuffer,PopMask) }
  //----------------------------------------------------------------
  /*! \brief Swap masks 
   *
   *  - If \a mask is not null:
   *   -# If \a index is not -1 then swap is done
   *      between \a mask and the mask stored at \a index. 
   *   -# If \a index is -1 then the swap is done
   *       between \a mask and the default mask. 
   *  - If \a mask is null:
   *   -# If \a index is not -1 then swap is done
   *      between default mask and the mask stored at \a index. 
   *   -# If \a index is -1 then the swap is done between the
   *       default mask and the top of the stack. 
   */
  //----------------------------------------------------------------
  bool SwapMask( dmRegion* mask, dm_uint index = dm_noindex ) {
    CCI_RETURN_IMETHOD_PP_OK(MaskBuffer,SwapMask, _in mask _in index )
  }

  bool SwapMask()                { return SwapMask((dmRegion*)NULL);       }
  bool SwapMask( dm_uint index ) { return SwapMask((dmRegion*)NULL,index); }

  //----------------------------------------------------------------
  /*! \brief do an arithmetic operation between \a mask and the top of
   *  the stack, then pop the stack.
   */
  //----------------------------------------------------------------
  bool PopMaskOp( dm_int op , dmRegion* mask = NULL ) {
    CCI_RETURN_IMETHOD_PP_OK(MaskBuffer,PopMaskOp, _in mask _in op )
  }

  //----------------------------------------------------------------
  /*! \brief Rotate the stack. and involve \a mask as the top mask
   *  in the rotation.
   */
  //----------------------------------------------------------------
  bool RotateMask( dmRegion* mask ) {
    CCI_RETURN_IMETHOD_PP_OK(MaskBuffer,RotateMask, _in mask )
  }
  //----------------------------------------------------------------
  /*! \brief Clear the stack. Not that will physically destroy all the elements */
  //----------------------------------------------------------------
  bool ClearStack() {  CCI_RETURN_IMETHOD_NP_OK(MaskBuffer,ClearStack) }
  //----------------------------------------------------------------
  /*! \brief Check if the stack is empty */
  //----------------------------------------------------------------
  bool IsEmptyStack() { CCI_RETURN_IMETHOD_NP_OK(MaskBuffer,IsEmptyStack ) }
  //----------------------------------------------------------------
  /*! \brief Return a pointer to the top mask of the stack */
  //----------------------------------------------------------------
  dmRegion* TopMask() { return CCI_IGET(MaskBuffer,TopMask,(dmRegion*)NULL); }

  //----------------------------------------------------------------
  /*! \brief Return a pointer to bottom mask of the stack */
  //----------------------------------------------------------------
  dmRegion* BottomMask() { return CCI_IGET(MaskBuffer,BottomMask,(dmRegion*)NULL); }
  //----------------------------------------------------------------
  /*! \brief Return a pointer to the default mask */
  //----------------------------------------------------------------
  dmRegion* DefaultMask() { return CCI_IGET(MaskBuffer,DefaultMask,(dmRegion*)NULL); }
  //----------------------------------------------------------------
  /*! \brief Return the size of the heap */
  //----------------------------------------------------------------
  dm_uint HeapSize() { return CCI_IGET(MaskBuffer,HeapSize,(dm_uint)0); }
  //----------------------------------------------------------------
  /*! \brief Return the size of of the default mask */
  //----------------------------------------------------------------
  dm_uint MaskSize() { return CCI_IGET(MaskBuffer,MaskSize,(dm_uint)0); }
  //----------------------------------------------------------------
  /*! \brief Check if the mask at \a index is empty. If \a index is -1
   *  then the default mask is checked. 
   */
  //----------------------------------------------------------------
  bool IsEmptyMask( dm_uint index = ~0 ) { 
    CCI_RETURN_IMETHOD_PP_OK(MaskBuffer,IsEmptyMask,_in index) 
  }

  //----------------------------------------------------------------
  /*! \brief Return a pointer to the mask at buffer \a index. */
  //----------------------------------------------------------------
  dmRegion* GetBuffer( dm_uint index  ) { 
    IF_CCI_IMETHOD_RP(MaskBuffer,GetBuffer, _in index _out(mask) NULL ) 
    THEN  return CCI_RETVAL(mask);
    ELSE  return NULL;
    ENDIF
  }
  
  //----------------------------------------------------------------
  /*! \brief Return the status and the bounding box of the mask 
   *         at \a index.
   * 
   * If \a index is -1
   *  then informations about the defautlt mask is checked. 
   */
  //----------------------------------------------------------------
  dm_uint GetMaskAttributes( dm_uint index, dm_rect* rect ) { 
     IF_CCI_IMETHOD_RP(MaskBuffer,GetMaskAttributes, _in index ) 
     THEN
       if(rect) {
          rect->top_left.x     = CCI_RETVAL(left);
          rect->top_left.y     = CCI_RETVAL(top);
          rect->bottom_right.x = CCI_RETVAL(right);
          rect->bottom_right.y = CCI_RETVAL(bottom);
       }
       return CCI_RETVAL(flags);
     ELSE
       return dm_noindex;
     ENDIF
  }

};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_MaskBuffer.h */
