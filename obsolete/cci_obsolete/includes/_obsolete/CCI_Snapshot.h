#ifndef CCI_Snapshot_h
#define CCI_Snapshot_h

/* ::: BEGIN LICENSE BLOCK:::
 * Copyright (c) 2005 David Marteau
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

//--------------------------------------------------------------------
// Project   : DAIM Image processing library
// File      : CCI_Snapshot.h
// Author    : David Marteau
// Date      : 29/06/2005
//--------------------------------------------------------------------


#define CCIC_Snapshot _TXT("cachemngr.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Snapshot, Notify )

  CCI_MODULE_UUID( Snapshot, 7be3753c-d059-4ee5-ac06-b113d25403e4)

  CCI_MODULE_VERSION( Snapshot, 1 ) 

  /* Methods */
  CCI_DECL_METHOD_PP( Snapshot,1, UpdateCache  , _in CONST char_t* file )
  CCI_DECL_METHOD_PP( Snapshot,2, LoadFromCache, _in CONST char_t* file )
  CCI_DECL_METHOD_PP( Snapshot,4, ClearCache   , _in CONST char_t* file )
  CCI_DECL_METHOD_NP( Snapshot,5, Synchronize )  

  CCI_DECL_METHOD_PP( Snapshot,6, GetProperties,     
     _in dm_param key
     _in dm_cci(PropertyList) propertylist 
  )

  /* Attributs */
  CCI_DECL_ATTR(Snapshot, 1, CacheName   , CONST char_t*    , [..G.] )
  CCI_DECL_ATTR(Snapshot, 2, ImagePath   , CONST char_t*    , [..G.] )
  CCI_DECL_ATTR(Snapshot, 3, ImageWidth  , dm_uint          , [..G.] )
  CCI_DECL_ATTR(Snapshot, 4, ImageHeight , dm_uint          , [..G.] )
  CCI_DECL_ATTR(Snapshot, 5, ImageFormat , dm_uint          , [..G.] )
  CCI_DECL_ATTR(Snapshot, 6, MimeType    , CONST char_t*    , [.SG.] )
  CCI_DECL_ATTR(Snapshot, 7, Name        , CONST char_t*    , [..G.] )
  CCI_DECL_ATTR(Snapshot, 8, Status      , dm_uint          , [..G.] )

CCI_END_MODULE_DECL( Snapshot )

#endif //__doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief interface to manupulate individual image cache
 *  
 *  You should never instanciate directly a dmISnapshot object :
 *  use \see dmICacheManager.
 */
//------------------------------------------------------------------------------------
class dmISnapshot : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(Snapshot)

   //-------------------------------------------------------
   /*! \brief Update the cache with the original image    
    *
    * The \a file parameter may be set to null or empty string if
    * the object has already been initialized with a valid image url.
    */
   //-------------------------------------------------------
   bool UpdateCache( const char_t* file ) {
      CCI_RETURN_IMETHOD_PP_OK(Snapshot,UpdateCache, _in file );
   }

   //-------------------------------------------------------
   /*! \brief Create the cache file if it does not exists  
    *
    * The cache will be updated only if the cached image does not exists.
    * For synchronizing between original image and cached image \see Synchronize
    *
    * The \a file parameter may be set to null or empty string if
    * the object has already been initialized with a valid image url.
    */
   //-------------------------------------------------------
   bool LoadFromCache( const char_t* file ) {
      CCI_RETURN_IMETHOD_PP_OK(Snapshot,UpdateCache, _in file );
   }

   //-------------------------------------------------------
   /*! \brief Delete physically the cached image          */
   //-------------------------------------------------------
   bool ClearCache( const char_t* file = NULL ) { 
     CCI_RETURN_IMETHOD_PP_OK(Snapshot,ClearCache, _in file );
   }

   //-------------------------------------------------------
   /*! \brief Return properties of the original image     */
   //-------------------------------------------------------
   bool GetProperties( dm_param key, dm_cci(PropertyList) propertylist ) { 
     CCI_RETURN_IMETHOD_PP_OK(Snapshot,GetProperties, _in key _in propertylist); 
   }

   //--------------------------------------------------------------
   /*! \brief Synchronize between original image and cached image
    *
    *  Note that the cache must exists. A cached image can be created
    *  by calling either the LoadFromCache() or the UpdateCache() method.
    */
   //--------------------------------------------------------------
   bool Synchronize() { CCI_RETURN_IMETHOD_NP_OK(Snapshot,Synchronize);  }

   //--------------------------------------------------------------
   /*! \brief Return the cache physical path */
   //--------------------------------------------------------------  
   const char_t* GetCacheName() { return CCI_IGET(Snapshot,CacheName,(char_t*)NULL); }

   //--------------------------------------------------------------
   /*! \brief Return the url of the original image */
   //--------------------------------------------------------------  
   const char_t* GetImagePath() { return CCI_IGET(Snapshot,ImagePath,(char_t*)NULL); }

   //--------------------------------------------------------------
   /*! \brief Return the width (in pixels) of the original image */
   //--------------------------------------------------------------  
   dm_uint GetImageWidth() { return CCI_IGET(Snapshot,ImageWidth,(dm_uint)0); }

   //--------------------------------------------------------------
   /*! \brief Return the height (in pixels) of the original image */
   //--------------------------------------------------------------  
   dm_uint GetImageHeight() { return CCI_IGET(Snapshot,ImageHeight,(dm_uint)0); }

   //--------------------------------------------------------------
   /*! \brief Return the format of the original image */
   //--------------------------------------------------------------  
   dm_uint GetImageFormat() { return CCI_IGET(Snapshot,ImageFormat,(dm_uint)0); }

   char_t* GetMimeType()  { return CCI_IGET(Snapshot,MimeType,(char_t*)NULL); }

   bool SetMimeType(const char_t* aMimeType) {
     return CCI_ISET(Snapshot,MimeType,aMimeType); 
   }

   char_t* GetName()  { return CCI_IGET(Snapshot,Name,(char_t*)NULL); }
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Snapshot_h */
