#ifndef CCI_FileOutputStream_h
#define CCI_FileOutputStream_h

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
// File         : CCI_FileOutputStream.h
// Date         : 4/2006
// Author       : David Marteau
//--------------------------------------------------------
#include "daim_modules/CCI_Prolog.h"
#define CCIC_FileOutputStream _TXT("fileoutputstream.cci")

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( FileOutputStream, OutputStream )
  
  CCI_MODULE_UUID( FileOutputStream, b7ae67a3-6e32-4fcd-a0f0-2a4e29dfab8a)

  CCI_MODULE_VERSION( FileOutputStream, 1 ) 

  /* Methods */
 
  CCI_DECL_METHOD_PP(FileOutputStream,1, Init, 
    _in  CONST char_t* spec
    _in  dm_uint32     ioFlags
    _in  dm_uint32     perm
    _in  dm_uint32     behaviors
  )

  CCI_DECL_METHOD_PP(FileOutputStream,2, Seek, 
    _in  dm_uint32     whence
    _in  dm_uint32     offset
  )

  /* Attributs */
  CCI_DECL_ATTR( FileOutputStream, 1, Spec, CONST char_t*, [..G.] )

CCI_END_MODULE_DECL( FileOutputStream )

#endif // __doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Daim FileOutputStream base interface
 */
//------------------------------------------------------------------------------------
class dmIFileOutputStream : public dmIOutputStream
{ 
  public:
    dmDECLARE_GENERIC_IINTERFACE(FileOutputStream)

   bool Init( const char_t* spec, dm_uint32 ioFlags, dm_uint32 perm, dm_uint32 behaviors ) {
     CCI_RETURN_IMETHOD_PP_OK(FileOutputStream,Init, _in spec _in ioFlags _in perm _in behaviors );
   }

   bool Seek( const char_t* spec, dm_uint32 whence, dm_uint32 offset ) {
     CCI_RETURN_IMETHOD_PP_OK(FileOutputStream,Seek, _in whence _in offset );
   }

   const char_t* Spec() { return CCI_IGET(FileOutputStream,Spec,(char_t*)NULL); }
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

//-------------------------------------------------------------
#endif // CCI_FileOutputStream_h
