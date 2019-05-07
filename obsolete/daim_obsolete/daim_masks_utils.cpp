
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

#define dmUseKernelImageTemplates
#define dmUseKernelBuffer
#define dmUseToolManager
#define dmUseToolPrefs
#define dmUseToolFactory
#include "daim_tools.h"
#include "daim_info.h"
#include "daim_manager.h"

#include "daim_kernel/operators/dmOperators.h"
#include "templates/processing/dmObjectDetection.h"

#include <stack>

//-------------------------------------------------------
#define _MASK daim_masks::instance()
//---------------------------------------------------------------
class daim_ext_masks : public daim_manager
{
 public:
  std::stack<dmRegion> mMasks;
  dmRegion             mExtMask;
  bool                 mbActivated;

  dmLink<dmImage>      mMaskBuffer;
  std::vector<dmPoint> mPtsBuffer;

 protected: 
  void Initialize();

 public:
   daim_ext_masks( dmTool* _parent = NULL  );
   daim_ext_masks( daim_info* _info, dmTool* _parent );
   daim_ext_masks( daim_info* _info, const dmRect& _r, dmTool* _parent = NULL  );
   virtual ~daim_ext_masks();

   bool ReallocMaskBuffer( const dmRegion&, bool bSelect=true );
   bool SelectMask( const dmPoint& p, int status = -1  );
   void RefreshExtMask();

   virtual bool InitialUpdate();

   virtual bool OnCommand ( dm_uint nCode );
   virtual bool OnCommandUpdate ( daim_CmdUI*  );

   void DeleteSelection(bool);

 protected:
   virtual bool ProcessMouseEvent( dmMouseEvent& );
   virtual bool ProcessKeyEvent  ( dmKeyEvent&   );

   virtual bool OnFocus( bool _focus );

 protected:
   __command void OnMaskSelection();
   __command void OnUpdateMaskSelection(daim_CmdUI*);
   __command void OnUpdateMask(daim_CmdUI*);
   __command void OnCleanBorders();
   __command void OnFillHoles();
   __command void OnEnableMaskSelection();
   __command void OnDisableMaskSelection();
   __command void OnResetMaskSelection();

   DECLARE_COMMAND_MAP()
};
/////////////////////////////////////////////////////////////////
static unsigned char T_OBJECT = 255;
static unsigned char T_SELECT = 1;
//-----------------------------------------------------
daim_ext_masks::daim_ext_masks( dmTool* _parent )
: daim_manager(_parent)
{
  Initialize();
}
//-----------------------------------------------------
daim_ext_masks::daim_ext_masks( daim_info* _info, dmTool* _parent )
: daim_manager(_info,_parent)
{
  Initialize();  
}
//-----------------------------------------------------
daim_ext_masks::daim_ext_masks( daim_info* _info, const dmRect& _r, dmTool* _parent )
: daim_manager(_info,_r,_parent) 
{
  Initialize();
}
//-----------------------------------------------------
daim_ext_masks::~daim_ext_masks() 
{
}
//-----------------------------------------------------
void daim_ext_masks::Initialize()
{
  mPtsBuffer.resize(4096);
  mbActivated = false;
  //SetDefaultCursor(eDMCursor_masks);
}
//-----------------------------------------------------
bool daim_ext_masks::OnFocus( bool _focus )
{
  if(_focus)
     SetDefaultCursor(eDMCursor_masks);
    
  return daim_manager::OnFocus(_focus);
}
//---------------------------------------------------------------
void daim_ext_masks::OnMaskSelection()
{
  if(!mbActivated)  {
    mbActivated = true;
    ReallocMaskBuffer(mInfo->GetMask());
    SetFocus(true,NULL);
  } else {
    if(!HasFocus()) 
      SetFocus(true,NULL);
    else {
      SetFocus(false,NULL);
      mbActivated = false;
      mInfo->SetExtMask(mExtMask); // clear mask
      mMaskBuffer.Release();       // release buffer
      
      mInfo->Refresh(); // 3/01/06 
    }
  }
}
//----------------------------------------------------------------------------
void daim_ext_masks::OnUpdateMaskSelection( daim_CmdUI* pCmdUI )
{
  bool bIsValidMask = !mInfo->GetMask().IsEmptyRoi();
  pCmdUI->Enable(bIsValidMask);
  pCmdUI->SetCheck(bIsValidMask && mbActivated && HasFocus());
}
//----------------------------------------------------------------------------
void daim_ext_masks::OnUpdateMask( daim_CmdUI* pCmdUI )
{
  pCmdUI->Enable(!mInfo->GetMask().IsEmptyRoi());
}
//----------------------------------------------------------------------------
bool daim_ext_masks::ReallocMaskBuffer( const dmRegion& _mask, bool bSelect )
{
  if(!_mask.IsEmptyRoi()) {
    const dmRect& r = mInfo->GetImageRect();
    if(!mMaskBuffer.IsNull()) {
      if(r.Width()>mMaskBuffer->Width() || r.Height()>mMaskBuffer->Height())
        mMaskBuffer.Release();
    }

    if(mMaskBuffer.IsNull()) 
       mMaskBuffer = dmCreateImage<dmPixelFormat8bppIndexed>(r.Width(),r.Height());
    
    mMaskBuffer->ClearArea(mMaskBuffer->Rect());
    mMaskBuffer->FillArea(_mask);

    dmIImage<dmPixelFormat8bppIndexed>* _imgbuff = dmIImage<dmPixelFormat8bppIndexed>::Cast(mMaskBuffer);

    if(bSelect) {
      mExtMask = mInfo->GetExtMask();
      daim::fill(mExtMask,_imgbuff->Gen(),T_SELECT);
      mExtMask.KillRoi();
    }
    return true;
  }
  // On detruit le buffer 
  mMaskBuffer.Release();
  return false;
}
//----------------------------------------------------------------------------
// return vrai si l'objet a change d'etat
//----------------------------------------------------------------------------
bool daim_ext_masks::SelectMask( const dmPoint& p, int status )
{
  if(!mMaskBuffer.IsNull() && dmPointInRectangle(p,mMaskBuffer->Rect())) 
  {
    dmIImage<dmPixelFormat8bppIndexed>* _imgbuff = dmIImage<dmPixelFormat8bppIndexed>::Cast(mMaskBuffer);

    if(_imgbuff->Gen()[p] == T_OBJECT && status != 0) 
    {
      daim::sweep_algorithm::select_object(
          daim::connect4,p,_imgbuff->Gen(),
          T_OBJECT,T_SELECT,mPtsBuffer);      
    }
    else if(_imgbuff->Gen()[p] == T_SELECT && status != 1)
    {
      daim::sweep_algorithm::select_object(
        daim::connect4,p,_imgbuff->Gen(),
        T_SELECT,T_OBJECT,mPtsBuffer);
    } 
    else return false;

    // Construit la region correspondant a T_SELECT
    daim::create_roi(_imgbuff->Gen(),std::bind2nd(std::equal_to<dm_uint8>(),T_SELECT),mExtMask); 
    
    mInfo->SetExtMask( mExtMask );
    mExtMask.KillRoi();
    return true;
  } 
  return false;
}
//-----------------------------------------------------
void daim_ext_masks::RefreshExtMask()
{
  if(mInfo) {
    dmGraphics* gr = GetGraphics();
    if(gr)
       gr->Refresh(mInfo->GetExtMask().Rectangle());
  }
}
//-----------------------------------------------------
bool daim_ext_masks::InitialUpdate()
{
  if(daim_manager::InitialUpdate()) {
    SetToolName(DM_MASK);
    return true;
  }
  return false;
}
//-----------------------------------------------------
bool daim_ext_masks::ProcessMouseEvent( dmMouseEvent& evt )
{
  switch(evt.mEventType) 
  {
    case evMouseDown: 
     if(mbActivated) 
     {
       dmGraphics* gr = GetGraphics();
       if(gr) gr->BeginRefresh(mInfo->GetExtMask().Rectangle());
       SelectMask(evt.mCursor);
       if(gr) gr->EndRefresh(mInfo->GetExtMask().Rectangle());
       return true;
     } break;
     
     default: break;
  }
  return daim_manager::ProcessMouseEvent(evt);
}
//----------------------------------------------------------------------------
bool daim_ext_masks::ProcessKeyEvent( dmKeyEvent& evt )
{
  switch(evt.mKeyCode) 
  {
    case DM_VK_RETURN : DeleteSelection(false); return true;
    case DM_VK_DELETE : DeleteSelection(true) ; return true;
  }
  return daim_manager::ProcessKeyEvent(evt);
}
//-------------------------------------------------------
void daim_ext_masks::DeleteSelection(bool bExclude) 
{
  mExtMask = mInfo->GetExtMask();
  if(!mExtMask.IsEmptyRoi()) {
    if(bExclude)
     mInfo->SetMask(mExtMask,dmTk::Math::SubPixels);
    else 
     mInfo->SetMask(mExtMask);
  }
  ReallocMaskBuffer(mInfo->GetMask());
}
//----------------------------------------------------------------------------
void daim_ext_masks::OnFillHoles()
{
  const dmRegion& rgnmask = mInfo->GetMask();

  if(!rgnmask.IsEmptyRoi()) {
     dmRegion _rgn;

     mMasks.push(rgnmask);             // Save current mask 

     dmRect r = mInfo->GetImageRect();
     _rgn.SetRectRoi(r);
     _rgn.XorRoi(mMasks.top());        // Invert the mask 

     // Fill mask buffer with this mask:
     if(!ReallocMaskBuffer(_rgn,false)) return;
     mMasks.push(_rgn);                // Save the mask

     r.Resize(-1);
     _rgn.SubCoordinates(r);  // Get border

     // The following will create the region corresponding
     // to the area wich is not part of interior of objects

     dmIImage<dmPixelFormat8bppIndexed>* _imgbuff = dmIImage<dmPixelFormat8bppIndexed>::Cast(mMaskBuffer);
     daim::sweep_algorithm::select_objects( 
       daim::connect8,_rgn,_imgbuff->Gen(),
       T_OBJECT,T_SELECT,mPtsBuffer);

     // Build that Region
     daim::create_roi(_imgbuff->Gen(),std::bind2nd(std::equal_to<dm_uint8>(),T_SELECT),_rgn);

     _rgn.XorRoi(mMasks.top()); mMasks.pop(); // Create the complementary of the region (not including borders)
     _rgn.AddRoi(mMasks.top()); mMasks.pop(); // Add to the original region

     if(mInfo) 
       mInfo->SetMask(_rgn);
  }  
}
//----------------------------------------------------------------------------
void daim_ext_masks::OnCleanBorders()
{
  if(ReallocMaskBuffer(mInfo->GetMask(),false)) 
  {
     dmRegion _rgn;
     dmRect r = mMaskBuffer->Rect();
     _rgn.SetRectRoi(r);

     r.Resize(-1);
     _rgn.SubCoordinates(r);  // Recupere le contour

     dmIImage<dmPixelFormat8bppIndexed>* _imgbuff = dmIImage<dmPixelFormat8bppIndexed>::Cast(mMaskBuffer);
     daim::sweep_algorithm::select_objects( 
       daim::connect4,_rgn,_imgbuff->Gen(), 
       T_OBJECT,T_SELECT,mPtsBuffer
     );
     
     // Build Region
     daim::create_roi(_imgbuff->Gen(),std::bind2nd(std::equal_to<dm_uint8>(),T_SELECT),_rgn); 

     if(mInfo) 
       mInfo->SetMask(_rgn,dmTk::Math::SubPixels);
  }
}
//-------------------------------------------------------
void daim_ext_masks::OnResetMaskSelection()
{
  ReallocMaskBuffer(mInfo->GetMask());
}
//-------------------------------------------------------
void daim_ext_masks::OnEnableMaskSelection()
{
 if(!mbActivated) 
    OnMaskSelection();
   
 // Need to activate focus if already activated 
 // See OnMaskSelection
 if(!HasFocus()) 
    SetFocus(true,NULL);

}
//-------------------------------------------------------
void daim_ext_masks::OnDisableMaskSelection()
{
  if(mbActivated)  
     OnMaskSelection();
}
//--------------------------------------------------------------------------------------
bool daim_ext_masks::OnCommand ( dm_uint nCode )
{
  switch(nCode) {
    case cmdResetMaskSelection:
      OnResetMaskSelection();
      return true;
    case cmdDeleteMaskSelection:
      DeleteSelection(true);
      return true;
    case cmdSelectMaskSelection:
      DeleteSelection(false);
      return true;
  }
  return daim_manager::OnCommand(nCode);
}
//--------------------------------------------------------------------------------------
bool daim_ext_masks::OnCommandUpdate ( daim_CmdUI* pCmdUI )
{
  // We need to neutralize the EditDelete commande
  // because it is linked to the 'del' key
  switch(pCmdUI->nCode) {
     case cmdEditDelete : pCmdUI->Enable(false); return true;
     case cmdSelectMaskSelection: 
     case cmdDeleteMaskSelection:
       pCmdUI->Enable(mbActivated);
       return true;
  }

  return daim_manager::OnCommandUpdate(pCmdUI);
}
//-------------------------------------------------------
BEGIN_COMMAND_MAP(daim_ext_masks, daim_manager)

  REG_COMMAND_UPDATE(daim_ext_masks, MaskSelection )

  REG_COMMAND(daim_ext_masks,  FillHoles    , OnUpdateMask  ) 
  REG_COMMAND(daim_ext_masks,  CleanBorders , OnUpdateMask  )  

  REG_COMMAND(daim_ext_masks,  EnableMaskSelection , OnUpdateMaskSelection )
  REG_COMMAND(daim_ext_masks,  DisableMaskSelection, OnUpdateMaskSelection )

END_COMMAND_MAP()
//-----------------------------------------------------
// Create MASK manager
//-----------------------------------------------------
daim_manager* DAIM_Create_MASK( daim_info* info, const dmRect& r, dmTool* parent )
{
  return new daim_ext_masks(info,r,parent);
}
//-----------------------------------------------------
