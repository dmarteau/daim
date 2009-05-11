#ifndef dmToolManips_h
#define dmToolManips_h

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
// File         : dmToolManips.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

//--------------------------------------------------
// Helpers for manipulating tools
//--------------------------------------------------
namespace dmToolManips {

struct Invalidate {
   dmTool* tool;
   Invalidate( dmTool *aTool ) : tool(aTool) { tool->Invalidate(true);  }
  ~Invalidate()                              { tool->Invalidate(false); }
};

struct LockUpdate
{
  dmTool* _tool;
  LockUpdate( dmTool* tool ) : _tool(tool) { _tool->LockUpdate();   }
 ~LockUpdate()                             { _tool->UnlockUpdate(); }
};

struct LockPosition
{
  dmTool* _tool;
  LockPosition( dmTool* tool ) : _tool(tool) { _tool->LockPosition(true);  }
 ~LockPosition()                             { _tool->LockPosition(false); }
};

struct LockRefresh
{
  dmTool* _tool;
  LockRefresh( dmTool* tool ) : _tool(tool) { _tool->BeginRefresh();  }
 ~LockRefresh()                             { _tool->EndRefresh();    }
};

#ifdef dmUseToolManips
//-------------------------------------------------------
// Alignment operators
//-------------------------------------------------------
struct Align
{
  struct Left {
    int x;
    Left( int _x ) : x(_x) {}
    void operator()( dmTool* tool ) { 
      if(tool->IsPositionnable())
       tool->MoveBy(x - tool->ObjectRect().Left(),0); 
    }
  };

  struct  Right { 
    int x;
    Right( int _x ) : x(_x) {}
    void operator()( dmTool* tool ) { 
     if(tool->IsPositionnable())
       tool->MoveBy(x - tool->ObjectRect().Right(),0); 
    }
  };

  struct Top {
    int y;
    Top( int _y ) : y(_y) {}
    void operator()( dmTool* tool ) { 
      if(tool->IsPositionnable())
         tool->MoveBy(0,y - tool->ObjectRect().Top()); 
    }
  };

  struct Bottom {
    int y;
    Bottom( int _y ) : y(_y) {}
    void operator()( dmTool* tool ) { 
      if(tool->IsPositionnable())
         tool->MoveBy(0,y - tool->ObjectRect().Bottom()); 
    }
  };

  struct VCenter {
    int x;
    VCenter( int _x ) : x(_x) {}
    void operator()( dmTool* tool ) { 
      if(tool->IsPositionnable())
         tool->MoveBy(x - tool->ObjectRect().Center().x,0); 
    }
  };

  struct HCenter {
    int y;
    HCenter( int  _y ) : y(_y) {}
    void operator()( dmTool* tool ) { 
      if(tool->IsPositionnable())
         tool->MoveBy(0,y - tool->ObjectRect().Center().y); 
    }
  };

}; // Align

//-------------------------------------------------------
#endif // dmUseToolManips


}; // namespace dmToolManips

#define O_INVALIDATE( tool )   dmToolManips::Invalidate    _Tool_Invalidate_ ## __LINE__(tool);
#define O_LOCKUPDATE( tool )   dmToolManips::LockUpdate    _Tool_LockUpdate_ ## __LINE__(tool);
#define O_LOCKPOSITION( tool ) dmToolManips::LockPosition  _Tool_LockPosition_ ## __LINE__(tool);
#define O_LOCKREFRESH( tool )  dmToolManips::LockRefresh   _Tool_LockRefresh_ ## __LINE__(tool);


#endif // dmToolManips_h
