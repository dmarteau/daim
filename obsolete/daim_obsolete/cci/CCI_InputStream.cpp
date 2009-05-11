
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

#define CCIC_NAME  CCIC_InputStream
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_InputStream_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2006"))
//-------------------------------------------------------------------
#define CCI_INTERNAL_MODULE

#include "daim_modules/CCI_Base.h"
//-----------------------------------------
CCI_DECL_DATA( InputStream )
{
  dm_param reason;
  dmString contentType;
};
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static inline CCI_CONSTRUCTOR( InputStream )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( InputStream )
  
  CCI_INIT_MEMBER(contentType);
  
  _This->contentType   = "application/octet-stream";
  _This->reason = 0;
  
  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static inline CCI_DESTRUCTOR( InputStream )
{
  CCI_INSTANCE( InputStream )
  CCI_DESTROY_MEMBER(contentType);
  CCI_DESTROY_SUPER()  
}
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( InputStream )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( InputStream )
{
  CCI_RETURN_INVOKE_SUPER()
}
*/
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( InputStream )
  CCI_ATTR_GET(InputStream, Reason       , CCIA_RETVAL = _This->reason )
  CCI_ATTR_GET(InputStream, ContentType  , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->contentType.Get()) )
  CCI_ATTR_GET(InputStream, ContentLength, CCIA_RETVAL = dm_noindex )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( InputStream )
  CCI_ATTR_SET(InputStream, Reason      , _This->reason = CCIA_DATA )
  CCI_ATTR_GET(InputStream, ContentType , _This->contentType = CCIA_TRANSLATE_DATA(char_t*))
  CCIA_DISCARD(InputStream, IsNonBlocking)
  CCIA_DISCARD(InputStream, ContentLength)
CCI_END_SETTER()
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( InputStream )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( InputStream )
{
  CCI_INVOKE_FACTORY(InputStream);
}
//---------------------------------------------------------------------
