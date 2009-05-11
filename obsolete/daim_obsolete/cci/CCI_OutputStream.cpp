
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
#include "daim_modules/CCI_OutputStream.h"
#include "daim_modules/CCI_InputStream.h"
#include "daim_modules/CCI_Interface.h"

#define CCIC_NAME  CCIC_OutputStream
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_OutputStream_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2006"))
//-------------------------------------------------------------------
#define CCI_INTERNAL_MODULE

#include "daim_modules/CCI_Base.h"
//-----------------------------------------
CCI_DECL_DATA( OutputStream )
{
  dm_param reason;  
  dm_byte* buffer;
  dm_uint  size;
};
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static inline CCI_CONSTRUCTOR( OutputStream )
{
  CCI_CONSTRUCT_SUPER()
  
  CCI_INSTANCE( OutputStream )

  _This->buffer = NULL;
  _This->size   = 0;
  _This->reason = 0;
  
  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static inline CCI_DESTRUCTOR( OutputStream )
{
  CCI_INSTANCE( OutputStream )
  
  if(_This->buffer)
     dmMemory::Free(_This->buffer);

  CCI_DESTROY_SUPER()
}
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( OutputStream )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( OutputStream )
{
  CCI_RETURN_INVOKE_SUPER()
}
*/
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( OutputStream )
  CCI_ATTR_GET( OutputStream, Reason, CCIA_RETVAL = _This->reason )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( OutputStream )
  CCI_ATTR_SET(OutputStream, Reason, _This->reason = CCIA_DATA )
  CCIA_DISCARD(OutputStream,IsNonBlocking)
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method:  WriteFrom
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( OutputStream, WriteFrom )
{
  CCI_INSTANCE( OutputStream ) 
  
  dmIInputStream _IStream;
  
  if(_IStream.QueryInterface(CCI_P(input)))
  {
    dm_uint count = CCI_P(count);
    
    if(_This->size < count) {
       _This->buffer = reinterpret_cast<dm_byte*>(dmMemory::Realloc(_This->buffer,count)); 
       _This->size   = CCI_P(count);
    }
  
    if(_IStream.Read(_This->buffer,count)==count) {
    	CCI_RETURN_IMETHOD_PP_(_THIS_OBJECT,OutputStream,Write,
               _in _This->buffer
               _in count );  
    }
  }

  CCI_RETURN_INVALID_PARAM();
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( OutputStream )
  CCI_REGISTER_METHOD_PP( OutputStream, WriteFrom )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( OutputStream )
{
  CCI_INVOKE_FACTORY(OutputStream);
}
//---------------------------------------------------------------------
