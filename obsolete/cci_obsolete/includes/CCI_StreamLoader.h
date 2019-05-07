#ifndef CCI_StreamLoader_h
#define CCI_StreamLoader_h

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
// File         : CCI_StreamLoader.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#include "daim_modules/CCI_Prolog.h"
#define CCIC_StreamLoader _TXT("streamloader.cci")

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( StreamLoader, Notify )
  
  CCI_MODULE_UUID( StreamLoader, 10bfe82c-b511-4d2e-aded-c1469893ae94)

  CCI_MODULE_VERSION( StreamLoader, 1 ) 

  /* Methods */

  CCI_DECL_METHOD_PP(StreamLoader,1,Read,
     _in dm_cci(InputStream) inputstream
     _in dm_cci(ImageList)   imagelist
     _in dm_uint             index
     _in dm_cci(Loader)      loader
  )

  CCI_DECL_METHOD_PP(StreamLoader,2,Write,
     _in dm_cci(OutputStream) outputstream
     _in dm_cci(ImageList)    imagelist
     _in dm_uint              index
     _in dm_cci(Loader)       loader
     _in dm_uint              bi_flags
     _in dm_uint              bi_index
     _in dm_uint              bi_count
  )

  CCI_DECL_METHOD_PP(StreamLoader,3,SetText,
     _in CONST char_t* attribute 
     _in CONST char_t* value
  )

  CCI_DECL_METHOD_PP(StreamLoader,4,GetText,
     _in CONST char_t* attribute 
     _in CONST char_t* value
  )

  /* Attributs */
  CCI_DECL_ATTR(StreamLoader, 1, DocError, dm_int , [..G.] )

CCI_END_MODULE_DECL( StreamLoader )

#endif // __doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Daim stream loader interface
 *
 *  Handle image streamed I/O 
 */
//------------------------------------------------------------------------------------
class dmIStreamLoader : public dmINotify
{ 
  public:
    dmDECLARE_GENERIC_IINTERFACE(StreamLoader)

    //-------------------------------------------------------
    /*! \brief Read data from a stream
     * 
     *  Read data from \a inputstream and store the resulting image
     *  in \a imagelist at index \a index.
     *  If \a loader is null, then the loader determined by the 
     *  dmIInputStream::GetContentType() will be used
     */
    //-------------------------------------------------------   
    bool Read( dm_cci(InputStream) inputstream,
               dm_cci(ImageList)   imagelist,
               dm_uint             index,
               dm_cci(Loader)      loader = NULL )
    {
      CCI_RETURN_IMETHOD_PP_OK(StreamLoader,Read,
         _in inputstream
         _in imagelist
         _in index
         _in loader
      )
    } 

    //-------------------------------------------------------
    /*! Write image data to a stream
     */
    //-------------------------------------------------------   
    bool Write( dm_cci(OutputStream) outputstream,
                dm_cci(ImageList)    imagelist,
                dm_uint              index,
                dm_cci(Loader)       loader,
                dm_uint              bi_flags = 0,
                dm_uint              bi_index = 0,
                dm_uint              bi_count = 1)
    {
      CCI_RETURN_IMETHOD_PP_OK(StreamLoader,Write,
         _in outputstream
         _in imagelist
         _in index
         _in loader
         _in bi_flags
         _in bi_index
         _in bi_count
      )
    }

   //-------------------------------------------------------
   /*! Set a text property */
   //-------------------------------------------------------
   bool SetText( const char_t* attribute , const char_t* value ) { 
      CCI_RETURN_IMETHOD_PP_OK(StreamLoader,SetText,_in attribute _in value);
   }
   
   //-------------------------------------------------------
   /*! Get a text property */
   //-------------------------------------------------------
   const char_t* GetText( const char_t* attribute )
   {
     IF_CCI_IMETHOD_RP(StreamLoader,GetText,_in attribute _out(value) NULL)
     THEN  return CCI_RETVAL(value); ENDIF
     return NULL;
   } 
   
   dm_int GetDocError() { return CCI_IGET(StreamLoader,DocError,(dm_int)0); }
 
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

//-------------------------------------------------------------
#endif // CCI_StreamLoader_h
