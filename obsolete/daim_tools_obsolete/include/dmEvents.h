#ifndef dmEvents_h
#define dmEvents_h

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
// File         :  dmEvents.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#include <math.h>

dmDefineSetOf( dmEventType, evNoEvents );

//-----------------------------------------------------
class dmEvent
{
  public:
    dmEventType       mEventType;
    dmEventStructType mStructType;
    dm_uint32         mFlags;
    dm_int32          mResult;

    dmEventTarget*    mTarget;         // Destination target
    dmEventTarget*    mRelatedTarget;
    dmEventTarget*    mCurrentTarget;  // Current processing target
    ECursorType       mCursorType;

  public :
    ~dmEvent() {}
     dmEvent()
     :mEventType(evNoEvents)
     ,mStructType(evStruct_GeneralEvent)
     ,mFlags(dmEVENT_FLAG_NONE)
     ,mResult(-1)
     ,mTarget(NULL)
     ,mRelatedTarget(NULL)
     ,mCurrentTarget(NULL)
     {}

    void StopPropagation() { 
      if(!(mFlags &  dmEVENT_FLAG_CANT_CANCEL))
       mFlags |= dmEVENT_FLAG_STOP_DISPATCH; 
    }

    void CancelDefault() { mFlags |= dmEVENT_FLAG_NO_DEFAULT; }

    bool CanCapture()  const { return !(mFlags & dmEVENT_FLAG_STOP_DISPATCH); }
    bool CanBubble()   const { return !(mFlags & (dmEVENT_FLAG_CANT_BUBBLE|dmEVENT_FLAG_STOP_DISPATCH)); }
    bool CanDefault()  const { return !(mFlags & (dmEVENT_FLAG_NO_DEFAULT |dmEVENT_FLAG_DISCARDED)); }

    bool IsCapturing() const { return (mFlags & dmEVENT_FLAG_CAPTURE)!=0; }
    bool IsBubbling()  const { return (mFlags & dmEVENT_FLAG_BUBBLE )!=0; }
    bool IsHandled()   const { return (mFlags & dmEVENT_FLAG_HANDLED)!=0; }

    bool IsValid()     const { return !(mFlags & dmEVENT_FLAG_DISCARDED );}

    void PreventBubble() {
      if (mFlags & dmEVENT_FLAG_BUBBLE)
        mFlags |= dmEVENT_FLAG_STOP_DISPATCH;
    }

    void PreventCapture() {
      if (mFlags & dmEVENT_FLAG_CAPTURE)
        mFlags |= dmEVENT_FLAG_STOP_DISPATCH;
    }

    void PreventDefault() {
      if (!(mFlags & dmEVENT_FLAG_CANT_CANCEL))
        mFlags |= dmEVENT_FLAG_NO_DEFAULT;
    }

    void Discard() {
      mFlags |= (dmEVENT_FLAG_STOP_DISPATCH | dmEVENT_FLAG_DISCARDED); 
    }
};
//-----------------------------------------------------
class dmMouseEvent : public dmEvent
{
  public:
    dm_uint  mButton;
    dm_uint  mKeyQual;
    dmPoint  mCursor;

    bool ShiftKey() const { return (mKeyQual & keyShift)!=0; }
    bool CtrlKey () const { return (mKeyQual & keyCtrl) !=0; }
    bool AltKey  () const { return (mKeyQual & keyAlt ) !=0; }

    dmMouseEvent() { 
      mStructType = evStruct_MouseEvent; 
    }
};
//-----------------------------------------------------
class dmKeyEvent : public dmEvent
{
  public:
    dm_uint mKeyCode;
    dm_uint mKeyQual;

    bool ShiftKey() const { return (mKeyQual & keyShift)!=0; }
    bool CtrlKey () const { return (mKeyQual & keyCtrl) !=0; }
    bool AltKey  () const { return (mKeyQual & keyAlt ) !=0; }

    dmKeyEvent() { 
      mStructType = evStruct_KeyEvent; 
    }
};
//---------------------------------------------------
#define dmMAX_EVENT_STRUCT_SIZE() \
  MAX(sizeof(dmKeyEvent),sizeof(dmMouseEvent))
//---------------------------------------------------
class dmInputEvents : public dmSetOf<dmEventType> 
{
   public:
    dmInputEvents() : dmSetOf<dmEventType>( dmSetOf<dmEventType>::All() ) {}
    dmInputEvents(dmEventType _ev) : dmSetOf<dmEventType>(_ev) {}
    dmInputEvents(const dmSetOf<dmEventType>& _set) : dmSetOf<dmEventType>(_set) {}
   ~dmInputEvents() {}
};
//--------------------------------------------------------
#endif // EventsH
