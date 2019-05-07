
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

#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "images/dmGenericLoader.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_LoaderFactory.h"
#include "daim_modules/CCI_PropertyList.h"
#include "daim_modules/CCI_StringList.h"
#include "daim_modules/CCI_Properties.h"
#include "daim_modules/CCI_InputStream.h"
#include "daim_modules/CCI_OutputStream.h"
#include "daim_modules/CCI_FileInputStream.h"
#include "daim_modules/CCI_FileOutputStream.h"
#include "daim_modules/CCI_Loader.h"

#include "daim_modules/CCI_Interface.h"


//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_LoaderFactory
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_LoaderFactory_Version
#define CCI_REVISION  1
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"

#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"  // for dmIRServices
#include "dmModuleLoader.h"

//--------------------------------------------------------------------
typedef daim::hashmap<dmString,dmString>    _hash_str_t;

CCI_DECL_DATA( LoaderFactory )
{
  _hash_str_t _mimes; 
  _hash_str_t _descr; 
};

typedef CCI_INSTANCE_PTR(LoaderFactory) _Instance_Ptr;
//--------------------------------------------------------------------
// LoaderFactory constructor
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( LoaderFactory )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( LoaderFactory )

  CCI_INIT_MEMBER(_mimes)
  CCI_INIT_MEMBER(_descr)

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// LoaderFactory destructor
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( LoaderFactory )
{
  CCI_INSTANCE( LoaderFactory )
  CCI_DESTROY_MEMBER(_mimes)
  CCI_DESTROY_MEMBER(_descr)
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// Getters
//---------------------------------------------------------------------
static inline CCI_METHOD_GET(LoaderFactory )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Setters
//---------------------------------------------------------------------
static inline CCI_METHOD_SET(LoaderFactory )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method    : Register
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( LoaderFactory, Register )
{
  CCI_UNUSUED_INSTANCE( LoaderFactory )

  // Register a new cci class as loader for 
  // the specified mime type

  if(EMPTY_STRING(CCI_P(loader))||EMPTY_STRING(CCI_P(mime))) 
     CCI_RETURN_FAIL()

  dmLoaderFactory* factory = dmLoaderFactory::Instance();
  if(factory) 
  {
    // Create unique id for our  loader/saver
    dmId _id = dmId::GetId();

    // Add loader 
    factory->AddLoaderDescription(
        CCI_P(mime), 
        CCI_P(loader),
        _id,
        &dmModuleLoader::CreateInstance,
        CCI_P(flags)
    );  
    
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
} 
//---------------------------------------------------------------------
// Method    : AddMimeExt
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( LoaderFactory, AddMimeExt )
{
  if( EMPTY_STRING(CCI_P(file_ext)) || EMPTY_STRING(CCI_P(mime)) )
    CCI_RETURN_FAIL()

  CCI_INSTANCE( LoaderFactory )

  dmString file_ext =  CCI_P(file_ext); 
  dmString ext;

  for(size_t i=0,pos = 0;pos!=dmString::npos;++i) 
  {
    pos = file_ext.Split(ext,pos,"|"); 
    if(!ext.Empty()) {
      _This->_mimes[ext] = CCI_P(mime);
    }
  }

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method    : GetMimeType
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( LoaderFactory, GetMimeType )
{
  if(EMPTY_STRING(CCI_P(filename)))
    CCI_RETURN_INVALID_PARAM()

  CCI_INSTANCE( LoaderFactory )

  // extract file extension
  dmString ext = dmString(CCI_P(filename)).FileExt();

  ext.ToLower();

  _hash_str_t::iterator it = _This->_mimes.find(ext.CStr());
  if(it!=_This->_mimes.end()) {
    CCI_RETVAL_P(mime) = (*it).second.CStr();
    CCI_RETURN_OK()
  }
  CCI_RETVAL_P(mime) = NULL;

  CCI_RETURN_FAIL()
} 
//---------------------------------------------------------------------
// Helpers
//---------------------------------------------------------------------
static void LoaderFactory_GetModuleMimes(
                     cci_Object* _This_ob, 
                     dmIStringList& _list, cci_Class* cl, const char_t* _clsid, 
                     dm_uint flags )
{
   _list.Clear();
   if(CCI_SUCCEEDED(CCI_InvokeService(cl,Loader,GetMimeTypes, 
                         _in dmIINTERFACE_PTR(_list) _in flags)))
   {
     dm_param pos = dm_null;
     while( (pos =_list.GetNext(pos)) ) 
     {
       CCI_Invoke(_This_ob,LoaderFactory,Register,
         _in _list.GetString(pos)
         _in _clsid 
         _in flags 
       );
     }
   }
}
//---------------------------------------------------------------------
// Load self registering modules
//---------------------------------------------------------------------
static void LoaderFactory_RegisterModules(_Instance_Ptr _This, 
                                          cci_Object* _This_ob,
                                          dm_param key,
                                          dmIStringList&   _list, 
                                          dmIPropertyList& _IPropList )
{
  if(_IPropList.GetSeq(key,_TXT("loaders:register")))
  {
    dmString _clsid;
    while(_IPropList.NextSeq(key)) 
    {
                     _clsid      = _IPropList.GetPropertyStr (key,NULL,_TXT("loaders:module"));
      const char_t*  _properties = _IPropList.GetPropertyStr (key,NULL,_TXT("loaders:urn"));
      const dm_int32 _clsVersion = _IPropList.GetPropertyLong(key,NULL,_TXT("loaders:version"),-1);

      if(!EMPTY_STRING(_properties)) { // Asked to look in properties file
         dmIInterface<dmIProperties> _IProps;
         if(_IProps.Open(_properties,_TXT("System"))) {
           _clsid = _IProps.GetData(NULL,_clsid.CStr());
         }
      }

      if(!_clsid.Empty()) {
         dmIModuleClass cl(_clsid.CStr(),_clsVersion);
         if(cl.IsValid()) {
           LoaderFactory_GetModuleMimes(_This_ob,_list,cl,_clsid.CStr(),CCIV_Loader_Read);
           LoaderFactory_GetModuleMimes(_This_ob,_list,cl,_clsid.CStr(),CCIV_Loader_Write);
         } 
      }
    }

    _IPropList.EndSeq(key);
  }
}
//---------------------------------------------------------------------
// Load mimes types definitions
//---------------------------------------------------------------------
void LoaderFactory_RegisterMimes(_Instance_Ptr _This,
                                 cci_Object* _This_ob, dm_param key, 
                                 dmIStringList&   _list,  
                                 dmIPropertyList& _IPropList )
{
  if(_IPropList.GetSeq(key,_TXT("media-types:list")))
  {
    while(_IPropList.NextSeq(key)) 
    {
      const char_t* _type    = _IPropList.GetPropertyStr(key,NULL,"media-types:type");
      const char_t* _encoder = _IPropList.GetPropertyStr(key,NULL,"media-types:encoder");
      const char_t* _decoder = _IPropList.GetPropertyStr(key,NULL,"media-types:decoder");
      const char_t* _files   = _IPropList.GetPropertyStr(key,NULL,"media-types:files");
      const char_t* _descr   = _IPropList.GetPropertyStr(key,NULL,"media-types:description");

      if(!EMPTY_STRING(_type))
      {
        if(!EMPTY_STRING(_descr)) {
          _This->_descr[_type] = _descr;
        }

        if(!EMPTY_STRING(_files))
          CCI_Invoke(_This_ob,LoaderFactory,AddMimeExt,
            _in _type
            _in _files 
          );
         
        if(!EMPTY_STRING(_decoder))
          CCI_Invoke(_This_ob,LoaderFactory,Register,
            _in _type
            _in _decoder 
            _in CCIV_Loader_Read 
          );

        if(!EMPTY_STRING(_encoder))
          CCI_Invoke(_This_ob,LoaderFactory,Register,
            _in _type
            _in _encoder 
            _in CCIV_Loader_Write 
          );
      }
    }
    _IPropList.EndSeq(key);
  }

}
//---------------------------------------------------------------------
// Method    : LoadMimes
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( LoaderFactory, LoadMimes )
{
  CCI_INSTANCE( LoaderFactory )

  // Get DAIM Resident service for PropertyList
  dmIGetService<dmIPropertyList> _IPropList("daim://");

  dm_param key = _IPropList.Open("daim://","//daim/medias");

  if(key)  
  {
    if(!_IPropList.CheckProperty(key,NULL,"media-types:initialized"))
    {
      dmLoaderFactory* factory = dmLoaderFactory::Instance();
      dmASSERT( factory != NULL );

      factory->RemoveExternalLoaders();

      dmIInterface<dmIStringList> _strings;
      dmASSERT(_strings.IsValid());

      LoaderFactory_RegisterModules(_This,_THIS_OBJECT,key,_strings,_IPropList);
      LoaderFactory_RegisterMimes  (_This,_THIS_OBJECT,key,_strings,_IPropList);

      // Add this property so we can see the next time this method
      // is called if the file has been reloaded or not 
      // (in this case the property will be not set)
      _IPropList.SetProperty(key,NULL,"media-types:initialized",NULL);
    }
    _IPropList.Close(key);
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
} 
//---------------------------------------------------------------------
// Helpers
//---------------------------------------------------------------------
void LoaderFactory_GetMimeInfos(_Instance_Ptr _This, const char_t* mime, dm_param key, 
                                dmIPropertyList& _IPropList )
{
  dmString _files;
  const char_t* _descr   = NULL;
  const char_t* _encoder = NULL;
  const char_t* _decoder = NULL;

  // Collect files extensions
  _hash_str_t::iterator __F = _This->_mimes.begin();
  _hash_str_t::iterator __L = _This->_mimes.end();
  for(;__F!=__L;++__F)
    if((*__F).second == mime) {
      _files.Append((*__F).first);
      _files.Append(1,_TXT('|'));
  }

  // Get Description
  __F = _This->_descr.find(mime);
  if(__F!=_This->_descr.end())
     _descr = (*__F).second.CStr();

  _files.TrimRight(_TXT("|"));

  // Find encoder/decoder for this type
  dmLoaderFactory* factory = dmLoaderFactory::Instance();
  if(factory) 
  {
    dmLOADER_DESCRIPTION* pldrDesc;
    pldrDesc = factory->GetDescription(mime,dmENCODER);
    if(pldrDesc) 
      _encoder =  pldrDesc->ld_name.CStr();

    pldrDesc = factory->GetDescription(mime,dmDECODER);
    if(pldrDesc) 
      _decoder =  pldrDesc->ld_name.CStr();
  }

  _IPropList.SetProperty(key,NULL,_TXT("media-types:type")       ,mime);
  _IPropList.SetProperty(key,NULL,_TXT("media-types:files")      ,_files.CStr());
  _IPropList.SetProperty(key,NULL,_TXT("media-types:encoder")    ,_encoder);
  _IPropList.SetProperty(key,NULL,_TXT("media-types:decoder")    ,_decoder);
  _IPropList.SetProperty(key,NULL,_TXT("media-types:description"),_descr);
}
//---------------------------------------------------------------------
// Method    : GetMimeInfos
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( LoaderFactory, GetMimeInfos )
{
  CCI_INSTANCE( LoaderFactory )

  dmIPropertyList _IPropList;
  if(_IPropList.QueryInterface(CCI_P(propertylist)))
  {
    if(!EMPTY_STRING(CCI_P(mime))) {
      LoaderFactory_GetMimeInfos(_This,CCI_P(mime),CCI_P(key),_IPropList);
      CCI_RETURN_OK()
    } else {
      dmLoaderFactory* factory = dmLoaderFactory::Instance();
      dmASSERT( factory != NULL );

      dmLoaderFactory::iterator __F = factory->Begin();
      dmLoaderFactory::iterator __L = factory->End();

      dm_param key = CCI_P(key);

      _IPropList.NewSeq(key,NULL); // Create a new sequence

      for(;__F!=__L;++__F) {
        if( ((*__F).ld_type & CCI_P(flags) )!=0 ) 
        {
          _IPropList.AddSeq(key);
          LoaderFactory_GetMimeInfos(_This,(*__F).ld_mime.CStr(),key,_IPropList); 
        }
      }
      _IPropList.EndSeq(key);
      CCI_RETURN_OK();
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method    : GetMimeList
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( LoaderFactory, GetMimeList )
{
  dmIStringList _Strings;
  if(_Strings.QueryInterface(CCI_P(stringlist)))
  {
    dmLoaderFactory* factory = dmLoaderFactory::Instance();
    dmASSERT( factory != NULL );

    int count = 0;

    dmLoaderFactory::iterator __F = factory->Begin();
    dmLoaderFactory::iterator __L = factory->End();
    for(;__F!=__L;++__F) {
       if( ((*__F).ld_type & CCI_P(flags) )!=0 ) 
       {
         _Strings.AddTailA((*__F).ld_mime);
         ++count;
       }
    }
    if(count)
      CCI_RETURN_OK() 
  } 
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method    : GetMimeExt
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( LoaderFactory, GetMimeExt )
{
  CCI_INSTANCE( LoaderFactory )

  const char_t* _mime = CCI_P(mime);

  // Get the first file extension corresponding
  // to the given mime type 
  _hash_str_t::iterator __F = _This->_mimes.begin();
  _hash_str_t::iterator __L = _This->_mimes.end();
  for(;__F!=__L;++__F) {
    if((*__F).second == _mime) {
      CCI_RETVAL_P(ext) = (*__F).first.CStr();
      CCI_RETURN_OK()
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method    : GetLoader
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( LoaderFactory, GetLoader )
{
  CCI_INSTANCE( LoaderFactory )

  const char_t* _mime  = CCI_P(mime);
  
 // Find encoder/decoder for this type
  dmLoaderFactory* factory = dmLoaderFactory::Instance();
  if(factory) 
  {
    dmLOADER_DESCRIPTION* pldrDesc;
    pldrDesc = factory->GetDescription(_mime,CCI_P(flags));
       
    if(pldrDesc) 
    {
      // Just a check for existence
      if(dm_ptr_to_long(CCI_P(loader)) == dm_noindex)
         CCI_RETURN_OK()
      else {
        // Create the object   
        cci_Object* aLoader = CCI_NewObjectVA(pldrDesc->ld_name.CStr(),TGZ_DEFAULT,TGZ_DONE,NULL);
        if(aLoader) {
           CCI_AddRef(aLoader); // Addref for the caller 
           CCI_RETVAL_P(loader) = aLoader;
           CCI_RETURN_OK()
        }
      }  
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( LoaderFactory )
  CCI_REGISTER_METHOD_PP( LoaderFactory, Register     )
  CCI_REGISTER_METHOD_PP( LoaderFactory, AddMimeExt   )
  CCI_REGISTER_METHOD_PP( LoaderFactory, GetMimeType  )
  CCI_REGISTER_METHOD_NP( LoaderFactory, LoadMimes    )
  CCI_REGISTER_METHOD_PP( LoaderFactory, GetMimeInfos )
  CCI_REGISTER_METHOD_PP( LoaderFactory, GetMimeList  )
  CCI_REGISTER_METHOD_PP( LoaderFactory, GetMimeExt   )
  CCI_REGISTER_METHOD_PP( LoaderFactory, GetLoader    )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
static void LoaderFactory_Finalize( cci_Class* , void * )
{
  // Free Memory pool from dmModuleLoader
  dmModuleLoader::_MemPool.Free();
  dmLoaderFactory::FreeInstance();
}
//---------------------------------------------------------------------
CCI_ENTRY_POINT( LoaderFactory )
{
  DM_INIT_API()
  CCI_INVOKE_FACTORY_(LoaderFactory,NULL,NULL,LoaderFactory_Finalize)
}
//---------------------------------------------------------------------
