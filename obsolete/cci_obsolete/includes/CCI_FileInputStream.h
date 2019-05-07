#ifndef CCI_FileInputStream_h
#define CCI_FileInputStream_h

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
// File         : CCI_FileInputStream.h
// Date         : 4/2006
// Author       : David Marteau
//--------------------------------------------------------
#include "daim_modules/CCI_Prolog.h"
#define CCIC_FileInputStream _TXT("fileinputstream.cci")

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( FileInputStream, InputStream )
  
  CCI_MODULE_UUID( FileInputStream, ae11cb50-77ba-4be3-9031-0835049658d4)

  CCI_MODULE_VERSION( FileInputStream, 1 ) 

  /* Methods */

  CCI_DECL_METHOD_PP(FileInputStream,1, Init, 
    _in  CONST char_t* spec
    _in  dm_uint32     ioFlags
    _in  dm_uint32     perm
    _in  dm_uint32     behaviors
  )

  CCI_DECL_METHOD_PP(FileInputStream,2, Seek, 
    _in  dm_uint     whence
    _in  dm_uint     offset
  )

  /* Attributs */
  CCI_DECL_ATTR( FileInputStream, 1, Spec, CONST char_t*, [..G.] )

CCI_END_MODULE_DECL( FileInputStream )

#endif // __doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Daim FileInputStream base interface
 */
//------------------------------------------------------------------------------------
class dmIFileInputStream : public dmIInputStream
{ 
  public:
    dmDECLARE_GENERIC_IINTERFACE(FileInputStream)

   bool Init( const char_t* spec, dm_int32 ioFlags, dm_int32 perm, dm_uint32 behaviors ) {
     CCI_RETURN_IMETHOD_PP_OK(FileInputStream,Init, _in spec _in ioFlags _in perm _in behaviors );
   }

   bool Seek( const char_t* spec, dm_uint32 whence, dm_uint32 offset ) {
     CCI_RETURN_IMETHOD_PP_OK(FileInputStream,Seek, _in whence _in offset );
   }

   const char_t* Spec() { return CCI_IGET(FileInputStream,Spec,(char_t*)NULL); }
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

//-------------------------------------------------------------
#endif // CCI_FileInputStream_h
