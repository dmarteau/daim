
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

#define dmUseUtilitiesExtra
#include "daim_utilities.h"
#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_XMLParser.h"
#include "daim_modules/CCI_StringList.h"

#define CCIC_NAME  CCIC_XMLParser
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR "David Marteau"
#define CCI_VERSION   CCI_XMLParser_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//-------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define XML_STATIC
#include "expat.h"

CCI_DECL_DATA( XMLParser);
typedef CCI_INSTANCE_PTR(XMLParser) _Instance_Ptr;

#define _NOINDEX_ dm_noindex

//-------------------------------------------------------------------
// XML Item management
//-------------------------------------------------------------------

//typedef std::map<dmString,dmString> PROPDICT;
//typedef std::hash_map<dmString,dmString> PROPDICT;

typedef daim::hashmap<dmString,dmString>    PROPDICT;

struct ITEMNODE;
typedef dmTNode<ITEMNODE*> _ITEMNODE;

struct ITEMNODE : public _ITEMNODE
{
  friend class dmOList<ITEMNODE>;

  ITEMNODE*   it_parent;
  int         it_depth;
  PROPDICT    it_properties;  

  dmString    it_name;
  dmString    it_text;
  dmStrStream it_path;

  ITEMNODE* Next() const { return reinterpret_cast<ITEMNODE*>(dmNode::Next()); }
  ITEMNODE* Prev() const { return reinterpret_cast<ITEMNODE*>(dmNode::Prev()); }

 ~ITEMNODE() {
    it_properties.clear();
  }

  ITEMNODE( ITEMNODE* _parent) 
  : dmTNode<ITEMNODE*>(NULL)
  , it_parent(_parent)
  , it_depth(0) 
  , it_properties(_parent?_parent->it_properties.get_allocator():
                  PROPDICT::allocator_type())
  {
    Assign(this);
    if(it_parent) 
      it_depth = it_parent->it_depth + 1;
  }
};

//--------------------------------------------------------------------
typedef ITEMNODE::iterator          ITEM;
typedef dmOList<ITEMNODE>           ITEMLIST;
typedef dmOList<ITEMNODE>::iterator ITERATOR;

CCI_DECL_DATA( XMLParser)
{
  ITEM      _currentItem;
  ITEMLIST  _items;

  dm_uint   _tagStatus;
  dm_uint   _bInDocument;
  dm_uint   _BufferSize;
  dm_uint   _ParseOpts;

  dmFastAllocator<ITEMNODE> _Allocator;

  size_t       _inlength;
  size_t       _outlength;
  const char*  _inbuf;
  char*        _outbuf;
  char*        _enc_buffer;
  size_t       _enc_bufsize;

  cci_Object* _self;
};
//--------------------------------------------------------------------
static inline ITEMNODE* _NEW_ITEMNODE( _Instance_Ptr _This,const dmString::E* _name, ITEMNODE* _parent )
{
  ITEMNODE* _node = _This->_Allocator.allocate(1,NULL);
  _This->_Allocator.construct(_node,_parent);

  _node->it_name = _name;
  return _node;
} 

static inline void _DELETE_ITEMNODE( _Instance_Ptr _This, ITEMNODE* _node ) 
{
  _This->_Allocator.destroy(_node);
  _This->_Allocator.deallocate(_node,1);
} 

//--------------------------------------------------------------------
// UTF8 decoder
//--------------------------------------------------------------------
#define ENC_OK    0
#define ENC_INVAL 1
#define ENC_E2BIG 2

#define ENC_OUTBUF_SIZE    1024
#define DFLT_INPUTBUF_SIZE 8192

int utf8_to_latin1(const char** inbuf  , size_t *inbytesleft,
                         char** outbuf , size_t *outbytesleft); 

static void decode_utf8( _Instance_Ptr _This, const char* utf8_str, size_t len )
{
   _This->_inbuf     = utf8_str;
   _This->_inlength  = len;
   _This->_outlength = _This->_enc_bufsize;
   _This->_outbuf    = _This->_enc_buffer;

   int enc_status = ENC_INVAL;

   while(enc_status!=ENC_OK) {

    enc_status = utf8_to_latin1(&_This->_inbuf ,&_This->_inlength,
                                &_This->_outbuf,&_This->_outlength);
    if(enc_status==ENC_E2BIG) {
      _This->_enc_buffer  = static_cast<char*>(dmMemory::Realloc(_This->_enc_buffer,_This->_enc_bufsize+ENC_OUTBUF_SIZE));
      _This->_outbuf      = _This->_enc_buffer + _This->_enc_bufsize;
      _This->_enc_buffer += ENC_OUTBUF_SIZE;
    } 
    else
    if(enc_status==ENC_INVAL) {
      _This->_outbuf[0] = _TXT('\0');
      break;
    }
  }
}
//--------------------------------------------------------------------
// Expat handlers
//--------------------------------------------------------------------
static void xml_start(void *userData,const XML_Char *name,const XML_Char **atts)
{
  _Instance_Ptr _This = static_cast<_Instance_Ptr>(userData);

  ITEMNODE* current = NULL;
  if(_This->_currentItem)
    current = *_This->_currentItem;

  // Create a new tag item
  _This->_items.Push_Back(_NEW_ITEMNODE(_This,name,current));
  _This->_currentItem = ITEM(_This->_items.Last());

  ITEMNODE* _current = _This->_items.Tail();

  while(*atts)
  {
    //-----------------------------------
    // Convert UTF8 to latin 1 
    //-----------------------------------
    decode_utf8(_This,atts[1],strlen(atts[1])+1);

    _current->it_properties[atts[0]] = _This->_enc_buffer;
    atts+=2;
  }

  if(_This->_ParseOpts & CCIV_XMLParser_CollectPath) {
    _current->it_path.Clear();
    if(_current->it_parent) 
     _current->it_path << _current->it_parent->it_path << _TXT("/") << _current->it_name;
    else                    
     _current->it_path << _TXT("//") << _current->it_name;
  }

  // Notify on tag start
  CCI_Set(_This->_self,CCIA_DECL_(XMLParser,Status),dm_true);  

}
//--------------------------------------------------------------------
static void xml_end(void *userData,const XML_Char *name)
{
  _Instance_Ptr _This = static_cast<_Instance_Ptr>(userData);

  // Notify on tag end
  CCI_Set(_This->_self,CCIA_DECL_(XMLParser,Status),dm_false);

  if(_This->_currentItem) {
    ITEMNODE* _current = *_This->_currentItem;
    _This->_currentItem = ITEM(_current->it_parent);
  }
}
//--------------------------------------------------------------------
static void xml_cdata(void *userData,const XML_Char *s,int len)
{
  _Instance_Ptr _This = static_cast<_Instance_Ptr>(userData);

  if(_This->_currentItem) 
  {
    ITEMNODE* _current = *_This->_currentItem;
    decode_utf8(_This,s,len);
    _current->it_text.Append(_This->_enc_buffer,len);
  }
}
//---------------------------------------------------------------------
static void mClear( _Instance_Ptr _This )
{
  _This->_currentItem = ITEM();
  while(!_This->_items.Empty()) {
    _DELETE_ITEMNODE(_This,_This->_items.Begin());
  }
}
//--------------------------------------------------------------------
// ME_NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR(  XMLParser )
{
  CCI_CONSTRUCT_SUPER()

  CCI_INSTANCE( XMLParser)

  _This->_tagStatus   = dm_false;
  _This->_bInDocument = dm_false;
  _This->_self        = _THIS_OBJECT;
  _This->_enc_buffer  = NULL;
  _This->_enc_bufsize = 0;
  _This->_BufferSize  = DFLT_INPUTBUF_SIZE;
  _This->_ParseOpts   = 0;

  CCI_INIT_MEMBER( _items       )
  CCI_INIT_MEMBER( _Allocator   )
  CCI_INIT_MEMBER( _currentItem )

  CCI_BEGIN_INIT_MAP(XMLParser)
    CCI_ATTR_SET( XMLParser, Options,  _This->_ParseOpts |= CCIA_DATA )
  CCI_END_INIT_MAP()

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// ME_DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR(  XMLParser )
{
  CCI_INSTANCE( XMLParser)
 
  if(_This->_enc_buffer)
    dmMemory::Free(_This->_enc_buffer);

  mClear(_This);

  CCI_DESTROY_MEMBER( _items       )
  CCI_DESTROY_MEMBER( _Allocator   )
  CCI_DESTROY_MEMBER( _currentItem )

  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
static const dmString::E* mGetContent( _Instance_Ptr _This )
{
  if(_This->_currentItem) {
    ITEMNODE* _current = *_This->_currentItem;
    const dmString& _text = _current->it_text;
    return _text.CStr();
  }
  return NULL;
}
//---------------------------------------------------------------------
static void mSetContent( _Instance_Ptr _This, const dmString::E* _content )
{
  if(_This->_currentItem) {
    ITEMNODE* _current = *_This->_currentItem;
    _current->it_text = _content;
  }
}
//---------------------------------------------------------------------
// ME_GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( XMLParser )
  CCI_ATTR_GET( XMLParser, Status    , CCIA_RETVAL = _This->_tagStatus  )
  CCI_ATTR_GET( XMLParser, Content   , CCIA_RETVAL = dm_ptr_to_long(mGetContent(_This))    )

  CCI_ATTR_GET( XMLParser, Path      , 
     CCIA_RETVAL = (_This->_currentItem ?
       dm_ptr_to_long((*_This->_currentItem)->it_path.CStr()) : dm_null) 
  )

  CCI_ATTR_GET( XMLParser, Tag       , 
   CCIA_RETVAL = (_This->_currentItem ?
       dm_ptr_to_long( (*_This->_currentItem)->it_name.CStr()) : dm_null) 
  )

  CCI_ATTR_GET( XMLParser, Document  , CCIA_RETVAL = _This->_bInDocument  )
  CCI_ATTR_GET( XMLParser, BufferSize, CCIA_RETVAL = _This->_BufferSize   )
  CCI_ATTR_GET( XMLParser, Options   , CCIA_RETVAL = _This->_ParseOpts    )

CCI_END_GETTER()
//---------------------------------------------------------------------
// ME_SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( XMLParser )
  CCI_ATTR_SET( XMLParser, Status    ,_This->_tagStatus = _BOOL(CCIA_DATA) )
  CCI_ATTR_SET( XMLParser, Content   , mSetContent(_This, CCIA_TRANSLATE_DATA(char_t*)) )
  CCI_ATTR_SET( XMLParser, Document  ,_This->_bInDocument = CCIA_DATA )
  CCI_ATTR_SET( XMLParser, BufferSize,
      _This->_BufferSize  = CCIA_DATA;
      if(_This->_BufferSize==0 || _This->_BufferSize==_NOINDEX_) 
         _This->_BufferSize = DFLT_INPUTBUF_SIZE;
      CCIA_DATA = _This->_BufferSize;
  )
  CCI_ATTR_SET( XMLParser, Options,  _This->_ParseOpts = CCIA_DATA )

CCI_END_SETTER()
//---------------------------------------------------------------------
// CCIM_XMLParser_Parse
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( XMLParser, ParseFile )
{
   CCI_INSTANCE( XMLParser)
   
   if(EMPTY_STRING(CCI_P(file)))
     CCI_RETURN_INVALID_PARAM()

   mClear(_This);
   if(_This->_enc_buffer==NULL) {
      _This->_enc_buffer  = static_cast<char*>(dmMemory::Realloc(NULL,ENC_OUTBUF_SIZE));
      _This->_enc_bufsize = ENC_OUTBUF_SIZE;
   }

   XML_Parser parser = XML_ParserCreate(CCI_P(encoding));
   if (!parser) {
     CCI_SET_ERROR_MSG(_TXT("Couldn't allocate memory for parser\n"));
     CCI_RETURN_FAIL()
   }

   FILE* _input;

   if(!(_input  = fopen(CCI_P(file),_TXT("r")))) {
     dmString _ErrMsg = dmString::FormatString(dmString::npos,_TXT("Cannot open %s : %s "),CCI_P(file),
                                               strerror( errno ));
     CCI_SET_ERROR_MSG( _ErrMsg.CStr() );     
     CCI_RETURN_IOERROR()
   }

   bool parse_error = false;

   int done;
   XML_SetUserData(parser,_This);
   XML_SetElementHandler(parser,xml_start,xml_end);
   XML_SetCharacterDataHandler(parser,xml_cdata);

   void *_input_buf = XML_GetBuffer(parser, _This->_BufferSize);
   if(_input_buf==NULL) {
     CCI_SET_ERROR_MSG(_TXT("Cannot allocate parser buffer !") );
     parse_error = true;
   } 
   else
   {
     CCI_Set(_THIS_OBJECT,CCIA_DECL_(XMLParser,Document),dm_true);
     do {
       size_t len = fread(_input_buf, 1,_This->_BufferSize, _input);
       done = len < _This->_BufferSize;

       // XXXDavid is there a bug here ? Parsing fail if read is done in two passes

       if(XML_ParseBuffer(parser,len,done) == XML_STATUS_ERROR) 
       {
         dmString _ErrMsg = dmString::FormatString(dmString::npos,
                        "file %s line %d column %d : %s\n",
                        CCI_P(file),
                        XML_GetCurrentLineNumber(parser),
                        XML_GetCurrentColumnNumber(parser),
                        XML_ErrorString(XML_GetErrorCode(parser)));
         CCI_SET_ERROR_MSG( _ErrMsg.CStr() );     
         parse_error = true;
         break;
       }
     } while (!done);


     CCI_Set(_THIS_OBJECT,CCIA_DECL_(XMLParser,Document),dm_false);
   }

   fclose(_input);

   XML_ParserFree(parser);
      
   if(_This->_tagStatus) CCI_Set(_THIS_OBJECT,CCIA_DECL_(XMLParser,Status),dm_false);
   _This->_currentItem = ITEM();

   // For DEBUG purposes
   //parse_error = true;

   if(parse_error) 
     mClear(_This);
   else 
     CCI_RETURN_OK()

   CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// CCIM_XMLParser_ParseBuffer
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( XMLParser, ParseBuffer )
{
   CCI_INSTANCE( XMLParser)
   
   if(EMPTY_STRING(CCI_P(buffer)))
     CCI_RETURN_INVALID_PARAM()

   mClear(_This);

   if(_This->_enc_buffer==NULL) {
      _This->_enc_buffer  = static_cast<char*>(dmMemory::Realloc(NULL,ENC_OUTBUF_SIZE));
      _This->_enc_bufsize = ENC_OUTBUF_SIZE;
   }

   XML_Parser parser = XML_ParserCreate(CCI_P(encoding));
   if (!parser) {
     CCI_SET_ERROR_MSG(_TXT("Couldn't allocate memory for parser\n"));
     CCI_RETURN_FAIL()
   }

   bool parse_error = false;

   XML_SetUserData(parser,_This);
   XML_SetElementHandler(parser,xml_start,xml_end);
   XML_SetCharacterDataHandler(parser,xml_cdata);

   CCI_Set(_THIS_OBJECT,CCIA_DECL_(XMLParser,Document),dm_true);

   if(XML_Parse(parser,CCI_P(buffer),CCI_P(size),true) == XML_STATUS_ERROR) 
   {
         dmString _ErrMsg = dmString::FormatString(dmString::npos,
                        "line %d column %d : %s\n",
                        XML_GetCurrentLineNumber(parser),
                        XML_GetCurrentColumnNumber(parser),
                        XML_ErrorString(XML_GetErrorCode(parser)));
      CCI_SET_ERROR_MSG( _ErrMsg.CStr() );     
      parse_error = true;
   }

   CCI_Set(_THIS_OBJECT,CCIA_DECL_(XMLParser,Document),dm_false);

   XML_ParserFree(parser);
      
   if(_This->_tagStatus) CCI_Set(_THIS_OBJECT,CCIA_DECL_(XMLParser,Status),dm_false);
   _This->_currentItem = ITEM();

   if(parse_error)      
     mClear(_This);
   else 
      CCI_RETURN_OK()

   CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// CCIM_XMLParser_GetProperty
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( XMLParser, GetProperty )
{
  CCI_INSTANCE( XMLParser)

  if(CCI_P(property) && _This->_currentItem ) 
  {
    ITEMNODE* _current = *_This->_currentItem;

    PROPDICT& _dict = _current->it_properties;
    PROPDICT::iterator it = _dict.find(CCI_P(property));
    if(it!=_dict.end()) { 
      const dmString& _result = (*it).second;
      switch(CCI_P(fmt)) 
      {
        case CCIV_XMLParser_String: 
          if(!_result.Empty()) { 
            CCI_RETVAL_P(value) =  dm_ptr_to_long(_result.CStr()); 
            CCI_RETURN_OK()
          } break;

        case CCIV_XMLParser_Long  : CCI_RETVAL_P(value) =  static_cast<dm_param>(_result.AsInt(0)); CCI_RETURN_OK()
        case CCIV_XMLParser_Toggle: CCI_RETVAL_P(value) =  dm_ptr_to_long(_result.CStr()); CCI_RETURN_OK()
      }
    }
  } 
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// CCIM_XMLParser_Play
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( XMLParser, Clear )
{
  CCI_INSTANCE( XMLParser)

  mClear(_This);

  _This->_tagStatus = dm_false;

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// CCIM_XMLParser_Play
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( XMLParser, Play )
{
  CCI_INSTANCE( XMLParser)

  if(!_This->_items.Empty())
  { 
    ITERATOR __F = _This->_items.Begin();
    ITERATOR __L = _This->_items.End();
   
    CCI_Set(_THIS_OBJECT,CCIA_DECL_(XMLParser,Document),dm_true);

    _This->_currentItem = ITEM();

    ITEMNODE* _current = NULL;

    int _depth = -1;

    for(;__F!=__L;++__F)
    {
      if(__F->it_depth <= _depth) {                             // Move out or sibling
        for(;_depth>=__F->it_depth;--_depth) {                  // Roll back
          CCI_Set(_THIS_OBJECT,CCIA_DECL_(XMLParser,Status),dm_false);// Close previous
          _This->_currentItem = ITEM(_current = _current->it_parent);
        }
      }

       // Open the current
       _current = *(_This->_currentItem = ITEM(__F));

      CCI_Set(_THIS_OBJECT,CCIA_DECL_(XMLParser,Status),dm_true); 

      _depth = __F->it_depth;
    }

    // Roll back;
    for(;_current;_current = _current->it_parent) {
      _This->_currentItem = ITEM(_current);
      CCI_Set(_THIS_OBJECT,CCIA_DECL_(XMLParser,Status),dm_false);
    }

    _This->_currentItem = ITEM();

    CCI_Set(_THIS_OBJECT,CCIA_DECL_(XMLParser,Document),dm_false);
    CCI_RETURN_OK()
  } 
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// CCIM_XMLParser_AddProperty
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( XMLParser, SetProperty )
{
  CCI_INSTANCE( XMLParser)

  if(_This->_currentItem && !EMPTY_STRING(CCI_P(property)) )
  {
    PROPDICT& _dict = (*_This->_currentItem)->it_properties;

    switch(CCI_P(fmt)) {
       case CCIV_XMLParser_String:
         _dict[CCI_P(property)] = (CCI_P(value)?reinterpret_cast<char_t*>(CCI_P(value)):"");
         break;
       case CCIV_XMLParser_Long  : {
          _dict[CCI_P(property)] = dmString( static_cast<long>(CCI_P(value)) );
       }; break;
       case CCIV_XMLParser_Hex   : {
         _dict[CCI_P(property)] = dmString::FormatString(dmString::npos,_TXT("0x%lx"),CCI_P(value));
       }; break;
       case CCIV_XMLParser_Toggle: {
         if(CCI_P(value)) _dict.erase(CCI_P(property));
         else             _dict[CCI_P(property)] = _TXT("");
       }
    }  
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// CCIM_XMLParser_RemProperty
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( XMLParser, RemProperty )
{
  CCI_INSTANCE( XMLParser)

  if( _This->_currentItem && !EMPTY_STRING(CCI_P(property)) ) 
  {
    (*_This->_currentItem)->it_properties.erase(CCI_P(property));
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// CCIM_XMLParser_InsertItem      
// If name is given this will insert an item as child of the current item.
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( XMLParser, InsertItem )
{ 
  CCI_INSTANCE( XMLParser)

  if( !EMPTY_STRING(CCI_P(name)) ) 
  {
    if(_This->_currentItem) 
    {
      ITEMNODE* _current = *_This->_currentItem;

      ITERATOR __F = ITEMLIST::Iterator(_current);
      ITERATOR __L = _This->_items.End();

      do { ++__F; } while( __F!=__L && __F->it_depth > _current->it_depth);
      _This->_items.Insert(__F,_NEW_ITEMNODE(_This,CCI_P(name),_current));
      CCI_RETURN_OK()
    } 
    else if(_This->_items.Empty()) {
      _This->_items.Push_Back(_NEW_ITEMNODE(_This,CCI_P(name),NULL));
      CCI_RETURN_OK()
    }
  }
    
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( XMLParser, DeleteItem )
{ 
  CCI_UNUSUED_INSTANCE( XMLParser)

  // TODO : NEED TO REMOVE ALL CHILDS
/*
  _ITEMNODE* _current = _This->_currentItem;
  if(_current) {
    ITEM it = _This->_currentItem--;
    _DELETE_ITEMNODE(_This,_This->_items.Remove(it));
    if(_This->_items.Empty()) 
        _This->_currentItem = ITERATOR();
    CCI_RETURN_OK()
  }
*/
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// CCIM_XMLParser_Select 
// Select a node from its path
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( XMLParser, Select )
{ 
  CCI_INSTANCE( XMLParser)

  const char* _path = CCI_P(path);
  if(!EMPTY_STRING(_path) && !_This->_items.Empty()) 
  { 
    ITEMNODE* _current = NULL;

    int _index;
    if(strncmp(_path,"//",2)==0) {
      _path += 2;
      _current = _This->_items.Begin();
    } else if(_This->_currentItem)
      _current = *_This->_currentItem;

    ITEM __L = ITEM(_This->_items.End());

    dmString tag,path = _path;
    for(size_t pos = 0;_current!=NULL && pos!=dmString::npos;)
    {
      pos = path.Split(tag,pos,_TXT("/")); 

      if(tag.Empty())
        break;

      if(tag==_TXT("..")) {
        if(_current->it_parent) 
          _current = _current->it_parent;
        continue;
      }

      if(tag==_TXT(".")) continue;

      _index = 0;

      // find child node having the corresponding tag 
      ITEM __F =  ++ITEM(_current);
      while( __F!=__L && (*__F)->it_depth > _current->it_depth) { 
        if((*__F)->it_parent==_current &&
           (*__F)->it_name  == tag     && 
           _index++ == static_cast<int>(CCI_P(index))) 
        {
          _current = *__F;
          break;
        }
        ++__F; 
      }

      if(_current != __F._Node()) 
        _current = NULL;
    }

    if(_current) {
      _This->_currentItem = ITEM(_current);
      CCI_RETURN_OK()
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// CCIM_XMLParser_Next 
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( XMLParser, Next )
{ 
  CCI_INSTANCE( XMLParser)

  if(_This->_currentItem) 
  {
    ITEMNODE* _current = *_This->_currentItem;

    ITERATOR __F = ITEMLIST::Iterator(_current);
    ITERATOR __L = _This->_items.End();

    if(EMPTY_STRING(CCI_P(name))) 
    {
      if(CCI_P(sibling)) {
        do { ++__F; } while(__F!=__L && __F->it_depth > _current->it_depth);
        if(__F!=__L && __F->it_depth==_current->it_depth) {
          _This->_currentItem = ITEM(__F);
          CCI_RETURN_OK()  
        }
      } else if(++__F != __L) {
        _This->_currentItem = ITEM(__F);
        CCI_RETURN_OK()  
      }
    }
    else
    {
      // find next sibling 
      if(CCI_P(sibling)) 
      {
        for(++__F; __F!=__L && __F->it_depth >= _current->it_depth;++__F) { 
          if(__F->it_depth == _current->it_depth &&
             __F->it_name  == CCI_P(name))  
          {
            _This->_currentItem = ITEM(__F);
            CCI_RETURN_OK()  
          }
        }
      } 
      else
      {
        for(++__F; __F!=__L;++__F)  
         if(__F->it_name  == CCI_P(name)) {
            _This->_currentItem = ITEM(__F);
            CCI_RETURN_OK()  
         }
      }
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// CCIM_XMLParser_Find
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( XMLParser, Find )
{ 
  CCI_INSTANCE( XMLParser)
   
  if(!EMPTY_STRING(CCI_P(name)) && !_This->_items.Empty()) 
  {

    ITERATOR __F,__L = _This->_items.End();

    if(!_This->_currentItem) 
      __F = _This->_items.Begin();
    else
      __F = ITEMLIST::Iterator(*_This->_currentItem);

    ITEMNODE *_current = __F;
    for(++__F;__F!=__L && __F->it_depth > _current->it_depth;++__F) { 
      if( __F->it_name == CCI_P(name) ) 
      {
        if(CCI_P(property)==NULL) 
           CCI_RETURN_OK() 
      
        PROPDICT::iterator it = __F->it_properties.find(CCI_P(property));
        if(it!=__F->it_properties.end() && 
          (CCI_P(value)==NULL || (*it).second == CCI_P(value))) 
           CCI_RETURN_OK()
      }
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// CCIM_XMLParser_GetProperties 
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( XMLParser, GetProperties )
{
  CCI_INSTANCE( XMLParser)
 
  dmIStringList _List;

  if(_List.QueryInterface(CCI_P(strlist)))
  {
    if(_This->_currentItem) 
    {
      ITEMNODE* _current = *_This->_currentItem;
      size_t sz = _current->it_properties.size();
      if(sz) 
      {
        PROPDICT::iterator it   = _current->it_properties.begin();
        PROPDICT::iterator last = _current->it_properties.end();
        for(;it!=last;++it) {
          _List.AddTailA((*it).first);
          _List.AddTailA((*it).second);
        }
      }
      CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( XMLParser, Export )
{ 
  CCI_INSTANCE( XMLParser)

  if(!EMPTY_STRING(CCI_P(file)) && !_This->_items.Empty())  
  {
    FILE* _output;
    if(!(_output  = fopen(CCI_P(file),_TXT("w")))) {
      dmString _ErrMsg = dmString::FormatString(dmString::npos,
                         _TXT("Cannot open %s : %s "),
                         CCI_P(file),
                         strerror( errno ));

      CCI_SET_ERROR_MSG( _ErrMsg.CStr() );     
      CCI_RETURN_IOERROR()
    }
    
    ITERATOR __F = _This->_items.Begin();
    ITERATOR __L = _This->_items.End();

    fputs("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>",_output);

    if(!EMPTY_STRING(CCI_P(preamble))) 
       fputs(CCI_P(preamble),_output);

    ITEMNODE* _current = NULL;

    int _depth = -1;

    for(;__F!=__L;++__F) 
    {
      if(__F->it_depth == _depth && _current->it_text.Empty()) {  // sibling 
        if(_current->it_text.Empty()) fputs("/>\n",_output);
        else fprintf(_output,"</%s>\n",_current->it_name.CStr());
        _current = _current->it_parent;
        --_depth;
      }

      if(__F->it_depth <= _depth) {                             // Move out or sibling
        for(;_depth>=__F->it_depth;--_depth) {                  // Roll back
          for(int i=0;i<_depth;++i) fprintf(_output,"%s","  ");
          fprintf(_output,"</%s>\n",_current->it_name.CStr());
          _current = _current->it_parent;
        }
      }

       _current = __F;

      for(int i=0;i<_depth;++i) fprintf(_output,"%s","  ");

      fprintf(_output,"<%s",_current->it_name.CStr());
      if(!_current->it_properties.empty()) {
        PROPDICT::iterator it   = _current->it_properties.begin();
        PROPDICT::iterator last = _current->it_properties.end();
        for(;it!=last;++it) {
          fprintf(_output," %s",(*it).first.CStr());
          if(!(*it).second.Empty()) {
            fprintf(_output,"=\"%s\"",(*it).second.CStr());
            fputc(' ',_output);
          }
        }
      }

      if(!_current->it_text.Empty()) {
        fputs(">",_output);
        fprintf(_output,"%s",_current->it_text.CStr());
      }    
      _depth = _current->it_depth;

    }

    // Roll back;
    for(;_current;_current = _current->it_parent) {
      for(int i=0;i<_depth;++i) fprintf(_output,"%s","  ");
      fprintf(_output,"</%s>\n",_current->it_name.CStr());
    }

    fclose(_output);
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
 
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP(XMLParser)
  CCI_REGISTER_METHOD_PP( XMLParser, GetProperty   )
  CCI_REGISTER_METHOD_PP( XMLParser, Select        )
  CCI_REGISTER_METHOD_PP( XMLParser, SetProperty   )
  CCI_REGISTER_METHOD_PP( XMLParser, RemProperty   )
  CCI_REGISTER_METHOD_PP( XMLParser, InsertItem    )
  CCI_REGISTER_METHOD_NP( XMLParser, DeleteItem    )
  CCI_REGISTER_METHOD_PP( XMLParser, ParseFile     )
  CCI_REGISTER_METHOD_PP( XMLParser, ParseBuffer   )
  CCI_REGISTER_METHOD_NP( XMLParser, Play          )
  CCI_REGISTER_METHOD_NP( XMLParser, Clear         )
  CCI_REGISTER_METHOD_PP( XMLParser, Next          )
  CCI_REGISTER_METHOD_PP( XMLParser, Find          )
  CCI_REGISTER_METHOD_PP( XMLParser, Export        )
  CCI_REGISTER_METHOD_PP( XMLParser, GetProperties )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( XMLParser )
{
  CCI_INVOKE_FACTORY(XMLParser)
}
//---------------------------------------------------------------------
