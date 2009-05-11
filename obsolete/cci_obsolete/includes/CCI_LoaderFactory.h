#ifndef CCI_LoaderFactory_H
#define CCI_LoaderFactory_H

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
// File         : CCI_LoaderFactory.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_LoaderFactory  _TXT("ldrfactory.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( LoaderFactory, Notify )
  
  CCI_MODULE_UUID( LoaderFactory, ac77b36d-d668-4216-a083-783992032a8b)

  CCI_MODULE_VERSION( LoaderFactory, 1 ) 

  /* Methods */

  CCI_DECL_METHOD_PP(LoaderFactory,1,  Register, 
     _in CONST char_t* mime
     _in CONST char_t* loader
     _in dm_uint       flags
  )

  CCI_DECL_METHOD_PP(LoaderFactory,2,  AddMimeExt,
     _in CONST char_t* mime     
     _in CONST char_t* file_ext 
  )

  CCI_DECL_METHOD_PP(LoaderFactory,3,  GetMimeType, 
     _in  CONST char_t* filename
     _out CONST char_t* mime 
  )

  CCI_DECL_METHOD_NP(LoaderFactory,4,  LoadMimes )

  CCI_DECL_METHOD_PP(LoaderFactory,5,  GetMimeInfos, 
     _in CONST char_t*        mime
     _in dm_param             key
     _in dm_cci(PropertyList) propertylist
     _in dm_uint              flags
  )

  CCI_DECL_METHOD_PP(LoaderFactory,6,  GetMimeList, 
     _in dm_cci(StringList) stringlist
     _in dm_uint            flags
  )

  CCI_DECL_METHOD_PP(LoaderFactory,7,  GetMimeExt, 
     _in  CONST char_t* mime
     _out CONST char_t* ext 
  )

  CCI_DECL_METHOD_PP(LoaderFactory,8,  GetLoader, 
     _in  CONST char_t*  mime
     _in  dm_uint        flags
     _out dm_cci(Loader) loader
  )
  

  
CCI_END_MODULE_DECL( LoaderFactory )

#endif // __doxygen


#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Implement methods for handling images based on mimes types
 *
 *  The LoaderFactory interface expose methods for referencing all registered
 *  mimes image types and associated loaders that the Daim library can handle. 
 *
 *  The LoaderFactory should be invoked as resident service and initialized
 *  at startup with the LoadMimes() method
 */
//------------------------------------------------------------------------------------
class dmILoaderFactory : public dmINotify
{
  public:
    dmDECLARE_GENERIC_IINTERFACE(LoaderFactory)

    //-------------------------------------------------------
    /*! Register a module associated to a mime type 
     * \param[in] mime the mime type associated to the loader
     * \param[in] module the module class id
     * \param[in] flags specifies the type of the loader (readable or writable).
     */
    //-------------------------------------------------------
    bool Register( CONST char_t* mime, CONST char_t* module, dm_uint flags ) {
      CCI_RETURN_IMETHOD_PP_OK(LoaderFactory,Register, _in mime _in module _in flags )
    }

    //-------------------------------------------------------
    /*! Associate a mime tye to a file extension 
     * \param[in] mime the mime type associated to the extension
     * \param[in] file_ext an list of '|' separated file extensions 
     * (without '.').
     */
    //-------------------------------------------------------
    bool AddMimeExt( CONST char_t* mime, CONST char_t* file_ext ) {
      CCI_RETURN_IMETHOD_PP_OK(LoaderFactory,AddMimeExt,_in mime _in file_ext)
    }

    //-------------------------------------------------------
    /*! Return the mime type associated to the file \c filename
     */
    //-------------------------------------------------------
    CONST char_t* GetMimeType( const char_t* filename ) {
      IF_CCI_IMETHOD_RP(LoaderFactory,GetMimeType, _in filename _out(mime) NULL )
      THEN  return CCI_RETVAL(mime);
      ELSE  return NULL;
      ENDIF
    }

    //-------------------------------------------------------
    /*! Load mimes type and register associated loaders.
     *  This method should be called at startup.
     */
    //-------------------------------------------------------
    bool LoadMimes() { CCI_RETURN_IMETHOD_NP_OK(LoaderFactory,LoadMimes) }

    //-------------------------------------------------------
    /*! Return informations about a particular mime type 
     *  in a dmIPropertyList object 
     * \param[in] mime the mime type associated to the extension, if NULL then
     *            informations about all mimes type are returned as a sequence in
                  the \a proplist obect.
     * \param[in] key      the key to the entry in the property list
     * \param[in] proplist the property list object
     * \param[in] flags    specifies the type (readable or writable) of
     *                     the list that you want to be returned.
     */
    //-------------------------------------------------------
    bool GetMimeInfos( const char_t* mime, dm_uint key, dm_cci(PropertyList) proplist, dm_uint flags ) {
      CCI_RETURN_IMETHOD_PP_OK(LoaderFactory,GetMimeInfos,
             _in mime 
             _in key 
             _in proplist
             _in flags
      )
    }

    //-------------------------------------------------------
    /*! Return all registered mimes type in a dmIStringList object
     *  The \a flags parameter specifies the type (readable or writable) of
     *  the list that you want to be returned.
     *  \sa dmILoader
     */ 
    //-------------------------------------------------------
    bool GetMimeList( dm_cci(StringList) stringlist, dm_uint flags ) {
      CCI_RETURN_IMETHOD_PP_OK(LoaderFactory,GetMimeList,_in stringlist _in flags )
    }

    //-------------------------------------------------------
    /*! Return the first available extension for given mime type */
    //-------------------------------------------------------
    const char* GetMimeExt( const char_t* mime ) {
      IF_CCI_IMETHOD_RP(LoaderFactory,GetMimeExt, _in mime _out(ext) NULL )
      THEN  return CCI_RETVAL(ext);
      ELSE  return NULL;
      ENDIF
    }
    
    //-------------------------------------------------------
    /*! Return a loader for the given the mime type */
    //-------------------------------------------------------
    dm_cci(Loader) GetLoader( const char_t* mime, dm_uint flags ) {
      IF_CCI_IMETHOD_RP(LoaderFactory,GetLoader, _in mime _in flags _out(loader) NULL )
      THEN  return CCI_RETVAL(loader);
      ELSE  return NULL;
      ENDIF
    }
    
    //-------------------------------------------------------
    /*! Return true if there is a default loader for the given mime type */
    //-------------------------------------------------------
    bool LoaderExists( const char_t* mime, dm_uint flags ) {
      CCI_RETURN_IMETHOD_PP_OK(LoaderFactory,GetLoader, _in mime _in flags _in (cci_Object*)dm_noindex )
    }
};
//------------------------------------------

#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif // CCI_LoaderFactory_H
