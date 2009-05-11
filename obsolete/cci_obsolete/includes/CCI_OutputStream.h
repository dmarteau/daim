#ifndef CCI_OutputStream_h
#define CCI_OutputStream_h

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
// File         : CCI_OutputStream.h
// Date         : 4/2006
// Author       : David Marteau
//--------------------------------------------------------
#include "daim_modules/CCI_Prolog.h"
#define CCIC_OutputStream _TXT("outputstream.cci")

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( OutputStream, Notify )
  
  CCI_MODULE_UUID( OutputStream, 65c78f50-b1a7-4e32-80aa-b687fc448add)

  CCI_MODULE_VERSION( OutputStream, 1 ) 

  /* Methods */
 
 CCI_DECL_METHOD_PP(OutputStream,1, Write, 
   _in  dm_ptr  buffer
   _in  dm_uint count
   _out dm_uint bytewritten
 )

 CCI_DECL_METHOD_NP(OutputStream,2, Close)
 CCI_DECL_METHOD_NP(OutputStream,3, Flush)

 CCI_DECL_METHOD_PP(OutputStream,4, WriteFrom,
   _in  dm_cci(InputStream) input
   _in  dm_uint             count   
   _out dm_uint             bytewritten
 )
 
 /* Attributs */
 CCI_DECL_ATTR(OutputStream, 1, IsNonBlocking , dm_bool ,[..G.] )
 CCI_DECL_ATTR(OutputStream, 2, Reason        , dm_uint ,[.SGN] )

CCI_END_MODULE_DECL( OutputStream )

#endif // __doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Daim OutputStream base interface
 *
 *  Base interface for all daim OutputStream implementations
 */
//------------------------------------------------------------------------------------
class dmIOutputStream : public dmINotify
{ 
  public:
    dmDECLARE_GENERIC_IINTERFACE(OutputStream)

   //-------------------------------------------------------
   /*! Write /a count bytes of data from /a buffer
    *  Return the number of bytes written
    */
   //-------------------------------------------------------
   dm_uint Write( dm_ptr buffer, dm_uint count ) 
   {
     IF_CCI_IMETHOD_RP(OutputStream,Write,_in buffer _in count _out(bytewritten) 0)
     THEN  return CCI_RETVAL(bytewritten); ENDIF
     return 0;
   } 

   //-------------------------------------------------------
   /*! Flush OutputStream */
   //-------------------------------------------------------
   bool Flush() { CCI_RETURN_IMETHOD_NP_OK(OutputStream,Flush) }

   //-------------------------------------------------------
   /*! Write /a count bytes from a dmIInputStream
    *  The default implementation allocate a buffer for passing data
    */
   //-------------------------------------------------------
   dm_uint WriteFrom( dm_cci(InputStream) input, dm_uint count ) 
   {
     IF_CCI_IMETHOD_RP(OutputStream,WriteFrom,_in input _in count _out(bytewritten) 0)
     THEN  return CCI_RETVAL(bytewritten); ENDIF
     return 0;
   } 

   //-------------------------------------------------------
   /*! Close OutputStream and free resources
    */
   //-------------------------------------------------------
   bool Close() { CCI_RETURN_IMETHOD_NP_OK(OutputStream,Close) }

   //-------------------------------------------------------
   /*! Return the last error status */
   //-------------------------------------------------------
   dm_param Reason() { return CCI_IGET(OutputStream,Reason,(dm_param)0); } 
   dm_bool  IsNonBlocking() { return CCI_IGET(OutputStream,IsNonBlocking,(dm_bool)dm_false);} 
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

//-------------------------------------------------------------
#endif // CCI_OutputStream_h
