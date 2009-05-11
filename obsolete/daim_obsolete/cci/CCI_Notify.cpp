
/* :::BEGIN LICENSE BLOCK:::
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
 *  :::END LICENSE BLOCK::: */

///////////////////////////////////////////////////////////////////
// This class define a base class for every CCI Object
///////////////////////////////////////////////////////////////////
#define dmUseCCI
#define dmUseUtilitiesExtra
#include "daim_utilities.h"
#include "daim_modules/CCI_Notify.h"

#define CCIC_NAME  CCIC_Notify
#define CCIC_SUPER NULL
#define CCIC_SUPER_VERSION TGZ_DEFAULT
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Notify_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2001-2005"))

#define CCI_INTERNAL_MODULE
#include "daim_modules/CCI_Base.h"
#include "daim_modules/CCI_Interface.h"
//----------------------------------------------------------------
struct Notify_Port : public dmShared
{
  static dmFastMemory<Notify_Port> _MemPool;

  cci_Object* mObject;
  dmMutex     mNotifyMutex;
  
  Notify_Port(cci_Object* o) : mObject(o) {}
  virtual ~Notify_Port() {}
  
  Notify_Port* Share()      { Obtain(); return this; }
  void         Invalidate() { mObject = NULL; Release(); }

  bool IsValid() const { return mObject!=NULL; }
  void* operator new(size_t)       { return _MemPool.Allocate(128); }
  void  operator delete( void* p ) { _MemPool.Free(p);  }
};

dmFastMemory<Notify_Port> Notify_Port::_MemPool;

//----------------------------------------------------------------

// Structure for CCIM_Set
struct SetMsg { dm_param me; cci_TagItem tg; };

struct Notification
{
  dmNode       no_Node;
  cci_TagItem  no_Trig;
  cci_Data     no_LastValue;
  dm_uint      no_Flags;
  Notify_Port *no_Port;
  bool         no_Removed;
  dm_uint      no_Size;
  cci_Object*  no_Dest; //
  // ...CCI message follows...
};

#define METHOD(no) (cci_Msg*)( reinterpret_cast<Notification*>(no) + 1 )
#define SIZEOF_ARG( msglength ) (sizeof( struct Notification ) + msglength)

#define dmNOTIFY_THREAD_LOCK(n)       \
    if(n->no_Flags & CCIV_LockNotify) \
      n->no_Port->mNotifyMutex.Lock();

#define dmNOTIFY_THREAD_UNLOCK(n)     \
    if(n->no_Flags & CCIV_LockNotify) \
      n->no_Port->mNotifyMutex.Unlock();

//---------------------------------------------------------------
struct NotifyStruct
{
  dm_uint  NoCount;      // Count nested levels of notifications
  bool     NoRefresh;    // Need to refresh notification list ?
  dmList   Notify_list;
};
//---------------------------------------------------------------
CCI_DECL_DATA( Notify )
{
  dm_param  UserData;
  dm_param  ObjectID;
  dm_uint   RefCount;

  Notify_Port  *NoPort;
  NotifyStruct *NoStruct;
};
//---------------------------------------------------------------
static void Notify_InitNotifyStruct( CCI_INSTANCE_PTR(Notify) _This )
{
  if(_This->NoStruct == NULL ) 
  {
    NotifyStruct* _NoStruct = new  NotifyStruct;
    _NoStruct->NoCount      = 0;
    _NoStruct->NoRefresh    = false;

    _This->NoStruct = _NoStruct;
  }
}
//---------------------------------------------------------------
static void Notify_DoNotify(NotifyStruct* _NoStruct,cci_TagItem *tag)
{
  dmList::iterator it  = _NoStruct->Notify_list.Begin();
  dmList::iterator end = _NoStruct->Notify_list.End();
  
  cci_Msg*    no_msg;
  cci_Object* no_Dest;
  
  struct Notification* n;

  for(;it!=end;++it)
  {
    n = reinterpret_cast<struct Notification*>(*it);
    if(!n->no_Removed)
    {
      //-----------------------
      // Check for invalid port
      //-----------------------
      if(!n->no_Port->IsValid())
      {
        _NoStruct->NoRefresh = n->no_Removed = true; 
        continue;
      }
      //-----------------------
      // Check for trigger
      //-----------------------
      if( n->no_Trig.ti_Tag == tag->ti_Tag )
      {
        if( (n->no_Flags & CCIV_DoNotCheck) || n->no_LastValue != tag->ti_Data) 
        {
          //-----------------------
          // Replace the current value
          // and check for trigger
          //-----------------------
          n->no_LastValue = tag->ti_Data;       
          if( (n->no_Flags & CCIV_EveryTime) || n->no_Trig.ti_Data == tag->ti_Data ) 
          {
            no_Dest = n->no_Port->mObject;
            no_msg  = METHOD(n);

            //-------------------------------
            // Get lock on the notify port 
            // of the dest object
            //-------------------------------
            dmNOTIFY_THREAD_LOCK(n)

            if(no_msg->Id==CCIM_DECL_(Notify,Map)) { 
              CCI_Set(no_Dest,
                       reinterpret_cast<CCIP_DECL_(Notify,Map)*>(no_msg)->Attr,
                       tag->ti_Data);
            } else 
              CCI_DoMethod(no_Dest,no_msg);

            dmNOTIFY_THREAD_UNLOCK(n)
          }
        }
	  }
    }
  }
}
//--------------------------------------------------------------------
static void Notify_KillNotification( struct Notification* n )
{
  dmDestroy(&n->no_Node);
  n->no_Port->Release();
  dmMemory::Free(n);
}
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( Notify )
{
  CCI_INSTANCE( Notify )

  _This->UserData  = 0;
  _This->ObjectID  = 0;
  _This->NoPort    = NULL;
  _This->NoStruct  = NULL;

  // Add a reference at creation for the caller

  _This->RefCount  = 1;

  // Set [I] Attributs
  CCI_BEGIN_INIT_MAP(Notify)
    CCI_ATTR_SET( Notify, UserData, _This->UserData = CCIA_DATA )
    CCI_ATTR_SET( Notify, ObjectID, _This->ObjectID = CCIA_DATA ) 
  CCI_END_INIT_MAP()

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( Notify )
{
  CCI_INSTANCE( Notify )

  //_This->ChildNode.self = NULL;
  
  if(_This->NoPort) 
     _This->NoPort->Invalidate(); 

  if(_This->NoStruct)
  {
    NotifyStruct* _NoStruct = _This->NoStruct;

    while( !_NoStruct->Notify_list.Empty() ) {
      Notify_KillNotification(
       reinterpret_cast<Notification*>(_NoStruct->Notify_list.Pop_Back())
      );
    }

    delete _NoStruct;
    _This->NoStruct = NULL;

  }

  // Tell the CCI system that our object
  // may be handled by someone

  if(_This->RefCount > 0) 
    CCI_RETURN_FAIL()
  
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static CCI_METHOD_SET( Notify ) 
{
   CCI_INSTANCE( Notify )

   bool no_flag = true;

   CCI_BEGIN_SETTER_MAP()
     CCI_ATTR_SET( Notify, NoNotify , no_flag = (CCIA_DATA==dm_false) )
     CCI_ATTR_SET( Notify, ObjectID , _This->ObjectID = CCIA_DATA  )
     CCI_ATTR_SET( Notify, UserData , _This->UserData = CCIA_DATA  )
     CCIA_DISCARD( Notify, RefCount ) 

   CCI_SETTER_PMAP( 

     if(_This->NoStruct) {
       NotifyStruct* _NoStruct = _This->NoStruct;
       // Begin notification process
       if(no_flag && !_NoStruct->Notify_list.Empty()) { 
         ++_NoStruct->NoCount; 
         Notify_DoNotify(_NoStruct,tag);
         --_NoStruct->NoCount; 
       }
     }

   )
   CCI_END_SETTER_PMAP()

  if(_This->NoStruct) 
  {
     NotifyStruct* _NoStruct = _This->NoStruct;
     //-------------------------------------------------
     // Clear notification list if Delayed Kill_Notify
     //-------------------------------------------------
     if(_NoStruct->NoCount<=0 && _NoStruct->NoRefresh) 
     {
       //-------------------------------------------------
       // The object may have been deleted during the notification
       // process
       //-------------------------------------------------
       if(_This->RefCount==0) {
          CCI_FreeObject(_THIS_OBJECT);
       } else {
         dmList& no_list = _NoStruct->Notify_list;
         dmList::iterator it = no_list.Begin();
         while(it!=no_list.End()) 
         {
           Notification* n = reinterpret_cast<Notification*>(*it); 
           ++it; // inc. before deletion

           if(n->no_Removed || !n->no_Port->IsValid())
              Notify_KillNotification(n);
         }
         _NoStruct->NoRefresh = false;
       } 
     } 
   }
   CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static CCI_METHOD_GET( Notify ) 
{
   CCI_INSTANCE( Notify )

   CCI_BEGIN_GETTER_MAP()
     CCI_ATTR_GET( Notify, UserData , CCIA_RETVAL = _This->UserData )
     CCI_ATTR_GET( Notify, ObjectID , CCIA_RETVAL = _This->ObjectID )
     CCI_ATTR_GET( Notify, RefCount , CCIA_RETVAL = _This->RefCount )
   CCI_END_GETTER_MAP()

   CCI_RETURN_NOT_IMPLEMENTED()
}
//---------------------------------------------------------------------
// Method: NotifySet
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Notify, NotifySet )
{
  return CCI_SetAttrs(_THIS_OBJECT,&CCI_P(tg));
}
//---------------------------------------------------------------------
// Method: NoNotifySet
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Notify, NoNotifySet )
{
   tg_TagItem  _tags[] = { 
      { CCIA_DECL_(Notify,NoNotify),dm_true },
      { TGZ_MORE, reinterpret_cast<cci_Data>(&CCI_P(tg)) }
   };
   
   return CCI_SetAttrs( _THIS_OBJECT, _tags );
}
//---------------------------------------------------------------------
// Method: NotifyMsg
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Notify, NotifyMsg )
{
  CCI_INSTANCE( Notify )

  Notify_InitNotifyStruct(_This); // Create the notify structure

  struct  Notification *buf;
  dm_param lastValue;

  cci_Object* objDest = NULL;

	// Get the Current value of our object attribut return NULL if not found
  if( CCI_FAILED(CCI_GetAttrs(_THIS_OBJECT,CCI_P(Attr),&lastValue)) ) {
    dmLOG(_TXT("NOTIFICATION ERROR: Cannot get property...\n"));
    CCI_RETURN_FAIL()
  }

  if( (CCI_P(Opts) & CCIV_NotifySelf)||CCI_P(ObjDest)==NULL ) objDest = _THIS_OBJECT;
  else objDest = CCI_P(ObjDest); 

  // Get the notify port
  CCI_INSTANCE_PTR(Notify) objdata = CCI_GetInstData(CCI_DECL_DATA_(Notify),_THIS_CLASS,objDest);
  if(!objdata->NoPort) 
    objdata->NoPort = new Notify_Port(objDest);

  buf = static_cast<Notification*>( dmMemory::Malloc(SIZEOF_ARG(CCI_P(MsgLength))) ); 
    
  buf->no_Flags        = CCI_P(Opts);
  buf->no_Trig.ti_Tag  = CCI_P(Attr);
  buf->no_Trig.ti_Data = CCI_P(Val); // Unusued if 'CCIV_EveryTime' set  
  buf->no_Size         = CCI_P(MsgLength);
  buf->no_LastValue    = lastValue;
  buf->no_Port         = objdata->NoPort->Share();
  buf->no_Removed      = false;
  buf->no_Dest         = (CCI_P(ObjDest) ? CCI_P(ObjDest) : objdata->NoPort->mObject);
 
  dmConstruct(&buf->no_Node); // construction du noeud
  
  // copy du message
  dmMemory::Memcpy( METHOD(buf),CCI_P(Msg),CCI_P(MsgLength));
  _This->NoStruct->Notify_list.Push_Back( &buf->no_Node );

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: Notify
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Notify, Notify )
{
  CCIP_DECL_(Notify,NotifyMsg) _NoMsg;
  dmMemory::Memcpy(&_NoMsg,_This_msg,sizeof(CCIP_DECL_(Notify,NotifyMsg)));
  _NoMsg.Msg = reinterpret_cast<cci_Msg*>(_This_msg+1); // Set Address of message

  return CCI_DECL_METHOD_(Notify,NotifyMsg)(_THIS_CLASS,_THIS_OBJECT,&_NoMsg);
}
//---------------------------------------------------------------------
// Method: KillNotify
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Notify, KillNotify )
{
  CCI_INSTANCE( Notify )

  NotifyStruct* _NoStruct = _This->NoStruct;

  if(_NoStruct!=NULL)
  {
    cci_Object* o  = CCI_P(object);
    cci_Tag     tg = CCI_P(attr);
	
    struct Notification *n;

    dmList& no_list = _NoStruct->Notify_list;
    dmList::iterator it = no_list.Begin();
    while(it!=no_list.End()) 
    {
      n = reinterpret_cast<Notification *>(*it); ++it;

      // Check for invalid port;
      if(!n->no_Port->IsValid()) {
        if(_NoStruct->NoCount>0) { 
          _NoStruct->NoRefresh = n->no_Removed = true;
        } else 
          Notify_KillNotification(n);

        continue;
      }

      if( ((tg)?(tg == n->no_Trig.ti_Tag):true) ) {
        if( ((o)?(o==n->no_Dest):true) ) {
          if(_NoStruct->NoCount>0) { 
            _NoStruct->NoRefresh = n->no_Removed = true; // Delayed delete
          } else 
            Notify_KillNotification(n);
        }
      }
    }
    CCI_RETURN_OK() 
  }
  
  CCI_RETURN_FAIL()
}

//---------------------------------------------------------------------
// Method: CallBack
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Notify, CallBack )
{
  return CCI_P(func)(CCI_P(data),_THIS_OBJECT,&CCI_P(param1));
}
//---------------------------------------------------------------------
// Method: CallBackHook
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Notify, CallBackHook )
{
  return CCI_P(hook)->cb_HookEntry(CCI_P(hook),_THIS_OBJECT,&CCI_P(param1));
}
//---------------------------------------------------------------------
// Method: AddRef  
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Notify, AddRef )
{  
  CCI_INSTANCE( Notify )
  return ++_This->RefCount;
}
//---------------------------------------------------------------------
// Method: Release  
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Notify, Release )
{
  CCI_INSTANCE( Notify )

  --_This->RefCount;

  if(_This->RefCount == 0) 
  {
    if(_This->NoStruct!=NULL)
    {
      // Make sure that we are not in notification
      if(_This->NoStruct->NoCount <= 0)
        CCI_FreeObject(_THIS_OBJECT);
      else 
      {
        // Kill all notifications attached to this object
    	  CCI_INVOKE_PP_(_THIS_OBJECT,Notify,KillNotify,_in 0L _in 0L);

        // Invalidate the Notify Port
        // so we cannot be notified any more
        if(_This->NoPort) {
           _This->NoPort->Invalidate();
           _This->NoPort = NULL;
        }
      }
    } else
      CCI_FreeObject(_THIS_OBJECT);
      
    return 0;
  }
  return _This->RefCount;
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Notify )
  CCI_REGISTER_METHOD_NP( Notify, AddRef      )
  CCI_REGISTER_METHOD_NP( Notify, Release     )
  CCI_REGISTER_METHOD_PP( Notify, NotifySet   )
  CCI_REGISTER_METHOD_PP( Notify, NoNotifySet )
  CCI_REGISTER_METHOD_PP( Notify, NotifyMsg   )
  CCI_REGISTER_METHOD_PP( Notify, Notify      )
  CCI_REGISTER_METHOD_PP( Notify, KillNotify  )
  CCI_REGISTER_METHOD_PP( Notify, CallBack    )
  CCI_REGISTER_METHOD_PP( Notify, CallBackHook)
CCI_END_DISPATCH_MAP()

//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
static void Notify_Finalize(cci_Class* cl,void* )
{
  // Free the memory handler for the ports
  Notify_Port::_MemPool.Free();
}
//---------------------------------------------------------------------
CCI_ENTRY_POINT(Notify)
{
  CCI_INVOKE_FACTORY_(Notify,NULL,NULL,Notify_Finalize);
}
//---------------------------------------------------------------------



