
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
#include "daim_modules/CCI_FileOutputStream.h"
#include "daim_modules/CCI_Interface.h"

#define CCIC_NAME  CCIC_FileOutputStream
#define CCIC_SUPER CCIC_OutputStream
#define CCIC_SUPER_VERSION CCI_OutputStream_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_FileOutputStream_Version
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
   CCI_Set(_This_ob,CCIA_DECL_(OutputStream,Reason),reason)

//-----------------------------------------
CCI_DECL_DATA( FileOutputStream )
{
  FILE*     fp;
  dm_uint32 ioFlags;
  dm_uint32 behaviors;
  dmString  spec;
};
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static inline CCI_CONSTRUCTOR( FileOutputStream )
{
  CCI_CONSTRUCT_SUPER()
  
  CCI_INSTANCE( FileOutputStream )  
  CCI_INIT_MEMBER(spec);

  _This->fp        = NULL;
  _This->ioFlags   = 0;
  _This->behaviors = 0;

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static inline CCI_DESTRUCTOR( FileOutputStream )
{
  CCI_INSTANCE( FileOutputStream )
  
  if(_This->fp)
     fclose(_This->fp);

  CCI_DESTROY_MEMBER(spec);
  CCI_DESTROY_SUPER()
}
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( FileOutputStream )
{
  CCI_RETURN_INVOKE_SUPER()
}
*/

CCI_IMPL_GETTER( FileOutputStream )
  CCI_ATTR_GET( OutputStream    , IsNonBlocking, CCIA_RETVAL = dm_false )
  CCI_ATTR_GET( FileOutputStream, Spec         , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->spec.Get()) )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
/*
static inline CCI_METHOD_SET( FileOutputStream )
{
  CCI_RETURN_INVOKE_SUPER()
}
*/

CCI_IMPL_SETTER( FileOutputStream )
  CCIA_DISCARD(FileOutputStream,Spec)
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method:  Init
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( FileOutputStream, Init )
{
  CCI_INSTANCE( FileOutputStream ) 

  // File must be closed first !!
  if(_This->fp)
     CCI_RETURN_FAIL();

  _This->spec     = CCI_P(spec);
  _This->ioFlags  = CCI_P(ioFlags);

  if(_This->behaviors & DM_IO_PRETEND_TO_OPEN)
     CCI_RETURN_OK()
  
  char_t* mode     = _TXT("wb");
  if(_This->ioFlags & DM_IO_APPEND)
     mode = _TXT("ab");
  
  _This->fp = _tfopen(_This->spec.Get(),mode);
  
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
     
  CCI_RETURN_NOT_INITIALIZED()
}
//---------------------------------------------------------------------
// Method:  Write
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( OutputStream, Write )
{
  CCI_INSTANCE( FileOutputStream ) 

  if(_This->fp) 
  {
    if(CCI_P(buffer))
    {
      if( (CCI_RETVAL_P(bytewritten) = fwrite(CCI_P(buffer),1,CCI_P(count),_This->fp)) != CCI_P(count))
        SET_REASON(errno);
    }
    else
    {
      FILE* fp = _This->fp; 
      for(size_t i=0;i<CCI_P(count);++i) 
        if(fputc(0,fp)==EOF) {
          SET_REASON(errno);
          CCI_RETURN_FAIL()
        }
    }

    CCI_RETURN_OK()
  }

  CCI_RETURN_NOT_INITIALIZED()
}
//---------------------------------------------------------------------
// Method:  Flush
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( OutputStream, Flush )
{
  CCI_INSTANCE( FileOutputStream ) 

  if(_This->fp) {
    if(fflush(_This->fp)!=0) {
      SET_REASON(errno);
      CCI_RETURN_FAIL()     
    }
    CCI_RETURN_OK()
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method:  Close
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( OutputStream, Close )
{
  CCI_INSTANCE( FileOutputStream ) 

  if(_This->fp) {
     fclose(_This->fp);
     _This->fp = NULL;
  }

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method:  Seek
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( FileOutputStream, Seek )
{
  CCI_INSTANCE( FileOutputStream ) 

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
CCI_BEGIN_DISPATCH_MAP( FileOutputStream )
  CCI_REGISTER_METHOD_PP( OutputStream    , Write )
  CCI_REGISTER_METHOD_NP( OutputStream    , Flush )
  CCI_REGISTER_METHOD_NP( OutputStream    , Close )
  CCI_REGISTER_METHOD_PP( FileOutputStream, Init  )
  CCI_REGISTER_METHOD_PP( FileOutputStream, Seek  )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( FileOutputStream )
{
  CCI_INVOKE_FACTORY(FileOutputStream);
}
//---------------------------------------------------------------------
