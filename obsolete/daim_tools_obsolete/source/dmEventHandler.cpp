
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
//-------------------------------------------------------------
bool dmEventHandler::HandleEvent( dmEvent& anEvent, dmEventTarget* aSource )
{
  if(aSource == NULL) 
     aSource = anEvent.mTarget;

  // Process Events listeners for the source
  if(aSource)
  {
    aSource->Obtain();

    // Save target
    dmEventTarget* _saved_target = anEvent.mTarget;

    // Set Current target
    anEvent.mCurrentTarget = aSource;

    dmEventNotifier* _notifier = aSource->GetEventNotifier(false);
    if(_notifier && _notifier->ProcessEvent(anEvent)) 
       anEvent.mFlags |= dmEVENT_FLAG_HANDLED;

    // Process default event 
    if(anEvent.CanDefault() && aSource->ProcessDefaultEvent(anEvent)) 
       anEvent.mFlags |= dmEVENT_FLAG_HANDLED;

    // Check if the target is hit : in this case begin the bubbling phase
    if(anEvent.mTarget == aSource && anEvent.IsCapturing())
    {
       anEvent.mFlags &= ~dmEVENT_FLAG_CAPTURE;
       anEvent.mFlags |=  dmEVENT_FLAG_BUBBLE;
     
       // Handle Bubble and propagate to parent
       dmEventTarget* _target = aSource;
       while(_target && anEvent.CanBubble()) 
       {   
         HandleEvent(anEvent,_target);
         _target = _target->Parent();
       }

       // Restore capture for this event
       anEvent.mFlags &= ~dmEVENT_FLAG_BUBBLE;
       anEvent.mFlags |=  dmEVENT_FLAG_CAPTURE;   
       anEvent.mTarget =  _saved_target;    
    }
   
    anEvent.mCurrentTarget = NULL;
    aSource->Release();
  }

  return anEvent.IsHandled();
}
//-------------------------------------------------------------
void dmEventHandler::InitEvent( dmEventTarget* aTarget, dmEvent& anEvent, dmEventType aType, dm_uint aFlags )
{
  anEvent.mTarget        = aTarget;
  anEvent.mEventType     = aType;
  anEvent.mRelatedTarget = NULL;
  anEvent.mFlags         = ( aFlags & ~(dmEVENT_FLAG_CAPTURE|dmEVENT_FLAG_BUBBLE) );
  anEvent.mCursorType    = eCursor_undefined;

  if(aFlags & dmEVENT_FLAG_INIT) {
    anEvent.mFlags |=  dmEVENT_FLAG_CAPTURE;
    anEvent.mFlags &= ~dmEVENT_FLAG_INIT;
  }
  else
    anEvent.mFlags |= dmEVENT_FLAG_BUBBLE;
}
//-------------------------------------------------------------
// Generate a basic events from specified target
// Event is generated on the stack
//-------------------------------------------------------------
bool dmEventHandler::GenerateEvent( dmEventTarget* aTarget, dmEventType aType, 
                                    bool _bubble, dm_int _result )
{
  if(aTarget) 
  {
    dmEventNotifier* _notifier = aTarget->GetEventNotifier(false);

    if(_bubble || _notifier) 
    {
      aTarget->Obtain();

      // Generate a bubbling event
      dmEvent theEvent;
      dmEventHandler::InitEvent(aTarget,theEvent,aType,0);

      theEvent.mResult        = _result;
      theEvent.mCurrentTarget = aTarget;

      if(_notifier && _notifier->ProcessEvent(theEvent)) 
        theEvent.mFlags |= dmEVENT_FLAG_HANDLED;

      if(_bubble) 
      {
        dmEventTarget* _target = aTarget->Parent();
        while(_target && theEvent.CanBubble()) {
          dmEventHandler::HandleEvent(theEvent,_target);
          _target = _target->Parent();
        }
      }
      aTarget->Release();
      return theEvent.IsHandled();
    }
  }
  return false;
}
//-------------------------------------------------------------
