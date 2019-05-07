#ifndef dmEventHandler_h
#define dmEventHandler_h

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
// File         : dmEventHandler.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class dmEventHandler
{
  public:
   static __dmTools bool HandleEvent  ( dmEvent& , dmEventTarget* );
   static __dmTools void InitEvent    ( dmEventTarget*, dmEvent& anEvent, dmEventType, dm_uint aFlags );
   static __dmTools bool GenerateEvent( dmEventTarget*, dmEventType, bool _bubble, dm_int _result = -1 );
};

#define dmHANDLE_EVENT( anEvent, aTarget ) \
  dmEventHandler::HandleEvent(anEvent,aTarget)
 
#define dmGENERATE_EVENT( aTarget, aType, doBubble )  \
  dmEventHandler::GenerateEvent(aTarget,aType,doBubble)

#define dmGENERATE_RESULT_EVENT( aTarget, aType, aResult, doBubble  )  \
  dmEventHandler::GenerateEvent(aTarget,aType,doBubble,aResult)

#define dmGENERATE_NOTIFY_EVENT( aTarget, aType )  \
  dmEventHandler::GenerateEvent(aTarget,aType,false)

#define dmGENERATE_NOTIFY_RESULT_EVENT( aTarget, aType, aResult )  \
  dmEventHandler::GenerateEvent(aTarget,aType,false,aResult)

#endif // dmEventHandler_h
