
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

/* $ChangeLog  v1.1, 31 août 2006 20:22:16 , David
 *
 * Added RGBToColorSpace and ColorSpaceToRGB
 */

#define dmUseCCI
#define dmUseUtilitiesExtra
#include "daim_utilities.h"
#include "daim_modules/CCI_Notify.h"

#include "daim_modules/CCI_ImageList.h"
#include "daim_modules/processing/CCI_Colorspace.h"
#include "daim_modules/processing/CCI_ColorMap.h"

#include "daim_kernel/images/dmRGBColor.h"

#define CCIC_NAME  CCIC_ColorMap
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_ColorMap_Version
#define CCI_REVISION  1
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2006"))
//-------------------------------------------------------------------
#define CCI_INTERNAL_MODULE

#include "daim_modules/CCI_Base.h"

//-----------------------------------------
CCI_DECL_DATA( ColorMap )
{
  dm_uint32* mLut;
  dm_uint32  mCount;
  dm_uint32  mCapacity;

  dmString mDescription;
};  
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static inline CCI_CONSTRUCTOR( ColorMap )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( ColorMap )
  
  CCI_INIT_MEMBER(mDescription);
  
  _This->mLut      = NULL;
  _This->mCount    = 0;
  _This->mCapacity = 0;
  
  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static inline CCI_DESTRUCTOR( ColorMap )
{
  CCI_INSTANCE( ColorMap )
  
  if(_This->mLut)
     dmMemory::Free(_This->mLut);
  
  CCI_DESTROY_MEMBER(mDescription);
  CCI_DESTROY_SUPER()  
}

//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
//static inline CCI_METHOD_GET( ColorMap )
//{
//  CCI_RETURN_INVOKE_SUPER()
//}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
//static inline CCI_METHOD_SET( ColorMap )
//{
//  CCI_RETURN_INVOKE_SUPER()
//}

//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( ColorMap )
  CCI_ATTR_GET(ColorMap, Count       , CCIA_RETVAL = _This->mCount )
  CCI_ATTR_GET(ColorMap, Description , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->mDescription.Get()) )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( ColorMap )
  CCI_ATTR_GET(ColorMap, Description , _This->mDescription = CCIA_TRANSLATE_DATA(char_t*))
  CCIA_DISCARD(ColorMap, Count)
CCI_END_SETTER()
//---------------------------------------------------------------------
// GetLUT
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ColorMap, GetLUT )
{
  CCI_INSTANCE( ColorMap )
  
  CCI_RETVAL_P(count)  = _This->mCount;
  CCI_RETVAL_P(colors) = _This->mLut;
  
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// SetLUT
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ColorMap, SetLUT )
{
  //__dm_DebugBreak__
  
  CCI_INSTANCE( ColorMap )

  if(CCI_P(count) == 0 || (CCI_P(colors) == dm_null) || (_This->mLut == CCI_P(colors))) 
     CCI_RETURN_INVALID_PARAM()

  if(CCI_RETVAL_P(count) > _This->mCapacity) 
  {
     void* newPtr = dmMemory::Realloc(_This->mLut,CCI_RETVAL_P(count)*sizeof(dm_uint32));
     if(!newPtr)
        CCI_RETURN_ERR(CCI_ERR_MEMORY);

     _This->mLut      = (dm_uint32*)newPtr;
     _This->mCapacity = CCI_RETVAL_P(count);
        
  }

  _This->mCount = CCI_P(count);
  dmMemory::Memcpy(_This->mLut,CCI_P(colors),CCI_P(count)*sizeof(dm_uint32));
  
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// GetColor32
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ColorMap, GetColor32 )
{
  CCI_INSTANCE( ColorMap )
  
  if(CCI_P(index)>_This->mCount)
     CCI_RETURN_INVALID_PARAM() 

  CCI_RETVAL_P(color) = _This->mLut[CCI_P(index)]; 
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// SetColor32
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ColorMap, SetColor32 )
{
  CCI_INSTANCE( ColorMap )
  
  if(CCI_P(index)>_This->mCount)
     CCI_RETURN_INVALID_PARAM() 
  
  _This->mLut[CCI_P(index)] = CCI_RETVAL_P(color); 
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Remap
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ColorMap, Remap )
{
  CCI_INSTANCE( ColorMap )

  if(CCI_P(count) != _This->mCount || CCI_P(cmap) == dm_null)
     CCI_RETURN_INVALID_PARAM() 

  dm_uint32* tmpPtr = 
    (dm_uint32*)dmMemory::Clone(_This->mLut,
                                _This->mCount*sizeof(dm_uint32));
  
  const dm_uint32* cmap  = CCI_P(cmap);
  
  dm_uint32  count =_This->mCount;
  
  for(dm_int i=static_cast<dm_int>(count);--i>=0;) {
     if(cmap[i] < count)
        tmpPtr[i] = _This->mLut[cmap[i]];
  }
  
  dmMemory::Free(_This->mLut);
  _This->mLut = tmpPtr;

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// RGBToColorSpace
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ColorMap, RGBToColorSpace )
{
  CCI_INSTANCE( ColorMap )
  
  // TODO: need to rewrite in a more efficient way
  
  dmIColorSpace _IColorSpace;
  if(_IColorSpace.QueryInterface(CCI_P(colorspace)))
  {
  	dm_uint32  count =_This->mCount,c;
    dm_uint32* lut   = _This->mLut;

  	dm_real q1,q2,q3,q4;
   
    for(dm_uint i=0;i<count;++i) 
    {
    	c  = lut[i];
    	q1 = DM_GetByteColor1(c);
    	q2 = DM_GetByteColor2(c); 
    	q3 = DM_GetByteColor3(c); 
    	q4 = DM_GetByteColor4(c); 
    	
    	_IColorSpace.SplitColor(q1,q2,q3,q4);
    	
    	lut[i] = DM_PackColorBytes( 
    	   static_cast<dm_uint8>(q1),
    	   static_cast<dm_uint8>(q2),
    	   static_cast<dm_uint8>(q3),
    	   static_cast<dm_uint8>(q4));
    }
  	
    CCI_RETURN_OK()
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// ColorSpaceToRGB
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ColorMap, ColorSpaceToRGB )
{
  CCI_INSTANCE( ColorMap )
  
  // TODO: need to rewrite in a more efficient way

  dmIColorSpace _IColorSpace;
  if(_IColorSpace.QueryInterface(CCI_P(colorspace)))
  {
  	dm_uint32  count =_This->mCount,c;
    dm_uint32* lut  = _This->mLut;

  	dm_real q1,q2,q3,q4;   
   
    for(dm_uint i=0;i<count;++i) 
    {
    	c  = lut[i];
    	q1 = DM_GetByteColor1(c);
    	q2 = DM_GetByteColor2(c); 
    	q3 = DM_GetByteColor3(c); 
    	q4 = DM_GetByteColor4(c); 
    	
    	_IColorSpace.MergeColor(q1,q2,q3,q4);
    	
    	lut[i] = DM_PackColorBytes( 
    	   static_cast<dm_uint8>(q1),
    	   static_cast<dm_uint8>(q2),
    	   static_cast<dm_uint8>(q3),
    	   static_cast<dm_uint8>(q4));
    }
  	  	
    CCI_RETURN_OK()
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( ColorMap )
  CCI_REGISTER_METHOD_PP( ColorMap , GetLUT )
  CCI_REGISTER_METHOD_PP( ColorMap , SetLUT )
  CCI_REGISTER_METHOD_PP( ColorMap , GetColor32 )
  CCI_REGISTER_METHOD_PP( ColorMap , SetColor32 )
  CCI_REGISTER_METHOD_PP( ColorMap , Remap )
  CCI_REGISTER_METHOD_PP( ColorMap , ColorSpaceToRGB )
  CCI_REGISTER_METHOD_PP( ColorMap , RGBToColorSpace )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( ColorMap )
{
  CCI_INVOKE_FACTORY(ColorMap);
}
//---------------------------------------------------------------------
