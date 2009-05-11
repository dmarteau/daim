
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
#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_StringList.h"
#include "daim_modules/CCI_Properties.h"
//

#define CCIC_NAME  CCIC_Properties
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR "David Marteau"
#define CCI_VERSION   CCI_Properties_Version
#define CCI_REVISION  3
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2008"))
//-------------------------------------------------------------------
#define CCI_INTERNAL_MODULE
#include "daim_modules/CCI_Base.h"
#include "daim_modules/CCI_Interface.h"

#define NVT_STRING  1
#define NVT_INT     2
#define NVT_REAL    3
#define NVT_OBJECT  4


//--------------------------------------------------------------------
struct Properties_NODE
{
  dmString _Key;
  dm_uint  _Flags;
  dmString _Value;

  union {
	dm_real      _RealValue;  
	dm_int       _IntValue;
	cci_Object*  _CplxValue;  
  };
  
 ~Properties_NODE() {}
  Properties_NODE() : _Flags(0) {}
  
  template<class T>
  Properties_NODE( const dmString& key,  T _val ) 
  : _Key(key)
  {
    _Key.Trim();
    SetValue(_val);
  }

  Properties_NODE( const dmString& key,  const char_t* _val ) 
  : _Key(key)
  {
    _Key.Trim();
    SetValue(_val);
  }

  void SetValue( const char_t* _val ) { _Flags = NVT_STRING; _Value.Assign(_val); }
  void SetValue( dm_real _val       ) { _Flags = NVT_REAL;   _RealValue = _val;   }
  void SetValue( dm_int _val        ) { _Flags = NVT_INT;    _IntValue  = _val;   }

  void SetValue( cci_Object* _val ) 
  {
    _Flags     = NVT_OBJECT;
    _CplxValue = _val;
   
    if(_CplxValue)
       CCI_AddRef(_CplxValue);
  }
  
  void ReleaseComplexValue() 
  {
    if(_Flags == NVT_OBJECT && _CplxValue)
       CCI_Release(_CplxValue);
   
    _CplxValue = dm_null;
  }
  
  bool isA( dm_uint _aType ) { return _aType == _Flags; }

};

typedef daim::hashmap<dmString,Properties_NODE>   PROPDICT;
//--------------------------------------------------------------------
CCI_DECL_DATA( Properties )
{
  dmString _Root;
  dmString _Buffer;
  PROPDICT _Properties;
};

typedef CCI_INSTANCE_PTR(Properties) _Instance_Ptr;

//--------------------------------------------------------------------
static Properties_NODE* Properties_GetProperty( _Instance_Ptr _This, 
		                                           const char_t* _Property )
{
  _This->_Buffer.Assign(_This->_Root);
  _This->_Buffer.Append(_Property);
  
  PROPDICT& dict = _This->_Properties;
  PROPDICT::iterator it = dict.find(_This->_Buffer);
  if(it!=dict.end()) 
     return &(*it).second;

  return dm_null;
}
//--------------------------------------------------------------------
template<class T>
static void Properties_SetProperty( _Instance_Ptr _This, 
		                               const char_t* _Property,
                                   T _Value )
{
   _This->_Buffer.Assign(_This->_Root);
   _This->_Buffer.Append(_Property);
   
   PROPDICT& dict = _This->_Properties;
   PROPDICT::iterator it = dict.find(_This->_Buffer);
   if(it!=dict.end()) {
     (*it).second.ReleaseComplexValue();
     (*it).second.SetValue(_Value);
   } 
   else {
   	 Properties_NODE _node(_Property,_Value);
     _This->_Properties[_node._Key] = _node;
   }
}
//--------------------------------------------------------------------
static void Properties_DeleteProperty( _Instance_Ptr _This, 
                                      const char_t* _Property )
{
  _This->_Buffer.Assign(_This->_Root);
  _This->_Buffer.Append(_Property);
  
  PROPDICT& dict = _This->_Properties;
  PROPDICT::iterator it = dict.find(_This->_Buffer);
  
  if(it!=dict.end()) {
    (*it).second.ReleaseComplexValue();
    dict.erase(it);
  } 
}
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR(  Properties )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( Properties )

  PROPDICT::iterator it   = _This->_Properties.begin();
  PROPDICT::iterator last = _This->_Properties.end();
  
  for(;it!=last;++it)
      (*it).second.ReleaseComplexValue(); 
  
  CCI_INIT_MEMBER( _Root       )
  CCI_INIT_MEMBER( _Properties )
  
  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR(  Properties )
{
  CCI_INSTANCE( Properties) 

  CCI_DESTROY_MEMBER( _Properties )
  CCI_DESTROY_MEMBER( _Root       )

  CCI_DESTROY_SUPER()
}
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( Properties )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( Properties )
{
  CCI_RETURN_INVOKE_SUPER()
}
*/
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Properties )
  CCI_ATTR_GET( Properties, Root, CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->_Root.Get()) ) 
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Properties )
  CCI_ATTR_SET( Properties, Root, 
     _This->_Root.Assign(CCIA_TRANSLATE_DATA(CONST char_t*));
  ) 
CCI_END_SETTER()

//---------------------------------------------------------------------
// Method: GetChildList
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Properties, GetChildList )
{
  CCI_INSTANCE( Properties)

  dmIStringList _Strings;
  if(!_Strings.QueryInterface(CCI_P(strlist)))
     CCI_RETURN_INVALID_PARAM()

  PROPDICT::iterator it   = _This->_Properties.begin();
  PROPDICT::iterator last = _This->_Properties.end();

  dmString& buf = _This->_Buffer;
  
  buf.Assign(_This->_Root);
  buf.Append(CCI_P(startingAt));
  
  if(!buf.Empty() && buf[buf.Size()-1] != '.')
	    buf.Append(".");
		  
  for(;it!=last;++it) 
  {
    Properties_NODE& node = (*it).second; 
    if(buf.Empty()||(node._Key.Compare(0,buf.Size(),buf)==0))
       _Strings.AddTailA( node._Key );
  }

  CCI_RETURN_OK() 
}
//---------------------------------------------------------------------
// Method: GetChildList
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Properties, ClearBranch )
{
  CCI_INSTANCE(Properties)

  dmString& buf = _This->_Buffer;
  
  buf.Assign(_This->_Root);
  buf.Append(CCI_P(startingAt));
  
  if(buf.Empty()) {
	 _This->_Properties.clear();  
  } else {
    PROPDICT& dict = _This->_Properties;
    
    PROPDICT::iterator it   = dict.begin();
    PROPDICT::iterator last = dict.end();
  
    if(buf[buf.Size()-1] != '.')
       buf.Append(".");
  
    while(it!=last) 
    {
      Properties_NODE& node = (*it).second; 
      if(node._Key.Compare(0,buf.Size(),buf)==0) {
         node.ReleaseComplexValue(); 
         dict.erase(it++);
      } else
        ++it;
    }
  }

  CCI_RETURN_OK() 
}
//---------------------------------------------------------------------
// Method: Data
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Properties, StringData )
{
  CCI_INSTANCE( Properties )
 
  if(!EMPTY_STRING(CCI_P(property))) 
  {
    switch(CCI_P(me)) 
    {
      case CCI_ME_SET :
        Properties_SetProperty(_This,CCI_P(property),CCI_P(value));
        CCI_RETURN_OK();
        break;
  
      case CCI_ME_GET : {
        Properties_NODE* node = Properties_GetProperty(_This,CCI_P(property));
        if(node && node->isA(NVT_STRING)) {
           CCI_RETVAL_P(value) = node->_Value.Get();
           CCI_RETURN_OK();
        }
      }
      break;

      case CCI_ME_DISPOSE: 
        Properties_DeleteProperty(_This,CCI_P(property));
        CCI_RETURN_OK();
        break;
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: IntData
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Properties, IntData )
{
  CCI_INSTANCE( Properties )
   
  dm_param      me        =  CCI_P(me);
  CONST char_t* property  =  CCI_P(property);
  dm_int        value     =  CCI_P(value); 
 
  switch(CCI_P(me)) 
  {     
     case CCI_ME_SET: {
       Properties_SetProperty(_This,CCI_P(property),value);
       CCI_RETURN_OK();
     } break;
     
     case CCI_ME_GET: {
       Properties_NODE* node = Properties_GetProperty(_This,CCI_P(property));
       if(node && node->isA(NVT_INT)) {
          CCI_RETVAL_P(value) = node->_IntValue;
          CCI_RETURN_OK();
       }
     } break;

     case CCI_ME_DISPOSE: 
       Properties_DeleteProperty(_This,CCI_P(property));
       CCI_RETURN_OK();
       break;
  }
  
  CCI_RETURN_FAIL()  
}
//---------------------------------------------------------------------
// Method: RealData
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Properties, RealData )
{
  CCI_INSTANCE( Properties )
   
  dm_param      me        =  CCI_P(me);
  CONST char_t* property  =  CCI_P(property);
  dm_real       value     =  CCI_P(value); 
 
  switch(CCI_P(me)) 
  {     
    case CCI_ME_SET: {
      Properties_SetProperty(_This,CCI_P(property),value);
      CCI_RETURN_OK();
    } break;

    case CCI_ME_GET: {
      Properties_NODE* node = Properties_GetProperty(_This,CCI_P(property));
      if(node && node->isA(NVT_REAL)) {
         CCI_RETVAL_P(value) = node->_RealValue;
         CCI_RETURN_OK();
      }
    } break;
     
    case CCI_ME_DISPOSE: 
      Properties_DeleteProperty(_This,CCI_P(property));
      CCI_RETURN_OK();
      break;
  }
  
  CCI_RETURN_FAIL()  
}
//---------------------------------------------------------------------
// Method: ObjectData
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Properties, ObjectData )
{
  CCI_INSTANCE( Properties )
   
  dm_param      me        =  CCI_P(me);
  CONST char_t* property  =  CCI_P(property);
  cci_Object*   value     =  CCI_P(value); 
 
  switch(CCI_P(me)) 
  {     
    case CCI_ME_SET: {
      Properties_SetProperty(_This,CCI_P(property),value);
      CCI_RETURN_OK();
    } break;

    case CCI_ME_GET: {
      Properties_NODE* node = Properties_GetProperty(_This,CCI_P(property));
      if(node && node->isA(NVT_OBJECT)) 
      {
         if(node->_CplxValue) 
        	CCI_AddRef(node->_CplxValue);
         
         CCI_RETVAL_P(value) = node->_CplxValue;
         CCI_RETURN_OK();
      }
    } break;
     
    case CCI_ME_DISPOSE: 
      Properties_DeleteProperty(_This,CCI_P(property));
      CCI_RETURN_OK();
      break;
  }
  
  CCI_RETURN_FAIL()  
}

//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP(Properties)
  CCI_REGISTER_METHOD_PP( Properties, GetChildList      )
  CCI_REGISTER_METHOD_PP( Properties, ClearBranch       ) 
  CCI_REGISTER_METHOD_PP( Properties, StringData        )
  CCI_REGISTER_METHOD_PP( Properties, IntData           )
  CCI_REGISTER_METHOD_PP( Properties, RealData          )
  CCI_REGISTER_METHOD_PP( Properties, ObjectData        )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Properties )
{
  CCI_INVOKE_FACTORY(Properties)
}
//---------------------------------------------------------------------
