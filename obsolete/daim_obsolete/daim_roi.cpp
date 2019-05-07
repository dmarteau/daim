
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

#define dmUseKernelImageTemplates
#define dmUseKernelBuffer
#define dmUseToolManager
#define dmUseToolPrefs
#define dmUseToolFactory
#include "daim_tools.h"
#include "daim_info.h"
#include "daim_manager.h"

#include "daim/daim_messages.h"

#include "dmStyleManager.h"
#include "dmRegionEdit.h"

#include "daim_kernel/operators/dmOperators.h"

// VK_OEM_102 (key '<' )

#define ADD_RGN_KEY() DM_GetKeyState('V')
#define SUB_RGN_KEY() DM_GetKeyState('X')

#define IF_ADD_REGION() (ADD_RGN_KEY()<0)
#define IF_SUB_REGION() (SUB_RGN_KEY()<0)

#define IF_MERGE_RGN()     ((ADD_RGN_KEY()< 0) || (SUB_RGN_KEY()<0 ))
#define IF_NOT_MERGE_RGN() ((ADD_RGN_KEY()>=0) && (SUB_RGN_KEY()>=0))

#define NO_TOOL 0

#define EDIT_CROP_TOOL()   (GetCurrentToolId() >= DM_TOOLID_LINECUTTER)

//-------------------------------------------------------
class daim_roi : public daim_manager
{
 public:
    daim_roi( dmTool* _parent = NULL )             : daim_manager(_parent)       { Init(); }
    daim_roi( daim_info* _info, dmTool* _parent )  : daim_manager(_info,_parent) { Init(); }
    daim_roi( daim_info* _info, const dmRect& _r, dmTool* _parent = NULL )
    : daim_manager(_info,_r,_parent) {
         Init(); 
     }
  
    virtual ~daim_roi();

  protected:
   int      mMergeMode;
   dmTool*  mpCurrentSel;
       
   void Init();

   virtual void OnEditNewTool  ( dmTool*, dm_uint nID=0 );
   virtual void OnInsertNewTool( dmTool* );

   void ClearCurrentSel();
   bool DeleteMask(bool bExclude, dmTool*  = NULL );
   void DeleteSelection();

   ECursorType GetCursorType(ECursorType _Default);

  public:
   virtual bool OnFocus( bool _select );
   virtual bool InitialUpdate();

   virtual bool OnCommand ( dm_uint nCode );
   virtual bool OnCommandUpdate ( daim_CmdUI*  );

   virtual void AbortEdit( dm_uint nID=0 );

  protected:
    virtual bool ProcessMouseEvent   ( dmMouseEvent& );
    virtual bool ProcessKeyEvent     ( dmKeyEvent&   );
    virtual bool ProcessBubblingEvent( dmEvent&      );

  protected:
   __command void OnEscape();

   __command void OnEditCopy();
   __command void OnUpdateEditCopy(daim_CmdUI*); 
   __command void OnEditCut();
   __command void OnUpdateEditCut(daim_CmdUI*); 
   __command void OnEditPaste();
   __command void OnUpdateEditPaste(daim_CmdUI*); 

   __command void OnGetROI();
   __command void OnShowROI();
   __command void OnKillROI();

   DECLARE_COMMAND_MAP()
};
//-----------------------------------------------------
// Members
//-----------------------------------------------------
daim_roi::~daim_roi()
{
 if(mpCurrentSel && mpCurrentSel->Release()==0) 
   delete mpCurrentSel; 
}
//-----------------------------------------------------
bool daim_roi::InitialUpdate()
{
  if(daim_manager::InitialUpdate()) {
    SetToolName(DM_ROI);
    return true;
  }
  return false;
}
//-----------------------------------------------------
bool daim_roi::OnFocus( bool _focus )
{
  if(mpCurrentSel) {
    mpCurrentSel->Select(_focus);
    mpCurrentSel->SetFocus(_focus,NULL);
  }
  if(!_focus) AbortEdit(DM_TOOLID_NONE);  // Abort edition if kill focus
  return daim_manager::OnFocus(_focus);
}
//-----------------------------------------------------
void daim_roi::Init()
{
  SetDrawStyle(dmGetDrawStyleClass(_TXT("*roi")));
  //SetDefaultCursor(eCursor_crosshair);

  mpCurrentSel = NULL;
  mMergeMode   = dmRegionEdit::rgnKILL;

  // Init style for crop tool 

  dmGetServices<dmStyleManager> _StyleSrvc;

  dmLink<dmDrawStyle> _CropStyle = _StyleSrvc->GetStyle(_TXT("*crop"),false);
  if(_CropStyle.IsNull())
  {
    dmTextAttributs _DfltText;
    dmPenAttributs  _PenAttrs(eClr_WHITE,eClr_BLACK,0,ePenStyle_solid );

    _StyleSrvc->SetStyle(
      dmDrawStyle(_TXT("*crop") ,_PenAttrs,_DfltText),true);
  }
}
//-----------------------------------------------------
void daim_roi::ClearCurrentSel()
{
  if(mpCurrentSel) {
    mpCurrentSel->Show(false);
    mpCurrentSel->Detach();
    if(mpCurrentSel->Release()==0)
      delete mpCurrentSel;

    mpCurrentSel = NULL;
  }
}
//-----------------------------------------------------
void daim_roi::OnEditNewTool( dmTool* aTool, dm_uint nID )
{
  if(nID >= DM_TOOLID_LINECUTTER) 
  {
    dmPenAttributs& _LinePen = dmGetDrawStylePen(_TXT("*crop"));
    const_cast<dmShape*>(aTool->Carrets())->SetLineAttrs(&_LinePen);
  }

  mMergeMode = dmRegionEdit::rgnKILL;
  daim_manager::OnEditNewTool(aTool,nID);
}
//-----------------------------------------------------
void daim_roi::OnInsertNewTool( dmTool* aTool )
{ 
  //================================
  // Handle cutter operation on mask
  //================================

  if(EDIT_CROP_TOOL()) 
  {
    DeleteMask(true,aTool);
    AbortEdit(DM_TOOLID_NONE);
    aTool = NULL;       
    // Keep current selection   
  }

  //==========================================
  // Handle ROI merging with current selection
  //==========================================

  if(aTool && mpCurrentSel && mMergeMode!=dmRegionEdit::rgnKILL) {
    dmRegionEdit* pRegion = new dmRegionEdit( mpCurrentSel->Region(),this);

    SetDefaultToolPrefs(pRegion);

    pRegion->MergeRoi(aTool->Region(), (dmRegionEdit::MergeMode)mMergeMode );               
    AbortEdit(DM_TOOLID_NONE);           // clear current object;
    ClearCurrentSel();
    if(!pRegion->Region().IsEmptyRoi())
    {
      pRegion->Show(true);
      pRegion->Initialize();
      aTool = pRegion;
    } else {
      delete pRegion;
      aTool = NULL;
    }
  }

  //================================
  // Insert tool
  //================================

  if(aTool) {
    if(mpCurrentSel) ClearCurrentSel();
    mpCurrentSel = aTool;
    mpCurrentSel->Obtain();
    daim_manager::OnInsertNewTool(aTool);
  }  
}
//-------------------------------------------------------
void daim_roi::DeleteSelection() 
{
  BeginRefresh();
  ClearSelection();
  ClearCurrentSel();
  EndRefresh();
}
//-------------------------------------------------------
// Ajoute/retranche la roi au masque courant
//-------------------------------------------------------
bool daim_roi::DeleteMask(bool bExclude, dmTool* pTool )
{
  dmTool* pSel = pTool ? pTool : mpCurrentSel;
  if(mInfo && pSel) {
    dmRegion mask;
    if(!mInfo->GetMask().IsEmptyRoi()) {
       const dmRegion& _roi = pSel->Region();
       if(bExclude) mInfo->SetMask(_roi,dmTk::Math::SubPixels);
       else         mInfo->SetMask(_roi,dmTk::Math::AndPixels);
       return true;
    }   
  }
  return false;
}
//----------------------------------------------------------------------------
ECursorType daim_roi::GetCursorType(ECursorType _Default)
{
  ECursorType _Cursor = _Default;

  if(mNewTool) {
    if (EDIT_CROP_TOOL()) _Cursor = eDMCursor_crop; else
    if (IF_SUB_REGION())  _Cursor = eDMCursor_roi_minus; else
    if (IF_ADD_REGION())  _Cursor = eDMCursor_roi_plus;  else
    _Cursor = eDMCursor_roi; 
  } 

  return _Cursor;
}
//----------------------------------------------------------------------------
bool daim_roi::ProcessKeyEvent( dmKeyEvent& anEvent  )
{
  switch(anEvent.mEventType)
  {
    case evKeyDown:
      if(!mNewTool) {
        switch(anEvent.mKeyCode) {
          case DM_VK_DELETE : if(!DeleteMask(true))  DM_MessageBeep(0); return true;
          case DM_VK_RETURN : if(!DeleteMask(false)) DM_MessageBeep(0); return true;
          case DM_VK_ESCAPE : 
            ClearCurrentSel();         // only current editing will be discarded
            AbortEdit(DM_TOOLID_NONE); // Abort editing, and send a message
            return true;
        }
      } else {
        switch(anEvent.mKeyCode) {
          case DM_VK_ESCAPE : AbortEdit(DM_TOOLID_NONE); return true;
        }
      }
      break;

    default:
      break;
  }

  SetDefaultCursor(anEvent.mCursorType = GetCursorType(anEvent.mCursorType));

  return daim_manager::ProcessKeyEvent(anEvent);
}
//-----------------------------------------------------
// - si '<' est enfoncée alors conserve le tool courant
//          sinon on le détruit
//-----------------------------------------------------
bool daim_roi::ProcessMouseEvent( dmMouseEvent& anEvent )
{
  switch(anEvent.mEventType) 
  {
    case evMouseDown: 

     if(mNewTool && mpCurrentSel) { // Creating a new ROI
       if(IF_NOT_MERGE_RGN() && !EDIT_CROP_TOOL())    
         ClearCurrentSel();          // Destroy current selection
         // Else we keep the current selection
     } 
     anEvent.mCursorType = GetCursorType(anEvent.mCursorType);
     break;
     //return daim_manager::ProcessMouseEvent(anEvent);

    case evMouseMove:
      SetDefaultCursor(anEvent.mCursorType = GetCursorType(anEvent.mCursorType));
      break;

    //-----------------------------------------------------
    // - On regarde de nouveau l'état de la touche '<'
    //   si enfoncée alors on ajoute ou on soustrait
    //   la région à la région du tool précedent
    //   sinon on détruit le tool précédent et on insère le nouveau
    //-----------------------------------------------------
    case evMouseUp: 
    {
      if(mNewTool) 
      {
        int  nCurrentTool     = GetCurrentToolId();
        bool bContinueEditing = (mNewTool && (IF_MERGE_RGN() || EDIT_CROP_TOOL()));
      
        dmClassInfo* pInfo = NULL;

        if(bContinueEditing) 
        {
          if(mpCurrentSel && !EDIT_CROP_TOOL()) {
             mMergeMode = (IF_SUB_REGION()?dmRegionEdit::rgnSUB:dmRegionEdit::rgnADD);
          }
          pInfo = mNewTool->ObjectInfo();
        } else
          ClearCurrentSel();

        // Insert/Merge new tool
        bool bstatus = daim_manager::ProcessMouseEvent(anEvent);
        if(pInfo && !mNewTool && nCurrentTool!=DM_TOOLID_NONE)
        { 
           SetDefaultCursor(anEvent.mCursorType = GetCursorType(anEvent.mCursorType));

           dmTool* pTool = dmTool::Cast(pInfo->Create());
           if(pTool) {
             InsertTool(pTool,true,nCurrentTool);
             return true; 
           } else {
             delete pTool;
             return false;
           }
        }
        return bstatus;
      }
    } break; 
    
    default: break;    
  }

  // default

  return daim_manager::ProcessMouseEvent(anEvent);
}
//-----------------------------------------------------
bool daim_roi::ProcessBubblingEvent( dmEvent& anEvent )
{
  //dmTool* _target = anEvent.mTarget;

  if(anEvent.mEventType==evDispose && 
     anEvent.mTarget==mpCurrentSel)
  {
    ClearCurrentSel();
    return true;
  }

  return daim_manager::ProcessBubblingEvent(anEvent);
}
//-----------------------------------------------------
// Commands
//-----------------------------------------------------
void daim_roi::OnEscape()
{
  BeginRefresh();
  daim_manager::OnEscape();
  DeleteSelection();
  EndRefresh();
}
//-----------------------------------------------------
void daim_roi::OnEditCopy() 
{
  if(mInfo)
  {
    if( mpCurrentSel != NULL ) {
      if(!mInfo->SaveRegion("clipboard://",mpCurrentSel->Region()))
        dmLOG("Copy() failed !");  
    } else
      if(!mInfo->SaveDocument("clipboard://",false))
        dmLOG("Copy() failed !");           
  }
   
}
//-------------------------------------------------------
void daim_roi::OnEditCut()   
{
  // Nothing to do
}
//-------------------------------------------------------
void daim_roi::OnEditPaste()   
{
  // Nothing to do
}
//-----------------------------------------------------
void daim_roi::OnUpdateEditCopy(daim_CmdUI* pCmdUI) 
{ 
  pCmdUI->Enable( mInfo && mInfo->IsValidView() ); 
} 
//-----------------------------------------------------
void daim_roi::OnUpdateEditCut(daim_CmdUI* pCmdUI) 
{ 
  pCmdUI->Enable( mInfo && mInfo->IsValidView() ); 
} 
//-----------------------------------------------------
void daim_roi::OnUpdateEditPaste(daim_CmdUI* pCmdUI) 
{ 
  pCmdUI->Enable( false ); 
} 
//--------------------------------------------------------------------------------------
// ROI Cache Manager
//--------------------------------------------------------------------------------------
void daim_roi::OnGetROI()
{
  if(mInfo) {
    if(mpCurrentSel) 
       mInfo->mROI = mpCurrentSel->Region();
    else
       mInfo->mROI.KillRoi();
  }   
}
//--------------------------------------------------------------------------------------
void daim_roi::OnKillROI()
{
  ClearCurrentSel();
}
//--------------------------------------------------------------------------------------
// Copy roi from info
//--------------------------------------------------------------------------------------
void daim_roi::OnShowROI()
{
  if(mInfo) 
  {
    const dmRegion& roi = mInfo->mROI;
    if(!roi.IsEmptyRoi()) 
    {
      if(!dmRegionEdit::Cast(mpCurrentSel)) {
        ClearCurrentSel();
        dmRegionEdit* pRegion =  new dmRegionEdit(roi, this);
        pRegion->Show(true);
        pRegion->Initialize();
        mMergeMode = dmRegionEdit::rgnKILL;
        OnInsertNewTool(pRegion);
      } else { 
        ((dmRegionEdit*)mpCurrentSel)->MergeRoi(roi,dmRegionEdit::rgnCOPY); 
      }
      return;
    }  
  }
  ClearCurrentSel();
}
//-----------------------------------------------------------------------
void daim_roi::AbortEdit( dm_uint nID )
{
  daim_manager::AbortEdit(nID);
  SetDefaultCursor(GetCursorType(eCursor_undefined));
}
//--------------------------------------------------------------------------------------
bool daim_roi::OnCommand ( dm_uint nCode )
{
  switch(nCode) {
    case cmdKillROI : OnKillROI() ; return true;
    case cmdGetROI  : OnGetROI()  ; return true;
    case cmdShowROI : OnShowROI() ; return true;
  }
  return daim_manager::OnCommand(nCode);
}
//--------------------------------------------------------------------------------------
bool daim_roi::OnCommandUpdate ( daim_CmdUI* pCmdUI )
{
  switch(pCmdUI->nCode) {
     case cmdEditDelete : pCmdUI->Enable(false); return true;
  }

  return daim_manager::OnCommandUpdate(pCmdUI);
}
//-----------------------------------------------------
BEGIN_COMMAND_MAP( daim_roi , daim_manager)

  REG_COMMAND_UPDATE(daim_roi, EditCopy   ) 
  REG_COMMAND_UPDATE(daim_roi, EditCut    ) 
  REG_COMMAND_UPDATE(daim_roi, EditPaste  ) 

END_COMMAND_MAP()
//-----------------------------------------------------
// Create ROI manager
//-----------------------------------------------------
daim_manager* DAIM_Create_ROI( daim_info* info, const dmRect& r, dmTool* parent )
{
  return new daim_roi(info,r,parent);
}
//-----------------------------------------------------
#endif // NO_DAIM_USER_INTERFACE
