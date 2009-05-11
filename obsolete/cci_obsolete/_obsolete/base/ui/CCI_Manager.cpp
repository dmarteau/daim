
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

#define dmUseToolManager
#include "daim_tools.h"

#include "daim/daim_base.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Document.h"
#include "daim_modules/CCI_Info.h"
#include "daim_modules/CCI_Manager.h"
#include "daim_modules/CCI_EventListener.h"
#include "daim_modules/CCI_ToolInfo.h"
#include "daim_modules/CCI_Interface.h"

//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_Manager
#define CCIC_SUPER CCIC_Document
#define CCIC_SUPER_VERSION CCI_Document_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Manager_Version
#define CCI_REVISION  1
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#define CCI_INTERNAL_MODULE

#include "daim_modules/CCI_Base.h"
#include "daim_manager.h"

//--------------------------------------------------------------------
CCI_DECL_DATA( Manager )
{
  daim_CmdUI    cmdui;
  daim_manager* manager;
};

#define CALL_MANAGER( _Me ) (_This->manager?_This->manager->_Me : 0)
//--------------------------------------------------------------------
static daim_manager* mCreateManager( dm_uint32 width, dm_uint32 height )
{
  daim_manager *manager = new daim_manager(NULL,dmRect(0,0,width,height));
  manager->Show(true);
  manager->Initialize();
  return manager;
}
//--------------------------------------------------------------------
// ME_NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( Manager )
{
  CCI_CONSTRUCT_SUPER()

  CCI_INSTANCE(Manager)
  _This->manager  = NULL;

  CCI_INIT_MEMBER( cmdui );

  dm_uint width = 0,height = 0;
  dm_uint nEditCaps = 0;

  CCI_BEGIN_INIT_MAP(Manager)
    CCI_ATTR_SET(Document,Width   , width     = CCIA_DATA )
    CCI_ATTR_SET(Document,Height  , height    = CCIA_DATA )
    CCI_ATTR_SET(Manager ,EditCaps, nEditCaps = CCIA_DATA )
  CCI_END_INIT_MAP()

  if(dm_ptr_to_long(_This->manager)==CCIV_Manager_New)
    _This->manager = mCreateManager(width,height);

  if(_This->manager) {
    _This->manager->Obtain();
    _This->manager->SetEditCaps(nEditCaps);
  }

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// ME_DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( Manager )
{
  CCI_INSTANCE(Manager)
  if(_This->manager && _This->manager->Release()==0)
    delete _This->manager;

  _This->manager = NULL;
  CCI_DESTROY_MEMBER(cmdui);

  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// ME_GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Manager )
  CCI_ATTR_GET( Document,Width        , CCIA_RETVAL = CALL_MANAGER( GetRect().Width()  ) )
  CCI_ATTR_GET( Document,Height       , CCIA_RETVAL = CALL_MANAGER( GetRect().Height() ) )
  CCI_ATTR_GET( Manager ,EditCaps     , CCIA_RETVAL = CALL_MANAGER( EditCaps()         ) )
  CCI_ATTR_GET( Manager ,CurrentToolId, CCIA_RETVAL = CALL_MANAGER( GetCurrentToolId() ) )
  CCI_ATTR_GET( Manager ,HasFocus     , CCIA_RETVAL = _BOOL(CALL_MANAGER( HasFocus() ))  )
  CCI_ATTR_GET( Manager ,IsEmpty      , CCIA_RETVAL = _BOOL(CALL_MANAGER( Empty()    ))  )
CCI_END_GETTER()
//---------------------------------------------------------------------
// ME_SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Manager )
  CCI_ATTR_SET( Manager , EditCaps,
     if(_This->manager)
        _This->manager->SetEditCaps(CCIA_DATA);
  )
  CCIA_DISCARD( Manager , CurrentToolId )
  CCIA_DISCARD( Document, Width  )
  CCIA_DISCARD( Document, Height )
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method: GetManager
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, GetManager )
{
  CCI_INSTANCE(Manager)
  dmToolManager* mngr = static_cast<dmToolManager*>(_This->manager);
  if(mngr) {
    CCI_RETVAL_P( toolmngr ) = mngr;
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetSize
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, GetSize )
{
  CCI_INSTANCE(Manager)
  if(_This->manager) {
    const dmRect& r = _This->manager->GetRect();
    CCI_RETVAL_P(width)  = r.Width();
    CCI_RETVAL_P(height) = r.Height();
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Draw
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, Draw )
{
  CCI_INSTANCE(Manager)

  if(_This->manager)
  {
    switch(CCI_P(flags))
    {
      case CCIV_Document_DrawNormal :
        _This->manager->Display();
        CCI_RETURN_OK()

      case CCIV_Document_DrawBckgnd :
        CCI_RETURN_NOT_IMPLEMENTED()

      default : CCI_RETURN_INVALID_PARAM()
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: CreateTool
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Manager, CreateTool )
{
  CCI_INSTANCE(Manager)
  daim_manager* manager = _This->manager;
  if(manager)
  {
    dmTool* pNewTool = NULL;
    if(CCI_P(pts) && CCI_P(count)>0)
    {
      dmPoly p(CCI_P(pts),CCI_P(count));
      pNewTool = manager->LoadClassTool(CCI_P(id),CCI_P(classid),CCI_P(name),&p,
                                        CCI_P(edit)!=dm_false);
    } else {
      pNewTool = manager->LoadClassTool(CCI_P(id),CCI_P(classid),CCI_P(name),NULL,
                                        CCI_P(edit)!=dm_false);
    }

    if(pNewTool) {
      CCI_RETVAL_P(tool) = pNewTool;
      CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Update
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Document, Update )
{
  CCI_INSTANCE(Manager)

  if(_This->manager) {
     _This->manager->UpdateObject();
     CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: OnCommand
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, OnCommand )
{
  CCI_INSTANCE(Manager)
  if( _This->manager)
  {
    dm_param rv = CCI_ERR_METHOD_FAILED;

    CCI_Set(_THIS_OBJECT,CCIA_DECL_(Document,Command),CCI_P(cmd));

    if(_This->manager->OnCommand(CCI_P(cmd)) )
      rv = CCI_ERR_OK;

    CCI_Set(_THIS_OBJECT,CCIA_DECL_(Document,Command),0);
    return rv;
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: OnCmdUpdate
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, OnCmdUpdate )
{
  CCI_INSTANCE(Manager)
  if(_This->manager) {
    daim_CmdUI& cmdui = _This->cmdui;
    cmdui.nCode   = CCI_P(cmd);
    cmdui.checked = CCI_P(checked);
    if(_This->manager->OnCommandUpdate(&cmdui)) {
      CCI_RETVAL_P(enabled) = _BOOL(cmdui.bEnabled);
      CCI_RETVAL_P(checked) = cmdui.checked;
      CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: SetDisplay
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, SetDisplay )
{
  CCI_INSTANCE(Manager)

  if(_This->manager) {
    _This->manager->SetRect(dmRect(CCI_P(width),CCI_P(height)));
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: OpenDocument
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, OpenDocument )
{
  CCI_UNUSUED_INSTANCE(Manager)
  CCI_RETURN_NOT_IMPLEMENTED()
}
//---------------------------------------------------------------------
// CCIM_Document_SaveDocument
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, SaveDocument )
{
  CCI_UNUSUED_INSTANCE(Manager)
  CCI_RETURN_NOT_IMPLEMENTED()
}
//---------------------------------------------------------------------
// Method: FromInfo
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Manager, FromInfo )
{
  CCI_INSTANCE(Manager)
  if(_This->manager==NULL)
  {
    dmIImageInfo _Info;

    if(CCI_P(info)!=NULL)
    {
      if(!_Info.QueryInterface(CCI_P(info)))
        CCI_RETURN_INVALID_PARAM()
    }
    else
    {
      dmINFO _current_info;
      if(_current_info.GetInfo())
      {
        _Info.QueryInterface(_current_info);
        _current_info.Release(); //TODO: SHOULD NOT BE RELEASED HERE
      }
    }

    if(_Info.IsValid())
    {
       daim_manager*  manager = daim_manager::Cast(_Info.SelectManager(CCI_P(name),CCI_P(flags)));

       if(manager)
       {
         _This->manager = manager;
         _This->manager->Obtain();

         CCI_RETURN_OK()
       }
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: ReleaseMngr
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Manager, ReleaseMngr )
{
  CCI_INSTANCE(Manager)

  daim_manager* manager = _This->manager;
  if(manager!=NULL)
  {
    if(manager->Release()==0)
       delete manager;

    _This->manager = NULL;
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()

}
//---------------------------------------------------------------------
// Method: Create
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Manager, Create )
{
  CCI_INSTANCE(Manager)
  if(_This->manager==NULL) {
    _This->manager = mCreateManager(CCI_P(width),CCI_P(height));
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
};
//---------------------------------------------------------------------
// Method: SetFocus
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Manager, SetFocus )
{
  CCI_INSTANCE( Manager)
  if(_This->manager) {
    _This->manager->SetFocus(CCI_P(state)!=dm_false,NULL);
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Show
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Manager, Show )
{
  CCI_INSTANCE(Manager)
  if(_This->manager) {
    _This->manager->Show(CCI_P(state)!=dm_false);
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetToolByName
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Manager, GetToolByName )
{
  if(EMPTY_STRING(CCI_P(name)))
    CCI_RETURN_INVALID_PARAM()

  CCI_INSTANCE(Manager)
  daim_manager* manager = _This->manager;
  if(manager)
  {
    dmToolManager::iterator it   = manager->Begin();
    dmToolManager::iterator last = manager->End();
    for(;it!=last;++it) {
      if( (*it)->ToolName()==CCI_P(name) ) {
        CCI_RETVAL_P(tool) = *it;
        CCI_RETURN_OK()
      }
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Clear
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Manager, Clear )
{
  CCI_INSTANCE(Manager)
  if(_This->manager)
  {
    if(CCI_P(flags) == CCIV_Manager_Selection)
     _This->manager->ClearSelection();
    else
     _This->manager->Clear();

     CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetCurrentTool
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Manager, GetCurrentTool )
{
  CCI_INSTANCE(Manager)
  if(_This->manager) {
    CCI_RETVAL_P(tool) = _This->manager->GetCurrentTool();
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetCurrentTool
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Manager, AbortEdit )
{
  CCI_INSTANCE(Manager)
  if(_This->manager) {
    _This->manager->AbortEdit(CCI_P(id));
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: AddListener
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Manager , AddListener )
{
  CCI_INSTANCE(Manager)

  if(_This->manager)
  {
    dmIEventListener _Listener;
    if(_Listener.QueryInterface(CCI_P(listener)) &&
       _Listener.AddListener(_This->manager,CCI_P(evtMask),CCI_P(flags))) {

       CCI_RETURN_OK()
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: RemoveListener
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Manager , RemoveListener )
{
  CCI_INSTANCE(Manager)
  if(_This->manager)
  {
    dmIEventListener _Listener;
    if(_Listener.QueryInterface(CCI_P(listener)) &&
       _Listener.RemoveListener(_This->manager,CCI_P(evtMask),CCI_P(flags))) {

       CCI_RETURN_OK()
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Manager , GetToolInfo )
{
  CCI_INSTANCE(Manager)

  if(_This->manager==NULL)
    CCI_RETURN_FAIL()

  dmIToolInfo _ToolInfo;
  if(_ToolInfo.QueryInterface(CCI_P(toolinfo)))
  {
    dmTool* tool = NULL;

    const char_t* name = CCI_P(toolname);
    if(!EMPTY_STRING(name))
    {
      dmIManager _Self;
      _Self.QueryInterface(_THIS_OBJECT);
      tool = _Self.GetToolByName(name);
    } else
      tool = _This->manager->GetCurrentTool();

    if(tool && _ToolInfo.Assign(tool))
       CCI_RETURN_OK()

    CCI_RETURN_FAIL()

  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
#define IF_EXECCOMMAND( cmd ) \
    if(EQUAL_STRING(cmd_name,cmd))
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document , ExecCommand )
{
  CCI_INSTANCE(Manager)

  if(_This->manager!=NULL)
  {
    const char_t* cmd_name = CCI_P(command);

    IF_EXECCOMMAND("SetPriority") {
       dmString::E* stopc;
       _This->manager->SetPriority(_tcstoul(CCI_P(arg),&stopc,10));
       CCI_RETURN_OK()
    }
  }

  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
#define RETURN_EXECCOMMAND_SUPPORTED( cmd ) \
    if(EQUAL_STRING(cmd_name,cmd)) CCI_RETURN_OK();
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document , IsCommandSupported )
{
  CCI_UNUSUED_INSTANCE(Manager)

  const char_t* cmd_name = CCI_P(command);

  RETURN_EXECCOMMAND_SUPPORTED("SetPriority");

  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Manager )
  CCI_REGISTER_METHOD_PP( Document, OnCommand          )
  CCI_REGISTER_METHOD_PP( Document, OnCmdUpdate        )
  CCI_REGISTER_METHOD_PP( Document, GetManager         )
  CCI_REGISTER_METHOD_PP( Document, GetSize            )
  CCI_REGISTER_METHOD_PP( Document, Draw               )
  CCI_REGISTER_METHOD_NP( Document, Update             )
  CCI_REGISTER_METHOD_PP( Manager , GetToolByName      )
  CCI_REGISTER_METHOD_PP( Manager , Clear              )
  CCI_REGISTER_METHOD_PP( Manager , GetCurrentTool     )
  CCI_REGISTER_METHOD_PP( Manager , SetFocus           )
  CCI_REGISTER_METHOD_PP( Manager , Show               )
  CCI_REGISTER_METHOD_PP( Manager , CreateTool         )
  CCI_REGISTER_METHOD_PP( Manager , AbortEdit          )
  CCI_REGISTER_METHOD_PP( Document, OpenDocument       )
  CCI_REGISTER_METHOD_PP( Document, SaveDocument       )
  CCI_REGISTER_METHOD_PP( Document, SetDisplay         )
  CCI_REGISTER_METHOD_PP( Manager , Create             )
  CCI_REGISTER_METHOD_PP( Manager , FromInfo           )
  CCI_REGISTER_METHOD_NP( Manager , ReleaseMngr        )
  CCI_REGISTER_METHOD_PP( Manager , AddListener        )
  CCI_REGISTER_METHOD_PP( Manager , RemoveListener     )
  CCI_REGISTER_METHOD_PP( Manager,  GetToolInfo        )
  CCI_REGISTER_METHOD_PP( Document, ExecCommand        )
  CCI_REGISTER_METHOD_PP( Document, IsCommandSupported )
CCI_NEXT_DISPATCH_MAP()

//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Manager )
{
  CCI_INVOKE_FACTORY(Manager)
}
//---------------------------------------------------------------------
