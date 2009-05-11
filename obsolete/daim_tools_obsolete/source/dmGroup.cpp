
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

#include "daim_tools.h"
#include "dmGroup.h"
//------------------------------------------------------------
#define dmGroup_SCHEMA_VERSION 1
dmImplementClassInfo( dmGroup, dmToolGroup, dmGroup_SCHEMA_VERSION );
//------------------------------------------------------------
dmGroup::dmGroup( dmTool* _parent  ) 
: dmToolGroup(_parent)
 ,mTestMove(false)
 ,mMoving(false)
{
  SetCarrets( &mRect );
}
//------------------------------------------------------------
dmGroup::dmGroup( const dmGroup& _toCopy , dmTool* _parent  ) 
: dmToolGroup(_toCopy,_parent)
 ,mTestMove(false)
 ,mMoving(false)
 ,mRect(_toCopy.mRect)
{
  SetCarrets( &mRect );
}
//------------------------------------------------------------
dmGroup::~dmGroup()
{}
//------------------------------------------------------------
void dmGroup::OnUpdateRegion( dmRegion& _rgn )
{
  if(!IsCarretsEnabled()) {
    _rgn.KillRoi();
    iterator it   = Begin();
    iterator last = End();
    for(;it!=last;++it) _rgn.AddRoi( (*it)->Region() );
  } else mRect.BuildRegion(_rgn);
}
//------------------------------------------------------------
bool dmGroup::InitialUpdate()
{
  return dmToolGroup::InitialUpdate();
}
//-----------------------------------------------------
void dmGroup::OnMove( int dx, int dy,bool _update )
{
  mRect.FrameMove(false);
  dmToolGroup::OnMove(dx,dy,_update);
}
//-----------------------------------------------------
bool dmGroup::OnShow( bool _visible  )
{ 
  return  dmToolGroup::OnShow( _visible );
}
//------------------------------------------------------------
void dmGroup::Recalculate()
{
  if(!Empty()) {
    iterator it   = Begin();
    iterator last = End();
    dmRect _r((*it)->ObjectRect());
    for(++it;it!=last;++it) {
      _r.Add((*it)->ObjectRect());
    }
    mRect = _r;
  }
  // par défaut : dmTooGroup recalcule tout les objets 
  // => on update seulement la région
  UpdateRegion();
}
//------------------------------------------------------------
#define MOVETEST 5

bool dmGroup::ProcessDefaultEvent( dmEvent& anEvent )
{
  if(anEvent.IsCapturing())
  {
    if(IsNullObject()) 
       return false;

    anEvent.mTarget = this;

    if(anEvent.mStructType==evStruct_MouseEvent) 
    {
      dmMouseEvent& _evt = static_cast<dmMouseEvent&>(anEvent);
      switch(_evt.mEventType) 
      {
        case evMouseDown : 
          mMouseDown = _evt.mCursor;
          mTestMove  = false;
          mMoving    = GetObject(mMouseDown,true)==this;
          SetCarretIndex(-1);
          return mMoving;

        case evMouseMove : 
          if(mMoving) 
          {
            if(!mTestMove && IsPositionnable()) {
              mTestMove =  
               ( daim::abs(_evt.mCursor.x - mMouseDown.x) > MOVETEST) || 
               ( daim::abs(_evt.mCursor.y - mMouseDown.y) > MOVETEST);
            }   
            if(_evt.mButton & buttonLeft) 
            {
              if(mTestMove) {
                MoveBy(_evt.mCursor - mMouseDown,false);
                mMouseDown = _evt.mCursor;
              }
              return mMoving;
            }
          }

        case evMouseUp:
          if(mMoving) {
            if(mTestMove) 
              MoveBy(_evt.mCursor - mMouseDown,true);
            mMoving=false;
            return true;
          }

        default:break;
      }
    }
  }
  return false;
}
//-------------------------------------------------------  
dmTool* dmGroup::GetObject( const dmPoint& p, bool bHitTest ) 
{
  return dmTool::GetObject( p, bHitTest ); 
} 
//------------------------------------------------------------
dmTool* dmGroup::Clone( dmTool* _parent ) const
{
  return new dmGroup(*this,_parent);
} 
//------------------------------------------------------------
