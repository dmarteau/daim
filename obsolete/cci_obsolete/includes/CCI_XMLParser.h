#ifndef CCI_XMLParser_h
#define CCI_XMLParser_h

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
// File         : CCI_XMLParser.h
// Date         : 7/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

//-----------------------------------------------------------------
// XML Expat Based Parser
//-----------------------------------------------------------------
#define CCIC_XMLParser  _TXT("xmlparser.cci")
#include "daim_modules/CCI_Prolog.h"

CCI_BEGIN_MODULE_DECL( XMLParser, Notify )
  
  CCI_MODULE_UUID( XMLParser, c4e4d685-1803-4775-99d7-e8b41480cd2e)

  CCI_MODULE_VERSION( XMLParser, 3 ) 

  /* Methods */

  CCI_DECL_METHOD_PP(XMLParser,1,  ParseFile     , _in CONST char_t* file _in CONST char_t* encoding )
  CCI_DECL_METHOD_PP(XMLParser,2,  ParseBuffer   , _in CONST char_t* buffer _in dm_uint size _in CONST char_t* encoding )
  CCI_DECL_METHOD_NP(XMLParser,3,  Play          )
  CCI_DECL_METHOD_PP(XMLParser,4,  GetProperty   , _in CONST char_t* property _out dm_param value _in dm_uint fmt )
  CCI_DECL_METHOD_PP(XMLParser,5,  SetProperty   , _in CONST char_t* property _in  dm_param value _in dm_uint fmt )
  CCI_DECL_METHOD_PP(XMLParser,6,  RemProperty   , _in CONST char_t* property )
  CCI_DECL_METHOD_PP(XMLParser,7,  InsertItem    , _in CONST char_t* name )
  CCI_DECL_METHOD_NP(XMLParser,8,  DeleteItem    )
  CCI_DECL_METHOD_NP(XMLParser,9,  Clear         )
  CCI_DECL_METHOD_PP(XMLParser,10, Select        , _in CONST char_t* path _in dm_uint index )
  CCI_DECL_METHOD_PP(XMLParser,11, Next          , _in CONST char_t* name _in dm_uint sibling  )

  CCI_DECL_METHOD_PP(XMLParser,12, Find, 
         _in CONST char_t* name
         _in CONST char_t* property
         _in CONST char_t* value 
  )

  CCI_DECL_METHOD_PP(XMLParser,13, Export        , _in CONST char_t* file _in CONST char_t* preamble )
  CCI_DECL_METHOD_PP(XMLParser,14, GetProperties , _in dm_cci(StringList) strlist )

  /* Attributs */

  CCI_DECL_ATTR(XMLParser,1,  Status      , dm_bool      , [..GN] )
  CCI_DECL_ATTR(XMLParser,2,  Tag         , char_t*      , [..GN] )
  CCI_DECL_ATTR(XMLParser,3,  Content     , char_t*      , [.SGN] )
  CCI_DECL_ATTR(XMLParser,5,  Document    , dm_bool      , [...N] )
  CCI_DECL_ATTR(XMLParser,6,  BufferSize  , dm_uint      , [ISGN] )
  CCI_DECL_ATTR(XMLParser,7,  Options     , dm_uint      , [ISGN] )
  CCI_DECL_ATTR(XMLParser,8,  Path        , char_t*      , [..G.] )

  /* special values for CCIM_XMLParser_AddProperty & CCIM_XMLParser_GetProperty */
  #define CCIV_XMLParser_String 0L /* return char_t*        */
  #define CCIV_XMLParser_Long   1L /* return dm_long_ptr    */ 
  #define CCIV_XMLParser_Toggle 2L /* return dm_bool        */
  #define CCIV_XMLParser_Hex    3L /* store has hexadecimal */

  /* special values for CCIM_XMLParser_AddProperty & CCIM_XMLParser_GetProperty */
  #define CCIV_XMLParser_CollectPath 0x1L
  

CCI_END_MODULE_DECL( XMLParser )


#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
// C++ interface
//------------------------------------------------------------------------------------
class dmIXMLParser : public dmINotify
{
  public:
    dmDECLARE_GENERIC_IINTERFACE(XMLParser)

    bool GetProperties(dm_cci(StringList) strlist) const { 
      CCI_RETURN_IMETHOD_PP_OK(XMLParser,GetProperties, _in strlist); 
    }

    const char_t* GetContent()    const { return CCI_IGET(XMLParser,Content,(char_t*)NULL); }
    const char_t* GetPath()       const { return CCI_IGET(XMLParser,Path   ,(char_t*)NULL); }

    dm_uint GetStatus() const { return CCI_IGET(XMLParser,Status,dm_false); }

    const char_t* GetTag() const        { return CCI_IGET(XMLParser,Tag ,(char_t*)NULL); }
    bool SetContent(const char_t* _str) { return CCI_ISET(XMLParser,Content,_str); }

    bool Clear() { CCI_RETURN_IMETHOD_NP_OK(XMLParser,Clear); }
    bool Play()  { CCI_RETURN_IMETHOD_NP_OK(XMLParser,Play);  }

    bool Select( const char_t* path, dm_uint index   = 0 ) { CCI_RETURN_IMETHOD_PP_OK(XMLParser,Select, _in path _in index   ) }
    bool Next  ( const char_t* name, dm_uint sibling = 1 ) { CCI_RETURN_IMETHOD_PP_OK(XMLParser,Next  , _in name _in sibling ) }

    bool SetProperty(const char_t* property, const char_t* value) { CCI_RETURN_IMETHOD_PP_OK(XMLParser,SetProperty,_in property _in (dm_param)value _in CCIV_XMLParser_String) }
    bool SetProperty(const char_t* property, bool value)          { CCI_RETURN_IMETHOD_PP_OK(XMLParser,SetProperty,_in property _in (value?1L:0L) _in CCIV_XMLParser_Toggle) }
    bool SetProperty(const char_t* property, long value)          { CCI_RETURN_IMETHOD_PP_OK(XMLParser,SetProperty,_in property _in value _in CCIV_XMLParser_Long) }
    bool SetProperty(const char_t* property, unsigned long value) { CCI_RETURN_IMETHOD_PP_OK(XMLParser,SetProperty,_in property _in value _in CCIV_XMLParser_Hex)  }

    bool RemProperty(const char_t* property ) { CCI_RETURN_IMETHOD_PP_OK(XMLParser,RemProperty, _in property) }
    bool InsertItem (const char_t* name     ) { CCI_RETURN_IMETHOD_PP_OK(XMLParser,InsertItem, _in name) }

    bool ParseFile(const char_t* _input, const char_t* _encoding = NULL) {
      CCI_RETURN_IMETHOD_PP_OK(XMLParser,ParseFile, _in _input _in _encoding)
    }

    bool ParseBuffer(const char_t* _buffer, size_t _size, const char_t* _encoding = NULL) {
      CCI_RETURN_IMETHOD_PP_OK(XMLParser,ParseBuffer, _in _buffer _in _size _in _encoding)
    }


    const char_t* GetProperty(const char_t* property, const char_t* dflt) {
      IF_CCI_IMETHOD_RP(XMLParser,GetProperty, _in property _in dm_null _in CCIV_XMLParser_String) 
      THEN  return reinterpret_cast<char_t*>( CCI_RETVAL(value) );
      ELSE  return dflt;
      ENDIF
    }

    long GetProperty(const char_t* property,long dflt) {
      IF_CCI_IMETHOD_RP(XMLParser,GetProperty, _in property _in 0 _in CCIV_XMLParser_Long) 
      THEN  return CCI_RETVAL(value);
      ELSE  return dflt;
      ENDIF
    }

    const char_t* GetProperty(const char_t* property) {
      IF_CCI_IMETHOD_RP(XMLParser,GetProperty, _in property _in 0 _in CCIV_XMLParser_Toggle) 
      THEN  return reinterpret_cast<char_t*>( CCI_RETVAL(value) );
      ELSE  return NULL;
      ENDIF
    }

    bool ChkProperty(const char_t* property) { return GetProperty(property)!=0; }

};
//------------------------------------------------------------------------------------
class dmIXMLHelper
{
  protected:
    dmIXMLParser& _parser;

    virtual void OnBeginDocument() {}
    virtual void OnEndDocument()   {}
    virtual void OnStartItem()     {}
    virtual void OnEndItem()       {}

 public:
    dmIXMLHelper(dmIXMLParser&);
    virtual ~dmIXMLHelper() {
      _parser.KillNotify(0,(cci_Object*)this);
    }

    dmIXMLParser* operator->() { return &_parser; }
};
//------------------------------------------
inline dmIXMLHelper::dmIXMLHelper( dmIXMLParser& parser )
: _parser(parser) 
{
  if(_parser) {
    _parser.BindNotifyMember0(CCIA_DECL_(XMLParser,Status  ),dm_true ,this,&dmIXMLHelper::OnStartItem,CCIV_NotifySelf|CCIV_DoNotCheck);
    _parser.BindNotifyMember0(CCIA_DECL_(XMLParser,Status  ),dm_false,this,&dmIXMLHelper::OnEndItem  ,CCIV_NotifySelf|CCIV_DoNotCheck);
    _parser.BindNotifyMember0(CCIA_DECL_(XMLParser,Document),dm_true ,this,&dmIXMLHelper::OnBeginDocument,CCIV_NotifySelf);
    _parser.BindNotifyMember0(CCIA_DECL_(XMLParser,Document),dm_false,this,&dmIXMLHelper::OnEndDocument  ,CCIV_NotifySelf);
  }
}
//------------------------------------------

#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

//-------------------------------------------------------------
#endif /* CCI_XMLParser_h */
