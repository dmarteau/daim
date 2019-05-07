
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
// Class dmEventListener
//-------------------------------------------------------------
dmEventListener::dmEventListener()
: mNotifyPort(NULL)
{
}
//-------------------------------------------------------------
dmEventListener::~dmEventListener() 
{  
  InvalidatePort();
  dmASSERT( References() == 0 );
}
//-------------------------------------------------------------
void dmEventListener::InvalidatePort()
{
  if(mNotifyPort) 
  {
    mNotifyPort->mEventListener = NULL; // invalidate port
    mNotifyPort->Release();
    mNotifyPort = NULL;
  } 
}
//-------------------------------------------------------------
dmNotifyPort* dmEventListener::GetPort()
{
  if(!mNotifyPort) 
  {
    mNotifyPort = new dmNotifyPort;
    mNotifyPort->mEventListener = this;
  }
  return mNotifyPort;
}
//-------------------------------------------------------------
void dmEventListener::HangUp()
{
  InvalidatePort();
}
//-------------------------------------------------------------
bool dmEventListener::ProcessEvent( dmEvent& _evt )
{
  return false;
}
//-------------------------------------------------------------
// Class dmNotifyPort
//-------------------------------------------------------------
dmNotifyPort::~dmNotifyPort()
{
  dmASSERT( References() <= 0 );
}
//-------------------------------------------------------------
// Normalement il n'y a pas de problème :
// un port est toujours maintenu par le "EventHandler" à l'écoute
// res == 1 signifie qu'il n'ya plus personne susceptible d'envoyer des messages
// et donc le port est invalidé.
//-------------------------------------------------------------
__dmTools int dmNotifyPort::Release()
{
  dmEventListener* no = mEventListener;  // Attention après "Shared::release()"  
  int refs = dmShared::Release();        // 'this' n'est peut être plus valide !!!
  if(no && refs<=1) no->HangUp();
  return  refs;
}
//-------------------------------------------------------------
// Class dmEventNotifier
//-------------------------------------------------------------
dmEventNotifier::dmEventNotifier()
: nRecurse(0)
{
}
//-------------------------------------------------------------
dmEventNotifier::~dmEventNotifier()
{
  Clear();
}
//-------------------------------------------------------------
void dmEventNotifier::Clear()
{
  iterator _end = End();
  for(iterator it = Begin();it!=_end;++it)
    (*it).no_Port->Release();

  no_List.Clear();
}
//-------------------------------------------------------------
void dmEventNotifier::Remove( iterator it  ) 
{ 
  iterator next = End();
  if(it!=End()) {
    (*it).no_Port->Release();
    no_List.Remove(it); 
  }
}
//------------------------------------------------------------- 
dmEventNotifier::iterator dmEventNotifier::Find( iterator _position, dmNotifyPort* _port )
{
  if(_port) {
    iterator _end = End();
    for(iterator it=_position;it!=_end;++it) {
      if((*it).no_Port == _port)
        return it;
    }
  }
  return End();
}
//------------------------------------------------------------- 
void dmEventNotifier::AddEventListener( const dmSetOf<dmEventType>& _evts , dmEventListener* _evh, dm_uint _flags )
{
  if(_evh) 
  {
    dmNotifyPort* _port = _evh->GetPort();

    _port->Obtain();

    __NotifyData noData;
    noData.no_Port    = _port;
    noData.no_Events  = _evts;
    noData.no_Flags   = (_flags & dmEVENT_FLAG_DISPATCH_MASK);

    no_List.Push_Back( noData ); 
  }
}
//-------------------------------------------------------------
void dmEventNotifier::RemoveEventListener( const dmSetOf<dmEventType>& _evts , dmEventListener* _evh, dm_uint _flags  )
{
  // Search for the corresponding element
  if(_evh && _evh->IsListening()) 
  {
    _flags &= dmEVENT_FLAG_DISPATCH_MASK;

    iterator _end = End();
    iterator it   = Begin();
    do {
      it = Find(it,_evh->GetPort());
      if(it!=_end && (*it).no_Events == _evts && ((*it).no_Flags & _flags))
      {
        Remove(it);
        return;
      }
    } while ( it != _end );
  }
}
//------------------------------------------------------------- 
void dmEventNotifier::CleanUp()
{
  iterator it = Begin();
  while( it != End() ) {
    if( !(*it).no_Port->IsValid() ) 
    {
      (*it).no_Port->Release();
      it = no_List.Erase(it);
    } 
    else ++it;
  }
}
//------------------------------------------------------------- 
bool dmEventNotifier::ProcessEvent( dmEvent& _evt )
{
  bool _isHandled = false;
  dmNotifyPort* no_Port;

  ++nRecurse;

  iterator _end = End();
  for(iterator it = Begin();it!=_end;++it) 
  {
    if((*it).no_Events.Contains(_evt.mEventType) && 
      ((*it).no_Flags & _evt.mFlags))
    {
      no_Port = (*it).no_Port;
      if(no_Port->IsValid()) 
      {
        _isHandled |= (*no_Port)->ProcessEvent(_evt);
      }
    }

    // Stop propagation if the event has been cancelled
    if(!_evt.IsValid()) 
        break;
  }

  if(--nRecurse<=0)
     CleanUp();
 
  return _isHandled;
}
//-------------------------------------------------------------


