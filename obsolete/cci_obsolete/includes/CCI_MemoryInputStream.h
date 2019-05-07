#ifndef CCI_MemoryInputStream_h
#define CCI_MemoryInputStream_h

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
// File         : CCI_MemoryInputStream.h
// Date         : 4/2006
// Author       : David Marteau
//--------------------------------------------------------
#include "daim_modules/CCI_Prolog.h"
#define CCIC_MemoryInputStream _TXT("memoryinputstream.cci")

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( MemoryInputStream, InputStream )
  
  CCI_MODULE_UUID( MemoryInputStream, f307efc7-c617-462e-b2ec-b76fdd6604c8)

  CCI_MODULE_VERSION( MemoryInputStream, 1 ) 

  /* Methods */

  CCI_DECL_METHOD_PP(MemoryInputStream,1, Init, 
    _in  CONST dm_byte*   buffer
    _in  dm_int           length
    _in  dm_uint32        behaviors
  )

  /* Attributs */
  CCI_DECL_ATTR( MemoryInputStream, 1, Buffer, dm_byte*, [..G.] )

CCI_END_MODULE_DECL( MemoryInputStream )

#endif // __doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Daim MemoryInputStream base interface
 */
//------------------------------------------------------------------------------------
class dmIMemoryInputStream : public dmIInputStream
{ 
  public:
    dmDECLARE_GENERIC_IINTERFACE(MemoryInputStream)

   bool Init( const dm_byte* buffer, dm_uint length, dm_uint32 behaviors ) {
     CCI_RETURN_IMETHOD_PP_OK(MemoryInputStream,Init, _in buffer _in length _in behaviors );
   }

   dm_byte* Buffer() { return CCI_IGET(MemoryInputStream,Buffer,(dm_byte*)NULL); }
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

//-------------------------------------------------------------
#endif // CCI_MemoryInputStream_h
