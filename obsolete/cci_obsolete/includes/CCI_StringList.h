#ifndef CCI_StringList_H
#define CCI_StringList_H

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
// File         : CCI_StringList.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_StringList  _TXT("StringList.cci")
#include "daim_modules/CCI_Prolog.h"
//--------------------------------------------
#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( StringList, Notify )
  
  CCI_MODULE_UUID( StringList, a4dbfe34-0e3a-4d40-b83e-e982e966e709)

  CCI_MODULE_VERSION( StringList, 1 ) 

  /* Operators */

  CCI_DECL_METHOD_PP(StringList,1,  AddTail   , _in CONST char_t* str )
  CCI_DECL_METHOD_PP(StringList,2,  AddHead   , _in CONST char_t* str )
  CCI_DECL_METHOD_PP(StringList,3,  Insert    , _in  dm_param pos _in CONST char_t* str )
  CCI_DECL_METHOD_PP(StringList,4,  Remove    , _in  dm_param pos )
  CCI_DECL_METHOD_NP(StringList,5,  Clear     )
  CCI_DECL_METHOD_PP(StringList,6,  GetFirst  , _out dm_param pos )
  CCI_DECL_METHOD_PP(StringList,7,  GetNext   , _in  dm_param pos _out dm_param next )
  CCI_DECL_METHOD_PP(StringList,8,  GetString , _in  dm_param pos _out CONST char_t* str )
  CCI_DECL_METHOD_PP(StringList,9,  Copy      , _in dm_cci(StringList) strlist )
  CCI_DECL_METHOD_PP(StringList,10, Append    , _in dm_cci(StringList) strlist )

  CCI_DECL_METHOD_PP(StringList,11, AddStrings, 
       _in CONST dm_array(char_t*,count) strlist
       _in dm_uint count
  )

  CCI_DECL_METHOD_PP(StringList,12, GetStringAt, 
       _in  dm_uint       index
       _out CONST char_t* str
  )

  CCI_DECL_METHOD_PP(StringList,13, FindString, 
      _in  CONST char_t* str
      _in  dm_param      pos 
      _out dm_param      found
  )

  #ifdef DAIM_UTILITIES
  /* Use shared strings for efficiency */
  CCI_DECL_METHOD_PP(StringList,20,  AddTailA   , _in CONST dmString* str )
  CCI_DECL_METHOD_PP(StringList,21,  AddHeadA   , _in CONST dmString* str )
  CCI_DECL_METHOD_PP(StringList,22,  InsertA    , _in dm_param pos _in CONST dmString* str )
  CCI_DECL_METHOD_PP(StringList,23,  GetStringA , _in dm_param pos _in dmString* str )
  #endif //DAIM_UTILITIES

  /* Attributs */

  CCI_DECL_ATTR(StringList,1,  Count  , dm_uint, [ISGN] )


CCI_END_MODULE_DECL( CCI_StringList )

#endif //__doxygen

//----------------------------------------------------------------
#ifdef __cplusplus
#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief String list container
 *
 *  This class is used to exchange string data between CCI objects and/or
 *  user code.
 */
//------------------------------------------------------------------------------------
class dmIStringList : public dmINotify
{  
  public:  
   dmDECLARE_GENERIC_IINTERFACE(StringList)

   //-------------------------------------------------------
   /*! \brief Return the number of strings in the stringlist */
   //-------------------------------------------------------
   dm_uint Count() { return CCI_IGET(StringList,Count,0L); }

   //-------------------------------------------------------
   /*! \brief Add a string at the end of the list         */
   //-------------------------------------------------------
   bool AddTail( const char_t* str ) { CCI_RETURN_IMETHOD_PP_OK(StringList,AddTail, _in str ) }

   //-------------------------------------------------------
   /*! \brief Add a string at the beginning of the list   */
   //-------------------------------------------------------
   bool AddHead( const char_t* str ) { CCI_RETURN_IMETHOD_PP_OK(StringList,AddHead, _in str ) }

   //-------------------------------------------------------
   /*! \brief  Insert a string at position \a pos.               
    *  Note that \a pos is an iterator parameter that must have
    *  been obtain using GetFirst() or GetNext() methods
    */
   //-------------------------------------------------------
   bool Insert( dm_param pos, const char_t* str ) { 
     CCI_RETURN_IMETHOD_PP_OK(StringList,Insert,_in pos _in str )
   }

   //-------------------------------------------------------
   /*! \brief  Return an iterator to the first string position in the list. */
   //-------------------------------------------------------
   dm_param GetFirst() {
     IF_CCI_IMETHOD_RP(StringList,GetFirst, _out(pos) 0 )
     THEN return CCI_RETVAL(pos); ENDIF
     return 0;
   }

   //-------------------------------------------------------
   /*! \brief Return the next string position in the list. */
   //-------------------------------------------------------
   dm_param GetNext( dm_param pos ) { 
     IF_CCI_IMETHOD_RP(StringList,GetNext, _in pos _out(next) 0 )
     THEN return CCI_RETVAL(next); ENDIF
     return 0;
   }

   //-------------------------------------------------------
   /*! \brief Remove the string at position designated by \a pos */
   //-------------------------------------------------------
   bool Remove( dm_param pos ) { CCI_RETURN_IMETHOD_PP_OK(StringList,Remove ,_in pos); }

   //-------------------------------------------------------
   /*! \brief Remove all strings in the stringlist        */
   //-------------------------------------------------------
   void Clear() { CCI_IINVOKE_NP(StringList,Clear); }

   //-------------------------------------------------------
   /*! \brief Copy one string list into another           */
   //-------------------------------------------------------
   bool Copy( dm_cci(StringList) strlist ) { 
     CCI_RETURN_IMETHOD_PP_OK(StringList,Copy, _in strlist) 
   }

   //-------------------------------------------------------
   /*! \brief Append all strings from \a strlist          */
   //-------------------------------------------------------
   bool Append( dm_cci(StringList) strlist ) { 
     CCI_RETURN_IMETHOD_PP_OK(StringList,Append, _in strlist) 
   }

   //-------------------------------------------------------
   /*! \brief Return the string at position designated by \a pos */
   //-------------------------------------------------------
   const char_t* GetString( dm_param pos ) { 
     IF_CCI_IMETHOD_RP(StringList,GetString, _in pos _out(str) NULL )
     THEN return CCI_RETVAL(str); ENDIF
     return NULL;
   }

   //-------------------------------------------------------
   /*! \brief Add string from null terminated array of C strings
    *         the string at position designated by \a pos 
    */
   //-------------------------------------------------------
   bool AddStrings( CONST char_t** strlist, dm_uint count ) { 
     CCI_RETURN_IMETHOD_PP_OK(StringList,AddStrings, _in strlist _in count )
   }

   bool Empty() { return Count()==0; }

   #ifdef DAIM_UTILITIES
   bool AddTailA  ( const dmString& str ) { CCI_RETURN_IMETHOD_PP_OK(StringList,AddTailA  ,_in &str ) }
   bool AddHeadA  ( const dmString& str ) { CCI_RETURN_IMETHOD_PP_OK(StringList,AddHeadA  ,_in &str ) }

   bool GetStringA( dm_param pos, dmString& str ) { 
     CCI_RETURN_IMETHOD_PP_OK(StringList,GetStringA,_in pos _in &str ) 
   }

   bool InsertA( dm_param pos, const dmString& str ) { 
     CCI_RETURN_IMETHOD_PP_OK(StringList,InsertA   ,_in pos _in &str ); 
   }
   #endif // DAIM_UTILITIES

   //-------------------------------------------------------
   /*! \brief Return string at index \a param.
    *
    *  This method is only here for convenience but it is absolutely
    *  inefficient for accessing elements iterativeley : 
    *  use GetFirst/GetNext instead.
    */
   //-------------------------------------------------------
   const char_t* GetStringAt( dm_uint index ) { 
     IF_CCI_IMETHOD_RP(StringList,GetStringAt, _in index )
     THEN return CCI_RETVAL(str); ENDIF
     return 0;
   }

   //-------------------------------------------------------
   /*! \brief Find the  string \a str from position \a pos
    *
    * Return the position where the string was found, or null 
    * if the string was not found in the sequence
    */
   //-------------------------------------------------------
   dm_param  FindString( const char_t* str , dm_param pos ) { 
     IF_CCI_IMETHOD_RP(StringList,FindString, _in str _in pos _out(found) 0 )
     THEN return CCI_RETVAL(found); ENDIF
     return 0;
   }


};
//------------------------------------------------------------------------------------
#endif // __cplusplus

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_StringList_H */
