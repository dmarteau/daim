#ifndef CCI_Manager_h
#define CCI_Manager_h

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
// File         : CCI_Manager.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_Manager  _TXT("manager.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Manager, Document )
  
  CCI_MODULE_UUID( Manager, 7f4ff747-e95a-42a4-a016-c8122ba47df1)

  CCI_MODULE_VERSION( Manager, 1 ) 

  /* Methods */
  CCI_DECL_METHOD_PP( Manager, 1, CreateTool,
     _in  dm_uint                        id
     _in  CONST char_t*                  classid
     _in  CONST char_t*                  name
     _in  CONST dm_array(dm_point,count) pts
     _in  dm_uint                        count
     _in  dm_bool                        edit
     _out dmTool*                        tool
  )

  CCI_DECL_METHOD_PP(Manager,2,  FromInfo, 
    _in CONST char_t*     name 
    _in dm_uint           flags 
    _in dm_cci(ImageInfo) info
  )

  CCI_DECL_METHOD_PP(Manager, 3,  Create         , _in  dm_uint width _in dm_uint height )
  CCI_DECL_METHOD_PP(Manager, 4,  SetFocus       , _in  dm_uint state )
  CCI_DECL_METHOD_PP(Manager, 5,  Show           , _in  dm_uint state )

  CCI_DECL_METHOD_PP(Manager, 6,  GetToolByName  , _in  CONST char_t* name _out dmTool* tool )

  CCI_DECL_METHOD_PP(Manager, 9,  Clear          , _in  dm_uint flags )
  CCI_DECL_METHOD_PP(Manager,10,  AbortEdit      , _in  dm_uint id    )
  CCI_DECL_METHOD_PP(Manager,12,  GetCurrentTool , _out dmTool* tool )

  CCI_DECL_METHOD_NP( Manager,14, ReleaseMngr )

  CCI_DECL_METHOD_PP( Manager, 20, AddListener    , 
     _in dm_cci(EventListener) listener 
     _in dm_uint               evtMask 
     _in dm_uint               flags 
  )

  CCI_DECL_METHOD_PP( Manager, 21, RemoveListener , 
     _in dm_cci(EventListener) listener 
    _in dm_uint                evtMask 
    _in dm_uint                flags 
  )

  CCI_DECL_METHOD_PP( Manager, 22, GetToolInfo,
    _in dm_cci(ToolInfo) toolinfo
    _in CONST char_t*    toolname
  )

  /* Attributs */
  CCI_DECL_ATTR( Manager, 1, CurrentToolId, dm_uint, [..G.] )
  CCI_DECL_ATTR( Manager, 2, EditCaps     , dm_uint, [ISGN] )
  CCI_DECL_ATTR( Manager, 3, HasFocus     , dm_bool, [..G.] )
  CCI_DECL_ATTR( Manager, 4, IsEmpty      , dm_bool, [..G.] )

  /* Special Values */
  #define CCIV_Manager_New       dm_mask32

  #define CCIV_Manager_All       0L
  #define CCIV_Manager_Selection 1L


CCI_END_MODULE_DECL( Manager )

#endif // __doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Daim tool manager document interface
 *
 *  Manager is a special kind of Daim document,used to display graphical
 *  informations using the tools library. It is possible to add/remove/control tools
 *  that will interact with user using the default gui environment (In this case, 
 *  you will need to have a proper dmIGraphics object in order to achieve display). 
 *  Capture of events must be done at implementor level wich must translate and 
 *  passe them to the document (see dmIDocument::OnCommand methods ).  
 */
//------------------------------------------------------------------------------------
class dmIManager : public dmIDocument
{    
  public:
   dmDECLARE_GENERIC_IINTERFACE(Manager)

   //-------------------------------------------------------
   /*! \brief Retrieve or create a manager from the current info object  
    *  
    * This method will attach a new or existing manager 
    * to this interface. The manager will be attached to the dmIImageInfo object
    * \a info ( or to the current dmIImageInfo object if \a info is set to ull )
    * \a flags can be a combination of the following values :
    *   - DM_Manager_Create: Create a new manager and insert it in the manager list
    *                         of the given dmIImageInfo object.
    *   - DM_Manager_Select: Select the manager and bring it to the foreground.
    */
   //-------------------------------------------------------
   bool FromInfo( const char_t* name, dm_uint flags, dm_cci(ImageInfo) info ) {
      CCI_RETURN_IMETHOD_PP_OK(Manager,FromInfo, _in name  _in flags _in info ) 
   }

   //-------------------------------------------------------
   /*! \brief Create a stand alone manager */
   //-------------------------------------------------------
   bool Create( dm_uint width, dm_uint height )       { CCI_RETURN_IMETHOD_PP_OK(Manager,Create  , _in width _in height ) }

   bool SetFocus( dm_uint state ) { CCI_RETURN_IMETHOD_PP_OK(Manager,SetFocus, _in state ) }
   bool Clear   ( dm_uint flags ) { CCI_RETURN_IMETHOD_PP_OK(Manager,Clear   , _in flags ) }
   bool Show    ( dm_uint state ) { CCI_RETURN_IMETHOD_PP_OK(Manager,Show    , _in state ) }

   //-------------------------------------------------------
   /*! \brief Return a tool from its name */
   //-------------------------------------------------------
   dmTool* GetToolByName( const char_t* name ) {
     IF_CCI_IMETHOD_RP(Manager,GetToolByName, _in name _out(tool) NULL ) 
     THEN return CCI_RETVAL(tool);
     ELSE return NULL;
     ENDIF
   }

   //-------------------------------------------------------
   /*! \brief Return the current active tool descriptor */
   //-------------------------------------------------------
   dmTool* GetCurrentTool() {
     IF_CCI_IMETHOD_RP(Manager,GetCurrentTool, _out(tool) NULL )
     THEN return CCI_RETVAL(tool);
     ELSE return NULL;
     ENDIF
   }

   //-------------------------------------------------------
   /*! \brief Create a new tool  
    *
    * \param id       Integer identifier returned by GetCurrentToolId()
    * \param classid  The class name of the tool to be created.
    *                 \a class must correspond to a registered tool class.
    *                 If this is parameter is null, \a id must be one of the 
    *                 predefined tool id defined in daim_ui.h.
    * \param pts      An array of dm_point for initializing the tools.
    *                 This parameter can be null.
    * \param count    The number of points in \a count.  
    * \param edit     Create the tool for manual editing.
    *                 The tool is inserted into the manager only when the editing 
    *                 sequence has been completed. Editing can be aborted with th
    *                 the AbortEdit method.
    */
   //-------------------------------------------------------
   dmTool* CreateTool(
       dm_uint         id,
       const char_t*   classid, 
       const char_t*   name,
       const dm_point* pts,
       dm_uint         count,
       bool            edit
     )
  {
     IF_CCI_IMETHOD_RP(Manager,CreateTool,
       _in  id
       _in  classid 
       _in  name
       _in  pts
       _in  count
       _in  (edit ? dm_true : dm_false)
       _out(tool) NULL )
     THEN return CCI_RETVAL(tool);
     ELSE return NULL;
     ENDIF
  }

  //-------------------------------------------------------
  /*! \brief Edit a new tool  
   *
   * This is a shortcut for CreateTool(id,classid,name,NULL,0,true)
   */
  //-------------------------------------------------------
  dmTool* EditTool( dm_uint id, const char_t* classid, const char_t* name ) {
    return CreateTool(id,classid,name,NULL,0,true);
  }


  //-------------------------------------------------------
  /*! \brief Return the current edited tool id */
  //-------------------------------------------------------
  dm_uint GetCurrentToolId() {
    return CCI_IGET(Manager,CurrentToolId,(dm_uint)0);
  }

  //-------------------------------------------------------
  /*! \brief Abort the current editing of a tool 
   *
   *  This method generate a evAbort bubbling event
   */
  //-------------------------------------------------------
  bool AbortEdit( dm_uint id = 0 ) {
     CCI_RETURN_IMETHOD_PP_OK(Manager,AbortEdit, _in id ) 
  }

  //-------------------------------------------------------
  /*! \brief Release the attached tool manager */
  //-------------------------------------------------------
  bool ReleaseMngr() {
     CCI_RETURN_IMETHOD_NP_OK(Manager,ReleaseMngr ) 
  }

  //-------------------------------------------------------
  /*! \brief Add an event listener to the attached manager.
   *
   * \see dmIEventListener::AddListener() 
   * for a description of the parameer
   */
  //-------------------------------------------------------
  bool AddListener( dm_cci(EventListener) listener, dm_uint evtMask, dm_uint flags ) {
    CCI_RETURN_IMETHOD_PP_OK(Manager,AddListener,_in listener _in evtMask _in flags )
  }

  //-------------------------------------------------------
  /*! \brief Remove an event listener from he attached manager.
   *
   * \see dmIEventListener::RemoveListener() 
   * for a description of the parameters
   */
  //-------------------------------------------------------
  bool RemoveListener( dm_cci(EventListener) listener, dm_uint evtMask, dm_uint flags ) {
    CCI_RETURN_IMETHOD_PP_OK(Manager,RemoveListener,_in listener _in evtMask _in flags )
  }

   //-------------------------------------------------------------------
   /*! \brief Initialize a dmIToolInfo object
    *
    * If \a toolname is a valid non empty string, the method will look for
    * for the first tool where the name match the passed string.
    * Otherwise, the current default tool (if any) is assigned to the 
    * dmIToolInfo object.
    */
   //-------------------------------------------------------------------
   bool GetToolInfo( dm_cci(ToolInfo) toolinfo, const char_t* toolname ) {
     CCI_RETURN_IMETHOD_PP_OK( Manager,GetToolInfo,_in toolinfo _in toolname )
   }

   //-------------------------------------------------------------------
   /*! \brief Return true if the manager has the focus */
   //-------------------------------------------------------------------
   bool HasFocus() {
     return CCI_IGET(Manager,HasFocus,(dm_bool)dm_false) != dm_false;
   }

   //-------------------------------------------------------------------
   /*! \brief Return true if the manager is empty  */
   //-------------------------------------------------------------------
   bool IsEmpty() {
     return CCI_IGET(Manager,IsEmpty,(dm_bool)dm_true) == dm_true;
   }
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Manager_h.h */
