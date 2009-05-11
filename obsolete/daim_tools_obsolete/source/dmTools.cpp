
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

#define dmUseServicesMngr
#include "daim_tools.h"
#include "dmStyleManager.h"
//-----------------------------------------------------

#define GENERATE_EVENT( aType )  \
  dmGENERATE_EVENT(const_cast<dmTool*>(this),aType,!mToolStates.Contains(stCancelBubble));

#define GENERATE_EVENT_NOBUBBLE( aType, aResult )  \
  dmGENERATE_RESULT_EVENT(const_cast<dmTool*>(this),aType,aResult,false);

#define GENERATE_RESULT_EVENT( aType, aResult )  \
  dmGENERATE_RESULT_EVENT(const_cast<dmTool*>(this),aType,aResult,!mToolStates.Contains(stCancelBubble));

//-----------------------------------------------------

#define dmTool_SCHEMA_VERSION 1 
dmImplementClassInfo( dmTool , dmObject, dmTool_SCHEMA_VERSION )

#define DEFAULT_PADDING 10

//-----------------------------------------------------
void dmTool::Invalidate( bool _st )
{
  if(!IsLocked()) 
   OnInvalidate( _st );
}	
//-----------------------------------------------------
// Virtual invalidation methode
//-----------------------------------------------------
void dmTool::OnInvalidate( bool _st )
{
  dmGraphics* gr = GetGraphics();
  if(gr) {
    if( IsVisible() && !IsNullObject() ) {
      dmRect r = ObjectRect();
      if(IsCarretsEnabled()) {
         r.Resize(mCarrets->CarretSize()+mPadding); 
      } 
      if(_st) gr->BeginRefresh( r );
      else    gr->EndRefresh( r );
    } else {
      if(_st) gr->BeginRefresh();
      else    gr->EndRefresh();
    }
  }  
}
//-----------------------------------------------------
void dmTool::BeginRefresh() const
{
  dmGraphics* gr = GetGraphics();
  if(gr) { gr->BeginRefresh(); }
}	
//-----------------------------------------------------
void dmTool::EndRefresh( bool bRedraw ) const
{
  dmGraphics* gr = GetGraphics();
  if(gr) { gr->EndRefresh(bRedraw); }
}	
//---------------------------------------------------
// Constructeurs
//---------------------------------------------------
dmTool::dmTool( dmTool* _parent )
: mParent(_parent)
 ,mCarrets(NULL)
 ,mCarretIndex(-1)
 ,mCachedObject(NULL)
 ,mNotifier(NULL)
 ,mLockCount(0)
 ,mMove(0,0)
 ,mToolStates(stLocked)
 ,mPadding(DEFAULT_PADDING)
{}
//---------------------------------------------------
dmTool::dmTool( const dmTool& _toCopy, dmTool* _parent )
: mParent(_parent)
 ,mCarrets(NULL)
 ,mCarretIndex(-1)
 ,mCachedObject(NULL)
 ,mNotifier(NULL) 
 ,mLockCount(0)
 ,mMove(0,0)
 ,mToolStates(_toCopy.mToolStates)
 ,mRoi(_toCopy.mRoi)
 ,mDrawStyle(_toCopy.mDrawStyle)
 ,mInternalName(_toCopy.mInternalName)
 ,mPadding(_toCopy.mPadding)
{}
//---------------------------------------------------
dmTool::dmTool( const dmRect& _r, dmTool* _parent )
: mParent(_parent)
 ,mCarrets(NULL)
 ,mCarretIndex(-1)
 ,mCachedObject(NULL)
 ,mNotifier(NULL)
 ,mMove(0,0)
 ,mToolStates(stLocked)
 ,mRoi(_r)
 ,mPadding(DEFAULT_PADDING)
{}
//---------------------------------------------------
// Destructeur
//---------------------------------------------------
dmTool::~dmTool()
{
  if(mCachedObject)
   delete mCachedObject;

  if(mNotifier) 
   delete mNotifier;
}
//---------------------------------------------------
// Initializer
//---------------------------------------------------
bool dmTool::InitialUpdate()
{
  Recalculate();
  return true;
}
//---------------------------------------------------
bool dmTool::Initialize()
{
  dmASSERT( !IsInitialized() );
  bool res = InitialUpdate();
  if(res) {
    mToolStates.Set(stInitialized,true);
    UnlockUpdate();
  }
  return res;
}
//---------------------------------------------------
// Membres
//---------------------------------------------------
void dmTool::Attach( dmTool* _parent )
{
  dmASSERT( mParent == NULL );
  mParent = _parent;
}
//---------------------------------------------------
dmTool* dmTool::Detach( bool _notify )
{
  dmTool* _former = mParent;

  if(_notify) 
    DoNotifyDetach(this);

  mParent = NULL;
  return _former;
}
//-----------------------------------------------------
void dmTool::OnNotifyDetach( dmTool* _tool )
{
  DoNotifyDetach(_tool); 
}
//-----------------------------------------------------
void dmTool::DoNotifyDetach( dmTool* _tool ) 
{
  dmTool* parent = Parent();
  if(parent) 
    parent->OnNotifyDetach(_tool); 
}
//-----------------------------------------------------
bool dmTool::IsChildOf( const dmTool* aTool ) const
{
  dmTool* _parent = mParent;
  while(_parent) {
    if(_parent==aTool) return true;
    _parent =  _parent->Parent();
  } 
  return false;
}
//-----------------------------------------------------
void dmTool::SetCarrets( dmShape* _carrets, bool _enabled ) 
{
  mCarrets = _carrets;
  SetCarretIndex(-1);
  EnableCarrets(_enabled);
}
//-----------------------------------------------------
void dmTool::EnableCarrets(bool _st)
{
  mToolStates.Set(stCarretsEnabled,_st && mCarrets!=NULL);
}
//-----------------------------------------------------
bool dmTool::IsCarretsVisible() const
{ 

  bool _v = ( IsVisible() && 
               mToolStates.Contains(stCarretsVisible) &&
               mToolStates.Contains(stCarretsEnabled) &&
              !mToolStates.Contains(stPinned)          
             );

  return _v;
}
//-----------------------------------------------------
// Drawing styles
//-----------------------------------------------------
const dmLink<dmDrawStyle>&  dmTool::GetDrawStyle() const
{
  if(mDrawStyle.IsNull() && mParent!=NULL)
    return mParent->GetDrawStyle();
  
  return mDrawStyle;
}
//-----------------------------------------------------
void dmTool::SetDrawStyle( const dmLink<dmDrawStyle>& _aStyle )
{
  mDrawStyle = _aStyle;
}
//-----------------------------------------------------
void dmTool::CloneStyle( const dmTool& aTool )
{
  const dmLink<dmDrawStyle>& _Style = aTool.mDrawStyle;
  if(!_Style.IsNull())
      mDrawStyle = _Style;

  // Clone carret style
  dmShape* _Carrets = const_cast<dmShape*>(aTool.Carrets());
  if(_Carrets && mCarrets) {
     mCarrets->SetPenAttrs (_Carrets->GetPenAttrs());
     mCarrets->SetLineAttrs(_Carrets->GetLineAttrs()); 
  }
}
//-----------------------------------------------------
dmGraphics* dmTool::GetGraphics() const
{	 
  return( mParent
     ? mParent->GetGraphics()
     : NULL );	 
}
//-----------------------------------------------------
void dmTool::DisplayObject( dmGraphics& gr ) const
{
  dmSavedDrawStyle(&gr,mDrawStyle);

  // Display the cached object if it exists
  if(mCachedObject)
     mCachedObject->DisplayObject(gr);
  else
    OnDisplay(gr);
} 
//------------------------------------------------------
void dmTool::Display() const
{
  if(IsVisible()) {
   //dmTRY 
     dmSurface surface = ObtainGraphics();	
     if(surface) 
      DisplayObject(*surface);
   //dmRETHROW_ERROR
  }
}
//-----------------------------------------------------
bool dmTool::Select( bool _select )
{ 
  if(IsSelected()!=_select) {
    mToolStates.Set(stSelected,OnSelection(_select));
    if(IsSelected()==_select) 
      GENERATE_EVENT(_select?evSelect:evUnSelect);
  }
  return IsSelected();
}
//-----------------------------------------------------
bool dmTool::SetFocus ( bool _focus, dmTool* _previous )
{
  if(_focus != HasFocus()) 
  {
    if( _focus  &&  _previous!=NULL && _previous != this ) 
      _previous->KillFocus();

    mToolStates.Set(stFocus,OnFocus(_focus)); 
    if(HasFocus() == _focus)
      GENERATE_EVENT(_focus?evFocus:evBlur); 
  }
  return HasFocus();
}
//-----------------------------------------------------
void dmTool::Redraw() 
{
  O_INVALIDATE(this)
}
//-----------------------------------------------------
bool dmTool::Show( bool _visible  ) 
{
  if(_visible != IsVisible()) {
    O_INVALIDATE(this)
    mToolStates.Set(stVisible,OnShow(_visible));
    if(IsVisible()==_visible) 
      GENERATE_EVENT(IsVisible()?evShow:evHide);
  }
  return IsVisible();
} 
//-----------------------------------------------------
bool dmTool::DisplayCarrets() const
{
  if(mCachedObject) 
    return mCachedObject->DisplayCarrets();

  if( IsCarretsVisible() )
  {
    dmGraphics* gr = GetGraphics();    
    if(gr) { mCarrets->DisplayCarrets(*gr); }
    return true;
  }

  return  false;
}
//-----------------------------------------------------
void dmTool::MoveBy( int dx, int dy,bool _update  ) 
{
  mMove.x += dx;
  mMove.y += dy;
  OnMove(dx,dy,_update); 
  if(_update) {  
    mMove.x = mMove.y = 0;
    SetCarretIndex( -1 );
  }
}
//-----------------------------------------------------
void dmTool::MoveTo( const dmPoint& p,bool _update )
{
  MoveBy(p - ObjectRect().TopLeft() ,_update );
}
//-----------------------------------------------------
void dmTool::SizeBy( int _index, int dx,int dy,bool _update )
{
  if(_index!=CarretIndex()) 
  {
    if(_index==-1 && mMove.x !=0 && mMove.y != 0)
       MoveBy( 0,0,true ); // Force update

    SetCarretIndex( _index );
  }  
  MoveBy( dx,dy,_update );
}
//-----------------------------------------------------
void dmTool::SizeTo( int _index, const dmPoint& p, bool _update )
{
  if(IsCarretsEnabled()) {
    if( _index < static_cast<int>(mCarrets->Size()) ) { 
      SizeBy( _index,p - (*mCarrets)[_index],_update ); 
    }
  }
}
//-----------------------------------------------------
// Virtual fonctions
//-----------------------------------------------------
void dmTool::OnDisplay( dmGraphics& ) const 
{
}
//-----------------------------------------------------
void dmTool::OnRedrawCarrets( dmGraphics& gr ) const
{
  if(IsCarretsEnabled() && mCarrets->IsMoving()) {
    mCarrets->RedrawFrame(&gr);
  }
}
//-----------------------------------------------------
bool dmTool::OnShow( bool _visible  )
{ 
  if( IsCarretsEnabled() ) {
    mCarrets->FrameMove(_visible);
  }
  return _visible;
}	
//-----------------------------------------------------
void dmTool::OnMove( int dx, int dy,bool _update ) 
{
  if(_update) 
  { 
    if(mCachedObject) {       // We update our object
       delete mCachedObject;  // We don't need the cached object anymore
       mCachedObject = NULL;
    }

    if( IsCarretsEnabled() ) {
       // Force moving state :
       // Needed because OnMove can be called
       // directly width _update == true and then 
       // the carrets are not in moving state.
       mCarrets->SetMoveState( true );
       mCarrets->EndMove(GetGraphics(),mCarretIndex,dx,dy);
    }
    else 
      if(mCarrets) 
        mCarrets->MoveCarret(NULL,mCarretIndex,dx,dy);
  
    GENERATE_RESULT_EVENT( mCarretIndex==-1 ? evDrag : evResize , mCarretIndex )

    O_INVALIDATE(this)

    if(mCarretIndex==-1)
    {
       mRoi.Translate(mMove.x,mMove.y);
       GENERATE_EVENT(evUpdate);
    } else 
      RecalcObject();
  }
  else 
  {
    // We need to create a copy of our object used when a display 
    // occurs as we are moving the carrets. This is because the moving
    // carrets are the *real* object carrets

    if(mCachedObject==NULL && IsInitialized())
       mCachedObject = Clone(Parent());

    if( IsCarretsEnabled() ) {
      mCarrets->MoveCarret(GetGraphics(),mCarretIndex,dx,dy);
    } else 
      if(mCarrets) 
        mCarrets->MoveCarret(NULL,mCarretIndex,dx,dy);  
 
    GENERATE_RESULT_EVENT( mCarretIndex==-1 ? evDrag : evResize , mCarretIndex )
  }
}
//-----------------------------------------------------
bool dmTool::OnSelection ( bool _select )
{
  if(IsCarretsEnabled()) {
    if(_select) {
      mToolStates.Set(stCarretsVisible,_select);
      DisplayCarrets();                       
    } else {
      O_INVALIDATE(this)
      mToolStates.Set(stCarretsVisible,_select);
    }
  }
  return _select;  
}
//-----------------------------------------------------
bool dmTool::OnFocus( bool _focus )
{    
  return _focus;
}
//-----------------------------------------------------
dmImage* dmTool::GetImage() const
{ 
  return (mParent ? mParent->GetImage() : NULL); 
}
//----------------------------------------------------------------
void dmTool::LockCarrets( bool bLock )
{
  if(IsCarretsEnabled())
  {
    GetCarrets()->Visible  (!bLock ); 
    GetCarrets()->RigidMode( bLock );
  }
}
//----------------------------------------------------------------
bool dmTool::IsSizable() const 
{
  if(IsCarretsEnabled()) {
    return !Carrets()->RigidMode();
  }
  return false;
}
//-----------------------------------------------------
void dmTool::LockPosition( bool bLock )
{
  mToolStates.Set(stPinned,bLock);
}
//-----------------------------------------------------
void dmTool::Recalculate()
{
   UpdateRegion();
}
//-----------------------------------------------------
const dmRegion& dmTool::UpdateRegion()
{
  OnUpdateRegion( mRoi );
  return mRoi;	 
}
//-----------------------------------------------------
void dmTool::RecalcObject()
{
  if(!IsInitialized()) { 
    Initialize();               // Recalculate via initialize()
  } else {
    dmTRY
      LockUpdate();
      Recalculate();
      UnlockUpdate();

    dmCATCH(_E)
     if(IsLocked()) UnlockUpdate();  // Don't let the object in locked state
     dmRETHROW(_E)
    dmDONE
  }

  // Modified on 11/04/05
  GENERATE_EVENT( evUpdate )

}
//-----------------------------------------------------
void dmTool::LockUpdate()
{
  ++mLockCount;
  mToolStates.Set(stLocked,true);
}
//-----------------------------------------------------
void dmTool::UnlockUpdate()
{
  if(--mLockCount<0) mLockCount = 0;
  mToolStates.Set(stLocked,false);
}
//-----------------------------------------------------
void dmTool::UpdateObject()
{
  if(!IsLocked()) 
  {
    O_INVALIDATE(this)
    RecalcObject();
  }
}
//-----------------------------------------------------
#define SELECT_AREA_SIZE 4
//-----------------------------------------------------
dmTool* dmTool::GetObject( const dmPoint& _where, bool bHitTest ) 
{
  if(bHitTest) 
  {
    if(IsVisible()) 
    {
      if(IsCarretsVisible()) { 
        mCarretIndex = mCarrets->CarretIndex( _where, 
                       mCarrets->ScaledCarretSize(GetGraphics()) );
      } else
        mCarretIndex = -1;

      if(mCarretIndex<0 && !mRoi.PointInRoi(_where)) 
      {
        size_t _w = 2*SELECT_AREA_SIZE+1;
        dmRect r( _where.x - SELECT_AREA_SIZE,_where.y - SELECT_AREA_SIZE,_w,_w);
        return (Region().Intersect(r) ? this : NULL); 
      } else 
        return this;
    }
  } else {
    return (mRoi.PointInRoi(_where) ? this : NULL);
  }
  return NULL;
}	 
//-----------------------------------------------------
dmTool* dmTool::GetObject( const dmRect& _rect, bool bHitTest ) 
{
  if(bHitTest) 
    return (IsVisible() && (_rect >= ObjectRect())  ? this : NULL );
  else
    return ((_rect >= ObjectRect())  ? this : NULL );
}
//-----------------------------------------------------
dmTool* dmTool::GetObject( const dmRegion& _rgn, bool bHitTest ) 
{
  if(bHitTest && !IsVisible()) 
     return NULL;

  if(!mRoi.IsEmptyRoi()) {
    dmRegion rgn = _rgn;
    rgn.AndRoi(mRoi);
    return (rgn==mRoi ? this : NULL);
  } else 
    return NULL;
}
//-----------------------------------------------------
dmTool* dmTool::Clone( dmTool* _parent  ) const
{
 	return new dmTool( *this, _parent );	 
}
//-----------------------------------------------------
const dmString& dmTool::ToolName() const
{		
  return mInternalName;	
}
//-----------------------------------------------------
void dmTool::SetToolName( const dmString& _name )
{		
  mInternalName = _name;
}
//-----------------------------------------------------
// Acyclic visistor pattern 
void dmTool::Accept( dmVisitor& aVisitor )
{
  dmToolVisitor* _Visitor = dmToolVisitor::Cast(&aVisitor);
  if(_Visitor) _Visitor->Visit(*this);
}
//-----------------------------------------------------
void dmTool::Accept( dmConstVisitor& aVisitor ) const
{
  dmToolConstVisitor* _Visitor = dmToolConstVisitor::Cast(&aVisitor);
  if(_Visitor) _Visitor->Visit(*this);
}
//-----------------------------------------------------
// Events handling
//-----------------------------------------------------
dmEventNotifier* dmTool::GetEventNotifier(bool _initialize)
{
  if(_initialize && mNotifier==NULL) 
    mNotifier = new dmEventNotifier;
  
  return mNotifier;
}
//-----------------------------------------------------
void dmTool::AddEventListener( const dmSetOf<dmEventType>& _evts, dmEventListener* _evh, dm_uint _flags )
{
  dmEventNotifier* evtno = GetEventNotifier(true);
  if(evtno)
    evtno->AddEventListener(_evts,_evh,_flags);
}
//-----------------------------------------------------
void dmTool::RemoveEventListener( const dmSetOf<dmEventType>& _evts, dmEventListener* _evh, dm_uint _flags )
{
  dmEventNotifier* evtno = GetEventNotifier(false);
  if(evtno)
    evtno->RemoveEventListener(_evts,_evh,_flags);
}
//-----------------------------------------------------
bool dmTool::ProcessEvent( dmEvent& anEvent )
{
  return dmHANDLE_EVENT(anEvent,this);
}
//-----------------------------------------------------
bool dmTool::ProcessDefaultEvent( dmEvent& )
{
  return false; // Event is not handled
}
//-----------------------------------------------------
