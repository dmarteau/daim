
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
#include "daim_modules/CCI_Properties.h"
#include "daim_modules/CCI_StringList.h"

/*
 * Last modification : 3/05/2006
 */

#define CCIC_NAME  CCIC_Properties
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR "David Marteau"
#define CCI_VERSION   CCI_Properties_Version
#define CCI_REVISION  2
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//-------------------------------------------------------------------
#define CCI_INTERNAL_MODULE
#include "daim_modules/CCI_Base.h"
#include "daim_modules/CCI_Interface.h"

#include <stdio.h>
#include <errno.h>

#define _BUFFER_SIZE 4096
#define _NULL_CHAR _TXT('\0')
#define _NULL_STR  _TXT("\0")

#define P_NODE_UNDEF_T  ~0UL  
#define P_NODE_COMMENT_T 0
#define P_NODE_SECTION_T 1
#define P_NODE_KEYVAL_T  2

//--------------------------------------------------------------------
struct Properties_NODE
{
  dmString _Key;
  dmString _Value;
  dm_uint  _Type;
};

typedef dmTList<Properties_NODE> Properties_LIST;
//--------------------------------------------------------------------
CCI_DECL_DATA( Properties )
{
  dmString _Section;
  dmString _File;
  dm_bool  _Modified;
  char_t   _Buffer[_BUFFER_SIZE];

  Properties_LIST _Content;
};

//--------------------------------------------------------------------
// Helpers
//--------------------------------------------------------------------
int Properties_OpenFile( CCI_INSTANCE_PTR(Properties) _This, const char* _path )
{
  if(!EMPTY_STRING(_path))
  {
    FILE* _file = _tfopen(_path,_TXT("r"));
    if(_file!=NULL) 
    {
      Properties_NODE _node;
      dmString _line;

      while(_fgetts(_This->_Buffer,_BUFFER_SIZE,_file)!=NULL)
      {
        _node._Type = P_NODE_UNDEF_T;
        _node._Value.Clear();

        _line  = _This->_Buffer;
        _line.Trim();

        dm_size pos = 0; 

        switch(_line[0]) 
        {
          case '\0': break;

          case '#' :
          case ';' :
            _node._Type = P_NODE_COMMENT_T;
            _node._Key  = _line;
            break;
       
          case '[':
            pos = _line.FindFirstOf(_TXT(']'),1);
            if(pos!=dmString::npos) {
              _node._Key  = _line.SubStr(1,pos-1);
              _node._Type = P_NODE_SECTION_T;
            }
            break;

          default:
            pos = _line.Parse(_node._Key  , pos,"=");
            pos = _line.Parse(_node._Value, pos,"=");
            _node._Type = P_NODE_KEYVAL_T;
            _node._Key.TrimRight();
            _node._Value.TrimLeft(); 
            break;
        }

        if(_node._Type!=P_NODE_UNDEF_T)
          _This->_Content.Push_Back(_node);
        
      }
      fclose(_file);
    } else 
      return errno;
  }
  return 0;
}
//--------------------------------------------------------------------
static Properties_LIST::iterator 
Properties_Find( CCI_INSTANCE_PTR(Properties) _This,
                 const char_t* _section,const char_t* _property )
{
  if(!EMPTY_STRING(_section))
  {
    Properties_LIST::iterator it   = _This->_Content.Begin();
    Properties_LIST::iterator last = _This->_Content.End();
   
    for(;it!=last;++it)
    {
      if( ((*it)._Type == P_NODE_SECTION_T) &&
          ((*it)._Key  == _section ))
      {
        if( _property == NULL ) return it;

        for(++it;it!=last;++it) {
          if((*it)._Type==P_NODE_SECTION_T) break; // Found a new section
          if((*it)._Type==P_NODE_KEYVAL_T && ((*it)._Key == _property)) 
            return it;
        }
        break;
      }
    }
  }

  return _This->_Content.End();
}
//--------------------------------------------------------------------
static bool Properties_ReadString( CCI_INSTANCE_PTR(Properties) _This, 
                                    const char* _section, const char* _property,
                                    const char* _value )
{
  Properties_LIST::iterator it = Properties_Find(_This,_section,_property);
  if(it!=_This->_Content.End()) 
    _value = (*it)._Value.CStr();

  if(_value==NULL) _value = _NULL_STR;
  _tcsncpy(_This->_Buffer,_value,_BUFFER_SIZE);    

  return true;
}
//--------------------------------------------------------------------
static bool Properties_WriteString( CCI_INSTANCE_PTR(Properties) _This, 
                                    const char* _section, const char* _property,
                                    const char* _value )
{
  if(EMPTY_STRING(_section) || EMPTY_STRING(_property))
    return false;

  if(_value!=NULL) {
    // First seek the section
    Properties_LIST::iterator it = Properties_Find(_This,_section,NULL);
    if(it==_This->_Content.End())
    {    
      Properties_NODE _node;

      // Create new section
      _node._Type = P_NODE_SECTION_T;
      _node._Key  = _section;
      _This->_Content.Push_Back(_node);
/*
      // Create new entry
      _node._Type  = P_NODE_KEYVAL_T;
      _node._Key   = _section;
      _node._Value = _value;
      _This->_Content.Push_Back(_node);
*/
      return true;
    }
    else
    { 
      for(++it;it!=_This->_Content.End();++it)
      {
        if((*it)._Type==P_NODE_SECTION_T) break; // Found a new section
        if((*it)._Type==P_NODE_KEYVAL_T && ((*it)._Key == _property)) {
          (*it)._Value = _value;
          return true;
        }
      }
      // Key has not been found, insert a new one
      Properties_NODE _node;
      _node._Type  = P_NODE_KEYVAL_T;
      _node._Key   = _property;
      _node._Value = _value;
      _This->_Content.Insert(it,_node);
      return true;
    }
  } 
  return false;
}
//--------------------------------------------------------------------
static bool Properties_WriteSection(CCI_INSTANCE_PTR(Properties) _This,
                                    const char* _section, dmIStringList& _Strings)
{
  if(EMPTY_STRING(_section))
    return false;

  Properties_NODE _node;

  Properties_LIST::iterator it = Properties_Find(_This,_section,NULL);
  if(it==_This->_Content.End())
  {    
    // Create new section
    _node._Type = P_NODE_SECTION_T;
    _node._Key  = _section;
    _This->_Content.Push_Back(_node);
    it = _This->_Content.Last();
  }

  dm_param pos    = dm_null;
  dm_size  strpos = 0;
  dmString line;
  while( (pos=_Strings.GetNext(pos)) ) 
  {
    if(_Strings.GetStringA(pos,line) && !line.Empty())
    { 
      ++it;
      strpos = line.Parse(_node._Key  , 0,"=");
      strpos = line.Parse(_node._Value, strpos,"=");
      _node._Key.Trim();
      _node._Value.Trim(); 
      _node._Type = P_NODE_KEYVAL_T;
      it = _This->_Content.Insert(it,_node); 
    }
  }
  return true;
}
//--------------------------------------------------------------------
static bool Properties_ReadSection(CCI_INSTANCE_PTR(Properties) _This,
                                   const char* _section, dmIStringList& _Strings)
{
  if(EMPTY_STRING(_section))
    return false;

  Properties_LIST::iterator it = Properties_Find(_This,_section,NULL);

  if(it!=_This->_Content.End())
  {
    dmStrStream line;
    for(++it;it!=_This->_Content.End();++it) {
      if((*it)._Type == P_NODE_SECTION_T) break; // New section
      if((*it)._Type == P_NODE_KEYVAL_T) {
        if((*it)._Value.Empty())
          _Strings.AddTailA( (*it)._Key );
        else {
          line.Clear();
          line << (*it)._Key << " = " << (*it)._Value;
          _Strings.AddTail(line.CStr());
        }
      }
    }
    return true;
  }
  return false;
}
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR(  Properties )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( Properties )

  CCI_INIT_MEMBER( _File    )
  CCI_INIT_MEMBER( _Section )
  CCI_INIT_MEMBER( _Content )

  _This->_Modified = dm_false;

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR(  Properties )
{
  CCI_INSTANCE( Properties) 

  CCI_DESTROY_MEMBER( _Content )
  CCI_DESTROY_MEMBER( _Section )
  CCI_DESTROY_MEMBER( _File    )

  CCI_DESTROY_SUPER()
}
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
//---------------------------------------------------------------------
// Method: Data
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Properties, Data )
{
  CCI_INSTANCE( Properties )
 
  if(!_This->_File.Empty() && !EMPTY_STRING(CCI_P(property))) 
  {
    const char_t* _Section = CCI_P(section);

    if(EMPTY_STRING(_Section)) 
       _Section = _This->_Section.CStr();

    switch(CCI_P(me)) 
    {
      case CCI_ME_SET :
       if(Properties_WriteString(_This,_Section,CCI_P(property),CCI_P(value))) {
          _This->_Modified = dm_true;
          CCI_RETURN_OK();
       }
       break;
  
      case CCI_ME_GET :
        if(Properties_ReadString(_This,_Section,CCI_P(property),CCI_P(value))) {
           CCI_RETVAL_P(value) = _This->_Buffer;
           CCI_RETURN_OK();
        }
        break;
 
      case CCI_ME_DISPOSE: {
        Properties_LIST::iterator it = Properties_Find(_This,_Section,CCI_P(property));
        if(it!=_This->_Content.End()) {
          _This->_Content.Remove(it);
          _This->_Modified = dm_true;
        }
      }
      CCI_RETURN_OK();
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
  CONST char_t* section   =  CCI_P(section);
  CONST char_t* property  =  CCI_P(property);
  dm_int        value     =  CCI_P(value); 
 
  switch(CCI_P(me)) 
  {     
     case CCI_ME_SET: {
       dmString strValue(value);
       CCI_RETURN_IMETHOD_PP_(_This_ob,Properties,Data, 
          _in  me
          _in  section
          _in  property
          _in  strValue.CStr()
        );
     } break;
     
     case CCI_ME_GET: {
       dmIProperties _Props;
       _Props.Attach(_This_ob);
       dmString _v = _Props.GetData(section,property);
       if(!_v.Empty()) {
         CCI_RETVAL_P(value) = _v.AsInt();
         CCI_RETURN_OK();   
       } else
         CCI_RETURN_FAIL()  
     } break;
  }
  
  CCI_RETURN_FAIL()  
}
//---------------------------------------------------------------------
// Method: IntData
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Properties, RealData )
{
  CCI_INSTANCE( Properties )
   
  dm_param      me        =  CCI_P(me);
  CONST char_t* section   =  CCI_P(section);
  CONST char_t* property  =  CCI_P(property);
  dm_real       value     =  CCI_P(value); 
 
  switch(CCI_P(me)) 
  {     
     case CCI_ME_SET: {
       dmString strValue(value);
       CCI_RETURN_IMETHOD_PP_(_This_ob,Properties,Data, 
          _in  me
          _in  section
          _in  property
          _in  strValue.CStr()
        );
     } break;

     case CCI_ME_GET: {
       dmIProperties _Props;
       _Props.Attach(_This_ob);
       dmString _v = _Props.GetData(section,property);
       if(!_v.Empty()) {
         CCI_RETVAL_P(value) = _v.AsDouble();
         CCI_RETURN_OK();   
       } else
         CCI_RETURN_FAIL()  
     } break;
  }
  
  CCI_RETURN_FAIL()  
}
//---------------------------------------------------------------------
// Method: Section
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Properties, Section )
{
  CCI_INSTANCE( Properties )

  if(!_This->_File.Empty()) 
  {
    dmIStringList _Strings;
    if(!_Strings.QueryInterface(CCI_P(strlist)))
      CCI_RETURN_INVALID_PARAM()

    const char_t* _Section = CCI_P(section);

    if(EMPTY_STRING(_Section)) 
       _Section = _This->_Section.CStr();

    switch(CCI_P(me)) 
    {
      case CCI_ME_SET :
          if(Properties_WriteSection(_This,_Section,_Strings)) {
            _This->_Modified = dm_true;
            CCI_RETURN_OK()
          }
          break;
  
      case CCI_ME_GET :
          if(Properties_ReadSection(_This,_Section,_Strings))
             CCI_RETURN_OK(); 
          break;
 
      case CCI_ME_DISPOSE: {
         Properties_LIST::iterator it = Properties_Find(_This,_Section,NULL);
         _This->_Modified  = (it!=_This->_Content.End());
         while(it!=_This->_Content.End()) {           
           it = _This->_Content.Erase(it);
           if((*it)._Type == P_NODE_SECTION_T) break; // New section
         }
       }
       CCI_RETURN_OK();
    }

  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Open
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Properties, Open )
{
  CCI_INSTANCE( Properties)

  if(!EMPTY_STRING(CCI_P(path)) && _This->_File.Empty())
  {
    dmString _path = CCI_P(path);
    dmString _protocol;

    _This->_Section.Clear();
    _This->_Content.Clear();
    _This->_Modified = dm_false;

    if(!_path.Empty())
    { 
      // If file exists open it
      if(dmPortability::AccessFile(_path.CStr(),dmPortability::eEXISTS)) 
      {
        int err = Properties_OpenFile(_This,_path.CStr());
        if(err!=0) 
        { 
          dmString _ErrMsg = dmString::FormatString(dmString::npos,_TXT("Cannot open %s : %s "),
                                                  _path.CStr(),strerror( err ));
          CCI_SET_ERROR_MSG( _ErrMsg.CStr() );     
          CCI_RETURN_IOERROR()
        }
      }

      _This->_File    = _path;
      if(!EMPTY_STRING(CCI_P(section)))
       _This->_Section = CCI_P(section);
      CCI_RETURN_OK()
    }
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method: Close
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( Properties, Close )
{
  CCI_INSTANCE( Properties)

  if(_This->_Modified && !_This->_File.Empty()) 
  { 
    FILE* _file = _tfopen(_This->_File.CStr(),_TXT("w"));
    if(_file!=NULL) 
    {
       dmStrStream _Str;

       Properties_LIST::iterator it   = _This->_Content.Begin();
       Properties_LIST::iterator last = _This->_Content.End();
       for(;it!=last;++it) {
         switch((*it)._Type) 
         {
           case P_NODE_COMMENT_T: 
             _fputts((*it)._Key.CStr(),_file);
             break;

           case P_NODE_SECTION_T:
             _Str.Clear();
             _Str << "\n[" << (*it)._Key << "]\n";
             _fputts(_Str.CStr(),_file);
             break;

           case P_NODE_KEYVAL_T :
             if((*it)._Value.Empty()) {
               _fputts((*it)._Key.CStr(),_file);
               _fputts("\n",_file);
             } else {
               _Str.Clear();
               _Str << (*it)._Key << " = " << (*it)._Value << "\n";
               _fputts(_Str.CStr(),_file);
             }
             break;
         }
       }
       fclose(_file);
    } 
    else 
    {
      dmString _ErrMsg = dmString::FormatString(dmString::npos,_TXT("Cannot open %s : %s "),
                                               _This->_File.CStr(),strerror( errno ));
      CCI_SET_ERROR_MSG( _ErrMsg.CStr() );     
      CCI_RETURN_IOERROR() 
    }
  }

  _This->_File.Clear();
  _This->_Section.Clear();

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: GetSections
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Properties, GetSections )
{
  CCI_INSTANCE( Properties)

  dmIStringList _Strings;
  if(!_Strings.QueryInterface(CCI_P(strlist)))
     CCI_RETURN_INVALID_PARAM()

  if(!_This->_File.Empty()) 
  {
    Properties_LIST::iterator it   = _This->_Content.Begin();
    Properties_LIST::iterator last = _This->_Content.End();
   
    for(;it!=last;++it) {
      if((*it)._Type == P_NODE_SECTION_T)
        _Strings.AddTailA( (*it)._Key );
    }
    CCI_RETURN_OK() 

  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetKeys
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Properties, GetKeys )
{
  CCI_INSTANCE( Properties)

  dmIStringList _Strings;
  if(!_Strings.QueryInterface(CCI_P(strlist)))
     CCI_RETURN_INVALID_PARAM()

  if(!_This->_File.Empty()) 
  {
    const char_t* _Section = CCI_P(section);

    if(EMPTY_STRING(_Section)) 
       _Section = _This->_Section.CStr();

    Properties_LIST::iterator it = Properties_Find(_This,_Section,NULL);
    if(it!=_This->_Content.End())
    {
      for(++it;it!=_This->_Content.End();++it) {
        if((*it)._Type == P_NODE_SECTION_T) break; // New section
        if((*it)._Type == P_NODE_KEYVAL_T)
          _Strings.AddTailA( (*it)._Key );
      }
      CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP(Properties)
  CCI_REGISTER_METHOD_PP( Properties, Open    )
  CCI_REGISTER_METHOD_NP( Properties, Close   )
  CCI_REGISTER_METHOD_PP( Properties, Data    )
  CCI_REGISTER_METHOD_PP( Properties, Section )
  CCI_REGISTER_METHOD_PP( Properties, GetSections )
  CCI_REGISTER_METHOD_PP( Properties, GetKeys     ) 
  CCI_REGISTER_METHOD_PP( Properties, IntData     )
  CCI_REGISTER_METHOD_PP( Properties, RealData    )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Properties )
{
  CCI_INVOKE_FACTORY(Properties)
}
//---------------------------------------------------------------------
