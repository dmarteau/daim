#ifndef CCI_ToolInfo_h
#define CCI_ToolInfo_h

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

//--------------------------------------------------------
// File         : CCI_ToolInfo.h
// Date         : 03/2005
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_ToolInfo  _TXT("toolinfo.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( ToolInfo, Notify )
  
  CCI_MODULE_UUID( ToolInfo, a80780ab-77ab-4585-a801-3b3a7aa2733b)

  CCI_MODULE_VERSION( ToolInfo, 1 ) 

  /* Methods   */
  CCI_DECL_METHOD_PP( ToolInfo, 1, GetPos , 
    _in  dm_int  index 
    _out dm_int  x 
    _out dm_int  y 
  )

  CCI_DECL_METHOD_PP( ToolInfo, 2,  Rename , _in CONST char_t* newname )
  CCI_DECL_METHOD_PP( ToolInfo, 3,  MoveTo ,
    _in dm_int index 
    _in dm_int x 
    _in dm_int y 
  )

  CCI_DECL_METHOD_PP( ToolInfo, 4,  MoveBy , 
    _in dm_int  index 
    _in dm_int  x 
    _in dm_int  y 
  )

  CCI_DECL_METHOD_PP( ToolInfo, 5,  Rectangle , 
    _out dm_int  left 
    _out dm_int  top
    _out dm_int  right 
    _out dm_int  bottom 
  )

  CCI_DECL_METHOD_PP( ToolInfo, 6,  ToolState ,
    _in  dm_uint state 
    _out dm_bool status 
    _in  dm_uint me 
  )

  CCI_DECL_METHOD_NP( ToolInfo, 7,  Remove   )
  CCI_DECL_METHOD_NP( ToolInfo, 8,  Update   )
  CCI_DECL_METHOD_PP( ToolInfo, 9,  GetClass, _out CONST char_t* classname )

  CCI_DECL_METHOD_PP( ToolInfo,10,  Assign, _in dmTool* tool )
  CCI_DECL_METHOD_NP( ToolInfo,11,  Clear  )

  CCI_DECL_METHOD_PP( ToolInfo, 20, AddListener , 
    _in dm_cci(EventListener) listener 
    _in dm_uint               evtMask 
    _in dm_uint               flags
  )

  CCI_DECL_METHOD_PP( ToolInfo, 21, RemoveListener ,
    _in dm_cci(EventListener) listener 
    _in dm_uint               evtMask 
    _in dm_uint               flags
  )

  CCI_DECL_METHOD_PP( ToolInfo, 30,  SetToolState ,
    _in dm_uint state 
    _in dm_bool status 
  )

  CCI_DECL_METHOD_PP( ToolInfo, 31,  GetToolState ,
    _in  dm_uint state 
    _out dm_bool status 
  )

  CCI_DECL_METHOD_PP( ToolInfo, 32,  GetProperties ,
    _in  CONST char_t*        category
    _in  dm_param             key
    _in  dm_cci(PropertyList) proplist
  )
  
  CCI_DECL_METHOD_PP( ToolInfo, 33,  SetProperties ,
    _in  CONST char_t*        category
    _in  dm_param             key
    _in  dm_cci(PropertyList) proplist
  )

  CCI_DECL_METHOD_PP( ToolInfo, 34,  ExecCommand ,
    _in CONST char_t*  command
    _in CONST char_t*  arg
    _in cci_Object*    param
  )

  CCI_DECL_METHOD_PP( ToolInfo, 35,  IsCommandSupported ,
    _in CONST char_t*  command
  )


  /* Attributs */
  CCI_DECL_ATTR( ToolInfo, 1, Target  , dmTool*      , [..G.] )
  CCI_DECL_ATTR( ToolInfo, 2, ToolKey , dm_param     , [..GN] )

  /* Special values */
  #define CCIV_ToolInfo_Visible  1L
  #define CCIV_ToolInfo_Focus    2L
  #define CCIV_ToolInfo_Select   3L
  #define CCIV_ToolInfo_Lock     4L
  #define CCIV_ToolInfo_NullObj  5L

CCI_END_MODULE_DECL( ToolInfo )

#endif // __doxygen

#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Tool manipulator
 */
//------------------------------------------------------------------------------------
class dmIToolInfo : public dmINotify
{    
  public:
   dmDECLARE_GENERIC_IINTERFACE(ToolInfo)

   enum Constants {
     stVisible = CCIV_ToolInfo_Visible,
     stFocus   = CCIV_ToolInfo_Focus  ,
     stSelect  = CCIV_ToolInfo_Select ,
     stLockPos = CCIV_ToolInfo_Lock
   };

   //-------------------------------------------------------
   /*! \brief Assign a tool  */
   //-------------------------------------------------------
   bool Assign( dmTool* tool ) {
     CCI_RETURN_IMETHOD_PP_OK(ToolInfo, Assign, _in tool )
   }

   //-------------------------------------------------------
   /*! \brief Clear/Release the current tool */
   //-------------------------------------------------------
   bool Clear() { CCI_RETURN_IMETHOD_NP_OK(ToolInfo,Clear) }

   //-------------------------------------------------------
   /*! \brief Add an event listener to the tool.
    *
    * \param listener a dmIEventListener object
    * \param evtMask  mask of event type to be intercepted (\see dmTk::dmEventType )
    *                 The mask should be a combination (OR) of \c dmEVENT_MASK(eventType)
    *                values.
    * \param flags   Propagation flags:
    *                \c dmEVENT_FLAG_CAPTURE - listen for capturing events. 
    *                \c dmEVENT_FLAG_BUBBLE - listen for bubbling  events. 
    *                ( see \ref daim_event_model for details ).
    */
   //-------------------------------------------------------
   bool AddListener( dm_cci(EventListener) listener, dm_uint evtMask, dm_uint flags ) {
     CCI_RETURN_IMETHOD_PP_OK(ToolInfo,AddListener,_in listener _in evtMask _in flags )
   }

   //-------------------------------------------------------
   /*! \brief Remove an event listener from a tool.
    *
    * \param listener a dmIEventListener object
    * \param evtMask mask of event type to be intercepted (\see dmTk::dmEventType )
    *                The mask should be a combination (OR) of \c dmEVENT_MASK(eventType)
    *                values.
    * \param flags   Propagation flags:
    *                \c dmEVENT_FLAG_CAPTURE - listen for capturing events. 
    *                \c dmEVENT_FLAG_BUBBLE - listen for bubbling  events. 
    *                ( see \ref daim_event_model for details ).
    */
   //-------------------------------------------------------
   bool RemoveListener( dm_cci(EventListener) listener, dm_uint evtMask, dm_uint flags ) {
     CCI_RETURN_IMETHOD_PP_OK(ToolInfo,RemoveListener,_in listener _in evtMask _in flags )
   }

   //-------------------------------------------------------
   /*! \brief Return the position of a carret
    *
    *  \param index index of the carret 
    *  \param pt    index of the carret 
    */
   //-------------------------------------------------------
   bool GetPos( dm_uint index, dm_point& pt )  
   {
     IF_CCI_IMETHOD_RP(ToolInfo,GetPos,_in index )
     THEN
       pt.x = CCI_RETVAL(x);
       pt.y = CCI_RETVAL(y);
     ENDIF_RETURN_BOOL()
   }

   //-------------------------------------------------------
   /*! \brief Rename a tool */
   //-------------------------------------------------------
   bool Rename( const char_t* newname ) {
     CCI_RETURN_IMETHOD_PP_OK(ToolInfo, Rename, _in newname )
   }

   //-------------------------------------------------------
   /*! \brief Move carret to a given position */
   //-------------------------------------------------------
   bool MoveTo( dm_int index, dm_int x, dm_int y ) {
     CCI_RETURN_IMETHOD_PP_OK(ToolInfo,MoveTo, _in index _in x _in y )
   }

   //-------------------------------------------------------
   /*! \brief Offset carret position  */
   //-------------------------------------------------------
   bool MoveBy( dm_int index, dm_int x, dm_int y ) {
     CCI_RETURN_IMETHOD_PP_OK(ToolInfo,MoveBy,_in index _in x _in y )
   }

   //-------------------------------------------------------
   /*! \brief Return the bounding rectangle of that tool   */
   //-------------------------------------------------------
   bool Rectangle( dm_rect& rect ) {
     IF_CCI_IMETHOD_RP(ToolInfo,Rectangle, _in 0 ) 
     THEN
       rect.top_left.x      = CCI_RETVAL(left);
       rect.top_left.y      = CCI_RETVAL(top);
       rect.bottom_right.x  = CCI_RETVAL(right);
       rect.bottom_right.y  = CCI_RETVAL(bottom);
     ENDIF_RETURN_BOOL()
   }

   //-------------------------------------------------------
   /*! \brief Set internal state of the tools */
   //-------------------------------------------------------
   bool SetToolState( dm_uint state, dm_bool status ) {
     CCI_RETURN_IMETHOD_PP_OK(ToolInfo,ToolState, 
       _in state 
       _in status 
       _in CCI_ME_SET 
     )
   }

   //-------------------------------------------------------
   /*! \brief Return internal state of the tools */
   //-------------------------------------------------------
   dm_bool GetToolState( dm_uint state ) {
      IF_CCI_IMETHOD_RP(ToolInfo,ToolState,_in state _out(status) 0 _in CCI_ME_GET ) 
        THEN return CCI_RETVAL(status);
        ELSE return dm_false;
      ENDIF
   }

   //-------------------------------------------------------
   /*! \brief Remove the tool from its container
    *
    * This method search all parent for a tool manager.
    * If the manager is found, the tool is removed from it and
    * eventually destroyed.
    */
   //-------------------------------------------------------
   bool Remove() { CCI_RETURN_IMETHOD_NP_OK( ToolInfo,Remove  ) }

   //-------------------------------------------------------
   /*! \brief Update the tool */
   //-------------------------------------------------------
   bool Update()  { CCI_RETURN_IMETHOD_NP_OK( ToolInfo,Update ) }

   //-------------------------------------------------------
   /*! \brief Return the classname for that tool */
   //-------------------------------------------------------
   const char_t* GetClass()  {
     IF_CCI_IMETHOD_RP(ToolInfo,GetClass, _out(classname) NULL ) 
       THEN return CCI_RETVAL(classname);
       ELSE return NULL;
     ENDIF
   }

   dm_param GetToolKey() { return CCI_IGET(ToolInfo,ToolKey,dm_param(0));  }
   dmTool*  GetTarget()  { return CCI_IGET(ToolInfo,Target,(dmTool*)NULL); }
   
   //-------------------------------------------------------
   /*! \brief Execute a command */
   //-------------------------------------------------------
   dm_bool ExecCommand( const char_t* command, const char_t* arg, cci_Object* param ) {
      CCI_RETURN_IMETHOD_PP_OK(ToolInfo,ExecCommand,_in command _in arg _in param ) 
   }
   
   //-------------------------------------------------------
   /*! \brief Check if a command is supported */
   //-------------------------------------------------------
   dm_bool IsCommandSupported( const char_t* command ) {
      CCI_RETURN_IMETHOD_PP_OK(ToolInfo,IsCommandSupported,_in command) 
   }
};

#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif // CCI_ToolInfo_H
