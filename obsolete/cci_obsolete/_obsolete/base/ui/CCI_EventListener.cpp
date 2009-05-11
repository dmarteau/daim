
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

#define dmUseToolManager
#include "daim_tools.h"

#include "daim/daim_base.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_EventListener.h"
#include "daim_modules/CCI_ToolInfo.h"
#include "daim_modules/CCI_Interface.h"

//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_EventListener
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_EventListener_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#define CCI_INTERNAL_MODULE

#include "daim_modules/CCI_Base.h"
#include "daim_manager.h"


//--------------------------------------------------------------------
// Attach an event listener to to the cci object
// The following object intercepts tool events and trigs notifications. 
//--------------------------------------------------------------------
class EvtListener_impl : public dmEventListener
{
  public:
    cci_Object* _Self;
    dmEvent*    _Event;
    bool        _HandledStatus;
    bool        _ReturnStatus;
    dm_uint     _Recurse;
    dm_bool     _CacheEvents;
   
    dm_int      _EvtX;    
    dm_int      _EvtY;     
    dm_uint     _EvtButtons;
    dm_uint     _EvtQual;  
    dm_uint     _EvtCode;  
      
   ~EvtListener_impl() {}
    EvtListener_impl() : 
     _Event(NULL)
    ,_Recurse(0)
    ,_CacheEvents(dm_false)
    {}

    void CacheEvent();

    virtual bool ProcessEvent( dmEvent& );
    virtual void HangUp();
};
//--------------------------------------------------------------------
void EvtListener_impl::HangUp()
{
  dmEventListener::HangUp();
  CCI_Set(_Self,CCIA_DECL_(EventListener,HangUp),dm_true);
}
//--------------------------------------------------------------------
void EvtListener_impl::CacheEvent()
{
  if(_Event) {
    switch(_Event->mStructType)
    { 
      case evStruct_MouseEvent: 
      {
        dmMouseEvent* _Mouse = static_cast<dmMouseEvent*>(_Event);
        _EvtX       = _Mouse->mCursor.x;
        _EvtY       = _Mouse->mCursor.y;
        _EvtButtons = _Mouse->mButton;
        _EvtQual    = _Mouse->mKeyQual;
      }
      break;

      case evStruct_KeyEvent: 
      {
        dmKeyEvent* _Key = static_cast<dmKeyEvent*>(_Event);
        _EvtCode = _Key->mKeyCode;
        _EvtQual = _Key->mKeyQual;
      }
      break;
      
      default: 
        break;
    }
  } 
}
//--------------------------------------------------------------------
bool EvtListener_impl::ProcessEvent( dmEvent& _theEvent )
{
  // Save previous event state on the stack
  dmEvent* _Prev = _Event; 
  bool _Status   = _HandledStatus;

  _HandledStatus = false;
  _Event         = &_theEvent;

  ++_Recurse;

  // Cache events
  if(_CacheEvents) 
      CacheEvent();

  // Signal for that event
  CCI_Set(_Self,CCIA_DECL_(EventListener,Event),dm_param(_theEvent.mEventType));
  
  this->_Event = _Prev;     // Restore previous

  if(_CacheEvents)    // Restore cached event;
     CacheEvent();

  CCI_Set(_Self,CCIA_DECL_(EventListener,Event),dm_param(evNoEvents)); 

  --_Recurse;
 
  _ReturnStatus  = _HandledStatus;
  _HandledStatus = _Status;

  return _ReturnStatus;   // Return status for that event 
}
//--------------------------------------------------------------------
CCI_DECL_DATA( EventListener )
{
  EvtListener_impl _Listener;
  dm_param         _LastEvent;
};

#define _THIS_EVENT( m )    (_This->_Listener._Event?_This->_Listener._Event->m:0 )
#define _THIS_TARGET()      _THIS_EVENT(mTarget)
#define _THIS_CACHED( m )   _This->_Listener._Evt##m
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( EventListener )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE(EventListener)

  CCI_INIT_MEMBER(_Listener);

  _This->_Listener._Self = _THIS_OBJECT;
  _This->_LastEvent      = evNoEvents;

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( EventListener )
{
  CCI_INSTANCE(EventListener)

  CCI_DESTROY_MEMBER(_Listener);
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( EventListener )
  CCI_ATTR_GET(EventListener, Event  , CCIA_RETVAL = _This->_LastEvent )
  CCI_ATTR_GET(EventListener, Result , CCIA_RETVAL = _THIS_EVENT(mResult) )
  CCI_ATTR_GET(EventListener, Target , 
    CCIA_RETVAL = CCIA_TRANSLATE_VAL(_THIS_EVENT(mTarget)) )

  CCI_ATTR_GET(EventListener, EvtX      , CCIA_RETVAL = _THIS_CACHED(X      ) )
  CCI_ATTR_GET(EventListener, EvtY      , CCIA_RETVAL = _THIS_CACHED(Y      ) )
  CCI_ATTR_GET(EventListener, EvtButtons, CCIA_RETVAL = _THIS_CACHED(Buttons) )
  CCI_ATTR_GET(EventListener, EvtQual   , CCIA_RETVAL = _THIS_CACHED(Qual   ) )
  CCI_ATTR_GET(EventListener, EvtCode   , CCIA_RETVAL = _THIS_CACHED(Code   ) )

  CCI_ATTR_GET(EventListener, Recurse ,CCIA_RETVAL = _This->_Listener._Recurse )
  CCI_ATTR_GET(EventListener, Status  ,CCIA_RETVAL = _THIS_EVENT(mFlags) )
  CCI_ATTR_GET(EventListener, HangUp  ,CCIA_RETVAL = _BOOL( !_This->_Listener.IsListening() ) )

  CCI_ATTR_GET(EventListener, CacheEvents , CCIA_RETVAL = _This->_Listener._CacheEvents )

  CCI_ATTR_GET(EventListener, IsOriginalTarget ,
     CCIA_RETVAL = 
       (_This->_Listener._Event != NULL && 
        _This->_Listener._Event->mCurrentTarget == _This->_Listener._Event->mTarget)
        ? dm_true : dm_false )
          
  CCI_ATTR_GET(EventListener, CurrentTarget, 
     CCIA_RETVAL = CCIA_TRANSLATE_VAL(_THIS_EVENT(mCurrentTarget)) )

CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( EventListener )
  CCI_ATTR_SET(EventListener, Event       , _This->_LastEvent   = CCIA_DATA )
  CCI_ATTR_GET(EventListener, CacheEvents , _This->_Listener._CacheEvents = CCIA_DATA )

  CCIA_DISCARD(EventListener, Result  )
  CCIA_DISCARD(EventListener, Target  )
  CCIA_DISCARD(EventListener, Recurse )
  CCIA_DISCARD(EventListener, Status  )

  CCIA_DISCARD(EventListener, EvtX       )
  CCIA_DISCARD(EventListener, EvtY       )
  CCIA_DISCARD(EventListener, EvtButtons )
  CCIA_DISCARD(EventListener, EvtQual    )
  CCIA_DISCARD(EventListener, EvtCode    )

  CCIA_DISCARD(EventListener, IsOriginalTarget )
  CCIA_DISCARD(EventListener, CurrentTarget    )

CCI_END_SETTER()

//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( EventListener , AddListener )
{
  CCI_INSTANCE(EventListener)

  dmTool* tool = CCI_P(tool);
  if(tool)
  { 
    dmSetOf<dmEventType> _EventMask;

    _EventMask.Assign(CCI_P(evtMask));
    tool->AddEventListener(_EventMask,&_This->_Listener,CCI_P(flags));

    CCI_Set(_THIS_OBJECT,CCIA_DECL_(EventListener,HangUp),dm_false);
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( EventListener , RemoveListener )
{
  CCI_INSTANCE(EventListener)

  dmTool* tool = CCI_P(tool);
  if(tool)
  { 
    dmSetOf<dmEventType> _EventMask;

    _EventMask.Assign(CCI_P(evtMask));
    tool->RemoveEventListener(_EventMask,&_This->_Listener,CCI_P(flags));

    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( EventListener , GetMouseEvent )
{
  CCI_INSTANCE(EventListener)

  dmEvent* _Event = _This->_Listener._Event; 
  if(_Event && _Event->mStructType == evStruct_MouseEvent) 
  {
    dmMouseEvent* _Mouse = static_cast<dmMouseEvent*>(_Event);
    CCI_RETVAL_P(x)       = _Mouse->mCursor.x;
    CCI_RETVAL_P(y)       = _Mouse->mCursor.y;
    CCI_RETVAL_P(buttons) = _Mouse->mButton;
    CCI_RETVAL_P(qual)    = _Mouse->mKeyQual;

    CCI_RETURN_OK()  
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( EventListener , GetKeyEvent )
{
  CCI_INSTANCE(EventListener)

  dmEvent* _Event = _This->_Listener._Event; 
  if(_Event && _Event->mStructType == evStruct_KeyEvent) 
  {
    dmKeyEvent* _Key = static_cast<dmKeyEvent*>(_Event);
    CCI_RETVAL_P(code) = _Key->mKeyCode;
    CCI_RETVAL_P(qual) = _Key->mKeyQual;

    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( EventListener , StopPropagation )
{
  CCI_INSTANCE(EventListener)

  dmEvent* _Event = _This->_Listener._Event;
  if(_Event != NULL) {
    _Event->StopPropagation();
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( EventListener , PreventDefault )
{
  CCI_INSTANCE(EventListener)

  dmEvent* _Event = _This->_Listener._Event;
  if(_Event != NULL) {
     _Event->PreventDefault();
     CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( EventListener , CancelBubble )
{
  CCI_INSTANCE(EventListener)

  dmEvent* _Event = _This->_Listener._Event;
  if(_Event != NULL) {
     _Event->PreventBubble();
     CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( EventListener , GetToolInfo )
{
  CCI_INSTANCE(EventListener)

  dmTool* _Tool = _THIS_TARGET();
  if(_Tool) {
    dmIToolInfo _ToolInfo;
    if(_ToolInfo.QueryInterface(CCI_P(toolinfo)) &&
       _ToolInfo.Assign(_Tool)) {

       CCI_RETURN_OK()
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( EventListener )
  CCI_REGISTER_METHOD_PP( EventListener , AddListener     )
  CCI_REGISTER_METHOD_PP( EventListener , RemoveListener  )
  CCI_REGISTER_METHOD_PP( EventListener , GetMouseEvent   )
  CCI_REGISTER_METHOD_PP( EventListener , GetKeyEvent     )
  CCI_REGISTER_METHOD_NP( EventListener,  StopPropagation )
  CCI_REGISTER_METHOD_NP( EventListener,  PreventDefault  )
  CCI_REGISTER_METHOD_NP( EventListener,  CancelBubble    )
  CCI_REGISTER_METHOD_PP( EventListener,  GetToolInfo     )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( EventListener )
{   
  CCI_INVOKE_FACTORY(EventListener)
}
//---------------------------------------------------------------------
