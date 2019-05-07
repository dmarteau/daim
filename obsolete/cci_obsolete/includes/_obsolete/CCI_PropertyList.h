#ifndef CCI_PropertyList_h
#define CCI_PropertyList_h

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
// File         : CCI_PropertyList.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_PropertyList  _TXT("propertylist.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( PropertyList, Notify )
  
  CCI_MODULE_UUID( PropertyList, 0697e6aa-bfd5-4b9c-8dc4-acebdacfc897)

  CCI_MODULE_VERSION( PropertyList, 1 ) 

  /* Methods */
  CCI_DECL_METHOD_PP(PropertyList,1, Open,        
       _in  CONST char_t* uri 
       _in  CONST char_t* section 
       _out dm_param      key 
  )
 
  CCI_DECL_METHOD_PP(PropertyList,2, Close,       _in dm_param key ) 
  CCI_DECL_METHOD_PP(PropertyList,3, GetProperty, 
      _in  dm_param      key
      _in  CONST char_t* section
      _in  CONST char_t* property 
      _out dm_param      value 
      _in  dm_param      fmt 
  )

  CCI_DECL_METHOD_PP(PropertyList,4, LoadProperties, _in CONST char_t* uri )

  CCI_DECL_METHOD_PP(PropertyList,5,  GetSeq     , _in dm_param key _in CONST char_t* section )
  CCI_DECL_METHOD_PP(PropertyList,10, NewSeq     , _in dm_param key _in CONST char_t* section )
  CCI_DECL_METHOD_PP(PropertyList,6,  EndSeq     , _in dm_param key ) 
  CCI_DECL_METHOD_PP(PropertyList,7,  NextSeq    , _in dm_param key ) 
  CCI_DECL_METHOD_PP(PropertyList,11, AddSeq     , _in dm_param key ) 


  CCI_DECL_METHOD_NP(PropertyList,8, Dump )

  CCI_DECL_METHOD_PP(PropertyList,9, SetProperty, 
      _in  dm_param      key
      _in  CONST char_t* section
      _in  CONST char_t* property 
      _in  CONST char_t* value
  )

  CCI_DECL_METHOD_PP(PropertyList,12, GetPropertyList, 
      _in  dm_param           key
      _in  CONST char_t*      section
      _in  CONST char_t*      property 
      _in  dm_cci(StringList) stringlist
  )


  CCI_DECL_METHOD_PP(PropertyList,20, GetPropertyLong, 
      _in  dm_param      key
      _in  CONST char_t* section
      _in  CONST char_t* property 
      _out dm_param      value 
  )

  CCI_DECL_METHOD_PP(PropertyList,21, GetPropertyStr, 
      _in  dm_param      key
      _in  CONST char_t* section
      _in  CONST char_t* property 
      _out char_t*       value 
  )

  CCI_DECL_METHOD_PP(PropertyList,21, GetPropertyReal, 
      _in  dm_param      key
      _in  CONST char_t* section
      _in  CONST char_t* property 
      _out dm_real       value 
  )

  CCI_DECL_METHOD_PP(PropertyList,22, CheckProperty, 
      _in  dm_param      key
      _in  CONST char_t* section
      _in  CONST char_t* property 
  )


  CCI_DECL_METHOD_PP(PropertyList,23, SetPropertyLong, 
      _in  dm_param      key
      _in  CONST char_t* section
      _in  CONST char_t* property 
      _in dm_param       value 
  )

  CCI_DECL_METHOD_PP(PropertyList,24, SetPropertyReal, 
      _in  dm_param      key
      _in  CONST char_t* section
      _in  CONST char_t* property 
      _in  dm_real       value 
  )

  /* special values for CCIM_PropertyList_GetProperty */
  #define CCIV_Property_String dm_param_t(0)  /* return char_t*   */
  #define CCIV_Property_Long   dm_param_t(1)  /* return dm_param  */ 
  #define CCIV_Property_Real   dm_param_t(2)  /* return dm_real   */ 
  #define CCIV_Property_Check  dm_param_t(2)  /* return TRUE or FALSE */

CCI_END_MODULE_DECL( PropertyList )

#endif // __doxygen

#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Implement methods for acessing structured properties.
 *
 *  The PropertyList interface expose methods for accessing external
 *  or local ressources in structured way. Resources can be either external
 *  XML files or user-built ressources (in which case it is a "local" resources )
 *
 *  Properties are specified as a list of '/' separated sections which references 
 *  a node in the resource.
 *
 *  Sequences can be specified, a sequence is a list of unnamed nodes that are sibling
 *  to a specific section.  
 */
//------------------------------------------------------------------------------------
class dmIPropertyList : public dmINotify
{
  public:
    dmDECLARE_GENERIC_IINTERFACE(PropertyList)

    //-------------------------------------------------------
    /*! Open a property list resource from located at \a uri
     * \param uri the name of the resource. See \ref named_res for
     *            more infos on named resources.
     * \param section the section that you are interested in.
     *     Can be NULL if you plan to inspect muliple sections.             
     * \return An access key, this value should be used in
     *         subsequent call to methods that require a key as input
     *         parameter. The key should always be released with
     *         the Close() method.
     */       
    //-------------------------------------------------------
    dm_param Open( CONST char_t* uri, CONST char_t* section = NULL ) {
      IF_CCI_IMETHOD_RP(PropertyList,Open, _in uri _in section _out(key) 0 )
      THEN return CCI_RETVAL(key);
      ELSE return dm_null;
      ENDIF
    }

    //-------------------------------------------------------
    /*! Close the session initiated by the Open() method.  */       
    //-------------------------------------------------------
    void  Close( dm_param key ) { CCI_IINVOKE_PP(PropertyList,Close, _in key); }

    //-------------------------------------------------------
    /*! Return a string value associated to the \a property 
     *  parameter.
     *  \param key      the key returned by Open() 
     *  \param section  the section (or node) you are interested in
     *    (can be null if already specified in Open().
     *  \param property the name of the property.
     *  \param dflt  a default value to be returned if the property is
     *               not found or has no value associated to it.
     *  \return The string value of the property.
     */      
    //-------------------------------------------------------
    const char_t* GetPropertyStr( dm_param key , const char_t* section, const char_t* property, const char_t* dflt = NULL) {
      IF_CCI_IMETHOD_RP(PropertyList,GetProperty, _in key _in section _in property _in dm_null _in CCIV_Property_String) 
      THEN  return reinterpret_cast<char_t*>( CCI_RETVAL(value) );
      ELSE  return dflt;
      ENDIF
    }

    //-------------------------------------------------------
    /*! Return a long value associated to the \a property 
     *  parameter. see GetPropertyStr() for parameters.
     */      
    //-------------------------------------------------------
    long GetPropertyLong( dm_param key, const char_t* section, const char_t* property,long dflt) {
      IF_CCI_IMETHOD_RP(PropertyList,GetProperty, _in key _in section _in property _in dm_null _in CCIV_Property_Long) 
      THEN  return CCI_RETVAL(value);
      ELSE  return dflt;
      ENDIF
    }

    //-------------------------------------------------------
    /*! Return a real (floating point) value associated to the \a property 
     *  parameter. see GetPropertyStr() for parameters.
     */      
    //-------------------------------------------------------
    dm_real GetPropertyReal( dm_param key, const char_t* section, const char_t* property,dm_real dflt) {
      IF_CCI_IMETHOD_RP(PropertyList,GetPropertyReal, _in key _in section _in property _in 0 ) 
      THEN  return CCI_RETVAL(value);
      ELSE  return dflt;
      ENDIF
    }

    //-------------------------------------------------------
    /*! Return a boolean value associated to the \a property 
     *  parameter. see GetPropertyStr() for parameters.
     */      
    //-------------------------------------------------------
    bool GetPropertyBool( dm_param key, const char_t* section, const char_t* property, bool dflt) {
      IF_CCI_IMETHOD_RP(PropertyList,GetProperty, _in key _in section _in property _in CCIV_Property_String) 
      THEN  return EQUAL_STRING(reinterpret_cast<char_t*>(CCI_RETVAL(value)),_TXT("true"));
      ELSE  return dflt;
      ENDIF
    }

    //-------------------------------------------------------
    /*! Check if the property exists or not. */
    //-------------------------------------------------------
    bool CheckProperty( dm_param key , const char_t* section, const char_t* property) {
      CCI_RETURN_IMETHOD_PP_OK(PropertyList,GetProperty, 
         _in key _in section _in property _in 0 _in CCIV_Property_Check) 
    }

    //-------------------------------------------------------
    /*! Start a sequence at the section \a section
     *  Return false if there is no sequence associated 
     *  to the specified section
     */
    //-------------------------------------------------------
    bool GetSeq ( dm_param key, const char_t *section ) { 
       CCI_RETURN_IMETHOD_PP_OK(PropertyList,GetSeq , _in key _in section ); 
    }

    //-------------------------------------------------------
    /*! Create a new sequence associated to the section \a section.
     *  Note that the section will be created if it does not exists.
     */
    //-------------------------------------------------------
    bool NewSeq( dm_param key, const char_t *section ) { 
       CCI_RETURN_IMETHOD_PP_OK(PropertyList,NewSeq , _in key _in section ); 
    } 
    //-------------------------------------------------------
    /*! Add a new item to the sequence opened by 
     *  GetSeq() or NewSeq() 
     */
    //-------------------------------------------------------
    bool AddSeq( dm_param key ) { 
       CCI_RETURN_IMETHOD_PP_OK(PropertyList,AddSeq , _in key ); 
    } 

    //-------------------------------------------------------
    /*! Move to the next item in a sequence */
    //-------------------------------------------------------
    bool NextSeq( dm_param key ) { CCI_RETURN_IMETHOD_PP_OK(PropertyList,NextSeq, _in key ); } 

    //-------------------------------------------------------
    /*! Terminate an acess to a sequence opened by GetSeq() or NewSeq() */
    //-------------------------------------------------------
    bool EndSeq ( dm_param key ) { CCI_RETURN_IMETHOD_PP_OK(PropertyList,EndSeq , _in key ); } 

    //-------------------------------------------------------
    /*! Set a property value at section \a section.
     *  \a section can be set to NULL if has been already specified 
     *  in Open().
     */
    //-------------------------------------------------------
    bool SetProperty( dm_param key, const char_t *section, const char_t *property, const char_t* value ) {
      CCI_RETURN_IMETHOD_PP_OK(PropertyList,SetProperty, _in key _in section _in property _in value ) 
    }

    bool SetPropertyStr( dm_param key, const char_t *section, const char_t *property, const char_t* value ) {
      return SetProperty(key,section,property,value);
    }

    //-------------------------------------------------------
    /*! Return a list of all values for the given property in a sequence
     *  associated to the section \a section. 
     *  \a stringlist must expose a valid dmIStringList interface.
     */
    //-------------------------------------------------------
    bool GetPropertyList( dm_param key, const char_t *section, const char_t* property, 
                          dm_cci(StringList) stringlist ) 
    {
      CCI_RETURN_IMETHOD_PP_OK(PropertyList,GetPropertyList, _in key 
                     _in section 
                     _in property 
                     _in stringlist ) 
    }

    void Dump() { CCI_IINVOKE_NP(PropertyList,Dump); } 

    //-------------------------------------------------------
    /*! Set a long int property value at section \a section.
     *  \a section can be set to NULL if has been already specified 
     *  in Open().
     */
    //-------------------------------------------------------
    bool SetPropertyLong( dm_param key, const char_t *section, const char_t *property, long value  ) {
      CCI_RETURN_IMETHOD_PP_OK(PropertyList,SetPropertyLong, _in key _in section _in property _in value ) 
    }

    //-------------------------------------------------------
    /*! Set a floating point value at section \a section.
     *  \a section can be set to NULL if has been already specified 
     *  in Open().
     */
    //-------------------------------------------------------
    bool SetPropertyReal( dm_param key, const char_t *section, const char_t *property, dm_real value ) {
      CCI_RETURN_IMETHOD_PP_OK(PropertyList,SetPropertyReal, _in key _in section _in property _in value ) 
    }

    bool SetPropertyBool( dm_param key, const char_t *section, const char_t *property, bool value ) {
      return SetProperty(key,section,property,(value?_TXT("true"):_TXT("false")));
    }

};
//------------------------------------------

#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

//-------------------------------------------------------------
#endif /* CCI_PropertyList_h */
