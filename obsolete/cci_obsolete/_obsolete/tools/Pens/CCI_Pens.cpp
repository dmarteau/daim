
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

#define dmUseCCI
#define dmUseUtilitiesExtra
#include "daim_tools.h"
#include "daim_modules/CCI_Notify.h"

#include "CCI_Pens.h"
//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_Pens
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR "David Marteau"
#define CCI_VERSION   CCI_Pens_Version
#define CCI_REVISION  1
#define CCI_IDSTRING  "$VER: " CCIC_NAME " %d.%d, " AUTHOR " , " __DATE__ "(C) 2006"
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"

bool dmRegisterPens();
//---------------------------------------------------------------------
CCI_DECL_DATA(Pens)
{
  dm_uint mDummy;
};
//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
static inline CCI_CONSTRUCTOR( Pens )
{
  CCI_CONSTRUCT_SUPER()
  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// ME_DISPOSE
//---------------------------------------------------------------------
static inline  CCI_DESTRUCTOR( Pens )
{
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( Pens )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( Pens )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Pens )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
dm_param Pens_Initialize(cci_Class* cl,void* _UserData)
{
  if(!dmRegisterPens()) {
    CCI_Message(CCI_ERR_GENERIC,0,cl,NULL,_TXT("ERROR: Cannot register pens!\n"));
    CCI_RETURN_FAIL()
  } else
    CCI_Flush(cl,~0);  // Make the class persistent (not flushable)

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Pens )
{
  CCI_INVOKE_FACTORY_(Pens,NULL,Pens_Initialize,NULL);
}
//---------------------------------------------------------------------
