#ifndef CCI_Memory_h
#define CCI_Memory_h

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
// File         : CCI_Memory.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#define CCIC_Memory  _TXT("memory.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Memory, Notify )

  CCI_MODULE_UUID( Memory, 229cef82-1528-4645-b1f4-56fc779dc928)

  CCI_MODULE_VERSION( Memory, 1 ) 

  /* Operators */

  CCI_DECL_METHOD_PP(Memory,1, Allocate, 
     _in size_t  size
     _out dm_ptr block 
  ) 

  CCI_DECL_METHOD_PP(Memory,2, Free, 
     _in dm_ptr block 
  )

  CCI_DECL_METHOD_PP(Memory,3, Realloc,
     _in dm_ptr  block  
     _in dm_size size 
  )

  CCI_DECL_METHOD_PP(Memory,4, Copy,
     _in dm_ptr  dest 
     _in dm_ptr  src 
     _in dm_size size 
  )

  CCI_DECL_METHOD_PP(Memory,5, Move,
     _in dm_ptr  dest 
     _in dm_ptr  src 
     _in dm_size size 
  )

  CCI_DECL_METHOD_PP(Memory,6, Set ,
     _in dm_ptr  block  
     _in dm_int  value 
     _in dm_size size 
  )

  CCI_DECL_METHOD_PP(Memory,7, Stat, 
     _in  dm_size blks 
     _in  dm_size blk_size
     _in  dm_int  flags 
     _out dm_int  result
  ) 

  /* Special values */ 

  #define CCIV_Memory_Ok       0
  #define CCIV_Memory_Low      1

  #define CCIV_Memory_Page     0
  #define CCIV_Memory_Physical 1

CCI_END_MODULE_DECL( Daim )

#endif // __doxygen
//----------------------------------------------------------------

#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 */
//------------------------------------------------------------------------------------
class dmIMemory : public dmINotify
{  
  public:  
   dmDECLARE_GENERIC_IINTERFACE(Memory)

   dm_ptr Allocate ( dm_size size  ) {
     IF_CCI_IMETHOD_RP(Memory,Allocate, _in size ) 
      THEN return CCI_RETVAL(block);
      ELSE return NULL;
     ENDIF
   }

   bool  Free ( dm_ptr block ) {
     CCI_RETURN_IMETHOD_PP_OK(Memory,Free,_in block )
   }

   dm_ptr Realloc( dm_ptr block, size_t size ) {
     IF_CCI_IMETHOD_RP(Memory,Realloc, _in block _in size ) 
       THEN return CCI_RETVAL(block);
       ELSE return NULL;
     ENDIF
   }

   dm_ptr Calloc( size_t nmemb, size_t size ) {
     size_t __sz = nmemb*size;
     dm_ptr block = Allocate(__sz);
     if(block) {
        Set(block,0,__sz);
        return block;
     }
     return NULL;
   }

   dm_ptr Copy( dm_ptr dest, const dm_ptr src, size_t size ) {
     CCI_IINVOKE_PP(Memory,Copy,_in dest _in src _in size );
     return dest;
   }

   dm_ptr Move( dm_ptr dest, const dm_ptr src, size_t size ) {
     CCI_IINVOKE_PP(Memory,Move,_in dest _in src _in size );
     return dest;
   }

   dm_ptr Set ( dm_ptr block, dm_int value , size_t size ) {
     CCI_IINVOKE_PP(Memory,Set,_in block _in value _in size);
     return block;
   }

   int Stat( size_t blks, size_t blk_size, int flags ) {
     IF_CCI_IMETHOD_RP(Memory,Stat, _in blks _in blk_size _in flags ) 
      THEN return CCI_RETVAL(result);
      ELSE return 0;
     ENDIF
   }
};
//------------------------------------------------------------------------------------
#endif // __cplusplus

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Daim.h */
