
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
#include "daim_modules/CCI_CommandHandler.h"

//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_CommandHandler
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_CommandHandler_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2006"))
//--------------------------------------------------------------------
#define CCI_INTERNAL_MODULE
#include "daim_modules/CCI_Base.h"

#define MAX_ATTRS 0xFF /* 255 */
typedef dm_param ATTRLIST[MAX_ATTRS];

#define TAG_AGENT(tg)  (((tg) & 0xFFFFFF00L)==_CCI_BASE_DECL_(CommandHandler))
#define TAG_ID(tg)     ((tg)  & 0x000000FFL)

//--------------------------------------------------------------------
CCI_DECL_DATA( CommandHandler )
{
  ATTRLIST mAttrs;
};

//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( CommandHandler )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE(CommandHandler)

  dmMemory::Memset(_This->mAttrs,0,sizeof(ATTRLIST));

  CCI_MAP_ITEMS_() { 
    if(TAG_AGENT(CCIA_TAG))
       _This->mAttrs[TAG_ID(CCIA_TAG)] = CCIA_DATA;
  }  

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( CommandHandler )
{
  CCI_UNUSUED_INSTANCE(CommandHandler)
  CCI_DESTROY_SUPER()
}
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Environment )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Environment )
CCI_END_SETTER()
*/
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( CommandHandler )
{
  CCI_INSTANCE(CommandHandler)
  CCI_GET_STORE_()

  if(TAG_AGENT(CCIA_RETVAL_TAG)) {
    CCIA_RETVAL = _This->mAttrs[TAG_ID(CCIA_RETVAL_TAG)];
    CCI_RETURN_OK()
  }

  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( CommandHandler )
{
  CCI_INSTANCE(CommandHandler)

  CCI_MAP_ITEMS_() { 
    if(TAG_AGENT(CCIA_TAG))
       _This->mAttrs[TAG_ID(CCIA_TAG)] = CCIA_DATA;
  }

  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( CommandHandler , ExecCommand )
{
  CCI_UNUSUED_INSTANCE(CommandHandler)

  if(!EMPTY_STRING(CCI_P(command)))
     dmLOG("CommandHandler:Unknow command %s\n",CCI_P(command));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( CommandHandler , IsCommandSupported )
{
  CCI_UNUSUED_INSTANCE(CommandHandler)
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( CommandHandler )
  CCI_REGISTER_METHOD_PP( CommandHandler , ExecCommand        )
  CCI_REGISTER_METHOD_PP( CommandHandler , IsCommandSupported )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( CommandHandler )
{   
  CCI_INVOKE_FACTORY(CommandHandler)
}
//---------------------------------------------------------------------
