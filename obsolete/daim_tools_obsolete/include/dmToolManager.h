#ifndef dmToolManager_h
#define dmToolManager_h

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
// File         : dmToolManager.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#include "dmRubber.h"
//---------------------------------------------------
// ToolManager
//---------------------------------------------------
class __dmTools dmToolManager : public dmToolGroup
{   
  private:	
     dmGraphics*       mGraphics;
     dmTool*           mCurrentTool;

     bool              mSelectTopAllways;
     bool              mLockNotify;
     bool              mHasMoved; 
     bool              mEnableCapture;
     bool              mEnableMultiSelection;

     dm_uint           mSelectCount;

     dmPoint           mMouseDown,mMouseOffset;
     dmRubber          mRubber;
     dmTList<dmTool*>  mWorkList;

     ECursorType       mDefaultCursorType;
     
  protected:	
     dmRect mRect;

     dmToolManager& operator=( const dmToolManager& ) { return *this; }

     virtual void OnUpdateRegion  ( dmRegion& );
     virtual void OnNotifyDetach  ( dmTool*   );
     virtual void OnGroupSelection( dmGroup*  );
     virtual void ClearAndDestroy();

     dmRubber& Rubber() { return mRubber; }

  public:	
     typedef dmTList<dmTool*>::iterator sel_iterator;

     dmToolManager( dmTool* _parent  = NULL );
     dmToolManager( const dmRect& , dmGraphics* gr = NULL);
 
     virtual ~dmToolManager();	 
  
     void                SetGraphics( dmGraphics* );
     virtual dmGraphics* GetGraphics() const;
    
     void          SetRect( const dmRect& );
     const dmRect& GetRect() const { return mRect; }

     sel_iterator Selection_Begin() { return mWorkList.Begin(); }
     sel_iterator Selection_End()   { return mWorkList.End();   }

     bool   MultiSelection() const { return mSelectCount > 1; }
     size_t SelectCount()    const { return mSelectCount;     }
   
  public:	 

     virtual void OnDisplay( dmGraphics& ) const;
     
     virtual dmTool* GetObject( const dmRect&  , bool bHitTest );
     virtual dmTool* GetObject( const dmRegion&, bool bHitTest );
     virtual dmTool* GetObject( const dmPoint& , bool bHitTest );

     virtual dmTool* Clone( dmTool* _parent = NULL ) const;

     dmTool* GetCurrentTool();

     void SelectTopAllways( bool );
     void SafeRemove( dmTool*  );
     bool SelectAll( bool _select );
     
     void EnableMultiSelection( bool bEnable );

     int MoveSelection( int dx,int dy,bool _update );
     int MoveSelection( const dmPoint& _offs,bool _update ) {
       return MoveSelection(_offs.x,_offs.y,_update); 
     }

     dmTool* GroupSelection();
     void    UngroupSelection();

     // Send selection to back/to front
     void SelectionToBack ();
     void SelectionToFront();
     void SelectionForward();
     void SelectionBackward();

     void AlignSelection( dm_uint );

     int  ClearSelection();
     void UpdateSelection();

     // Cut an paste operztions
     size_t CopySelection ( dmToolGroup& rBuffer ) const;
     size_t CutSelection  ( dmToolGroup& rBuffer );
     size_t PasteSelection( const dmToolGroup& rBuffer );

     bool GetRectSelection( dmRect&   );
     bool GetRgnSelection ( dmRegion& );

     bool IsCapturing();
     bool GetCaptureRect( dmRect& );
     void EnableCapture( bool bEnable );

     virtual bool ProcessEvent(  dmEvent& );
     virtual bool ProcessDefaultEvent(  dmEvent& );

     ECursorType SetDefaultCursor( ECursorType );

     dmDeclareClassInfo( dmToolManager );

  protected:
     virtual bool ProcessMouseEvent   ( dmMouseEvent& );
     virtual bool ProcessKeyEvent     ( dmKeyEvent&   );
     virtual bool ProcessBubblingEvent( dmEvent&      );

};
//---------------------------------------------------
#endif // ToolManager.h
