#ifndef CCI_Properties_h
#define CCI_Properties_h

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
// File         : CCI_Properties.h
// Date         : 02/2008
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_Properties  _TXT("properties.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Properties, Notify )
  
  CCI_MODULE_UUID( Properties, 7e5ae400-c524-4ceb-b174-3bef78eb7f4d)

  CCI_MODULE_VERSION( Properties, 1 ) 


  CCI_DECL_METHOD_PP(Properties,1, GetChildList,
       _in  CONST char_t* startingAt
       _in  dm_cci(StringList)  strlist 
  )

  CCI_DECL_METHOD_PP(Properties,2, ClearBranch,
      _in  CONST char_t* startingAt 
  )

  CCI_DECL_METHOD_PP(Properties,3, StringData,
      _in  dm_param      me 
      _in  CONST char_t* property 
      _out CONST char_t* value 
  )

  CCI_DECL_METHOD_PP(Properties,4, IntData,
      _in  dm_param      me 
      _in  CONST char_t* property 
      _out dm_int        value 
  )

  CCI_DECL_METHOD_PP(Properties,5, RealData,
      _in  dm_param      me 
      _in  CONST char_t* property 
      _out dm_real       value 
  )

  CCI_DECL_METHOD_PP(Properties,6, ObjectData,
    _in  dm_param      me 
    _in  CONST char_t* property 
    _out cci_Object*   value 
  )

  /* Attributs */
  CCI_DECL_ATTR(Properties, 1,  Root,  CONST char_t* , [.SG.] )

CCI_END_MODULE_DECL( Properties )

#endif  // __doxygen


#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Implement properties access methods
 */
//------------------------------------------------------------------------------------
class dmIProperties : public dmINotify
{
  public:
    dmDECLARE_GENERIC_IINTERFACE(Properties)
    
    const char_t* GetStringData( const char_t* property, const char_t* dflt = dm_null ) {
      IF_CCI_IMETHOD_RP(Properties,StringData,_in CCI_ME_GET _in property _out(value) dm_null ) 
      THEN  return CCI_RETVAL(value);
      ELSE  return dflt;
      ENDIF
    }

    bool SetStringData( const char_t* property, const char_t* value ) {
      CCI_RETURN_IMETHOD_PP_OK(Properties,StringData,_in CCI_ME_SET _in property _in value ) 
    }

    dm_int GetIntData( const char_t* property, dm_int dflt ) {
      IF_CCI_IMETHOD_RP(Properties,IntData,_in CCI_ME_GET _in property _out(value) 0 ) 
      THEN  return CCI_RETVAL(value); 
      ELSE  return dflt;
      ENDIF
    }

    bool SetIntData( const char_t* property, dm_int value  ) {
      CCI_RETURN_IMETHOD_PP_OK(Properties,IntData,_in CCI_ME_SET _in property _in value ) 
    }

    dm_real GetRealData( const char_t* property, dm_real dflt ) {
      IF_CCI_IMETHOD_RP(Properties,IntData,_in CCI_ME_GET _in property _out(value) 0 ) 
      THEN  return CCI_RETVAL(value);
      ELSE  return dflt;
      ENDIF
    }

    bool SetRealData( const char_t* property, dm_real value  ) {
      CCI_RETURN_IMETHOD_PP_OK(Properties,RealData,_in CCI_ME_SET _in property _in value ) 
    }    
    
    cci_Object* GetObjectData( const char_t* property ) {
      IF_CCI_IMETHOD_RP(Properties,ObjectData,_in CCI_ME_GET _in property _out(value) 0 ) 
      THEN  return CCI_RETVAL(value);
      ELSE  return dm_null;
      ENDIF
    }

    bool SetObjectData( const char_t* property, cci_Object*  value ) {
      CCI_RETURN_IMETHOD_PP_OK(Properties,ObjectData,_in CCI_ME_SET _in property _in value ) 
    }    

};
//------------------------------------------

#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

//-------------------------------------------------------------
#endif /* CCI_Properties_h */
