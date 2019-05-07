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
#include "daim_kernel.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/processing/CCI_KernelFamilies.h"

//------------------------------------------------------------------
#define CCIC_NAME  CCIC_KernelFamilies
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_KernelFamilies_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"

#include "dmKernelsMngr.h"
#include "dmKernelsLoader.h"

CCI_DECL_DATA(KernelFamilies)
{
  dmKernelsMngr  KernelsMngr;
  dmStrStream    StrBuffer;
  dm_bool        Initialized;
};
//--------------------------------------------------------------------
static dmIKernelFamilies DAIM_KernelFamilies;
//--------------------------------------------------------------------
static inline dmKernelFamily* KernelFamilies_GetFamily(
              CCI_INSTANCE_PTR(KernelFamilies) _This,
              const dmString::E* _kernel )
{
  dmKernelsSet::Element* pElement = _This->KernelsMngr.Find_Family(_kernel);
  if(pElement) return &pElement->family;
  return NULL;
}

//--------------------------------------------------------------------
// ME_NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( KernelFamilies )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE(KernelFamilies)
  CCI_INIT_MEMBER(StrBuffer)
  CCI_INIT_MEMBER(KernelsMngr)

  _This->Initialized = dm_false;

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// ME_DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( KernelFamilies )
{
  CCI_INSTANCE(KernelFamilies)
  CCI_DESTROY_MEMBER(KernelsMngr)
  CCI_DESTROY_MEMBER(StrBuffer)
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( KernelFamilies )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( KernelFamilies )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method: AddKernels 
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( KernelFamilies, AddKernels )
{
  CCI_INSTANCE(KernelFamilies)
  if(_This->KernelsMngr.LoadFile(CCI_P(file))) 
      CCI_RETURN_OK()
    else
      CCI_RETURN_IOERROR()    
}
//---------------------------------------------------------------------
// Method: LoadConfig 
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( KernelFamilies, LoadConfig )
{
  CCI_INSTANCE(KernelFamilies)
  dmKernelsMngr& mngr = _This->KernelsMngr;
  if(mngr.LoadConfig())
    CCI_RETURN_OK()

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetFamilies 
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( KernelFamilies, GetFamilies )
{
  CCI_INSTANCE(KernelFamilies)
  _This->StrBuffer.Clear();

  dmStrStream path;

  if(CCI_P(set) && CCI_P(set[0])!=_TXT('\0')) {
    dmKernelsMngr::iterator  it = _This->KernelsMngr.Find_Set(CCI_P(set));
    if(it!=_This->KernelsMngr.end()) {
      for(int i=0;i<(*it)->Size();++i) {
        _This->StrBuffer << (**it)[i]->name << _TXT('\t') << (**it)[i]->title;
        _This->StrBuffer << _TXT("\n");
      }
    }
  } else if(_tcscmp(CCI_P(set),_TXT("*"))==0) {
    dmKernelsMngr::iterator  it  = _This->KernelsMngr.begin();
    dmKernelsMngr::iterator last = _This->KernelsMngr.end();
    for(;it!=last;++it) {
      path.Clear();
      path << _TXT("./") << (*it)->GetName() << _TXT("::") << (*it)->GetTitle() << _TXT('/');
      for(int i=0;i<(*it)->Size();++i) {
        _This->StrBuffer << (path << (**it)[i]->name << _TXT('\t') << (**it)[i]->title).Str();
        _This->StrBuffer << _TXT("\n");
      }
    }
  } else {
    dmKernelsMngr::iterator  it  = _This->KernelsMngr.begin();
    dmKernelsMngr::iterator last = _This->KernelsMngr.end();
    for(;it!=last;++it) {
      _This->StrBuffer << (*it)->GetName() << _TXT('\t') << (*it)->GetTitle();
      _This->StrBuffer << _TXT("\n");
    }
  }
  CCI_RETVAL_P(data) = _This->StrBuffer.CStr();
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: KernelInfo
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( KernelFamilies, KernelInfo )
{
  CCI_INSTANCE(KernelFamilies)

  if(CCI_P(family)) {
    dmKernelFamily* _family = KernelFamilies_GetFamily(_This,CCI_P(family));
    if( _family ) {
      if( CCI_P(index)>=0 && (CCI_P(index) < static_cast<int>(_family->Size())) )
      {
         dmStructuringElement& el = (*_family)[CCI_P(index)];
         CCI_RETVAL_P(x)      = el.Ox();
         CCI_RETVAL_P(y)      = el.Oy();
         CCI_RETVAL_P(width)  = el.Width();
         CCI_RETVAL_P(height) = el.Height(); 
         CCI_RETVAL_P(kernel) = el.Data();

         CCI_RETURN_OK()
      } 
    } else
      CCI_RETURN_FAIL()
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method: GetInstance
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( KernelFamilies, GetInstance )
{
  if(!DAIM_KernelFamilies.IsValid()) 
  {
    DAIM_KernelFamilies.Create();
    if(!DAIM_KernelFamilies.LoadConfig())
       dmLOG(_TXT("ERROR: Failed to load Kernels\n"));
  }
  // Addref for the caller
  DAIM_KernelFamilies.AddRef();
  return dm_ptr_to_long( dmIINTERFACE_PTR(DAIM_KernelFamilies) ); 
}
//---------------------------------------------------------------------
// Method: FindFamily
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( KernelFamilies, FindFamily )
{
  CCI_INSTANCE(KernelFamilies)
  if(CCI_P(name) && CCI_P(name[0])!=_TXT('\0')) {
    dmKernelFamily* _family = KernelFamilies_GetFamily(_This,CCI_P(name));
    if(_family) {
      CCI_RETVAL_P(family) = _family;
      CCI_RETURN_OK()
    } else
     CCI_SET_ERROR_MSG(
       dmString::FormatString(dmString::npos,_TXT("Cannot find kernel family %s !"),
                              CCI_P(name)).CStr()
     );
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( KernelFamilies )
  CCI_REGISTER_METHOD_PP( KernelFamilies, FindFamily  )
  CCI_REGISTER_METHOD_PP( KernelFamilies, AddKernels  )
  CCI_REGISTER_METHOD_NP( KernelFamilies, LoadConfig  )  
  CCI_REGISTER_METHOD_PP( KernelFamilies, GetFamilies )
  CCI_REGISTER_METHOD_PP( KernelFamilies, KernelInfo  )
  CCI_REGISTER_METHOD_NP( KernelFamilies, GetInstance )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
void KernelFamilies_Finalize(cci_Class* cl,void* _UserData) 
{ 
  if(DAIM_KernelFamilies.IsValid()) {
     DAIM_KernelFamilies.Release();
     DAIM_KernelFamilies.Detach();
  }
}
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( KernelFamilies )
{
  CCI_INVOKE_FACTORY_(KernelFamilies,NULL,NULL,KernelFamilies_Finalize)
}
//---------------------------------------------------------------------
