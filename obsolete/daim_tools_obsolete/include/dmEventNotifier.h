#ifndef dmEventNotifier_h
#define dmEventNotifier_h

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
// File         :  dmEventNotifier.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------


struct __dmToolsForward dmNotifyPort;
//---------------------------------------------------------
// A dmEventHandler can accept event notifications
// from multiple sources through the dmNotifyPort instance
//---------------------------------------------------------
class __dmTools dmEventListener : public dmReferenced
{ 
  private:
    dmNotifyPort*   mNotifyPort;

  public:
    dmEventListener();
    virtual ~dmEventListener();
       
    // Return true if the event is handled
    virtual bool ProcessEvent( dmEvent& );

    // Called when the the last notifier hangs up
    virtual void HangUp();

    bool IsListening() const { return mNotifyPort != NULL; }
    void InvalidatePort();

    dmNotifyPort* GetPort();

    typedef bool (dmEventListener::*pfnHandler)( dmEvent& );
};
//---------------------------------------------------
// This object ensure that it is safe to destroy
// the notifier or the notifiable object 
//---------------------------------------------------
struct __dmTools dmNotifyPort : public dmShared
{
  dmEventListener* mEventListener;   
  dmEventListener* operator->() { return mEventListener; }

  bool IsValid() { return mEventListener != NULL; }

  virtual int Release();
  virtual ~dmNotifyPort();
};
//---------------------------------------------------
class __dmTools dmEventNotifier : public dmEventListener
{
  public:
    dmEventNotifier();
    virtual ~dmEventNotifier();

  private:
    struct __NotifyData {
       dmNotifyPort*  no_Port;
       dmInputEvents  no_Events;
       dm_uint        no_Flags;
    };

    int nRecurse;

    dmTList<__NotifyData> no_List;

    void CleanUp();
          
  protected:
    typedef dmTList<__NotifyData>::iterator iterator;
    typedef dmTList<__NotifyData>::const_iterator const_iterator;
     
    iterator Begin() { return no_List.Begin(); }
    iterator End()   { return no_List.End();   }
    
    const_iterator Begin() const { return no_List.Begin(); }
    const_iterator End()   const { return no_List.End();   }

    iterator Find( iterator, dmNotifyPort* );
    void     Remove( iterator it  );

  public:
    void Clear();   
    void AddEventListener   ( const dmSetOf<dmEventType>& , dmEventListener*, dm_uint _flags );
    void RemoveEventListener( const dmSetOf<dmEventType>& , dmEventListener*, dm_uint _flags );

  public:
    virtual bool ProcessEvent( dmEvent& );

};
//-------------------------------------------------------------------------
#endif
