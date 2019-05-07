
/* :::BEGIN LICENSE BLOCK:::
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
 *  :::END LICENSE BLOCK::: */

#include "daim_utilities.h"
#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Memory.h"

#define CCIC_NAME  CCIC_Memory
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Memory_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2005"))
//-------------------------------------------------------------------
#define CCI_INTERNAL_MODULE

#include "daim_modules/CCI_Base.h"

//-----------------------------------------
CCI_DECL_DATA( Memory )
{
  dm_int _Dummy;
};

typedef CCI_INSTANCE_PTR(Memory) _Instance_Ptr;

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
static inline CCI_CONSTRUCTOR( Memory )
{
  CCI_CONSTRUCT_SUPER()
  CCI_UNUSUED_INSTANCE( Memory )

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// ME_DISPOSE
//---------------------------------------------------------------------
static inline  CCI_DESTRUCTOR( Memory )
{
  CCI_UNUSUED_INSTANCE( Memory )
  
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( Memory )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( Memory )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method    : Allocate
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Memory, Allocate )
//     _in size_t  size
//     _out dm_ptr block 
{
  CCI_UNUSUED_INSTANCE( Memory )
 
  CCI_RETVAL_P(block) = dmMemory::Malloc(CCI_P(size));

  if(CCI_RETVAL_P(block) == NULL)
     CCI_RETURN_FAIL()
    
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method    : Free
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Memory, Free )
//     _in dm_ptr block 
{
  CCI_UNUSUED_INSTANCE( Memory )
  
  dmMemory::Free(CCI_P(block));
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method    : Realloc
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Memory, Realloc )
//     _out dm_ptr  block  
//     _in  dm_size size 
{
  CCI_UNUSUED_INSTANCE( Memory )
  
  CCI_RETVAL_P(block) = dmMemory::Realloc(CCI_P(block),CCI_P(size));
  if(CCI_P(size) > 0 && CCI_RETVAL_P(block)==NULL)
     CCI_RETURN_FAIL()
     
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method    : Copy
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Memory, Copy )
//     _in dm_ptr  dest 
//     _in dm_ptr  src 
//     _in dm_size size 
{
  CCI_UNUSUED_INSTANCE( Memory )
  
  if(dmMemory::Memcpy(CCI_P(dest),CCI_P(src),CCI_P(size))!=NULL)
     CCI_RETURN_OK()

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method    : Move
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Memory, Move )
//     _in dm_ptr  dest 
//     _in dm_ptr  src 
//     _in dm_size size 
{
  CCI_UNUSUED_INSTANCE( Memory )
  
  if(dmMemory::Memmove(CCI_P(dest),CCI_P(src),CCI_P(size))!=NULL)
     CCI_RETURN_OK()

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method    : Set
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Memory, Set )
//     _in dm_ptr  block  
//     _in dm_int  value 
//     _in dm_size size 
{
  CCI_UNUSUED_INSTANCE( Memory )
  
  if(dmMemory::Memset(CCI_P(block),CCI_P(value),CCI_P(size))!=NULL)
     CCI_RETURN_OK()

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method    : Stat
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Memory, Stat )
//     _in  dm_size blks 
//     _in  dm_size blk_size
//     _in  dm_int  flags 
//     _out dm_int  result
{
  CCI_UNUSUED_INSTANCE( Memory )

  CCI_RETVAL_P(result) = dmMemory::Memstat(CCI_P(blks),CCI_P(blk_size),CCI_P(flags));
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Memory )
  CCI_REGISTER_METHOD_PP(Memory, Allocate )
  CCI_REGISTER_METHOD_PP(Memory, Free     ) 
  CCI_REGISTER_METHOD_PP(Memory, Realloc  ) 
  CCI_REGISTER_METHOD_PP(Memory, Copy     )
  CCI_REGISTER_METHOD_PP(Memory, Move     )
  CCI_REGISTER_METHOD_PP(Memory, Set      ) 
  CCI_REGISTER_METHOD_PP(Memory, Stat     ) 
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Memory )
{
  CCI_INVOKE_FACTORY(Memory);
}
//---------------------------------------------------------------------
