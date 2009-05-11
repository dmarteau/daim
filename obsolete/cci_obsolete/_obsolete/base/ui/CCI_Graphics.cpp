
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

#define dmUseCCI
#define dmUseToolManager
#include "daim_tools.h"
#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Graphics.h"
#include "daim_modules/CCI_Document.h"

#define CCIC_NAME  CCIC_Graphics
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Graphics_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//-------------------------------------------------------------------
#define CCI_INTERNAL_MODULE

#include "daim_modules/CCI_Base.h"

//-----------------------------------------
CCI_DECL_DATA( Graphics )
{
  dm_bool     _StopCapture;
  dmIDocument _Document;
  dmGraphics* _Graphics;
};

typedef CCI_INSTANCE_PTR(Graphics) _Instance_Ptr;
//--------------------------------------------------------------------
// Helpers
//--------------------------------------------------------------------
void Graphics_ReleaseDocument( _Instance_Ptr _This )
{
  if(_This->_Document.IsValid()) {
      _This->_Document.SetGraphics(NULL);
      _This->_Document.Release();
      _This->_Document.Detach();
  }
}
//--------------------------------------------------------------------

#define THIS_GRAPHICS(me) if(_This->_Graphics) _This->_Graphics->me

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
static inline CCI_CONSTRUCTOR( Graphics )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( Graphics )

  CCI_INIT_MEMBER(_Document);

  _This->_Graphics    = NULL;
  _This->_StopCapture = dm_false;

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// ME_DISPOSE
//---------------------------------------------------------------------
static inline  CCI_DESTRUCTOR( Graphics )
{
  CCI_INSTANCE( Graphics )
  
  Graphics_ReleaseDocument(_This);

  CCI_DESTROY_SUPER()
}
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( Graphics )
{
  CCI_RETURN_INVOKE_SUPER()
}
*/
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Graphics )
  CCI_ATTR_GET( Graphics, StopCapture, CCIA_RETVAL = _This->_StopCapture )
  CCI_ATTR_GET( Graphics, CursorType ,
     CCIA_RETVAL = (_This->_Graphics?_This->_Graphics->GetCursorType():0))
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Graphics )
  CCI_ATTR_SET(Graphics, StopCapture, _This->_StopCapture = CCIA_DATA )
  CCIA_DISCARD(Graphics,ScreenX)  
  CCIA_DISCARD(Graphics,ScreenY) 
  CCIA_DISCARD(Graphics,CursorType) 
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method    : InitGraphics
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Graphics, InitGraphics  )
{
  CCI_INSTANCE( Graphics )
 
  if(_This->_Graphics == NULL ){
    if( (_This->_Graphics = CCI_P(nativeGraphics))!= NULL )
      CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method    : Document
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Graphics, Document ) 
{
  CCI_INSTANCE( Graphics )

  switch(CCI_P(me))
  {
    case CCI_ME_SET :
       Graphics_ReleaseDocument(_This);
       if(_This->_Document.QueryInterface(CCI_P(document))) {
          _This->_Document.SetGraphics(_This->_Graphics);
          _This->_Document.Initialize();
          CCI_RETURN_OK()
       }
     break;

    case CCI_ME_GET : {
       cci_Object* _object = _This->_Document;
       if(_object != NULL ) {
         CCI_AddRef(_object); // AddRef for the Caller
         CCI_RETVAL_P(document) = _object;
         CCI_RETURN_OK()
       }
     }
     break;

    case CCI_ME_DISPOSE :
       Graphics_ReleaseDocument(_This);
       CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method    : UpdateCursor
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( Graphics, UpdateCursor  )
{
  CCI_INSTANCE( Graphics )

  dmGraphics* _Graphics = _This->_Graphics;
    
  if(_Graphics && _Graphics->UpdateCursor()) 
     CCI_RETURN_OK()

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method    : Draw
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Graphics, Draw ) 
{
  CCI_INSTANCE( Graphics )

  if(_This->_Document.IsValid() && 
     _This->_Document.Draw(CCI_P(paintFlags)))
       CCI_RETURN_OK()
           
  CCI_RETURN_FAIL()
 
}
//---------------------------------------------------------------------
// Method : SystemEvent
//---------------------------------------------------------------------
#define EVT_TYPE(evtType) static_cast<dmEventType>(evtType)
#define PROCESS_EVENT( isHandled, mngr, type ) \
  dmEventHandler::InitEvent(NULL,_Event,type,dmEVENT_FLAG_SYSTEM_EVENT|dmEVENT_FLAG_INIT);\
  isHandled = mngr->ProcessEvent(_Event);
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Graphics, ProcessSystemEvent )
{
  CCI_INSTANCE( Graphics )

  if(_This->_StopCapture)
     CCI_RETURN_FAIL()
    
  if(_This->_Document.IsValid()) 
  {
    dmToolManager* pManager = _This->_Document.GetManager();
    if(pManager) 
    {
      bool eventHandled =  false;
      dmEventType evtType        = EVT_TYPE(CCI_P(evtType));
      ECursorType returnedCursor = eCursor_undefined;

      switch(CCI_P(strType)) 
      {
        case  evStruct_MouseEvent : 
        {
          dmMouseEvent _Event;        
          _Event.mButton   = CCI_P(button);
          _Event.mKeyQual  = CCI_P(keyQual);
          _Event.mCursor.x = CCI_P(cursorX);
          _Event.mCursor.y = CCI_P(cursorY);
     
          PROCESS_EVENT(eventHandled,pManager,evtType);
          returnedCursor = _Event.mCursorType;
        }
        break;

        case evStruct_KeyEvent : 
        {
          dmKeyEvent _Event;        
          _Event.mKeyCode  = CCI_P(keyCode);
          _Event.mKeyQual  = CCI_P(keyQual);
          PROCESS_EVENT(eventHandled,pManager,evtType);
          returnedCursor = _Event.mCursorType;
        }
        break;
      }

      if(returnedCursor!=eCursor_undefined) 
      {
         THIS_GRAPHICS(SetCursor)(returnedCursor);

          // This is needed on windows for activating cursor on 
          // MouseDown events
         if(evtType==evMouseDown || 
            evtType==evKeyDown   || 
            evtType==evKeyUp) 
           
            THIS_GRAPHICS(UpdateCursor)();
      }

      if(eventHandled) {
         CCI_RETURN_OK()
      }
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : SetDisplay
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Graphics, SetDisplay )
{
  CCI_INSTANCE( Graphics )
  if(_This->_Document.IsValid()) {
     _This->_Document.SetDisplay(CCI_P(left),CCI_P(top),CCI_P(width),CCI_P(height));
  }

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Graphics, SetCursorType )
{
  CCI_INSTANCE( Graphics )
    
  // Set document default cursor   
  if(_This->_Document.IsValid()) 
     _This->_Document.SetCursorType(CCI_P(cursor));

  // Update the displayed cursor  
  // Note that this function should be overloaded 
  // native Graphics implementation so as to take into account
  // the location of the cursor
  if(CCI_P(cursor)!=eCursor_undefined) 
  {
    THIS_GRAPHICS(SetCursor)(static_cast<ECursorType>(CCI_P(cursor)));
    THIS_GRAPHICS(UpdateCursor)();
  }  
  
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Graphics )
  CCI_REGISTER_METHOD_PP(Graphics, ProcessSystemEvent )
  CCI_REGISTER_METHOD_PP(Graphics, Draw         ) 
  CCI_REGISTER_METHOD_PP(Graphics, SetDisplay   )
  CCI_REGISTER_METHOD_NP(Graphics, UpdateCursor )
  CCI_REGISTER_METHOD_PP(Graphics, Document     ) 
  CCI_REGISTER_METHOD_PP(Graphics, InitGraphics )
  CCI_REGISTER_METHOD_PP(Graphics, SetCursorType)
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Graphics )
{
  CCI_INVOKE_FACTORY(Graphics);
}
//---------------------------------------------------------------------
