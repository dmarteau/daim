
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
#include "daim_utilities.h"
#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_FrameGrabber.h"
#include "daim_modules/CCI_ImageList.h"

#define CCIC_NAME  CCIC_FrameGrabber
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_FrameGrabber_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2005"))
//-------------------------------------------------------------------
#define CCI_INTERNAL_MODULE

#include "daim_modules/CCI_Base.h"
#include "daim_modules/CCI_Interface.h"

//-----------------------------------------
CCI_DECL_DATA( FrameGrabber )
{
  dm_int State;
  dm_int XferStatus;
};

typedef CCI_INSTANCE_PTR(FrameGrabber) _Instance_Ptr;

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
static inline CCI_CONSTRUCTOR( FrameGrabber )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( FrameGrabber )

  _This->State      = 0;
  _This->XferStatus = 0;

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// ME_DISPOSE
//---------------------------------------------------------------------
static inline  CCI_DESTRUCTOR( FrameGrabber )
{
  CCI_UNUSUED_INSTANCE( FrameGrabber )
  
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
CCI_IMPL_GETTER( FrameGrabber )
  CCI_ATTR_GET( FrameGrabber, State      , CCIA_RETVAL = _This->State      ) 
  CCI_ATTR_GET( FrameGrabber, XferStatus , CCIA_RETVAL = _This->XferStatus ) 
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( FrameGrabber )
  CCI_ATTR_SET( FrameGrabber, State      , _This->State      = CCIA_DATA )
  CCI_ATTR_SET( FrameGrabber, XferStatus , _This->XferStatus = CCIA_DATA )
CCI_END_SETTER()
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( FrameGrabber , CopyFrameBuffer )
{
  dmIImageList _ImageList;
  if(_ImageList.QueryInterface(CCI_P(imagelist)))
  {
    dm_uint index = CCI_P(index); 

    IF_CCI_INVOKE_RP(_THIS_OBJECT,FrameGrabber,GetFrameBuffer, _in 0 ) 
    THEN
    CCI_RETURN_IMETHOD_PP_(_ImageList,ImageList,SetBufferData, 
        _in index
        _in CCI_RETVAL(format)
        _in CCI_RETVAL(width)
        _in CCI_RETVAL(height) 
        _in CCI_RETVAL(stride) 
        _in CCI_RETVAL(scan0) 
     );
    ENDIF

    CCI_RETURN_FAIL()
  }

  CCI_RETURN_INVALID_PARAM()
}
/*
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( FrameGrabber , Initialize )
{
  CCI_INSTANCE( FrameGrabber )
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( FrameGrabber , ShutDown )
{
  CCI_INSTANCE( FrameGrabber )
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( FrameGrabber , Suspend )
{
  CCI_INSTANCE( FrameGrabber )
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( FrameGrabber , Resume )
{
  CCI_INSTANCE( FrameGrabber )
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( FrameGrabber , Reset )
{
  CCI_INSTANCE( FrameGrabber )
  CCI_RETURN_FAIL()
}
*/
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( FrameGrabber , ExecCommand )
{
  CCI_UNUSUED_INSTANCE(FrameGrabber)

  if(!EMPTY_STRING(CCI_P(command)))
     dmLOG("Document:Unknow command %s\n",CCI_P(command));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( FrameGrabber , IsCommandSupported )
{
  CCI_UNUSUED_INSTANCE(FrameGrabber)
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( FrameGrabber )
  CCI_REGISTER_METHOD_PP( FrameGrabber, CopyFrameBuffer  )
  CCI_REGISTER_METHOD_PP( FrameGrabber, ExecCommand        )
  CCI_REGISTER_METHOD_PP( FrameGrabber, IsCommandSupported )

/*
  CCI_REGISTER_METHOD_PP( FrameGrabber, Initialize )
  CCI_REGISTER_METHOD_NP( FrameGrabber, ShutDown   )   
  CCI_REGISTER_METHOD_NP( FrameGrabber, Suspend    )
  CCI_REGISTER_METHOD_NP( FrameGrabber, Resume     )
  CCI_REGISTER_METHOD_NP( FrameGrabber, Reset      )
*/
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( FrameGrabber )
{
  CCI_INVOKE_FACTORY(FrameGrabber);
}
//---------------------------------------------------------------------
