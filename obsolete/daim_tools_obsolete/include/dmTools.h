#ifndef dmTools_h
#define dmTools_h

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

//--------------------------------------------------------
// File         : dmTools.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#include "daim/daim_forwards.h"

//---------------------------------------------------
// dmTool
//---------------------------------------------------
class __dmTools dmTool : public dmObject, public dmEventListener
{ 
  private:	
    typedef enum {
      stSelected,    
      stVisible,
      stCarretsEnabled, 
      stCarretsVisible,
      stPinned,
      stInitialized,
      stLocked,
      stCancelBubble,
      stFocus,
    } _ToolStates;
 
    friend class dmSetOf<_ToolStates>;
 
    dmTool*              mParent;
    dmShape*             mCarrets;
    int                  mCarretIndex;
    dmTool*              mCachedObject;
    dmEventNotifier*     mNotifier;
    dm_uint              mLockCount;
    dmPoint              mMove;
    dmSetOf<_ToolStates> mToolStates;
    dmRegion             mRoi;
    dmLink<dmDrawStyle>  mDrawStyle;
    dmString             mInternalName;

  protected:
    dm_uint mPadding;
	 
    dmTool( const dmRect& , dmTool* _parent=NULL );
    dmTool& operator=(const dmTool& ) { return *this; }
    
    dmShape* GetCarrets()   { return mCarrets; } 
    void     SetCarrets( dmShape* _shape, bool _enabled = true );
    void     SetCarretIndex( int _index ) { mCarretIndex = _index; }

    void DoNotifyDetach( dmTool* );

    virtual void OnInvalidate( bool );
    virtual void OnNotifyDetach( dmTool* );

  protected:    
    virtual bool OnShow( bool _visible );
    virtual bool OnSelection( bool _select );
    virtual void OnUpdateRegion( dmRegion& ) {} 
    virtual void OnMove( int dx, int dy,bool _update );
    virtual bool OnFocus( bool _focus );

    const dmRegion& UpdateRegion();
    
  public:
    virtual void Recalculate();
    virtual bool InitialUpdate();

  public:
    // Drawing attributs

    const dmLink<dmDrawStyle>&  GetDrawStyle()    const;
    void  SetDrawStyle( const dmLink<dmDrawStyle>& );
    void  CloneStyle  ( const dmTool& );

    bool DisplayCarrets() const;
    void EnableCarrets (bool);

    bool IsCarretsVisible() const;
    bool IsCarretsEnabled() const { return mToolStates.Contains(stCarretsEnabled); }
    bool IsLocked()         const { return mToolStates.Contains(stLocked);         }
    bool IsInitialized()    const { return mToolStates.Contains(stInitialized);    }

    int            CarretIndex() const { return mCarretIndex; }
    const dmShape* Carrets()     const { return mCarrets;     }
 
    void RecalcObject();
    void LockUpdate();   // Lock   UpdateObject() && Invalidate()
    void UnlockUpdate(); // Unlock UpdateObject()

    void UpdateObject(); // Call RecalcObject()

    void BeginRefresh()                    const;
    void EndRefresh( bool bRedraw = true ) const;

    void Invalidate( bool _st );

    // Acyclic Visitor pattern
    virtual void Accept( dmVisitor& );
    virtual void Accept( dmConstVisitor& ) const;
    
  public:	 
    dmTool( dmTool* _parent = NULL );	 
    dmTool( const dmTool& _toCopy, dmTool* _parent = NULL );
    virtual ~dmTool();

    bool Initialize();

    //------------------------------------------------------
    // Attach the object to a parent : this function is valid  
    // only if the object is not already attached to a parent
    //------------------------------------------------------
    void    Attach( dmTool* _parent );
    dmTool* Detach( bool _notify = true );

    //------------------------------------------------------
    // Graphics functions
    //------------------------------------------------------

    virtual dmGraphics* GetGraphics() const;

    dmSurface ObtainGraphics() const { 
       return dmSurface(GetGraphics()); 
    }

    virtual void OnDisplay      ( dmGraphics& ) const;
    virtual void OnRedrawCarrets( dmGraphics& ) const;

    void DisplayObject( dmGraphics& ) const; 
    void Display() const;
    void Redraw();

    //------------------------------------------------------
    // Moving and sizing
    //------------------------------------------------------
    void MoveBy( int dx, int dy , bool _update = true );
    void MoveBy( const dmPoint& p,bool _update = true );
 
    void SizeBy( int _index, int dx,int dy   ,bool _update=true );
    void SizeBy( int _index, const dmPoint& p,bool _update=true );
    void SizeTo( int _index, const dmPoint& p,bool _update=true );

    void MoveTo( const dmPoint& p,bool _update=true );

    const dmPoint& GetMove() const { return mMove; }
  
  public:
    dmTool* Parent()                   const { return mParent; }
    bool    IsChildOf( const dmTool* ) const;

    // Return objects working area
    const dmRegion&  Region()     const { return mRoi; }	 
    const dmRect&    ObjectRect() const { return mRoi.Rectangle(); }
  
    bool IsSelected()      const { return  mToolStates.Contains(stSelected); } 
    bool IsVisible()       const { return  mToolStates.Contains(stVisible);  }
    bool IsPositionnable() const { return !mToolStates.Contains(stPinned);   }
    bool HasFocus ()       const { return  mToolStates.Contains(stFocus);    }   
    bool IsSizable()       const; 

    bool Show  ( bool _visible );
    bool Select( bool _st      );
    
    bool SetFocus ( bool, dmTool* ); 
    void KillFocus() { SetFocus(false,NULL); }

    bool IsNullObject() const { return mRoi.IsEmptyRoi(); }

    void LockCarrets ( bool );
    void LockPosition( bool );

    virtual dmImage* GetImage() const; 

    //-------------------------------------------------------
    // Fonctions de sélections
    // par défaut elles utilisent la région associée à l'objet
    // si elle existe
    //--------------------------------------------------------
    virtual dmTool* GetObject( const dmPoint& , bool bHitTest );
    virtual dmTool* GetObject( const dmRect&  , bool bHitTest );
    virtual dmTool* GetObject( const dmRegion&, bool bHitTest );

    virtual dmTool* Clone( dmTool* _parent = NULL ) const;
    
    const dmString& ToolName() const;
    void  SetToolName( const dmString& );

  public:
    //---------------------------------------
    // Events
    //---------------------------------------
    virtual bool ProcessEvent( dmEvent& );
    virtual bool ProcessDefaultEvent( dmEvent& );

    // Prevent event bubbling for internally generated events
    void CancelBubble    ( bool _st ) { mToolStates.Set(stCancelBubble,_st);  }
    bool BubbleCancelled() const { return mToolStates.Contains(stCancelBubble); }

    dmEventNotifier* GetEventNotifier(bool _initialize=true);

    void AddEventListener   ( const dmSetOf<dmEventType>& , dmEventListener*, dm_uint _flags );
    void RemoveEventListener( const dmSetOf<dmEventType>& , dmEventListener*, dm_uint _flags );

  public:
    dmDeclareClassInfo( dmTool );
};
//--------------------------------------------------
// inline members
//--------------------------------------------------

inline  void dmTool::SizeBy( int _index, const dmPoint& p,bool _update )
{ 
   SizeBy(_index,p.x,p.y,_update ); 
}

inline void dmTool::MoveBy( const dmPoint& p,bool _update ) 
{ 
  MoveBy(p.x, p.y,_update); 
}
//--------------------------------------------------
#endif
