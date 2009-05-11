
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

#define dmUseToolManager
#define dmUseToolManips
#include "daim_tools.h"
#include "dmGroup.h"

#include <algorithm>

#define THIS_GRAPHICS(me) if(mGraphics) mGraphics->me

//------------------------------------------------------
#define dmToolManager_SCHEMA_VERSION 1
dmImplementClassInfo( dmToolManager, dmToolGroup, dmToolManager_SCHEMA_VERSION );
//---------------------------------------------------
dmToolManager::dmToolManager( dmTool* _parent )
: dmToolGroup(_parent)
 ,mGraphics(NULL)
 ,mCurrentTool(NULL)
 ,mSelectTopAllways(false)
 ,mLockNotify(false)
 ,mHasMoved(false)
 ,mEnableCapture(false)
 ,mEnableMultiSelection(true)
 ,mSelectCount(0)
 ,mDefaultCursorType(eCursor_undefined)
{
  mRubber.Attach(this);
  mRubber.Obtain();
}
//---------------------------------------------------
dmToolManager::dmToolManager( const dmRect& _r, dmGraphics* gr )
: dmToolGroup(_r)
 ,mGraphics(NULL)
 ,mCurrentTool(NULL)
 ,mSelectTopAllways(false)
 ,mLockNotify(false)
 ,mHasMoved(false)
 ,mEnableCapture(false)
 ,mEnableMultiSelection(true)
 ,mSelectCount(0)
 ,mDefaultCursorType(eCursor_undefined)
 ,mRect(_r)
{
  mRubber.Attach(this);
  mRubber.Obtain();

  SetGraphics( gr );
  Show(true);
}
//---------------------------------------------------
dmToolManager::~dmToolManager()
{  
  dmToolManager::ClearAndDestroy();
  SetGraphics(NULL);
  mRubber.Release();
}
//---------------------------------------------------
void dmToolManager::UpdateSelection()
{
  mWorkList.Clear();
  mSelectCount = 0;
  for(iterator it=Begin();it!=End();++it) {
    if((*it)->IsSelected()) {
      mWorkList.Push_Back(*it);
      ++mSelectCount;
    }
  }
}
//---------------------------------------------------
void dmToolManager::SelectTopAllways( bool _st )
{
  mSelectTopAllways = _st;
}
//------------------------------------------------------
void dmToolManager::OnNotifyDetach( dmTool* _tool )
{
  if(_tool) 
  {
    _tool->Select(false);
    if( mCurrentTool == _tool ) {
      mCurrentTool->KillFocus();
      mCurrentTool = NULL;
    }
    dmToolGroup::Remove( _tool );
  }
}
//------------------------------------------------------
ECursorType dmToolManager::SetDefaultCursor( ECursorType newCursor )
{
  ECursorType oldCursor = mDefaultCursorType;
  mDefaultCursorType = newCursor;

  return oldCursor;
}
//--------------------------------------------------------------------------------------
bool dmToolManager::ProcessEvent( dmEvent& anEvent )
{
  // In case of null target, the event will not bubble
  // Set ourself as the default target so that bubbling 
  // will fall back on this

  if(anEvent.IsCapturing())
     anEvent.mTarget = this;

  return dmToolGroup::ProcessEvent(anEvent);
}
//------------------------------------------------------
bool dmToolManager::ProcessDefaultEvent( dmEvent& anEvent )
{
  if(anEvent.IsCapturing()) 
  {
    switch(anEvent.mStructType)
    {
      case evStruct_MouseEvent : 
        return ProcessMouseEvent(static_cast<dmMouseEvent&>(anEvent));

      case evStruct_KeyEvent : 
        return ProcessKeyEvent(static_cast<dmKeyEvent&>(anEvent));

      default: 
        break;
    }
  }

  if(anEvent.IsBubbling())
    return ProcessBubblingEvent(anEvent);

  return false;
}
//------------------------------------------------------
bool dmToolManager::ProcessMouseEvent( dmMouseEvent& anEvent )
{
  if(mDefaultCursorType != eCursor_undefined) 
     anEvent.mCursorType = mDefaultCursorType;

  // Process the tool which has the focus
  if(!(anEvent.mEventType == evMouseDown && mSelectTopAllways))
  {
    if(mCurrentTool!=NULL && mCurrentTool->ProcessEvent(anEvent)) 
         return true; // The event is handled by the default tool 
                      // so we stop the processing
  }

  // At this point the event has not been handled

  switch(anEvent.mEventType)
  {
    //------------------------------------------------------
    // MouseDown
    //------------------------------------------------------
    case evMouseDown : {

      dmTool* pProcessedTool = mCurrentTool;

      // First find if we have a tool under the cursor position
      dmTool* tool = dmToolGroup::GetObject(anEvent.mCursor,true);
      if(tool)
      {
        if(anEvent.ShiftKey() && mEnableMultiSelection)  // Enter the multiselection mode
        {
          if(mCurrentTool!=NULL)                         // Kill the focus for the current tool
             mCurrentTool->KillFocus();

          tool->Select(!tool->IsSelected());             // Toggle selection
        }
        else
        {
          if(!tool->IsSelected())
          { 
            SelectAll(false); 
            tool->Select(true);      
          }
          
          tool->SetFocus(true,mCurrentTool);  // Set Focus for this tool :             
                                              // Will kill previous focus
        } 
      }
      else 
      {
        if(!anEvent.ShiftKey() && mEnableMultiSelection)  
           SelectAll(false );

        if(mEnableCapture) {
          // Activate the rubber
          mRubber.SetFocus(true,mCurrentTool);
          mRubber.Activate(true);
          pProcessedTool = NULL;  // We want the rubber to process that command
        } else {
          if(mCurrentTool)
             mCurrentTool->KillFocus();
        }
      }

      // Process the event for the current tool
      if(mCurrentTool!=NULL && pProcessedTool != mCurrentTool)
        return mCurrentTool->ProcessEvent(anEvent);
      
    }
    break;  // not handled 

    //------------------------------------------------------
    // MouseMove
    //------------------------------------------------------
    case evMouseMove :
      if(MultiSelection() && !anEvent.ShiftKey())   // if Shift do nothing
      {    
        if(!(anEvent.mButton & buttonLeft)) {
          if(mHasMoved) 
          {
            mHasMoved = false;                 // on est sortie de la fenêtre puis entré de nouveau  
            MoveSelection( dmPoint() ,true);   // avec le bouton de la souris relaché
            THIS_GRAPHICS(ReleaseGraphics)();  // See below
          }
        } else {                              // Multiselection drag

          // We do an ObtainGraphics() in order to initialize
          // the graphics so that subsequent repetitives calls to 
          // ObtainGraphics() will not (re)allocate ressources
          // (like for "rubber" drawings)
         
          if(!mHasMoved)
             THIS_GRAPHICS(ObtainGraphics)();

          if(MoveSelection( anEvent.mCursor - mMouseOffset ,false)>0)
             mHasMoved = true;
          else
             THIS_GRAPHICS(ReleaseGraphics)();
        }
        mMouseOffset = anEvent.mCursor;
        return mHasMoved;
      }
      break;  // not handled 

    //------------------------------------------------------
    // MouseUp
    //------------------------------------------------------
    case evMouseUp   :
      if(MultiSelection() && !anEvent.ShiftKey()) {  // Multiselection drag
        if(mHasMoved)
        {
          MoveSelection( anEvent.mCursor - mMouseOffset ,true);
          mHasMoved = false;
          THIS_GRAPHICS(ReleaseGraphics)();
          return true; // handled
        } 
      }  
      break;  // not handled 


    default: break;
  }

  return false;   // not handled 
}
//------------------------------------------------------
bool dmToolManager::ProcessKeyEvent( dmKeyEvent& anEvent )
{
  if(mCurrentTool!=NULL)
     return mCurrentTool->ProcessEvent(anEvent);

  return false;
}
//------------------------------------------------------
bool dmToolManager::ProcessBubblingEvent( dmEvent& anEvent )
{
  dmTool* _target = anEvent.mTarget;

  if(_target==NULL)
     return false;

  // Some handlers may change the cursor in notifiables events,
  // so we need to propagate this change to the event
  if(anEvent.mCursorType == eCursor_undefined && mDefaultCursorType != eCursor_undefined) 
     anEvent.mCursorType = mDefaultCursorType;

  switch(anEvent.mEventType)
  {
     case evUnSelect :
       if(!mLockNotify) {
         mWorkList.Remove(_target);
         --mSelectCount;
       }
       return true;

     case evSelect   : 
       if(!mLockNotify) 
       {
         mWorkList.Push_Back(_target);
         ++mSelectCount;
       }
       return true;

      case evFocus :
        if(mCurrentTool!=_target)
        {
          if(mCurrentTool!=NULL) 
             mCurrentTool->KillFocus();

          mCurrentTool = _target;
        }
        return true;

      case evBlur  : 
      
        if(mCurrentTool==_target)
           mCurrentTool = NULL;
           
        return true;

      default: break;
  }
  return false;
}
//------------------------------------------------------
void dmToolManager::SetGraphics( dmGraphics* gr )
{		
  mGraphics = gr;
}
//------------------------------------------------------	 
dmGraphics* dmToolManager::GetGraphics() const
{
  return (mGraphics ? mGraphics : dmTool::GetGraphics());	 
}
//------------------------------------------------------- 
void dmToolManager::OnDisplay( dmGraphics& gr ) const
{	
  dmToolGroup::OnDisplay(gr);
  
  if(HasFocus()) 
  {
    const_iterator it   = mWorkList.Begin();
    const_iterator last = mWorkList.End();

    for(;it!=last;++it) {    
      (*it)->DisplayCarrets();
    }
  }

  if(gr.IsUI()) {
     dmToolGroup::OnRedrawCarrets(gr);
     if(mCurrentTool == static_cast<const dmTool*>(&mRubber)) 
        mRubber.OnRedrawCarrets(gr);
  }
}	 
//------------------------------------------------------
void dmToolManager::SetRect( const dmRect& r ) 
{ 
  mRect = r;
  UpdateObject();
} 
//------------------------------------------------------
void dmToolManager::OnUpdateRegion( dmRegion& _rgn )
{
  _rgn.SetRectRoi( mRect );
}
//------------------------------------------------------	
bool dmToolManager::SelectAll( bool _select )
{
  BeginRefresh();
  for(iterator it = Begin();it != End();++it ) { 
    (*it)->Select(_select); 
  }
  EndRefresh();
  return !mWorkList.Empty();	 
}
//-------------------------------------------------------
void dmToolManager::EnableMultiSelection( bool bEnable )
{
  mEnableMultiSelection = bEnable;
}
//-------------------------------------------------------
void dmToolManager::SafeRemove( dmTool* _tool  )
{ 
  if(_tool) 
  {
    dmToolGroup::Remove(_tool);
    BeginRefresh();
    _tool->SetFocus(false,NULL);
    _tool->Select(false);
    _tool->Show(false);
    EndRefresh();
  }
}
//-------------------------------------------------------
void dmToolManager::ClearAndDestroy()
{
  mCurrentTool=NULL;
  mWorkList.Clear();
  mSelectCount = 0;
  dmToolGroup::ClearAndDestroy();
}
//-------------------------------------------------------
int dmToolManager::ClearSelection()
{
  int count = 0;
  BeginRefresh();
  if(!mWorkList.Empty()) 
  {
    iterator __F = mWorkList.Begin();
    iterator __L = mWorkList.End();
    for(;__F!=__L;++__F) 
       (*__F)->Show(false); 
   
    mWorkList.Clear(); 
    mSelectCount = 0;
    
    iterator it = Begin();
    while( it!=End() ) {  // Delete selected objects from main list
      if((*it)->IsSelected() && !(*it)->IsVisible() ) { 
        dmTool* tool = Remove(it++);
        if(tool->References()==0) 
        {
           //XXX Watch out for current active tool
           if(mCurrentTool==tool)
              mCurrentTool = NULL;

           delete tool;
        }
        ++count;
      } else ++it;
    }
  }
  EndRefresh();
  return count;
}
//-------------------------------------------------------
size_t dmToolManager::CopySelection ( dmToolGroup& rBuffer ) const
{
  rBuffer.Clear();
  size_t Cnt = 0;
  for(const_iterator it = Begin();it!=End();++it) {
    if( (*it)->IsSelected() ) {
      rBuffer.Add((*it)->Clone(&rBuffer));
      ++Cnt;
    }
  }
  return Cnt;
}
//-------------------------------------------------------
size_t dmToolManager::CutSelection  ( dmToolGroup& rBuffer )
{
  size_t Cnt = CopySelection(rBuffer);
  ClearSelection();
  return Cnt;
}
//-------------------------------------------------------
size_t dmToolManager::PasteSelection( const dmToolGroup& rBuffer )
{
  BeginRefresh();
  SelectAll(false);
  size_t Cnt = Append( rBuffer );
  UpdateSelection();
  sel_iterator it = Selection_Begin();
  for(;it != Selection_End(); ++it ) { O_INVALIDATE(*it) }
  EndRefresh();
  return Cnt;
}
//-------------------------------------------------------
int dmToolManager::MoveSelection( int dx,int dy, bool _update )
{
  if(_update) 
    BeginRefresh();

  size_t Cnt = 0;

  iterator it = mWorkList.Begin();
  for(;it != mWorkList.End(); ++it ) 
  { 
    if( (*it)->IsPositionnable() ) { 
      (*it)->MoveBy(dx,dy,_update);
      ++Cnt;
    }
  }

  if(_update) 
    EndRefresh();
  return Cnt;
}
//-------------------------------------------------------
void dmToolManager::SelectionToBack()
{  
  BeginRefresh();
  iterator it,first = Begin();
  it = first;
  while( it!=End() )
  {
    if( (*it)->IsSelected() && it!=first ) { 
       O_INVALIDATE( *it )
       Insert( first, Remove( it++ ) );
    } else ++it;
  }
  EndRefresh();
}
//-------------------------------------------------------
void dmToolManager::SelectionToFront()
{
  BeginRefresh();
  iterator last = --End();
  for(iterator it=Begin();it!=last;++it)
  {
    if( (*it)->IsSelected() ) { 
       O_INVALIDATE( *it )
       Push_Back( Remove( it-- ) ); 
    }
  }
  EndRefresh();
}
//-------------------------------------------------------
void dmToolManager::SelectionForward()
{  
  if(Count()>1 && SelectCount()>0) 
  {
    BeginRefresh();
    iterator prev,it = End();
    iterator last    = Begin();
    --it;
    do {
      prev = it; --it;
      if( (*it)->IsSelected()) {
        O_INVALIDATE( *it )
        std::swap(*it,*prev);
      }
      prev = it; 
    } while(it!=last);
    EndRefresh();
  }
}
//-------------------------------------------------------
void dmToolManager::SelectionBackward()
{
  if(Count()>1 && SelectCount()>0) {
    BeginRefresh();
    iterator prev,it = Begin();
    iterator last    = End();
    for(prev=it,++it;it!=last;++it) { 
      if( (*it)->IsSelected()) {
        O_INVALIDATE( *it )
        std::swap(*it,*prev);
      }
      prev = it; 
    }
    EndRefresh();
  }
}
//-------------------------------------------------------
#define APPLY_TO_SELECTION( op ) {                        \
     std::for_each( Selection_Begin(),Selection_End(),op);\
}
//-------------------------------------------------------
void dmToolManager::AlignSelection(  dm_uint _align )
{
  if(mSelectCount>1) {
    const dmRect& r = (*mWorkList.Begin())->ObjectRect();
    switch( _align ) {
      case eAlignStyle_left    : APPLY_TO_SELECTION(dmToolManips::Align::Left   (r.Left()  ))  ;break;
      case eAlignStyle_right   : APPLY_TO_SELECTION(dmToolManips::Align::Right  (r.Right() ))  ;break;
      case eAlignStyle_top     : APPLY_TO_SELECTION(dmToolManips::Align::Top    (r.Top()   ))  ;break;
      case eAlignStyle_bottom  : APPLY_TO_SELECTION(dmToolManips::Align::Bottom (r.Bottom()))  ;break;
      case eAlignStyle_vcenter : APPLY_TO_SELECTION(dmToolManips::Align::VCenter(r.Center().x));break;
      case eAlignStyle_center  : APPLY_TO_SELECTION(dmToolManips::Align::HCenter(r.Center().y));break;
    }
  }
}
//-------------------------------------------------------
template<class I,class R>
static int __select_objects( I _start, I _end, const R& r, bool _add, bool _hit )
{
  int cnt = 0;
  dmTool* tool = NULL;
  for(I it = _start; it != _end ; ++it ) {
    tool=(*it)->GetObject(r,_hit);
    if(tool) { 
      tool->Select(true); 
      ++cnt;
    }
    else if(!_add) 
      (*it)->Select(false); 
  }
  return cnt;
}
//------------------------------------------------------- 
dmTool* dmToolManager::GetObject( const dmRect& r, bool bHitTest ) 
{
  int cnt = __select_objects( rBegin(), rEnd(),r, true, bHitTest );
  return( cnt ? this : NULL );
}   
//------------------------------------------------------- 
dmTool* dmToolManager::GetObject( const dmRegion& r, bool bHitTest ) 
{
  int cnt = __select_objects( rBegin(), rEnd(),r, true, bHitTest );
  return( cnt ? this : NULL );
}
//------------------------------------------------------- 
dmTool* dmToolManager::GetObject( const dmPoint& p, bool bHitTest ) 
{
  return dmToolGroup::GetObject(p,bHitTest);
}
//-------------------------------------------------------     
dmTool* dmToolManager::Clone( dmTool* _parent ) const
{
  return NULL;	
}
//--------------------------------------------------------
dmTool* dmToolManager::GetCurrentTool()
{ 
  if(mCurrentTool!=NULL && mCurrentTool!=&mRubber) 
    return mCurrentTool;

  return NULL;
}
//--------------------------------------------------------
// Group / Ungroup
//--------------------------------------------------------
dmTool* dmToolManager::GroupSelection()
{
  if(mSelectCount>1) {
    iterator prev,it = Selection_Begin();
    iterator last    = Selection_End();
    dmGroup *gr = new dmGroup(this);
    gr->Show(true);
    BeginRefresh();
    while(it!=last) 
    {
      if((*it)->Parent()==this)
      {
        gr->Push_Back( *it );
        (*it)->Detach(false);
        (*it)->Attach(gr);
        prev = it; ++it;
        Remove(prev);
      } else 
        ++it;
    }
    OnGroupSelection(gr);
    gr->Initialize();
    Add( gr );
    EndRefresh();
    return gr;
  }
  return NULL;
}
//--------------------------------------------------------
void dmToolManager::UngroupSelection()
{
  iterator __F  = Selection_Begin();
  iterator __L  = Selection_End();

  for(;__F!=__L;++__F)
  {
    dmGroup* gr = dmGroup::Cast(*__F);
    if(gr && gr->Parent()==this) 
    {
      dmGroup::iterator prev,it = gr->Begin();
      dmGroup::iterator last    = gr->End();
      while(it!=last)
      {
        (*it)->Detach();
        (*it)->Attach(this);
        prev = it; ++it;
        (*prev)->Select(true);
        Add( *prev );
      }
      gr->Select(false);
      Remove(gr);
      if(gr->References()==0) 
       delete gr;
    }
  }
}
//--------------------------------------------------------
void dmToolManager::OnGroupSelection( dmGroup* gr )
{
  SelectAll(false);
  gr->Select(true);
  gr->SetFocus(true,mCurrentTool);
}
//--------------------------------------------------------
bool dmToolManager::GetRectSelection( dmRect& _rect )
{
  sel_iterator it   = Selection_Begin();
  sel_iterator last = Selection_End();
  for(;it!=last;++it) { _rect.Add( (*it)->ObjectRect() ); }
  return mSelectCount>0;
}
//--------------------------------------------------------
bool dmToolManager::GetRgnSelection ( dmRegion& _rgn )
{
  sel_iterator it   = Selection_Begin();
  sel_iterator last = Selection_End();
  for(;it!=last;++it) { _rgn.AddRoi( (*it)->Region() ); }
  return mSelectCount>0;  
}
//--------------------------------------------------------
void dmToolManager::EnableCapture( bool bEnable )
{
  mEnableCapture = bEnable;
}
//--------------------------------------------------------
bool dmToolManager::IsCapturing()
{
  return mRubber.IsActive();
}
//--------------------------------------------------------
bool dmToolManager::GetCaptureRect( dmRect& r )
{
  if(mRubber.IsActive()) {
     r = mRubber.GetRect();
     return true;
  }
  return false;
}
//--------------------------------------------------------
