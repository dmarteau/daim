
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
#include "daim_modules/CCI_PropertyList.h"
#include "daim_modules/CCI_StringList.h"

#include "daim/daim_interfaces.h"


#define CCIC_NAME  CCIC_PropertyList
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR "David Marteau"
#define CCI_VERSION   CCI_PropertyList_Version
#define CCI_REVISION  1
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//-------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
//--------------------------------------------------------------------
// Map XML node properties onto a hash for fast access.
// Each property is mapped as "URI#NODEPATH@Property" string 
// The mapping URI <-> FILE is hold into a database, so that a URI
// that map onto the same file will not be read twice.
//
// Each time a client request properties, a access key will be returned
// identifying a request on a specific URI. 
//--------------------------------------------------------------------
#include "daim_modules/CCI_Interface.h"

struct _FILE_RECORD
{
  dmString fr_URI;
  dmTime   fr_Time; 

 ~_FILE_RECORD() {}
  _FILE_RECORD() {}
  _FILE_RECORD( const dmString& _uri, const dmTime& _time )
   : fr_URI(_uri),fr_Time(_time) {}
};

struct _ACCESS_RECORD
{
  dmString ar_URI;
  dmString ar_Buffer; 
  dmString ar_Section; 
  dmString ar_Sequence; 
  dm_int32 ar_Seq;


 ~_ACCESS_RECORD() {}
  _ACCESS_RECORD() {}
  _ACCESS_RECORD( const dmString& _uri, const dmString& _section )
  : ar_URI(_uri), ar_Seq(-1) {
    ar_Section.Assign(ar_URI);
    ar_Section.Append(1,_TXT('#'));
    if(!_section.Empty()) {
      ar_Section.Append(_section);
      ar_Section.Append(_TXT("/"));
    }
  }
};

typedef daim::hashmap<dmString,dmString>       PROPDICT;
typedef daim::hashmap<dmString,_FILE_RECORD>   FILEDICT;
typedef daim::hashmap<dm_param,_ACCESS_RECORD> KEYSDICT;

#define RDF_SEQ_ID    _TXT("id")
#define RDF_SEQ_TAG   _TXT("rdf:seq")
#define RDF_SEQ_SFX   _TXT("rdf:seq/rdf:li_")

CCI_DECL_DATA( PropertyList)
{
  PROPDICT _Properties;
  FILEDICT _FileDB;
  KEYSDICT _Keys;
};

typedef CCI_INSTANCE_PTR(PropertyList) _Instance_Ptr;

//--------------------------------------------------------------------
// Helpers
//--------------------------------------------------------------------
static bool PropertyList_ResolveURI( _Instance_Ptr _This,
                                     dmString& _uri, 
                                     dmString& _protocol, 
                                     dmString& _path 
                                   )
{
  // Find protocol
  size_t pos = _uri.Find(_TXT("://"));
  if(pos!=dmString::npos) 
  {
    _protocol  = _uri.SubStr(0,pos);
    if(!_protocol.Empty()) 
    {
      _path.Assign(_uri,pos+3,dmString::npos);

      //------------------------------------------------
      // Protocols
      //------------------------------------------------
      if(_protocol==_TXT("daim")) {
        if(_path.FileName().Empty()) _path.Assign(_TXT("etc/config.xml"));
        if(_path.FileExt().Empty())  _path.Append(_TXT(".xml"));

        dmString _realpath = dmPortability::SearchEnv(__DAIM_LIBPATH__,_path.CStr());
        if(_realpath.Empty())
          dmLOG(_TXT("ERROR: %s not found !\n"),_path.CStr());
        else {

          _path = _realpath;

          // Find if this file is already mapped onto a URI

          FILEDICT::iterator it = _This->_FileDB.find(_path);
          if(it != _This->_FileDB.end())
            _uri = (*it).second.fr_URI;

          return true;
        }
      } else
      //------------------------------------------------
      // Create a local access with no file associated
      //------------------------------------------------
      if(_protocol==_TXT("local")) {
        _path.Clear();
        return true;
      }
    }
  }
  return false;
}
//--------------------------------------------------------------------
static void PropertyList_GetPropertyName( _Instance_Ptr _This,
                                          _ACCESS_RECORD& _UserKey,
                                          const char_t* _Section,
                                          const char_t* _Property )
{
   dmString& _Buffer = _UserKey.ar_Buffer;

   if(EMPTY_STRING(_Section) && _UserKey.ar_Seq >= 0) {
     _Buffer.Assign(_UserKey.ar_Sequence);
     _Buffer.Append(_UserKey.ar_Seq);
     _Buffer.Append(1,_TXT('/'));
   } 
   else {
     _Buffer.Assign(_UserKey.ar_Section);    
     if(!EMPTY_STRING(_Section)) { 
       _Buffer.Append(_Section);
       _Buffer.Append(1,_TXT('/'));
     }
   }

  _Buffer.Append(1,_TXT('@'));
  _Buffer.Append(_Property); 
}
//--------------------------------------------------------------------
static bool PropertyList_GetProperty( _Instance_Ptr _This,
                                      _ACCESS_RECORD& _UserKey,
                                      const char_t* _Section, 
                                      const char_t* _Property
                                    )
{
  PropertyList_GetPropertyName(_This,_UserKey,_Section,_Property);

  PROPDICT& _dict = _This->_Properties;
  PROPDICT::iterator it = _dict.find(_UserKey.ar_Buffer);
  if(it!=_dict.end()) { 
    _UserKey.ar_Buffer = (*it).second;
    return true;
  }
  return false;
}
//--------------------------------------------------------------------
static void PropertyList_SetProperty( _Instance_Ptr _This,
                                      _ACCESS_RECORD& _UserKey,
                                      const char_t* _Section, 
                                      const char_t* _Property,
                                      const char_t* _Value )
{
  PropertyList_GetPropertyName(_This,_UserKey,_Section,_Property);
  _This->_Properties[_UserKey.ar_Buffer] = _Value;
}
//--------------------------------------------------------------------
static bool PropertyList_GetSeq( _Instance_Ptr _This,
                                 _ACCESS_RECORD& _UserKey,
                                 const char_t* _Section )
{
  if(_UserKey.ar_Seq<=0) 
  {
    dmString& _Buffer = _UserKey.ar_Buffer;

    _Buffer.Assign(_UserKey.ar_Section);
    if(!EMPTY_STRING(_Section)) {
      _Buffer.Append(_Section);
      _Buffer.Append(_TXT("/"));
    }

    PROPDICT& _dict = _This->_Properties;
    PROPDICT::iterator seq_it = _dict.find(dmStrStream(_Buffer) << RDF_SEQ_TAG _TXT("/@") RDF_SEQ_ID);
    if(seq_it!=_dict.end()) { 
      _UserKey.ar_Sequence.Assign(_Buffer);
      _UserKey.ar_Sequence.Append(RDF_SEQ_SFX);
      _UserKey.ar_Seq = 0; // Init the sequence counter
      return true;
    }
  }
  return false;
}
//--------------------------------------------------------------------
static void PropertyList_EndSeq( _Instance_Ptr _This,
                                 _ACCESS_RECORD& _UserKey )
{
  _UserKey.ar_Seq = -1; // End the sequence counter for the caller
  _UserKey.ar_Sequence.Clear();
}
//---------------------------------------------------------------------
static bool PropertyList_NextSeq( _Instance_Ptr _This,
                                  _ACCESS_RECORD& _UserKey )

{
  // Ge the next list item in sequence
  // by searching for the property 'id' associated
  // to this element
  if(_UserKey.ar_Seq>=0) 
  {
    _UserKey.ar_Seq++;  // Inc the sequence counter

    dmString& _Buffer = _UserKey.ar_Buffer;
    _Buffer.Assign(_UserKey.ar_Sequence);
    _Buffer.Append(_UserKey.ar_Seq);
    _Buffer.Append(_TXT("/@"));
    _Buffer.Append(RDF_SEQ_ID); 

    PROPDICT& _dict = _This->_Properties;
    PROPDICT::iterator it = _dict.find(_Buffer);
    if(it!=_dict.end())
       return true;
  }  
  return false;
}
//--------------------------------------------------------------------
// PropertyList constructor
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR(  PropertyList )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( PropertyList)

  CCI_INIT_MEMBER( _Properties )
  CCI_INIT_MEMBER( _FileDB     )
  CCI_INIT_MEMBER( _Keys       )

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// PropertyList destructor
//---------------------------------------------------------------------
static CCI_DESTRUCTOR(  PropertyList )
{
  CCI_INSTANCE( PropertyList) 

  CCI_DESTROY_MEMBER( _Keys       )
  CCI_DESTROY_MEMBER( _FileDB     )
  CCI_DESTROY_MEMBER( _Properties )

  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// Getters
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( PropertyList )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Setters
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( PropertyList )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method      : GetProperty
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( PropertyList, GetProperty )
{
  CCI_INSTANCE( PropertyList)
   
  if(!EMPTY_STRING(CCI_P(property))) 
  {
    KEYSDICT::iterator it = _This->_Keys.find(CCI_P(key));
    if(it != _This->_Keys.end()) 
    {
      _ACCESS_RECORD& _UserKey = (*it).second;
      if(PropertyList_GetProperty(_This,_UserKey,CCI_P(section),CCI_P(property)))
      {
        const dmString& _result = _UserKey.ar_Buffer;
        switch(CCI_P(fmt)) 
        {
          case CCIV_Property_String: 
              CCI_RETVAL_P(value) =  dm_ptr_to_long(_result.CStr()); 
              CCI_RETURN_OK()

          case CCIV_Property_Long  : 
              CCI_RETVAL_P(value) =  static_cast<dm_param>(_result.AsInt(0)); 
              CCI_RETURN_OK()
		      
          case CCIV_Property_Check: 
              CCI_RETURN_OK()
        }
      }
    }
  } 

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method      : GetPropertyReal
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( PropertyList, GetPropertyReal )
{
  CCI_INSTANCE( PropertyList)
   
  if(!EMPTY_STRING(CCI_P(property))) 
  {
    KEYSDICT::iterator it = _This->_Keys.find(CCI_P(key));
    if(it != _This->_Keys.end()) 
    {
      _ACCESS_RECORD& _UserKey = (*it).second;
      if(PropertyList_GetProperty(_This,_UserKey,CCI_P(section),CCI_P(property)))
      {
        CCI_RETVAL_P(value) =  _UserKey.ar_Buffer.AsDouble(); 
        CCI_RETURN_OK()
      }
    }
  } 

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method      : LoadProperties
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( PropertyList, LoadProperties )
{
  CCI_INSTANCE( PropertyList)

  if(!EMPTY_STRING(CCI_P(uri)))
  {
    dmString _uri = CCI_P(uri);
    dmString _path;
    dmString _protocol;

    if(PropertyList_ResolveURI(_This,_uri,_protocol,_path))
    {
      //------------------------------------------------
      // Load data
      //------------------------------------------------
      dmIInterface<dmIXMLParser> 
              Parser(CCIA_DECL_(XMLParser,Options),CCIV_XMLParser_CollectPath,
                     NULL,TGZ_DONE);

      if(Parser.ParseFile(_path.CStr(),NULL)) 
      {
         //------------------------------------------------
         // Collect properties
         // Handle rdf:li node content only as attributes 
         //------------------------------------------------
         Parser.Select(_TXT("//"));  

         dmString _Buffer,_Path,_Tag;
         dm_int32  _RDF_Seq = -1;

         // Create a StringList object
         dmIInterface<dmIStringList> _List;
         dmDEBUG_ASSERT_VALID_INTERFACE(_List)  

         do {

           if(Parser.GetProperties(_List)) 
           {
             _Tag  = Parser.GetTag();
             _Path = Parser.GetPath(); // Get the full path for the node

             if(_Tag=="rdf:seq") {  // Begining of a new sequence
               _RDF_Seq = 0;        // Reset seq counter
               _List.AddTail(RDF_SEQ_ID);
               _List.AddTail("");
             } else {
               if(_RDF_Seq >= 0) {
                 if(_Tag=="rdf:li") { 
                   ++_RDF_Seq;
                   _Path.Append("_");
                   _Path.Append(_RDF_Seq);

                   _List.AddTail(RDF_SEQ_ID);
                   _List.AddTail("");

                 } else
                   _RDF_Seq = -1;   // End of sequence
               }
             }         

             dm_param pos = dm_null;
             while( (pos =_List.GetNext(pos)) ) 
             {
               _Buffer.Assign(_uri.CStr());
               _Buffer.Append(1,_TXT('#'));

               _Buffer.Append(_Path);
               _Buffer.Append(_TXT("/@"));

               _Buffer.Append(_List.GetString(pos));

               // Value is at the next position
               if((pos = _List.GetNext(pos))==dm_null) break;
               _List.GetStringA(pos,_This->_Properties[_Buffer]);

             }
             _List.Clear();
           }
         } while( Parser.Next(NULL,0) ); // Next node
         
         //------------------------------------------------
         // Update file database
         //------------------------------------------------
         _This->_FileDB[_path] = _FILE_RECORD(_uri,
                                   dmPortability::GetFileTime(_path.CStr())
                                 );
         CCI_RETURN_OK()
      }
    }
  } else {
    CCI_SET_ERROR_MSG(dmString::FormatString(dmString::npos,"Empty URI !").CStr());
    CCI_RETURN_INVALID_PARAM()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method    : Open
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( PropertyList, Open )
{
  CCI_INSTANCE( PropertyList)

  if(!EMPTY_STRING(CCI_P(uri)))
  {
    dmString _uri = CCI_P(uri);
    dmString _path;
    dmString _protocol;

    if(PropertyList_ResolveURI(_This,_uri,_protocol,_path))
    {
      if(!_path.Empty()) { 
        // Check that the file is up to date
        FILEDICT::iterator it = _This->_FileDB.find(_path);

        if(it == _This->_FileDB.end() ||
           (*it).second.fr_Time.IsOlderThan(dmPortability::GetFileTime(_path.CStr())))
        {
           dm_param result;
           CCI_IMETHOD_PP_(result,_THIS_OBJECT,PropertyList,LoadProperties,_in _uri.CStr());         
           if(!CCI_SUCCEEDED(result))
               CCI_RETURN_FAIL()
        }
      }
      
      dm_param new_key = dmId::GetId().Hash(); // Generate a user key

      _This->_Keys[new_key] =  _ACCESS_RECORD(_uri,CCI_P(section));
      CCI_RETVAL_P(key) = new_key;
      CCI_RETURN_OK()

    } else 
      CCI_SET_ERROR_MSG(dmString::FormatString(dmString::npos,"Empty URI !").CStr());
  }

  CCI_RETURN_INVALID_PARAM()
}

//---------------------------------------------------------------------
// Method      : Close
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( PropertyList, Close )
{
  CCI_INSTANCE( PropertyList)

  if(CCI_P(key)) {
    KEYSDICT::iterator it = _This->_Keys.find(CCI_P(key));
    if(it!=_This->_Keys.end()) { 
      _This->_Keys.erase(it);
      CCI_RETURN_OK()  
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method      : GetSeq
// Description : Find a sequence for the given section
//               and initialize sequences operations  
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( PropertyList, GetSeq )
{
  CCI_INSTANCE( PropertyList)
   
  KEYSDICT::iterator it = _This->_Keys.find(CCI_P(key));
  if(it != _This->_Keys.end()) {
    if(PropertyList_GetSeq(_This,(*it).second,CCI_P(section)))
      CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method      : NewSeq
// Description : Create a new sequence for the given section
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( PropertyList, NewSeq )
{ 
  CCI_INSTANCE( PropertyList)
   
  KEYSDICT::iterator it = _This->_Keys.find(CCI_P(key));
  if(it != _This->_Keys.end()) 
  {
    _ACCESS_RECORD& _UserKey = (*it).second;

    dmString _Section = CCI_P(section);
    if(!_Section.Empty()) _Section.Append(1,_TXT('/'));
    
    _Section.Append(RDF_SEQ_TAG);

    // First create the property associated to the Seq entry
    PropertyList_SetProperty(_This,_UserKey,_Section.CStr(),RDF_SEQ_ID,NULL);
    
    // Then move to it
    if(PropertyList_GetSeq(_This,_UserKey,CCI_P(section)))
      CCI_RETURN_OK()
  }  

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method      : EndSeq
// Description : End a serie of sequence operations 
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( PropertyList, EndSeq )
{
  CCI_INSTANCE( PropertyList)
   
  KEYSDICT::iterator it = _This->_Keys.find(CCI_P(key));
  if(it != _This->_Keys.end()) 
  {
    PropertyList_EndSeq(_This,(*it).second);
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method      : NextSeq
// Description : Move to the next element in sequence
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( PropertyList, NextSeq )
{
  CCI_INSTANCE( PropertyList)
   
  KEYSDICT::iterator it = _This->_Keys.find(CCI_P(key));
  if(it != _This->_Keys.end()) 
  {
    _ACCESS_RECORD& _UserKey = (*it).second;

    if(PropertyList_NextSeq(_This,_UserKey))
      CCI_RETURN_OK()

    // Terminate the sequence
    PropertyList_EndSeq(_This,_UserKey);
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method      : AddSeq
// Description : Add a property as a sequence element
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( PropertyList, AddSeq )
{
  CCI_INSTANCE( PropertyList)
   
  KEYSDICT::iterator it = _This->_Keys.find(CCI_P(key));
  if(it != _This->_Keys.end()) 
  {
    _ACCESS_RECORD& _UserKey = (*it).second;

    if(_UserKey.ar_Seq>=0) 
    {
      _UserKey.ar_Seq++;  // Inc the sequence counter

      dmString& _Buffer = _UserKey.ar_Buffer;
      _Buffer.Assign(_UserKey.ar_Sequence);
      _Buffer.Append(_UserKey.ar_Seq);
      _Buffer.Append(_TXT("/@"));
      _Buffer.Append(RDF_SEQ_ID); 

      // Insert this as new entry
      _This->_Properties[_Buffer] = _TXT('\0');

      CCI_RETURN_OK()
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method      : SetProperty
// Description : Set the value for the given property
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( PropertyList, SetProperty )
{
  CCI_INSTANCE( PropertyList)

  KEYSDICT::iterator it = _This->_Keys.find(CCI_P(key));
  if(it != _This->_Keys.end()) 
  {
    PropertyList_SetProperty(_This,(*it).second,
                             CCI_P(section),
                             CCI_P(property),
                             CCI_P(value));
        CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method      : GetPropertyList
// Description : Return a list of all values for the given property
//               in a sequence
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( PropertyList, GetPropertyList )
{
  CCI_INSTANCE( PropertyList)

  KEYSDICT::iterator it = _This->_Keys.find(CCI_P(key));
  if(it != _This->_Keys.end()) 
  {
    dmIStringList _Strings;
    if(_Strings.QueryInterface(CCI_P(stringlist)))
    {
      _ACCESS_RECORD& _UserKey = (*it).second;

      // Look for the sequence
      if(PropertyList_GetSeq(_This,_UserKey,CCI_P(section)))
      {
        while(PropertyList_NextSeq(_This,_UserKey)) {
          if(PropertyList_GetProperty(_This,_UserKey,NULL,CCI_P(property))) 
          {
            const dmString& _Result = _UserKey.ar_Buffer;
            if(!_Result.Empty())
              _Strings.AddTailA(_Result);
          }
        } 
        PropertyList_EndSeq(_This,_UserKey);
        CCI_RETURN_OK()
      }
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method      : Dump
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( PropertyList, Dump )
{
  CCI_INSTANCE( PropertyList)

  PROPDICT& _dict = _This->_Properties;
  PROPDICT::iterator __F = _dict.begin();
  PROPDICT::iterator __L = _dict.end();

  for(;__F!=__L;++__F)
    dmLOG("%s = %s\n",(*__F).first.CStr(),(*__F).second.CStr()); 
  
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method      : SetPropertyLong
// Description : Set the value as long for the given property
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( PropertyList, SetPropertyLong )
{
  CCI_INSTANCE( PropertyList)

  KEYSDICT::iterator it = _This->_Keys.find(CCI_P(key));
  if(it != _This->_Keys.end()) 
  {
    dmString _Buf;
    _Buf.Format(dmString::npos,"%ld",CCI_P(value));
    
    PropertyList_SetProperty(_This,(*it).second,
                             CCI_P(section),
                             CCI_P(property),
                             _Buf.CStr());
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method      : SetPropertyReal
// Description : Set the floating point value for the given property
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( PropertyList, SetPropertyReal )
{
  CCI_INSTANCE( PropertyList)

  KEYSDICT::iterator it = _This->_Keys.find(CCI_P(key));
  if(it != _This->_Keys.end()) 
  {
    dmString _Buf;
    _Buf.Format(dmString::npos,"%g",CCI_P(value));
    
    PropertyList_SetProperty(_This,(*it).second,
                             CCI_P(section),
                             CCI_P(property),
                             _Buf.CStr());
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP(PropertyList)
  CCI_REGISTER_METHOD_PP( PropertyList, GetProperty     )
  CCI_REGISTER_METHOD_PP( PropertyList, GetPropertyReal )
  CCI_REGISTER_METHOD_PP( PropertyList, LoadProperties  )
  CCI_REGISTER_METHOD_PP( PropertyList, Open            )
  CCI_REGISTER_METHOD_PP( PropertyList, Close           )
  CCI_REGISTER_METHOD_PP( PropertyList, GetSeq          )
  CCI_REGISTER_METHOD_PP( PropertyList, EndSeq          )
  CCI_REGISTER_METHOD_PP( PropertyList, NextSeq         )
  CCI_REGISTER_METHOD_PP( PropertyList, AddSeq          )
  CCI_REGISTER_METHOD_PP( PropertyList ,NewSeq          ) 
  CCI_REGISTER_METHOD_PP( PropertyList ,SetProperty     ) 
  CCI_REGISTER_METHOD_PP( PropertyList ,GetPropertyList )
  CCI_REGISTER_METHOD_NP( PropertyList ,Dump            ) 
  CCI_REGISTER_METHOD_PP( PropertyList ,SetPropertyLong ) 
  CCI_REGISTER_METHOD_PP( PropertyList ,SetPropertyReal ) 
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( PropertyList )
{
  CCI_INVOKE_FACTORY(PropertyList)
}
//---------------------------------------------------------------------
