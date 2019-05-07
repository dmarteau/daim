#define dmUseCCI
#define dmUseUtilitiesExtra

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


#include "daim_utilities.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_UserInterface.h"
#include "daim_modules/CCI_AdvTools.h"
//------------------------------------------------------------------
#define CCIC_NAME  CCIC_UserInterface
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_UserInterface_Version
#define CCI_REVISION  1
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2006"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"

#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"

//--------------------------------------------------------------------
CCI_DECL_DATA( UserInterface )
{
  dm_uint _Dummy;
};
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( UserInterface )
{
  CCI_GENERIC_CONSTRUCT(UserInterface)
  CCI_RETURN_THIS()
}
//--------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( UserInterface )
{
  CCI_DESTROY_SUPER()
}
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( UserInterface )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( UserInterface )
CCI_END_SETTER()
*/
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( UserInterface )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( UserInterface )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( UserInterface )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
cci_Class* CCI_Document_Init();
cci_Class* CCI_ImageInfo_Init();
cci_Class* CCI_Manager_Init();      
cci_Class* CCI_Graphics_Init();
cci_Class* CCI_EventListener_Init();
cci_Class* CCI_ToolInfo_Init();

static dm_param _Initialize(cci_Class* cl,void* _UserData)
{
  //__dm_DebugBreak__
  
  CCI_Flush(cl,~0);  // Make the class persistent (not flushable)

  // Register modules
  VERIFY_OR_RETURN(CCI_Document_Init()     ,CCI_ERR_GENERIC,_TXT("ERROR: cannot initialise cci module 'document' !\n"))

#ifndef NO_DAIM_USER_INTERFACE
  VERIFY_OR_RETURN(CCI_Manager_Init()      ,CCI_ERR_GENERIC,_TXT("ERROR: cannot initialise cci module 'tool manager' !\n"))
  VERIFY_OR_RETURN(CCI_Graphics_Init()     ,CCI_ERR_GENERIC,_TXT("ERROR: cannot initialise cci module 'graphics' !\n"))
  VERIFY_OR_RETURN(CCI_EventListener_Init(),CCI_ERR_GENERIC,_TXT("ERROR: cannot initialise cci module 'event listener' module !\n"))
  VERIFY_OR_RETURN(CCI_ToolInfo_Init()     ,CCI_ERR_GENERIC,_TXT("ERROR: cannot initialise cci module 'event toolinfo' module !\n"))
#endif
  
  VERIFY_OR_RETURN(CCI_ImageInfo_Init()    ,CCI_ERR_GENERIC,_TXT("ERROR: cannot initialise cci module 'image info' !\n"))

  #ifndef NO_DAIM_USER_INTERFACE
     dmIRegisterAdvTools();
  #endif

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
static void _Finalize(cci_Class* cl,void* _UserData) 
{ 
  // TODO: Should deregister handler 
}
//---------------------------------------------------------------------
CCI_ENTRY_POINT( UserInterface )
{
  //__dm_DebugBreak__
    
  //DM_INIT_API()

  CCI_INVOKE_FACTORY_(UserInterface,NULL,_Initialize,_Finalize);
}
//---------------------------------------------------------
