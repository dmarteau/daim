
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

#include "daim_utilities.h"
#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_StringList.h"
//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_StringList
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_StringList_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#define CCI_INTERNAL_MODULE
#include "daim_modules/CCI_Base.h"
#include "daim_modules/CCI_Interface.h"
//--------------------------------------------------------------------

typedef dmTList<dmString>            STRLIST;
typedef dmTList<dmString>::iterator  ITERATOR;

CCI_DECL_DATA( StringList )
{
  STRLIST _List;
  dm_uint _Count;
};
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( StringList )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( StringList )
  CCI_INIT_MEMBER( _List     )

  _This->_Count = 0;

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( StringList )
{
  CCI_INSTANCE( StringList )

  _This->_List.Clear();
  _This->_Count = 0;

  CCI_DESTROY_MEMBER( _List     )
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( StringList )
  CCI_ATTR_GET( StringList, Count, CCIA_RETVAL = _This->_Count ) 
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( StringList )
  CCI_ATTR_SET( StringList, Count, CCIA_DATA =  _This->_Count ) 
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method: AddTail
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( StringList, AddTail )
{ 
  CCI_INSTANCE( StringList )

  _This->_List.Push_Back(CCI_P(str));
  CCI_Set(_THIS_OBJECT,CCIA_DECL_(StringList,Count),++_This->_Count);

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: AddHead
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( StringList, AddHead )
{ 
  CCI_INSTANCE( StringList )

  _This->_List.Push_Front(CCI_P(str));
  CCI_Set(_THIS_OBJECT,CCIA_DECL_(StringList,Count),++_This->_Count);

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: Insert
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( StringList, Insert )
{ 
  CCI_INSTANCE( StringList )

  if(CCI_P(pos)) 
  {
    ITERATOR _I = ITERATOR(reinterpret_cast<STRLIST::_Nt*>(CCI_P(pos)));

    _This->_List.Insert(_I,CCI_P(str));
    CCI_Set(_THIS_OBJECT,CCIA_DECL_(StringList,Count),++_This->_Count);
 
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Remove
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( StringList, Remove )
{ 
  CCI_INSTANCE( StringList )

  if(CCI_P(pos) && !_This->_List.Empty()) 
  {
    ITERATOR _I = ITERATOR(reinterpret_cast<STRLIST::_Nt*>(CCI_P(pos)));

    _This->_List.Remove(_I);
    CCI_Set(_THIS_OBJECT,CCIA_DECL_(StringList,Count),--_This->_Count);

    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Clear
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( StringList, Clear )
{ 
  CCI_INSTANCE( StringList )

  _This->_List.Clear();
  _This->_Count = 0;

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: GetFirst
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( StringList, GetFirst )
{ 
  CCI_INSTANCE( StringList )

  if(!_This->_List.Empty()) {
    CCI_RETVAL_P(pos) =  dm_ptr_to_long(_This->_List.Begin()._Node());
  } else
    CCI_RETVAL_P(pos) = dm_null;

  CCI_RETURN_OK()

}
//---------------------------------------------------------------------
// Method: GetNext
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( StringList, GetNext )
{ 
  CCI_INSTANCE( StringList )

  if(!_This->_List.Empty()) 
  {
    if(CCI_P(pos)==dm_null) 
      CCI_RETVAL_P(next) = dm_ptr_to_long(_This->_List.Begin()._Node());
    else 
    {
      ITERATOR _I = ITERATOR(reinterpret_cast<STRLIST::_Nt*>(CCI_P(pos)));

      if(++_I!=_This->_List.End()) 
        CCI_RETVAL_P(next) = dm_ptr_to_long((_I)._Node());
      else
        CCI_RETVAL_P(next) = dm_null;
    }
  } else
    CCI_RETVAL_P(next) = dm_null;

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: GetString
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( StringList, GetString )
{ 
  CCI_INSTANCE( StringList )

  if(CCI_P(pos) && !_This->_List.Empty()) 
  {
    ITERATOR _I = ITERATOR(reinterpret_cast<STRLIST::_Nt*>(CCI_P(pos)));
    CCI_RETVAL_P(str) = _I->Get();

    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Copy
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( StringList, Copy )
{ 
  CCI_INSTANCE( StringList )

  _This->_List.Clear();
  CCI_RETURN_IMETHOD_CLASS_PP_(_THIS_CLASS,_THIS_OBJECT,
                         StringList,Append, _in CCI_P(strlist));
}
//---------------------------------------------------------------------
// Method: Append
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StringList, Append )
{ 
  CCI_INSTANCE( StringList )

  cci_Object* _ToCopy = CCI_P(strlist);

  if(_ToCopy && CCI_ObjectType(CCIC_StringList,_ToCopy))
  {
    CCI_INSTANCE_PTR(StringList) 
    _XData = CCI_GetInstData(CCI_DECL_DATA(StringList),_THIS_CLASS,_ToCopy);
   
    ITERATOR _it  = _XData->_List.Begin();
    ITERATOR _lst = _XData->_List.End();

    STRLIST& _List = _This->_List;

    for(;_it!=_lst;++_it) {
      _List.Push_Back(*_it);
      ++_This->_Count;
    }
    CCI_Set(_THIS_OBJECT,CCIA_DECL_(StringList,Count),_This->_Count);
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: AddStrings
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( StringList, AddStrings )
{ 
  CCI_INSTANCE( StringList )

  if(CCI_P(strlist)!=NULL) 
  {
    CONST char_t** strlist = CCI_P(strlist);
    dm_uint        count   = CCI_P(count);

    for(dm_uint cnt=0;*strlist!=NULL && cnt < count ;++strlist,++cnt) 
    {
      _This->_List.Push_Back(*strlist);
      ++_This->_Count;
    }
    CCI_Set(_THIS_OBJECT,CCIA_DECL_(StringList,Count),_This->_Count);
    CCI_RETURN_OK()
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method: GetStringAt
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( StringList, GetStringAt )
{
  CCI_INSTANCE( StringList )

  ITERATOR _it  = _This->_List.Begin();
  ITERATOR _lst = _This->_List.End();

  for(dm_uint i=0;i<_This->_Count;++i,++_it) 
  {
    if(_it == _lst) {
      CCI_RETURN_FAIL() 
    }

    if(i==CCI_P(index)) {
      CCI_RETVAL_P(str) = (*_it).Get();
      CCI_RETURN_OK() 
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: FindString
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( StringList, FindString )
{
  CCI_INSTANCE( StringList )

  if(CCI_P(str)==NULL)
    CCI_RETURN_INVALID_PARAM()

  if(!_This->_List.Empty()) 
  {
    ITERATOR _I;

    if(CCI_P(pos)==dm_null) {
      _I = _This->_List.Begin();
    } else
      _I = ITERATOR(reinterpret_cast<STRLIST::_Nt*>(CCI_P(pos)));

    for(;_I!=_This->_List.End();++_I) {
      if((*_I)==CCI_P(str)) {
         CCI_RETVAL_P(found) = dm_ptr_to_long((_I)._Node());
         CCI_RETURN_OK();
      }
    }
  }
 
  CCI_RETVAL_P(found) = dm_null;
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: GetStringA
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( StringList, GetStringA )
{ 
  CCI_INSTANCE( StringList )

  if(CCI_P(pos) && !_This->_List.Empty()) {
    ITERATOR _I = ITERATOR(reinterpret_cast<STRLIST::_Nt*>(CCI_P(pos)));
   *CCI_RETVAL_P(str) = *_I;
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: AddTailA
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( StringList, AddTailA )
{ 
  CCI_INSTANCE( StringList )

  _This->_List.Push_Back(*CCI_P(str));
  CCI_Set(_THIS_OBJECT,CCIA_DECL_(StringList,Count),++_This->_Count);
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: AddHead
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( StringList, AddHeadA )
{ 
  CCI_INSTANCE( StringList )

  _This->_List.Push_Front(*CCI_P(str));
  CCI_Set(_THIS_OBJECT,CCIA_DECL_(StringList,Count),++_This->_Count);
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: InsertA
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( StringList, InsertA )
{ 
  CCI_INSTANCE( StringList )

  if(CCI_P(pos)) {
    ITERATOR _I = ITERATOR(reinterpret_cast<STRLIST::_Nt*>(CCI_P(pos)));
    _This->_List.Insert(_I,*CCI_P(str));
    CCI_Set(_THIS_OBJECT,CCIA_DECL_(StringList,Count),++_This->_Count);
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( StringList )

  CCI_REGISTER_METHOD_PP(StringList, GetFirst   )
  CCI_REGISTER_METHOD_PP(StringList, GetNext    )
  CCI_REGISTER_METHOD_PP(StringList, GetString  )
  CCI_REGISTER_METHOD_PP(StringList, AddTail    )
  CCI_REGISTER_METHOD_PP(StringList, AddHead    )
  CCI_REGISTER_METHOD_PP(StringList, Insert     )
  CCI_REGISTER_METHOD_PP(StringList, Remove     )
  CCI_REGISTER_METHOD_NP(StringList, Clear      )
  CCI_REGISTER_METHOD_PP(StringList, Copy       )
  CCI_REGISTER_METHOD_PP(StringList, Append     )
  CCI_REGISTER_METHOD_PP(StringList, AddStrings )
  CCI_REGISTER_METHOD_PP(StringList, GetStringAt)
  CCI_REGISTER_METHOD_PP(StringList, FindString )

  CCI_REGISTER_METHOD_PP(StringList, AddTailA   )
  CCI_REGISTER_METHOD_PP(StringList, AddHeadA   )
  CCI_REGISTER_METHOD_PP(StringList, GetStringA )
  CCI_REGISTER_METHOD_PP(StringList, InsertA    )

CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_METHOD StringList_Initialize( cci_Class* cl, void* )
{
  // Make that class resident
  CCI_Flush(cl,~0);
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
CCI_ENTRY_POINT(StringList)
{
  CCI_INVOKE_FACTORY_(StringList,NULL,StringList_Initialize,NULL);
}
//---------------------------------------------------------------------
