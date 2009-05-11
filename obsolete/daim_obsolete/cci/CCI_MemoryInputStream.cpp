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

#define dmUseCCI
#define dmUseUtilitiesExtra
#include "daim_utilities.h"
#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_InputStream.h"
#include "daim_modules/CCI_MemoryInputStream.h"

#define CCIC_NAME  CCIC_MemoryInputStream
#define CCIC_SUPER CCIC_InputStream
#define CCIC_SUPER_VERSION CCI_InputStream_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_MemoryInputStream_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2006"))
//-------------------------------------------------------------------
#define CCI_INTERNAL_MODULE
#include "daim_modules/CCI_Base.h"
#include "daim/daim_io.h"

#define ENSURE_POINTER( ptr ) \
   if(ptr == NULL) \
      CCI_RETURN_INVALID_POINTER()

#define SET_REASON( reason ) \
   CCI_Set(_This_ob,CCIA_DECL_(InputStream,Reason),reason)

//-----------------------------------------
CCI_DECL_DATA( MemoryInputStream )
{
  const dm_byte*  buffer;
  
  dm_uint   length;
  dm_uint   offset;
  dm_uint   left;
  dm_uint32 behaviors;
};
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static inline CCI_CONSTRUCTOR( MemoryInputStream )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( MemoryInputStream )
   
  _This->buffer    = NULL;
  _This->length    = 0;
  _This->offset    = 0;
  _This->left      = 0;
  _This->behaviors = 0;

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static inline CCI_DESTRUCTOR( MemoryInputStream )
{
  CCI_UNUSUED_INSTANCE( MemoryInputStream )
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( MemoryInputStream )
  CCI_ATTR_GET( InputStream, IsNonBlocking, CCIA_RETVAL = dm_false )
  CCI_ATTR_GET( InputStream, Available    , CCIA_RETVAL = _This->length )
  CCI_ATTR_GET( InputStream, ContentLength, CCIA_RETVAL = _This->length )
  CCI_ATTR_GET( MemoryInputStream, Buffer , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->buffer) )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( MemoryInputStream )
  CCIA_DISCARD(MemoryInputStream,Buffer)
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method:  Init
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( MemoryInputStream, Init )
{
  CCI_INSTANCE( MemoryInputStream ) 

  _This->buffer    = CCI_P(buffer);
  _This->length    = CCI_P(length);
  _This->behaviors = CCI_P(behaviors); 

  if(_This->length > 0 && _This->buffer == NULL)
     CCI_RETURN_INVALID_PARAM()

  _This->left = _This->length;
     
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method:  Read
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( InputStream, Read )
{
  CCI_INSTANCE( MemoryInputStream ) 

  if(_This->buffer)
  {
    dm_uint count = dm_min(_This->left,CCI_P(count));

    if(CCI_P(buffer)) {
       dmMemory::Memcpy(CCI_P(buffer),_This->buffer + _This->offset,count); 
    }
    
    _This->offset += count;
    _This->left   -= count;
    
    CCI_RETVAL_P(byteread) = count;
    CCI_RETURN_OK()
  }

  CCI_RETURN_NOT_INITIALIZED()
}
//---------------------------------------------------------------------
// Method:  Close
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( InputStream, Close )
{
  CCI_UNUSUED_INSTANCE( MemoryInputStream ) 
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( MemoryInputStream )
  CCI_REGISTER_METHOD_PP( InputStream      , Read  )
  CCI_REGISTER_METHOD_NP( InputStream      , Close )
  CCI_REGISTER_METHOD_PP( MemoryInputStream, Init  )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( MemoryInputStream )
{
  CCI_INVOKE_FACTORY(MemoryInputStream);
}
//---------------------------------------------------------------------
