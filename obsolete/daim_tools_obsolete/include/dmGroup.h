#ifndef dmGroup_h
#define dmGroup_h

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
// File         : dmGroup.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class __dmTools dmGroup : public dmToolGroup
{   
  private: 
     bool        mTestMove;
     bool        mMoving;
     dmRectShape mRect;
     dmPoint     mMouseDown;

  protected:	
     dmGroup& operator=( const dmGroup& ) { return *this; }
     virtual void OnUpdateRegion( dmRegion& _rgn );
     virtual void Recalculate();
     virtual bool InitialUpdate();
     virtual void OnMove( int dx, int dy, bool _update );
     virtual bool OnShow( bool );

  public:	
     dmGroup( dmTool* _parent  = NULL );
     dmGroup( const dmGroup&, dmTool* _parent = NULL );
     virtual ~dmGroup();	 

     virtual dmTool* GetObject( const dmPoint&, bool bHitTest );
     virtual dmTool* Clone( dmTool* _parent = NULL ) const;

  public:
    virtual bool ProcessDefaultEvent( dmEvent & );

    dmDeclareClassInfo( dmGroup );
};
//---------------------------------------------------
#endif // ToolManager.h

