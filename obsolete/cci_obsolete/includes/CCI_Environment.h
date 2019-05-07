#ifndef CCI_Environment_h
#define CCI_Environment_h

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
// File         : CCI_Environment.h
// Date         : 10/2004
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_Environment  _TXT("environment.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Environment, Notify )
  
  CCI_MODULE_UUID( Environment, c7a0939b-0535-4234-8d17-2d9bad4b499a)

  CCI_MODULE_VERSION( Environment, 1 ) 

  CCI_DECL_METHOD_PP( Environment, 1, GetFile, 
       _in  CONST char_t* name 
       _out CONST char_t* location
       _in  dm_uint       flags
  )

  CCI_DECL_METHOD_PP( Environment, 2, GetSpecialFolder, 
       _in  CONST char_t* name 
       _out CONST char_t* location
  )

  CCI_DECL_METHOD_PP( Environment, 3, CreateFolder, 
       _in  CONST char_t* name 
  )

  CCI_DECL_METHOD_PP( Environment, 4, RegisterModules, 
       _in  CONST char_t* location 
       _in  dm_uint       flags
  )

  /* Attributs */

  #define CCIV_Environment_Shared 0x01
  #define CCIV_Environment_User   0x02
  #define CCIV_Environment_Lib    0x03

CCI_END_MODULE_DECL( Environment )

#endif // __doxygen

#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Environment
 *
 *  
 */
//------------------------------------------------------------------------------------
class dmIEnvironment : public dmINotify
{    
  public:
   dmDECLARE_GENERIC_IINTERFACE(Environment)

   /*! Flags Constants */
   enum Constants {
     flSharedDir = CCIV_Environment_Shared,
     flUserDir   = CCIV_Environment_User,
     flLibDir    = CCIV_Environment_Lib
   };

   //-------------------------------------------------------
   /*! \brief Return path for the given filename
    */
   //-------------------------------------------------------
   CONST char_t* GetFile( const char_t* name, dm_bool shared ) {
     IF_CCI_IMETHOD_RP(Environment,GetFile, _in name _out(location) NULL _in shared ) 
      THEN return CCI_RETVAL(location);
      ELSE return NULL;
     ENDIF
   }

   //-------------------------------------------------------
   /*! \brief Return special folder path
    *
    * \a name can be one the followings:
    * - LibDir   : daim library path(s)
    * - RootDir  : daim library install path
    * - UserDir  : daim user directory
    * - CacheDir : daim user cache  directory
    */
   //-------------------------------------------------------
   const char_t* GetSpecialFolder( const char_t* name ) {
     IF_CCI_IMETHOD_RP(Environment,GetSpecialFolder, _in name _out(location) ) 
      THEN return CCI_RETVAL(location);
      ELSE return NULL;
     ENDIF
   }

   //-------------------------------------------------------
   /*! \brief Create a new folder in the user folder */
   //-------------------------------------------------------
   bool CreateFolder( const char_t* name ) {
     CCI_RETURN_IMETHOD_PP_OK(Environment,CreateFolder,_in name)
   } 
   
   //-------------------------------------------------------
   /*! \brief Register path for cci modules  */
   //-------------------------------------------------------
   bool RegisterModules( const char_t* location, dm_uint flags = 0 ) {
     CCI_RETURN_IMETHOD_PP_OK(Environment,RegisterModules,_in location _in flags)
   } 
};

#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif // CCI_Environment_H
