
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

#include "daim_info_mngr.h"
#include "daim_manager.h"

class daim_layers;

daim_manager* DAIM_Create_ROI   (daim_info*,const dmRect&,dmTool*);
daim_manager* DAIM_Create_MASK  (daim_info*,const dmRect&,dmTool*);
daim_manager* DAIM_Create_LAYERS(daim_info*,daim_layers*,const dmRect&,dmTool*);

#include <list>
#define THIS_INFO() mInfo
//---------------------------------------------------------------------------
// Gestion d'une pile de manager 
// lorsqu'un objet d'un manager est sélectionné
// celui passe en avant plan
//---------------------------------------------------------------------------
class daim_tools : public daim_CmdTarget, public dmTool 
{
  public:
    std::list<daim_manager*> mManagers;

  public:
   daim_tools( dmTool* _parent ) : dmTool(_parent) {}

   typedef std::list<daim_manager*>::iterator         iterator;
   typedef std::list<daim_manager*>::const_iterator   const_iterator;
   typedef std::list<daim_manager*>::reverse_iterator reverse_iterator;

  public:
   iterator Begin() { return mManagers.begin(); }
   iterator End()   { return mManagers.end();   }

   const_iterator Begin() const { return mManagers.begin(); }
   const_iterator End()   const { return mManagers.end();   }

   reverse_iterator rBegin() { return mManagers.rbegin(); }
   reverse_iterator rEnd()   { return mManagers.rend();   }

   bool Empty() const { return mManagers.empty(); }

   daim_manager* Find( const char_t*_name ) {
      iterator it   = Begin();
      iterator last = End();
      for(;it!=last;++it) {
        if((*it)->ToolName()==_name)
          return *it;
      }
      return NULL;
   }

   virtual void OnDisplay( dmGraphics& ) const;
   virtual void Recalculate();

   virtual dmTool* GetObject( const dmPoint&, bool bHitTest );
   
   virtual bool ProcessDefaultEvent( dmEvent& );

   virtual bool OnCommandUpdate ( daim_CmdUI*  );
   virtual bool OnCommand       ( dm_uint nCode );

   virtual void OnNotifyDetach( dmTool* );

   void ClearAndDestroy() {
      iterator it   = Begin();
      iterator last = End();
      for(;it!=last;++it)
         if((*it)->Release()==0) {
           delete *it;
         }
      mManagers.clear();
   }

   virtual ~daim_tools() {
      ClearAndDestroy(); 
   }  
};
//-----------------------------------------------------------------------
void daim_tools::OnDisplay( dmGraphics& gr ) const
{
  const_iterator it   = mManagers.begin();
  const_iterator last = mManagers.end();
  for(;it!=last; ++it) 
  { 
    if((*it)->IsVisible()) 
        (*it)->DisplayObject(gr); 
  }
}
//------------------------------------------------------
void daim_tools::OnNotifyDetach( dmTool* _tool )
{
  daim_manager* pManager = daim_manager::Cast(_tool);
  if(pManager) 
  {
    pManager->Select(false);
    pManager->KillFocus();

    iterator it = std::find(Begin(),End(),pManager);
    if(it!=End()) {
      (*it)->Release();
      mManagers.erase(it);
    }
  }
}
//--------------------------------------------------------------------------------------
// If a manager take the focus put it in front
// (i.e last in list)
//--------------------------------------------------------------------------------------
bool daim_tools::ProcessDefaultEvent( dmEvent& anEvent )
{
  if(anEvent.IsBubbling()) 
  {
    daim_manager* pManager = daim_manager::Cast(anEvent.mTarget);
    if(pManager) 
    {
      switch(anEvent.mEventType)
      {
        case evFocus : {
          iterator it = std::find(Begin(),End(),pManager);
          if(it!=End()) {
            mManagers.erase(it);
            mManagers.push_back(pManager);
          }        
          return true;  // handled
        } break;
        
        default:break;
      }
    }
    else 
    {
      // Ensure that parent notifications are handled
      // on bubbling events

      dmTool* parent = Parent();
      if(parent) {
        dmEventNotifier* _notifier = parent->GetEventNotifier(false);
        if(_notifier && _notifier->ProcessEvent(anEvent)) 
          anEvent.mFlags |= dmEVENT_FLAG_HANDLED;
      }

      // Prevent anything bubbling out 
      anEvent.PreventBubble();
    }
  }
  return false; // not handled 
}
//--------------------------------------------------------------------------------------
dmTool* daim_tools::GetObject( const dmPoint& p, bool bHitTest ) 
{
  reverse_iterator it   = mManagers.rbegin();
  reverse_iterator last = mManagers.rend();
  
  dm_uint       nPriority  = 0;
  daim_manager* pCandidate = NULL;
          
  // Rules for selecting a manager :
  // if it has the focus , select it, 
  // if not, compare to the actual priority
  // if the priority his higher, keep it as best candidate 
  
  for(;it!=last; ++it) 
  {
    if((*it)->HasFocus()) {
       pCandidate = *it;
       break; 
    }
      
    if((*it)->IsVisible() && (*it)->GetObject(p,bHitTest)) 
    {
      if((*it)->GetPriority() > nPriority) {
         pCandidate = *it;
         nPriority  = pCandidate->GetPriority();
      }
    }
  }         

  if(pCandidate == NULL) 
  {
    if(!mManagers.empty() && (*rBegin())->IsVisible()) {
       pCandidate = *rBegin();
    }
  }

  return pCandidate;
}
//--------------------------------------------------------------------------------------
void daim_tools::Recalculate()
{
  dmTool::Recalculate();
  for(iterator it=Begin();it!=End();++it) {
    (*it)->SetRect( Parent()->ObjectRect() );
  }
}
//--------------------------------------------------------------------------------------
// go through all visible managers and search for the first one 
// handling the command , needs to go backward because the top displayed manager
// is the last one in list
//--------------------------------------------------------------------------------------
bool daim_tools::OnCommandUpdate( daim_CmdUI* pCmdUI )
{
  reverse_iterator it   = mManagers.rbegin();
  reverse_iterator last = mManagers.rend();
  for(;it!=last; ++it) {
   if((*it)->OnCommandUpdate(pCmdUI)) 
       return true;
  }
  return false; 
}
//--------------------------------------------------------------------------------------
bool daim_tools::OnCommand( dm_uint nCode )
{
  reverse_iterator it   = mManagers.rbegin();
  reverse_iterator last = mManagers.rend();
   for(;it!=last; ++it) {
   if((*it)->OnCommand(nCode)) 
       return true;
  }
  return false; 
}

//--------------------------------------------------------------------------------------
// daim_info_manager members
//----------------------------------------------------------------------------
void daim_info_manager::Create_Tools()
{
  mTools = new daim_tools(this);
  mTools->Show(true);
  Add(mTools);
}
//----------------------------------------------------------------------------
void daim_info_manager::Destroy_Tools()
{
  if(mTools) {
    mTools->Detach();
    delete mTools;
  }
}
//--------------------------------------------------------------------------------------
daim_manager* daim_info_manager::SelectManager( const dmString::E* _name, dm_uint createflags )
{
  if(mTools && !mTools->Empty())
  {
    daim_manager* pManager = NULL;

    if(!EMPTY_STRING(_name))  
    {
      pManager = mTools->Find(_name);

      if(pManager == NULL && (createflags & DM_Manager_Create)) {
        pManager = new daim_manager(THIS_INFO(),GetRect(),mTools);
        pManager->SetToolName(_name);
        pManager->Show(true);
        pManager->Initialize(); 
        pManager->Obtain();
        mTools->mManagers.push_front(pManager);
      }

      // Check the active manager
      // against the given name
      if(createflags & DM_Manager_Check) {
         pManager = *mTools->rBegin();
         if(!(pManager->ToolName() == _name))
              pManager = NULL;
      }
    } 
    else // No name is specified => return the active manager
    {
      pManager = *mTools->rBegin();
    }
    
    if(pManager && (createflags & DM_Manager_Select)) {
       pManager->SetFocus(true,NULL);
    }
    return pManager;
  } 
  return NULL;
}
//--------------------------------------------------------------------------------------
void daim_info_manager::InsertManager( daim_manager* manager )
{
  if(mTools && manager) {
    manager->Show(true);
    if(!IsInitialized()) 
      manager->Initialize();

    manager->Obtain();
    mTools->mManagers.push_front(manager);
  }
}
//--------------------------------------------------------------------------------------
void daim_info_manager::InitROI()
{
  if(mTools)
    InsertManager(DAIM_Create_ROI(THIS_INFO(),GetRect(),mTools));
}
//--------------------------------------------------------------------------------------
void daim_info_manager::InitMask()
{
  if(mTools)
    InsertManager(DAIM_Create_MASK(THIS_INFO(),GetRect(),mTools));
}
//--------------------------------------------------------------------------------------
void daim_info_manager::InitLayers()
{
  if(mTools)
    InsertManager(DAIM_Create_LAYERS(THIS_INFO(),mLayers,GetRect(),mTools));
}
//--------------------------------------------------------------------------------------
bool daim_info_manager::OnCommandUpdate( daim_CmdUI* pCmdUI )
{
  // Propagate commands to tools managers
  if(mTools && mTools->OnCommandUpdate(pCmdUI))
    return true;

  return daim_CmdTarget::OnCommandUpdate(pCmdUI);
}
//--------------------------------------------------------------------------------------
bool daim_info_manager::OnCommand( dm_uint nCode )
{
  // Propagate commands to tools managers
  if(mTools && mTools->OnCommand(nCode))
    return true;

  return daim_CmdTarget::OnCommand(nCode);
}
//--------------------------------------------------------------------------------------
