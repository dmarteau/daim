
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
#define dmUseToolPrefs
#define dmUseKernelBuffer
#define dmUseToolFactory
#include "daim_tools.h"
#include "daim_info.h"
#include "daim_manager.h"

#define GENERATE_NOTIFY_RESULTS_EVENT( aType, aResult )  \
  dmGENERATE_NOTIFY_RESULT_EVENT(this,aType,aResult)

#define NO_TOOL 0
/////////////////////////////////////////////////////////////////////////
#define daim_manager_SCHEMA_VERSION 1
dmImplementClassInfo( daim_manager, dmToolManager, daim_manager_SCHEMA_VERSION );
//-----------------------------------------------------------------------
daim_manager::daim_manager( dmTool* _parent )
: dmToolManager(_parent)
 ,mInfo(NULL)
{
  Init();
}
//-----------------------------------------------------------------------
daim_manager::daim_manager( daim_info* pInfo , dmTool* _parent )
: dmToolManager(_parent)
 ,mInfo(pInfo)
{ 
  Init();
}
//-----------------------------------------------------------------------
daim_manager::daim_manager( daim_info* pInfo, const dmRect& _r, dmTool* _parent )
: dmToolManager(_parent)
 ,mInfo(pInfo)
{ 
  SetRect(_r); 
  Init();
}
//-----------------------------------------------------------------------
daim_manager::~daim_manager() {}
//-----------------------------------------------------
dmImage* daim_manager::GetImage() const
{
  if(mInfo)
     return mInfo->GetImage();
   
  return NULL;
}
//-----------------------------------------------------
void daim_manager::Init()
{
  mNewTool       = NULL;
  mnCurrentTool  = NO_TOOL;
  mnPriority     = 0;
  mEditCaps      = 0;

  SetDrawStyle(dmGetDrawStyleClass(_TXT("*default")));

  LockPosition(true); 
}
//-----------------------------------------------------
void daim_manager::SetEditCaps( dm_uint caps )
{
  mEditCaps = caps;
}
//-----------------------------------------------------
void daim_manager::SetPriority( dm_uint pri )
{
   mnPriority = pri;
}
//-----------------------------------------------------------------------
void daim_manager::OnEditNewTool( dmTool* aNewTool, dm_uint nID  )
{
  AbortEdit(nID);
  if(aNewTool) {
    mNewTool = aNewTool;
    mNewTool->Show(true);//???
  }
}
//-----------------------------------------------------------------------
void daim_manager::OnInsertNewTool( dmTool* aNewTool )
{
  if(aNewTool) 
  {
    Add( aNewTool );        // On ajoute l'objet à la liste  
    BeginRefresh();
    SelectAll(false);
    aNewTool->Redraw();            // Display
    aNewTool->Select(true);        // Select
    aNewTool->SetFocus(true,NULL); // Set focus    
    EndRefresh();
  }
}
//-----------------------------------------------------------------------
dmTool* daim_manager::InsertTool( dmTool* aTool , bool bEdit, dm_uint nID )
{
  if(aTool)
  {
    if(!aTool->Parent())
        aTool->Attach(this);
        
    SetDefaultToolPrefs(aTool);
    
    if(bEdit)
       OnEditNewTool(aTool,nID);
    else 
    {
      // XXX 3/11/05 Ensure initialisation of the tools
      // This was missing when inserting non-edited tools
      // May be can find a better place ?
      if(!aTool->IsInitialized()) 
      {
        if(aTool->Initialize()) {
          dmGENERATE_EVENT(aTool,evUpdate,!aTool->BubbleCancelled()); 
        } else {
          dmLOG("WARNING: failed to initialize tool in <%s> manager\n",ToolName().CStr());
        }
      }
        
      OnInsertNewTool(aTool);
    }
  }  
  return aTool;
}
//---------------------------------------------------------------
void daim_manager::SetDefaultToolPrefs( dmTool* aTool )
{
  dmToolPrefsVisitor& _visitor = dmToolPrefsVisitor::GetPrefs(ToolName().CStr());        
  aTool->Accept(_visitor);
}
//-----------------------------------------------------
dmTool* daim_manager::LoadClassTool( dm_uint nID, 
                                     const dmString::E* classname, 
                                     const dmString::E* toolname, 
                                     const dmPoly* pts,
                                     bool  bEdit )
{
  if(EMPTY_STRING(classname))
  {
    switch(nID) 
    {
      case DM_TOOLID_LINECUTTER     :
      case DM_TOOLID_LINE           : classname = dmTOOL_LINE;           break;
      case DM_TOOLID_RECT           : classname = dmTOOL_RECT_EX;        break;
      case DM_TOOLID_ELLIPSE        : classname = dmTOOL_ELLIPSE;        break;
      case DM_TOOLID_POLYGONE       : classname = dmTOOL_POLYLINES;      break;
      case DM_TOOLID_POLYLINE       : classname = dmTOOL_POLY_EX;        break;
      case DM_TOOLID_CIRCLE         : classname = dmTOOL_CIRCLE;         break;
      case DM_TOOLID_CIRCLE3        : classname = dmTOOL_CIRCLE_EX;      break;
      case DM_TOOLID_FREECUTTER     :
      case DM_TOOLID_OPENFREEHAND   : classname = dmTOOL_OPENFREEHAND;   break;
      case DM_TOOLID_CLOSEDFREEHAND : classname = dmTOOL_CLOSEDFREEHAND; break;
    }
  }
  
  dmTool* pTool = dmCreateTool(classname,pts);
  if(pTool) {  
    if(toolname) pTool->SetToolName(toolname);
    InsertTool(pTool,bEdit, nID );
  }
  return pTool;
}
//-----------------------------------------------------------------------
void daim_manager::AbortEdit( dm_uint nID )
{
  if(mNewTool) {
    delete mNewTool;
    mNewTool = NULL;
    Redraw();
  }

  // Generate event only if Id is changed
  if(mnCurrentTool != nID) {
     mnCurrentTool  = nID;
     GENERATE_NOTIFY_RESULTS_EVENT( evAbort, nID );
  }
}
//-----------------------------------------------------------------------
// dmToolManager overrides
//-----------------------------------------------------------------------
void daim_manager::OnDisplay( dmGraphics& gr ) const
{
  dmToolManager::OnDisplay(gr);
}
//------------------------------------------------------
void daim_manager::OnMove(int dx,int dy,bool _update)
{
  //XXX why a manager should move !!!
  //NOOP
}
//-----------------------------------------------------
bool daim_manager::OnFocus( bool _focus )
{
  if(!_focus) AbortEdit(mnCurrentTool);
  else        Show(true);

  return dmToolManager::OnFocus(_focus);
}
//-----------------------------------------------------
bool daim_manager::HandleNewToolEvent( dmEvent& evt )
{
  if(mNewTool) 
  { 
    bool retval = mNewTool->ProcessEvent(evt);
    if(mNewTool->IsInitialized()) {
       OnInsertNewTool(mNewTool);
       mNewTool = NULL;
       AbortEdit(NO_TOOL);
    }
    return retval;
  }
  return false;
}
//-----------------------------------------------------
bool daim_manager::ProcessMouseEvent( dmMouseEvent& evt )
{
  if(HandleNewToolEvent(evt))
    return true;
/*
  // If we dont have any Edit capabilities, then just query the default tool
  if(mEditCaps==0) 
  {
     dmTool* currentTool =  GetCurrentTool();
     switch(evt.mEventType) 
     {
       case evMouseDown : 
         if(currentTool==NULL) {
           // First find if we have a tool under the cursor position
           dmTool* tool = GetObject(evt.mCursor,true);
           if(tool) tool->SetFocus(true,currentTool);
         }

         // Fetch current tool again
         currentTool = GetCurrentTool();
         if(currentTool) {
           return currentTool->ProcessEvent(evt);
         }
         break; // evMouseDown
     }
  }
*/
  return  dmToolManager::ProcessMouseEvent(evt);
     
}
//----------------------------------------------------------------------------
bool daim_manager::ProcessKeyEvent( dmKeyEvent& evt  )
{
  if(HandleNewToolEvent(evt))
    return true;

  // Same as ProcessMouseEvent
/*
  if(mEditCaps==0) 
  {
     dmTool* currentTool =  GetCurrentTool();
     if(currentTool)
       return currentTool->ProcessEvent(evt);
  }
*/ 
  if(!dmToolManager::ProcessKeyEvent(evt) && 
           evt.mEventType == evKeyDown)
  {
    switch(evt.mKeyCode) 
    {
     // let OnEditDelete() to do the job
     // case DM_VK_DELETE : OnEditDelete(); return true;

      case DM_VK_UP     : MoveSelection( 0,-1, true ); return true; 
      case DM_VK_DOWN   : MoveSelection( 0, 1, true ); return true;
      case DM_VK_RIGHT  : MoveSelection( 1, 0, true ); return true;
      case DM_VK_LEFT   : MoveSelection(-1, 0, true ); return true;

      case DM_VK_ESCAPE : OnEscape(); return true;
    } 
  }
  return  false; // not handled
}
//-----------------------------------------------------------------------   
// Commands
//-----------------------------------------------------
void daim_manager::OnEscape()
{
  SelectAll(false);
  AbortEdit(NO_TOOL);
}
//-------------------------------------------------------
void daim_manager::OnEditDelete() 
{
  ClearSelection();
}
//-------------------------------------------------------
void daim_manager::OnUpdateEditDelete(daim_CmdUI* pCmdUI) 
{	
  pCmdUI->Enable( SelectCount() >= 1 );
}
//-------------------------------------------------------
void daim_manager::OnSelectAll()
{
  if(mEditCaps!=0)
    SelectAll(true);
} 
//-------------------------------------------------------
void daim_manager::OnUpdateSelectAll(daim_CmdUI* pCmdUI)
{
  pCmdUI->Enable( mEditCaps !=0 );
}
//-------------------------------------------------------
// Edit commands
//-------------------------------------------------------
void daim_manager::OnEditCopy()   { CopySelection (mClipBuffer); }
void daim_manager::OnEditPaste()  { PasteSelection(mClipBuffer); }
void daim_manager::OnEditCut()    { CutSelection  (mClipBuffer); }
//-----------------------------------------------------
void daim_manager::OnUpdateEditPaste(daim_CmdUI* pCmdUI) 
{ 
  pCmdUI->Enable( mClipBuffer.Count()>0 ); 
} 
//-----------------------------------------------------
void daim_manager::OnMvforward()  { SelectionForward(); }
void daim_manager::OnMvbackward() { SelectionBackward();}
void daim_manager::OnMvback()     { SelectionToBack();  }
void daim_manager::OnMvfront()    { SelectionToFront(); }
//-----------------------------------------------------
void daim_manager::OnUpdateEditCommand( daim_CmdUI* pCmdUI, dm_uint nCmd )
{
  bool bCheck = (nCmd & mEditCaps)!= 0;
  if(bCheck) bCheck = bCheck && (SelectCount()>0);

  if(bCheck && (nCmd & (DM_EDIT_CAPS_EditAlign|DM_EDIT_CAPS_EditGroup))) 
     pCmdUI->Enable(bCheck && MultiSelection());
}
//-----------------------------------------------------
void daim_manager::OnUpdateEditOrder(daim_CmdUI* pCmdUI) { OnUpdateEditCommand(pCmdUI,DM_EDIT_CAPS_EditOrder);  }
void daim_manager::OnUpdateEditAlign(daim_CmdUI* pCmdUI) { OnUpdateEditCommand(pCmdUI,DM_EDIT_CAPS_EditAlign);  }
void daim_manager::OnUpdateEditCopy (daim_CmdUI* pCmdUI) { OnUpdateEditCommand(pCmdUI,DM_EDIT_CAPS_EditCopy);   }
//-----------------------------------------------------
void daim_manager::OnLeftalign()    { AlignSelection( eAlignStyle_left    ); }
void daim_manager::OnRightalign()   { AlignSelection( eAlignStyle_right   ); }
void daim_manager::OnHcenteralign() { AlignSelection( eAlignStyle_center  ); }
void daim_manager::OnTopalign()     { AlignSelection( eAlignStyle_top     ); }
void daim_manager::OnBottomalign()  { AlignSelection( eAlignStyle_bottom  ); }
void daim_manager::OnVcenteralign() { AlignSelection( eAlignStyle_vcenter ); }
//-----------------------------------------------------
bool daim_manager::OnCommand( dm_uint nCode )
{
  if(nCode==cmdEscape) {
    OnEscape();
    return true;
  }
  return daim_CmdTarget::OnCommand(nCode);
}
//-----------------------------------------------------
BEGIN_COMMAND_MAP(daim_manager, daim_CmdTarget)

  REG_COMMAND_UPDATE(daim_manager,  EditDelete )
  REG_COMMAND_UPDATE(daim_manager,  SelectAll  )

  REG_COMMAND(daim_manager,  Mvforward  ,OnUpdateEditOrder)
  REG_COMMAND(daim_manager,  Mvbackward ,OnUpdateEditOrder)
  REG_COMMAND(daim_manager,  Mvback     ,OnUpdateEditOrder)
  REG_COMMAND(daim_manager,  Mvfront    ,OnUpdateEditOrder)
	
  REG_COMMAND(daim_manager,  Hcenteralign ,OnUpdateEditAlign)
  REG_COMMAND(daim_manager,  Vcenteralign ,OnUpdateEditAlign)
  REG_COMMAND(daim_manager,  Leftalign    ,OnUpdateEditAlign)
  REG_COMMAND(daim_manager,  Topalign     ,OnUpdateEditAlign)
  REG_COMMAND(daim_manager,  Rightalign   ,OnUpdateEditAlign)
  REG_COMMAND(daim_manager,  Bottomalign  ,OnUpdateEditAlign)

  REG_COMMAND(daim_manager,  EditCopy, OnUpdateEditCopy) 
  REG_COMMAND(daim_manager,  EditCut , OnUpdateEditCopy)
  
  REG_COMMAND_UPDATE(daim_manager,  EditPaste ) 

END_COMMAND_MAP()
//--------------------------------------------------------------
