
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
#define dmUseUtilitiesExtra
#include "daim_utilities.h"

#define CCIC_NAME  CCIC_StreamLoader
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_StreamLoader_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2006"))
//-------------------------------------------------------------------
#define CCI_INTERNAL_MODULE
#include "daim_modules/CCI_Base.h"

#include "daim/daim_base.h"
#include "daim/daim_loaders.h"

#include "daim_modules/CCI_StringList.h"
#include "daim_modules/CCI_PropertyList.h"
#include "daim_modules/CCI_Interface.h"
//-----------------------------------------

CCI_DECL_DATA( StreamLoader )
{
  dmIStringList mStringList;
  dm_int        mLoadError;
};
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static inline CCI_CONSTRUCTOR( StreamLoader )
{
  CCI_CONSTRUCT_SUPER()  
  
  CCI_INSTANCE( StreamLoader )
  CCI_INIT_MEMBER(mStringList)
  
  _This->mLoadError = ERR_LOAD_NOERROR;
  
  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static inline CCI_DESTRUCTOR( StreamLoader )
{
  CCI_INSTANCE( StreamLoader )
  CCI_DESTROY_MEMBER(mStringList)
  
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
/*
static inline CCI_METHOD_GET( StreamLoader )
{
  CCI_RETURN_INVOKE_SUPER()
}
*/
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( StreamLoader ) 
  CCI_ATTR_GET( StreamLoader, DocError, CCIA_RETVAL = _This->mLoadError )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( StreamLoader )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method:  Read
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StreamLoader, Read )
{
  CCI_INSTANCE( StreamLoader )
  
  _This->mLoadError = ERR_LOAD_NOERROR;
  
  dmIImageList    anImageList;
  dmIInputStream  anInputStream;
  dmILoader       aLoader;
  
  if(!anImageList.QueryInterface(CCI_P(imagelist)) ||
     !anInputStream.QueryInterface(CCI_P(inputstream)))
  {
     CCI_RETURN_INVALID_PARAM() 
  }

  cci_Object* loader = CCI_P(loader);
  
  // Get a default loader
  if(loader==NULL) 
  {
    // Get Mime Type from Input Stream
    const char_t* mime = anInputStream.GetContentType();
    if(EMPTY_STRING(mime)) {
       _This->mLoadError = ERR_LOAD_TYPE_UNKNOWN;
       CCI_RETURN_FAIL()     
    }

    _This->mLoadError = DM_GetLoader(aLoader,mime,dmILoader::fl_DECODE);
    if(_This->mLoadError!=ERR_LOAD_NOERROR)
       CCI_RETURN_FAIL()  
    
    if(!aLoader.HasStreamSupport()) {
      _This->mLoadError = ERR_LOAD_NOSTREAMSUPPORT;
      CCI_RETURN_FAIL()         
    } 
   
    // Initialize the loader
    aLoader.Initialize(NULL,mime,dmILoader::fl_DECODE);

  } else
    aLoader.QueryInterface(CCI_P(loader));

  if(aLoader.IsValid()) 
  {
    dmLOADER_BUFFER_INFO bfi;
    bfi.bi_index = 0;     // we want the first/default image in file
    bfi.bi_flags = 0;
 
    if(DM_GetLoaderBufferInfo(aLoader,anInputStream,bfi)) {      
      if(anImageList.CreateBuffer(CCI_P(index),
               bfi.bi_format,
               bfi.bi_width,
               bfi.bi_height)) {
         
                 dmImageData _Data;
                 anImageList.GetBufferData(_Data,CCI_P(index));
                 if(aLoader.GetImageData(_Data)) {
                    aLoader.Close();
                    CCI_RETURN_OK()
                 }
               }         
    } else 
      _This->mLoadError = ERR_LOAD_IOERROR;   
  } else {
    CCI_RETURN_INVALID_PARAM() 
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method:  Write
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StreamLoader, Write )
{
  CCI_INSTANCE( StreamLoader )
  
  dmIImageList    anImageList;
  dmIOutputStream anOutputStream;
  dmILoader       aLoader;

  if(!anImageList.QueryInterface(CCI_P(imagelist)) ||
     !anOutputStream.QueryInterface(CCI_P(outputstream)) ||
     !aLoader.QueryInterface(CCI_P(loader)) ) 
  {
     CCI_RETURN_INVALID_PARAM() 
  }

  dmImageData _Data;
  if(anImageList.GetBufferData(_Data,CCI_P(index)))
  {
    if(aLoader.IsSupportedFormat(_Data.PixelFormat))
    {
      dmLOADER_BUFFER_INFO lbi;
      lbi.bi_flags = CCI_P(bi_flags);
      lbi.bi_index = CCI_P(bi_index);   // this is the first/default image in file
      lbi.bi_count = CCI_P(bi_count);   // just one record

      if(DM_WriteLoaderBuffer(aLoader,anOutputStream,_Data,lbi))
         CCI_RETURN_OK()
    }
  }
  
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method:  GetText
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StreamLoader, GetText )
{
  CCI_INSTANCE( StreamLoader )
  
  const char_t* attribute = CCI_P(attribute);

  if(_This->mStringList.IsValid() && attribute!=NULL) 
  {
    dm_param pos = 0;
    if( (pos = _This->mStringList.FindString(attribute,pos)) != 0 ) {
         pos = _This->mStringList.GetNext(pos);

      if(pos) {
         CCI_RETVAL_P(value) = _This->mStringList.GetString(pos);
         CCI_RETURN_OK()
      }
    }
  }

  CCI_RETURN_FAIL();
}
//---------------------------------------------------------------------
// Method:  SetText
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StreamLoader, SetText )
{
  CCI_INSTANCE( StreamLoader )

  const char_t* attribute = CCI_P(attribute);
  
  if(attribute == NULL) { // reset text infos 
     if(_This->mStringList.IsValid())
        _This->mStringList.Clear();
  }
  else if( CCI_P(value) != NULL ) 
  {
    if(!_This->mStringList.IsValid())
        _This->mStringList.Create();

    _This->mStringList.AddTail(attribute);
    _This->mStringList.AddTail(CCI_P(value));

    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL();
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( StreamLoader )
  CCI_REGISTER_METHOD_PP( StreamLoader, Read    )
  CCI_REGISTER_METHOD_PP( StreamLoader, Write   )
  CCI_REGISTER_METHOD_PP( StreamLoader, GetText )
  CCI_REGISTER_METHOD_PP( StreamLoader, SetText )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( StreamLoader )
{
  CCI_INVOKE_FACTORY(StreamLoader);
}
//---------------------------------------------------------------------
