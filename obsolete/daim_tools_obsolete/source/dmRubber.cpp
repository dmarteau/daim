
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
#include "dmRubber.h"

#define GENERATE_EVENT( aType )  \
  dmGENERATE_EVENT(this,aType,!this->BubbleCancelled());

#define GENERATE_RESULT_EVENT( aType, aResult )  \
  dmGENERATE_RESULT_EVENT(this,aType,aResult,!this->BubbleCancelled());

//------------------------------------------------------
// Rubber
//------------------------------------------------------
dmRubber::dmRubber( dmTool* parent ) 
:dmTool(parent) 
,mActivated(false)
{
  SetCarrets( &mRect );
}
//-------------------------------------------------------
void dmRubber::Activate( bool bActive )
{
  mActivated = bActive;
}
//-------------------------------------------------------
bool dmRubber::ProcessDefaultEvent( dmEvent& evt )
{
  if(!mActivated) 
     return false;

  // Handle events only in capture phase
  if(evt.IsCapturing())
  {
    evt.mTarget = this;

    if(evt.mStructType==evStruct_MouseEvent) 
    {
      dmMouseEvent& _evt      = static_cast<dmMouseEvent&>(evt);
      dmGraphics*   _graphics = GetGraphics();

      mCursor = _evt.mCursor;

      switch(_evt.mEventType)
      {
        case evMouseMove: 
          if(_evt.mButton & buttonLeft) { 
            mRect.MoveCarret(_graphics,2,mCursor - mRect[2] );
            return true;
          }
          mCursor = mRect[2]; // Process evMouseUp
 
        case evMouseUp:
          if(mActivated) 
          {
             mRect.EndMove(_graphics,2,mCursor - mRect[2]);
             GENERATE_RESULT_EVENT(evCapture,dm_false)   
             Activate(false);

             if(_graphics)
                _graphics->ReleaseGraphics();

             if(Parent()) Parent()->GetObject( mRect, true );
             return true;
          }
          break;

         case evMouseDown: 

          // We do an ObtainGraphics() in order to initialize
          // the graphics so that following calls to 
          // ObtainGraphics() will not (re)allocate ressources
           
          if(_graphics)
             _graphics->ObtainGraphics();

          mRect = dmRect(mCursor,mCursor);
          GENERATE_RESULT_EVENT(evCapture,dm_true);
          return true;

         default: break;
      }
    } 
  }
  return false; // Event is not handled
}
//------------------------------------------------------
