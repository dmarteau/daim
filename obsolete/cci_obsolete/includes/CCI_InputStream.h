#ifndef CCI_InputStream_h
#define CCI_InputStream_h

/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2006 David Marteau
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
// File         : CCI_InputStream.h
// Date         : 4/2006
// Author       : David Marteau
//--------------------------------------------------------
#include "daim_modules/CCI_Prolog.h"
#define CCIC_InputStream _TXT("inputstream.cci")

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( InputStream, Notify )
  
  CCI_MODULE_UUID( InputStream, b34246e2-f9be-4ef1-9b1c-ce3da046b9ed)

  CCI_MODULE_VERSION( InputStream, 1 ) 

  /* Methods */
 
 CCI_DECL_METHOD_PP(InputStream,1, Read, 
   _in  dm_ptr  buffer
   _in  dm_uint count
   _out dm_uint byteread
 )

 CCI_DECL_METHOD_NP(InputStream,2, Close)
 
 /* Attributs */
 CCI_DECL_ATTR(InputStream, 1, Available     , dm_uint ,[..G.] )
 CCI_DECL_ATTR(InputStream, 2, IsNonBlocking , dm_bool ,[..G.] )
 CCI_DECL_ATTR(InputStream, 3, Reason        , dm_uint ,[.SGN] )
 CCI_DECL_ATTR(InputStream, 4, ContentType   , CONST char_t* ,[.SGN] )
 CCI_DECL_ATTR(InputStream, 5, ContentLength , dm_uint       ,[..G.] )

CCI_END_MODULE_DECL( InputStream )

#endif // __doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Daim InputStream base interface
 *
 *  Base interface for all daim InputStream implementations
 */
//------------------------------------------------------------------------------------
class dmIInputStream : public dmINotify
{ 
  public:
    dmDECLARE_GENERIC_IINTERFACE(InputStream)

   //-------------------------------------------------------
   /*! Read /a count bytes of data and store them in /a buffer
    *  Return the number of bytes read
    */
   //-------------------------------------------------------
   dm_uint Read( dm_ptr buffer, dm_uint count ) 
   {
     IF_CCI_IMETHOD_RP(InputStream,Read,_in buffer _in count _out(byteread) 0)
     THEN  return CCI_RETVAL(byteread); ENDIF
     return 0;
   } 

   //-------------------------------------------------------
   /*! Close InputStream and free resources */
   //-------------------------------------------------------
   bool Close() { CCI_RETURN_IMETHOD_NP_OK(InputStream,Close) }

   //-------------------------------------------------------
   /*! Return the number of bytes availables */
   //-------------------------------------------------------
   dm_uint Available() { return CCI_IGET(InputStream,Available,(dm_uint)0);} 

   //-------------------------------------------------------
   /*! Return the last error status */
   //-------------------------------------------------------
   dm_param Reason() { return CCI_IGET(InputStream,Reason,(dm_param)0); } 

   //-------------------------------------------------------
   /*! Return dm_true if the input stream is blocking for reading */
   //-------------------------------------------------------
   dm_bool  IsNonBlocking() { return CCI_IGET(InputStream,IsNonBlocking,(dm_bool)dm_false);} 

   //-------------------------------------------------------
   /*! Return the content type (i.e the mime type) of the content */
   //-------------------------------------------------------
   const char_t* GetContentType() { return CCI_IGET(InputStream,ContentType,(char_t*)NULL); } 

   bool SetContentType( const char_t* contenttype ) {
      return CCI_ISET(InputStream,ContentType,contenttype); 
   } 

   dm_uint GetContentLength() { return CCI_IGET(InputStream,ContentLength,dm_noindex); } 

};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

//-------------------------------------------------------------
#endif // CCI_InputStream_h
