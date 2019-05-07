
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
#include "daim_modules/CCI_ToolInfo.h"
#include "daim_modules/CCI_EventListener.h"
#include "daim_modules/CCI_Interface.h"

//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_ToolInfo
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_ToolInfo_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2005"))
//--------------------------------------------------------------------
#define CCI_INTERNAL_MODULE

#include "daim_modules/CCI_Base.h"

//--------------------------------------------------------------------
CCI_DECL_DATA( ToolInfo )
{
  dmTool* mTool;
};

//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( ToolInfo )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE(ToolInfo)

  _This->mTool  = NULL;  

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( ToolInfo )
{
  CCI_INSTANCE(ToolInfo)

  if(_This->mTool && _This->mTool->Release()==0)
    delete _This->mTool;

  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( ToolInfo )
  CCI_ATTR_GET(ToolInfo, Target  , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->mTool) )
  CCI_ATTR_GET(ToolInfo, ToolKey , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->mTool) )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( ToolInfo )
  CCIA_DISCARD( ToolInfo, Target  )
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method: Assign
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolInfo , Assign )
{
  CCI_INSTANCE(ToolInfo)

  if(_This->mTool != CCI_P(tool))
  {
    // Release previous tool

    if(_This->mTool && _This->mTool->Release()==0)
       delete _This->mTool;

     _This->mTool = CCI_P(tool);

     if(_This->mTool!=NULL) 
       _This->mTool->Obtain();

     CCI_Set(_THIS_OBJECT,CCIA_DECL_(ToolInfo,ToolKey),
              dm_ptr_to_long(_This->mTool));

     CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Clear
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( ToolInfo , Clear )
{
  CCI_INSTANCE(ToolInfo)

  if(_This->mTool && _This->mTool->Release()==0)
      delete _This->mTool;

  _This->mTool = NULL;
  CCI_Set(_THIS_OBJECT,CCIA_DECL_(ToolInfo,ToolKey),0L);

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: AddListener
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolInfo , AddListener )
{
  CCI_INSTANCE(ToolInfo)

  if(_This->mTool)
  { 
    dmIEventListener _Listener;
    if(_Listener.QueryInterface(CCI_P(listener)) &&
       _Listener.AddListener(_This->mTool,CCI_P(evtMask),CCI_P(flags))) {
  
       CCI_RETURN_OK()
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: RemoveListener
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolInfo , RemoveListener )
{
  CCI_INSTANCE(ToolInfo)
  if(_This->mTool)
  { 
    dmIEventListener _Listener;
    if(_Listener.QueryInterface(CCI_P(listener)) &&
       _Listener.RemoveListener(_This->mTool,CCI_P(evtMask),CCI_P(flags))) {
  
       CCI_RETURN_OK()
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetPos
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolInfo, GetPos )
{
  CCI_INSTANCE(ToolInfo)

  dmTool* tool = _This->mTool;
  if(tool)
  {
    const  dmShape* carrets = tool->Carrets();
    if(carrets) 
    {
      dm_uint index  = CCI_P(index);
      if(index<carrets->Size()) 
      {
        CCI_RETVAL_P(x) = (*carrets)[index].x;
        CCI_RETVAL_P(y) = (*carrets)[index].y;

        CCI_RETURN_OK()
      }
      CCI_RETURN_INVALID_PARAM()
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolInfo , Rename )
{
  CCI_INSTANCE(ToolInfo)

  dmTool* tool = _This->mTool;
  if(tool)
  {
    tool->SetToolName(CCI_P(newname));
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolInfo , MoveTo  )
{
  CCI_INSTANCE(ToolInfo)

  dmTool* tool = _This->mTool;
  if(tool)
  {
    if(CCI_P(index) == -1 ) 
      tool->MoveTo(dmPoint(CCI_P(x),CCI_P(y)));
    else
      tool->SizeTo(CCI_P(index),dmPoint(CCI_P(x),CCI_P(y)));
      
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}

//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolInfo , MoveBy )
{
  CCI_INSTANCE(ToolInfo)

  dmTool* tool = _This->mTool;
  if(tool)
  {
    if(CCI_P(index) == -1 ) 
      tool->MoveBy(CCI_P(x),CCI_P(y));
    else
      tool->SizeBy(CCI_P(index),CCI_P(x),CCI_P(y));
      
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolInfo , Rectangle )
{
  CCI_INSTANCE(ToolInfo)

  dmTool* tool = _This->mTool;
  if(tool) {
    const dmRect& rect = tool->ObjectRect();
    if(!rect.IsEmpty()) 
    {
      CCI_RETVAL_P(left)   = rect.Left();
      CCI_RETVAL_P(top)    = rect.Top();
      CCI_RETVAL_P(right)  = rect.Right();
      CCI_RETVAL_P(bottom) = rect.Bottom();
      
      CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ToolInfo , ToolState )
{
  CCI_INSTANCE(ToolInfo)

  dmTool* tool = _This->mTool;
  if(tool) 
  {
    bool status = CCI_P(status)!=dm_false;

    switch(CCI_P(me)) 
    {
      case CCI_ME_SET:
        switch(CCI_P(state)) 
        {
           case CCIV_ToolInfo_Visible : tool->Show(status)         ; break;
           case CCIV_ToolInfo_Select  : tool->Select(status)       ; break;
           case CCIV_ToolInfo_Lock    : tool->LockPosition(status) ; break;
           case CCIV_ToolInfo_Focus   : tool->SetFocus(status,NULL); break;
           default:
             CCI_RETURN_INVALID_PARAM()
        }
        break;

       case CCI_ME_GET:
        switch(CCI_P(state)) 
        {
           case CCIV_ToolInfo_Visible  : CCI_RETVAL_P(status) = _BOOL(tool->IsVisible());       break;
           case CCIV_ToolInfo_Select   : CCI_RETVAL_P(status) = _BOOL(tool->IsSelected());      break;
           case CCIV_ToolInfo_Lock     : CCI_RETVAL_P(status) = _BOOL(tool->IsPositionnable()); break;
           case CCIV_ToolInfo_Focus    : CCI_RETVAL_P(status) = _BOOL(tool->HasFocus());        break;
           case CCIV_ToolInfo_NullObj  : CCI_RETVAL_P(status) = _BOOL(tool->IsNullObject());        break;
           default:
             CCI_RETURN_INVALID_PARAM()
        }
        break;

      default:
        CCI_RETURN_INVALID_PARAM()
    }
      
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( ToolInfo , Remove )
{
  CCI_INSTANCE(ToolInfo)

  dmTool* tool = _This->mTool;
  if(tool) 
  {
    tool->Obtain();

    // First Send a dispose event
    if(!dmGENERATE_EVENT(tool,evDispose,true)) 
    {
       // Event is not handled: need to do the job ourself !
 
       // If this object is associate to a manager, then we can remove it safely
      dmTool* _parent = tool->Parent();
      while(_parent)
      { 
        dmToolManager* manager = dmToolManager::Cast(_parent);
        if(manager) 
        {
          manager->SafeRemove(tool);
          break;
        }
      }
      _parent = _parent->Parent();
    }

    if(tool->Release()==0) 
    {
      delete tool;

      _This->mTool = NULL;
      CCI_Set(_THIS_OBJECT,CCIA_DECL_(ToolInfo,ToolKey),0L);
    }

    CCI_RETURN_OK()

  } 

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( ToolInfo , Update )
{
  CCI_INSTANCE(ToolInfo)

  dmTool* tool = _This->mTool;
  if(tool) {
     tool->UpdateObject();
  }
  // Always return OK, 
  // there is no point to return failure
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolInfo , GetClass )
{
  CCI_INSTANCE(ToolInfo)

  dmTool* tool = _This->mTool;
  if(tool) {
    CCI_RETVAL_P(classname) = tool->ObjectKey();
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolInfo , ExecCommand )
{
  CCI_UNUSUED_INSTANCE(ToolInfo)

  if(!EMPTY_STRING(CCI_P(command)))
     dmLOG("ToolInfo:Unknow command %s\n",CCI_P(command));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolInfo , IsCommandSupported )
{
  CCI_UNUSUED_INSTANCE(ToolInfo)
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( ToolInfo )
  CCI_REGISTER_METHOD_PP( ToolInfo , GetPos             )
  CCI_REGISTER_METHOD_PP( ToolInfo , Rename             )
  CCI_REGISTER_METHOD_PP( ToolInfo , MoveTo             )
  CCI_REGISTER_METHOD_PP( ToolInfo , MoveBy             )
  CCI_REGISTER_METHOD_PP( ToolInfo , Rectangle          )
  CCI_REGISTER_METHOD_PP( ToolInfo , ToolState          )
  CCI_REGISTER_METHOD_NP( ToolInfo , Remove             )
  CCI_REGISTER_METHOD_NP( ToolInfo , Update             )
  CCI_REGISTER_METHOD_PP( ToolInfo , GetClass           )
  CCI_REGISTER_METHOD_PP( ToolInfo , AddListener        )
  CCI_REGISTER_METHOD_PP( ToolInfo , RemoveListener     )
  CCI_REGISTER_METHOD_PP( ToolInfo , Assign             )
  CCI_REGISTER_METHOD_NP( ToolInfo , Clear              )
  CCI_REGISTER_METHOD_PP( ToolInfo , ExecCommand        )
  CCI_REGISTER_METHOD_PP( ToolInfo , IsCommandSupported )
CCI_NEXT_DISPATCH_MAP()

//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( ToolInfo )
{   
  CCI_INVOKE_FACTORY(ToolInfo)
}
//---------------------------------------------------------------------
