#ifndef daim_manager_h
#define daim_manager_h

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


#ifndef NO_DAIM_USER_INTERFACE

#include "daim_cmdtarget.h"

class daim_info;

class daim_manager : public daim_CmdTarget, public dmToolManager
{
  private:
    dm_uint  mnCurrentTool;
    dm_uint  mnPriority;

  public:
    daim_manager( dmTool* _parent = NULL );
    daim_manager( daim_info*, dmTool* _parent );
    daim_manager( daim_info*, const dmRect& _r, dmTool* _parent = NULL );
 
    virtual ~daim_manager();

    bool Empty() { return Count()==0; }

    daim_info* mInfo;

    void    SetEditCaps( dm_uint );
    dm_uint EditCaps()         const { return mEditCaps;     }
    dm_uint GetCurrentToolId() const { return mnCurrentTool; }

    void    SetPriority( dm_uint pri );
    dm_uint GetPriority() const { return mnPriority; }

    virtual dmImage* GetImage() const;

  protected:
    dmTool*  mNewTool;
    dm_uint  mEditCaps;

    dmToolGroup mClipBuffer;

  protected:
    virtual void OnInsertNewTool( dmTool* );
    virtual void OnEditNewTool  ( dmTool*, dm_uint nID=0);

    virtual bool OnFocus( bool _focus );
    virtual void OnMove (int dx,int dy,bool _update);

  public:
    virtual void SetDefaultToolPrefs( dmTool* );

    virtual dmTool* InsertTool( dmTool* , bool bEdit, dm_uint nID );
    virtual dmTool* LoadClassTool( dm_uint nID, 
                                   const dmString::E* classname, 
                                   const dmString::E* toolname, 
                                   const dmPoly*, bool  bEdit );

    virtual void AbortEdit( dm_uint nID=0 );

    virtual bool OnCommand ( dm_uint nCode );
    virtual void OnDisplay( dmGraphics& ) const;

  protected:
    virtual bool ProcessMouseEvent( dmMouseEvent& );
    virtual bool ProcessKeyEvent  ( dmKeyEvent&   );

    virtual void OnEscape  ();

    bool HandleNewToolEvent(dmEvent&);

    void OnUpdateEditCommand( daim_CmdUI*, dm_uint nCmd );
    void Init();

  protected:
    __command void OnEditDelete();
    __command void OnUpdateEditDelete(daim_CmdUI*);
    __command void OnSelectAll();   
    __command void OnUpdateSelectAll(daim_CmdUI*);

    // Edition
    __command void OnToolOpts();
    __command void OnUpdateToolOpts(daim_CmdUI*);

    __command void OnMvforward();
    __command void OnMvbackward();
    __command void OnMvback();
    __command void OnMvfront();

    __command void OnHcenteralign();
    __command void OnLeftalign();
    __command void OnVcenteralign();
    __command void OnTopalign();
    __command void OnRightalign();
    __command void OnBottomalign();

    __command void OnEditCopy();
    __command void OnEditCut();
    __command void OnEditPaste();
    __command void OnUpdateEditPaste(daim_CmdUI*); 
  
    __command void OnUpdateEditAlign(daim_CmdUI*); 
    __command void OnUpdateEditOrder(daim_CmdUI*);
    __command void OnUpdateEditCopy (daim_CmdUI*); 
  
    DECLARE_COMMAND_MAP()

public:
   dmDeclareClassInfo( daim_manager );  
};

#endif // NO_DAIM_USER_INTERFACE


#endif // daim_manager_h
