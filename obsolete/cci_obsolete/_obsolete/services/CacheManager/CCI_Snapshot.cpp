/* ::: BEGIN LICENSE BLOCK:::
 * Copyright (c) 2005 David Marteau
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

//--------------------------------------------------------------------
// Project   : DAIM Image processing library
// File      : CCI_Snapshot.cpp
// Author    : David Marteau
// Date      : 29/06/2005
//--------------------------------------------------------------------

#include "daim_kernel.h"
#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_PropertyList.h"
#include "daim_modules/CCI_Snapshot.h"
#include "daim_modules/CCI_Interface.h"

#include "images/dmGenericLoader.h"

//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_Snapshot
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Snapshot_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004-2005"))
//--------------------------------------------------------------------

#include "daim_modules/CCI_Base.h"
#include "dmSnapshot.h"

//----------------------------------------------------------------
CCI_DECL_DATA( Snapshot ) {
  dmSnapshot mCached;
  dmString   mMimeType;
  dmString   mName;
};
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( Snapshot )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE(Snapshot)

  CCI_CONSTRUCT_DATA()

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( Snapshot )
{
  CCI_INSTANCE(Snapshot)

  dmGenericLoader* _Encoder =  _This->mCached.SetEncoder(NULL);
  dmGenericLoader* _Decoder =  _This->mCached.SetDecoder(NULL);

  if(_Encoder)
     _Encoder->Release(); 

  if(_Decoder)
     _Decoder->Release(); 

  CCI_DESTROY_DATA()
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Snapshot )
  CCI_ATTR_GET(Snapshot, CacheName    , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->mCached.GetCachedImagePath().CStr()  ) )
  CCI_ATTR_GET(Snapshot, ImagePath    , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->mCached.GetOriginalImagePath().CStr()) )
  CCI_ATTR_GET(Snapshot, ImageWidth   , CCIA_RETVAL = _This->mCached.ImageWidth()    )
  CCI_ATTR_GET(Snapshot, ImageHeight  , CCIA_RETVAL = _This->mCached.ImageHeight()   )
  CCI_ATTR_GET(Snapshot, ImageFormat  , CCIA_RETVAL = _This->mCached.ImageFormat()   )
  CCI_ATTR_GET(Snapshot, MimeType     , 
     CCIA_RETVAL = dm_ptr_to_long(_This->mMimeType.CStr())
  )
  CCI_ATTR_GET(Snapshot, Name     , 
     CCIA_RETVAL = dm_ptr_to_long(_This->mName.CStr())
  )

CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Snapshot )
  CCI_ATTR_SET(Snapshot, MimeType    , 
    _This->mMimeType = CCIA_TRANSLATE_DATA(char_t*);
    _This->mCached.SetEncoder(dmLoaderFactory::GetLoader(_This->mMimeType,dmENCODER));
    _This->mCached.SetDecoder(dmLoaderFactory::GetLoader(_This->mMimeType,dmDECODER))
  )
  CCIA_DISCARD(Snapshot, CacheName   )
  CCIA_DISCARD(Snapshot, ImagePath   )
  CCIA_DISCARD(Snapshot, ImageWidth  )
  CCIA_DISCARD(Snapshot, ImageHeight )
  CCIA_DISCARD(Snapshot, ImageFormat )
  CCIA_DISCARD(Snapshot, Name        )
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method LoadFromCache 
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Snapshot, LoadFromCache )
{
  CCI_INSTANCE(Snapshot)
  CCI_Try

    if(!EMPTY_STRING(CCI_P(file)))
       _This->mCached.SetImagePath(CCI_P(file));

    switch(_This->mCached.AttemptLoadFromCache())
    {
      case dmSnapshot::eNeedUpdateCache:
        if(_This->mCached.UpdateImageCache()) {
           _This->mName = _This->mCached.GetOriginalImagePath().FileName();
           CCI_RETURN_OK()
        }
        break;

      case dmSnapshot::eAttemptOK:
        _This->mName = _This->mCached.GetOriginalImagePath().FileName();
        CCI_RETURN_OK()
        break;
      
      default:  
        break;  
    }
 
  CCI_Catch( Snapshot, LoadFromCache )
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method UpdateCache   
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Snapshot, UpdateCache )
{
  CCI_INSTANCE(Snapshot)
  CCI_Try

    if(!EMPTY_STRING(CCI_P(file)))
       _This->mCached.SetImagePath(CCI_P(file));

    if(_This->mCached.UpdateImageCache()) {
        _This->mName = _This->mCached.GetOriginalImagePath().FileName();
        CCI_RETURN_OK()
    }

  CCI_Catch( Snapshot, UpdateCache )
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method ClearCache    
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Snapshot, ClearCache )
{
  CCI_INSTANCE(Snapshot)
  CCI_Try

  if(!EMPTY_STRING(CCI_P(file))) 
    _This->mCached.SetImagePath(CCI_P(file));

  _This->mCached.ClearImageCache();
  CCI_RETURN_OK()

  CCI_Catch( Snapshot, ClearCache )
  CCI_RETURN_FAIL()
}

//---------------------------------------------------------------------
// Method: GetProperties
//---------------------------------------------------------------------
#define LONG2STR(fmt,lvalue) (fmt.Format(dmString::npos,"%ld",lvalue),fmt.CStr())
#define DBLE2STR(fmt,lvalue) (fmt.Format(dmString::npos,"%lf",lvalue),fmt.CStr())

static inline CCI_IMPL_METHOD( Snapshot, GetProperties )
{
  CCI_INSTANCE( Snapshot )

  dmIPropertyList _IPropList;
  if(_IPropList.QueryInterface(CCI_P(propertylist)))
  {
    dm_param key = CCI_P(key);
    dmString fmt;

    _IPropList.SetProperty(key,NULL,_TXT("image:width" ),LONG2STR(fmt,_This->mCached.ImageWidth() ));
    _IPropList.SetProperty(key,NULL,_TXT("image:height"),LONG2STR(fmt,_This->mCached.ImageHeight()));
    _IPropList.SetProperty(key,NULL,_TXT("image:format"),LONG2STR(fmt,_This->mCached.ImageFormat()));
    _IPropList.SetProperty(key,NULL,_TXT("image:file"  ),_This->mCached.GetOriginalImagePath().CStr());
    _IPropList.SetProperty(key,NULL,_TXT("image:cache" ),_This->mCached.GetCachedImagePath().CStr());
    _IPropList.SetProperty(key,NULL,_TXT("image:id"    ),_This->mCached.GetCacheName().CStr());

    CCI_RETURN_OK();
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method Synchronize    
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Snapshot, Synchronize )
{
  CCI_INSTANCE(Snapshot)
  CCI_Try

   const dmString& _OriginalPath = _This->mCached.GetOriginalImagePath();

   bool _NeedUpdate = true;

   if(!_OriginalPath.Empty()) 
   {
     // If original file is true disk file then
     // Compare date between original file and cached image
     if(_OriginalPath.Compare(0,7,_TXT("file://")) == 0)
     {
       const char_t* nativeOriginalPath = _OriginalPath.CStr()+7;

       if(!dmPortability::AccessFile(nativeOriginalPath,dmPortability::eREAD))
           CCI_RETURN_FAIL()

       const dmString& _CachedPath = _This->mCached.GetCachedImagePath();
       if(!_CachedPath.Empty() && 
          dmPortability::AccessFile(_CachedPath.CStr(),dmPortability::eEXISTS))
       {
          dmTime _tzSource = dmPortability::GetFileTime(nativeOriginalPath);
          dmTime _tzCache  = dmPortability::GetFileTime(_CachedPath.CStr());

          _NeedUpdate = _tzCache.IsOlderThan(_tzSource);
       }
     } 

     if(_NeedUpdate) {
       if(!_This->mCached.UpdateImageCache())
         CCI_RETURN_FAIL()
     }
   
     CCI_RETURN_OK()
  }

  CCI_Catch( Snapshot, Synchronize )
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Snapshot )  	
  CCI_REGISTER_METHOD_PP( Snapshot, LoadFromCache )
  CCI_REGISTER_METHOD_PP( Snapshot, UpdateCache   )
  CCI_REGISTER_METHOD_PP( Snapshot, ClearCache    )
  CCI_REGISTER_METHOD_PP( Snapshot, GetProperties )
  CCI_REGISTER_METHOD_NP( Snapshot, Synchronize   )  
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Snapshot )
{
  DM_INIT_API()
  CCI_INVOKE_FACTORY( Snapshot )
}
//---------------------------------------------------------------------
