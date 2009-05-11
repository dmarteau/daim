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
#include "daim_modules/CCI_FileInputStream.h"

#define CCIC_NAME  CCIC_FileInputStream
#define CCIC_SUPER CCIC_InputStream
#define CCIC_SUPER_VERSION CCI_InputStream_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_FileInputStream_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2006"))
//-------------------------------------------------------------------
#define CCI_INTERNAL_MODULE
#include "daim_modules/CCI_Base.h"
#include "daim/daim_io.h"

#include <stdio.h>
#include <errno.h>

#define ENSURE_POINTER( ptr ) \
   if(ptr == NULL) \
      CCI_RETURN_INVALID_POINTER()

#define SET_REASON( reason ) \
   CCI_Set(_This_ob,CCIA_DECL_(InputStream,Reason),reason)

//-----------------------------------------
CCI_DECL_DATA( FileInputStream )
{
  FILE*     fp;
  dm_uint32 ioFlags;
  dm_uint32 behaviors;
  dmString  spec;
};
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static inline CCI_CONSTRUCTOR( FileInputStream )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( FileInputStream )
  CCI_INIT_MEMBER(spec)
   
  _This->fp        = NULL;
  _This->ioFlags   = 0;
  _This->behaviors = 0;

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static inline CCI_DESTRUCTOR( FileInputStream )
{
  CCI_INSTANCE( FileInputStream )
  
  if(_This->fp)
     fclose(_This->fp);

  CCI_DESTROY_MEMBER(spec);
  CCI_DESTROY_SUPER()
}

//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
/*
static inline CCI_METHOD_GET( FileInputStream )
{
  CCI_RETURN_INVOKE_SUPER()
}
*/

CCI_IMPL_GETTER( FileInputStream )
  CCI_ATTR_GET( InputStream, IsNonBlocking, CCIA_RETVAL = dm_false )
  CCI_ATTR_GET( FileInputStream, Spec , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->spec.Get()) )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
/*
static inline CCI_METHOD_SET( FileInputStream )
{
  CCI_RETURN_INVOKE_SUPER()
}
*/

CCI_IMPL_SETTER( FileInputStream )
  CCIA_DISCARD(FileInputStream,Spec)
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method:  Init
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( FileInputStream, Init )
{
  CCI_INSTANCE( FileInputStream ) 

  // File must be closed first !!
  if(_This->fp)
     CCI_RETURN_FAIL();

  _This->spec      = CCI_P(spec);
  _This->ioFlags   = CCI_P(ioFlags);
  _This->behaviors = CCI_P(behaviors); 
  
  if(_This->behaviors & DM_IO_PRETEND_TO_OPEN) {
     CCI_RETURN_OK()
  }
  
  _This->fp = _tfopen(_This->spec.Get(),"rb");
  
  if(_This->fp)
  {
    CCI_RETURN_OK()
  } else {
    int err = errno;
    dmString _ErrMsg = dmString::FormatString(dmString::npos,_TXT("Open failed %s : %s "),
                                              _This->spec.Get(),strerror( err ));
    CCI_SET_ERROR_MSG( _ErrMsg.Get() );
    SET_REASON(err);
    CCI_RETURN_FAIL()
  }
     
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method:  Read
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( InputStream, Read )
{
  CCI_INSTANCE( FileInputStream ) 

  if(_This->fp) 
  {
    if(CCI_P(buffer)) 
    {
      if( (CCI_RETVAL_P(byteread) = fread(CCI_P(buffer),1,CCI_P(count),_This->fp)) != CCI_P(count)) 
         SET_REASON(errno);
    } else {
      if(fseek(_This->fp,CCI_P(count),SEEK_CUR)) {
        SET_REASON(errno);
        CCI_RETVAL_P(byteread) = 0;
        CCI_RETURN_FAIL()
      } else
        CCI_RETVAL_P(byteread) = CCI_P(count);
    }
    CCI_RETURN_OK()
  }

  CCI_RETURN_NOT_INITIALIZED()
}
//---------------------------------------------------------------------
// Method:  Close
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( InputStream, Close )
{
  CCI_INSTANCE( FileInputStream ) 

  if(_This->fp) {
     fclose(_This->fp);
     _This->fp = NULL;     
  }

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method:  Seek
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( FileInputStream, Seek )
{
  CCI_INSTANCE( FileInputStream ) 

  if(_This->fp) 
  {
    if(fseek(_This->fp,CCI_P(offset),CCI_P(whence))) {
       SET_REASON(errno);
       CCI_RETURN_FAIL();
    }
  }

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( FileInputStream )
  CCI_REGISTER_METHOD_PP( InputStream    , Read  )
  CCI_REGISTER_METHOD_NP( InputStream    , Close )
  CCI_REGISTER_METHOD_PP( FileInputStream, Seek  )
  CCI_REGISTER_METHOD_PP( FileInputStream, Init  )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( FileInputStream )
{
  CCI_INVOKE_FACTORY(FileInputStream);
}
//---------------------------------------------------------------------
