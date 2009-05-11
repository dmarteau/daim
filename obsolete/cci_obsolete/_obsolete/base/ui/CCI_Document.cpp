
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

#ifndef NO_DAIM_USER_INTERFACE
#define dmUseToolManager
#include "daim_tools.h"
#else  //NO_DAIM_USER_INTERFACE
#include "daim_utilities.h"
#endif //NO_DAIM_USER_INTERFACE

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Document.h"

//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_Document
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Document_Version
#define CCI_REVISION  1
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#define CCI_INTERNAL_MODULE
#include "daim_modules/CCI_Base.h"
#include "daim_modules/CCI_Interface.h"

CCI_DECL_DATA( Document )
{
  dm_bool bModified;
  dm_bool bUpdate;
  dm_bool bNew;
  dm_uint nLastCommand;
};

//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( Document )
{
  CCI_CONSTRUCT_SUPER()

  CCI_INSTANCE( Document )
  _This->bModified    = dm_false;
  _This->bUpdate      = dm_true;
  _This->bNew         = dm_false;
  _This->nLastCommand = 0;

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static inline CCI_DESTRUCTOR( Document )
{
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Document )
  CCI_ATTR_GET( Document, Modified, CCIA_RETVAL = _This->bModified    )
  CCI_ATTR_GET( Document, Updated , CCIA_RETVAL = _This->bUpdate      )
  CCI_ATTR_GET( Document, New     , CCIA_RETVAL = _This->bNew         )
  CCI_ATTR_GET( Document, Command , CCIA_RETVAL = _This->nLastCommand )
  CCI_ATTR_GET( Document, EmptyDocument , CCIA_RETVAL = dm_true )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Document )
  CCI_ATTR_SET( Document, Modified,_This->bModified    = CCIA_DATA )
  CCI_ATTR_SET( Document, Updated ,_This->bUpdate      = CCIA_DATA )
  CCI_ATTR_SET( Document, New     ,_This->bNew         = CCIA_DATA )
  CCI_ATTR_SET( Document, Command ,_This->nLastCommand = CCIA_DATA )
  CCIA_DISCARD( Document, EmptyDocument );
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method : GetManager
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, GetManager )
{
  CCI_RETURN_NOT_IMPLEMENTED()
}
//---------------------------------------------------------------------
// Method : GetSize
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, GetSize )
{
  CCI_RETURN_NOT_IMPLEMENTED()
}
//---------------------------------------------------------------------
// Method : Draw
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, Draw )
{
  CCI_RETURN_NOT_IMPLEMENTED()
}
//---------------------------------------------------------------------
// Method : SetGraphics
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Document, SetGraphics )
{
#ifndef NO_DAIM_USER_INTERFACE

  CCI_UNUSUED_INSTANCE( Document )
  dmToolManager* pManager = NULL;

  IF_CCI_INVOKE_RP(_THIS_OBJECT,Document,GetManager, _out(toolmngr) NULL)
  THEN pManager =  CCI_RETVAL(toolmngr);
  ENDIF

  if(pManager) {
    dmGraphics* new_graphics = CCI_P(new_gfx);
    dmGraphics* old_graphics = pManager->GetGraphics();

    pManager->SetGraphics(new_graphics);
    CCI_RETVAL_P(old_gfx) = old_graphics;
    CCI_RETURN_OK()
  }
#endif
  CCI_RETURN_FAIL();
}
//---------------------------------------------------------------------
// Method : Initialize
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Document, Initialize )
{
#ifndef NO_DAIM_USER_INTERFACE

  dmToolManager* pManager = NULL;

  IF_CCI_INVOKE_RP(_THIS_OBJECT,Document,GetManager, _out(toolmngr) NULL)
  THEN pManager = CCI_RETVAL(toolmngr);
  ENDIF

  if(pManager && !pManager->IsInitialized()) {
    pManager->RecalcObject();
    CCI_RETURN_OK()
  }

#endif // NO_DAIM_USER_INTERFACE
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : SetCursorType
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, SetCursorType )
{
#ifndef NO_DAIM_USER_INTERFACE

  dmToolManager* pManager = NULL;

  IF_CCI_INVOKE_RP(_THIS_OBJECT,Document,GetManager, _out(toolmngr) NULL)
  THEN pManager =  CCI_RETVAL(toolmngr);
  ENDIF

  if(pManager) {
     pManager->SetDefaultCursor( static_cast<ECursorType>(CCI_P(cursor)) );     
     CCI_RETURN_OK()
  }

#endif // NO_DAIM_USER_INTERFACE
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : IsParentOf
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, IsParentOf )
{
#ifndef NO_DAIM_USER_INTERFACE

  dmToolManager* pManager = NULL;
  dmTool*        pTool    = CCI_P(tool);

  if(pTool != NULL )
  {
    IF_CCI_INVOKE_RP(_THIS_OBJECT,Document,GetManager, _out(toolmngr) NULL)
    THEN pManager =  CCI_RETVAL(toolmngr);
    ENDIF

    if(pManager && pTool->IsChildOf(pManager))
       CCI_RETURN_OK()
  }
#endif // NO_DAIM_USER_INTERFACE
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
#define IF_EXECCOMMAND( cmd ) \
    if(EQUAL_STRING(cmd_name,cmd))

static CCI_IMPL_METHOD( Document , ExecCommand )
{
  CCI_UNUSUED_INSTANCE(Document)

  const char_t* cmd_name = CCI_P(command);
  const char_t* arg      = CCI_P(arg);

#ifndef NO_DAIM_USER_INTERFACE
  
  IF_EXECCOMMAND("EnableCapture") 
  {
    dmToolManager* pManager = NULL;

    IF_CCI_INVOKE_RP(_THIS_OBJECT,Document,GetManager, _out(toolmngr) NULL)
    THEN 
      pManager = CCI_RETVAL(toolmngr);
      if(pManager) {
         pManager->EnableCapture(EQUAL_STRING(arg,"true"));
         CCI_RETURN_OK()
      }
    ENDIF 
    
    CCI_RETURN_FAIL()
  }
#endif // NO_DAIM_USER_INTERFACE

  if(!EMPTY_STRING(CCI_P(command)))
     dmLOG("Document:Unknow command %s\n",CCI_P(command));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document , IsCommandSupported )
{
  CCI_UNUSUED_INSTANCE(Document)
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Document )
  CCI_REGISTER_METHOD_PP( Document, GetManager         )
  CCI_REGISTER_METHOD_PP( Document, GetSize            )
  CCI_REGISTER_METHOD_PP( Document, Draw               )
  CCI_REGISTER_METHOD_PP( Document, SetGraphics        )
  CCI_REGISTER_METHOD_NP( Document, Initialize         )
  CCI_REGISTER_METHOD_PP( Document, SetCursorType      )
  CCI_REGISTER_METHOD_PP( Document, IsParentOf         )
  CCI_REGISTER_METHOD_PP( Document, ExecCommand        )
  CCI_REGISTER_METHOD_PP( Document, IsCommandSupported )
CCI_NEXT_DISPATCH_MAP()

//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT(Document)
{   
  CCI_INVOKE_FACTORY(Document);
}
//---------------------------------------------------------------------
