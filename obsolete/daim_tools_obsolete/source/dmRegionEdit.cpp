
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

#define dmUseAreaEdit
#include "daim_tools.h"
#include "dmRegionEdit.h"
//----------------------------------------------
static void ___DisplayRgnNode( const dmRgnNode& aNode, dmGraphics& gr, int flags ) 
{
  dmRgnNode::const_iterator it   = aNode.Begin();
  dmRgnNode::const_iterator last = aNode.End();
  for(;it!=last;++it) {
    gr.Draw((*it).Coordinates(),flags);
    ___DisplayRgnNode(*it,gr,flags); 
  }
}
//----------------------------------------------
static void ___MoveRgnNode( dmRgnNode& aNode, int dx, int dy ) 
{
  aNode.Coordinates().Translate(dx,dy);
  aNode.Region().Translate(dx,dy);
  dmRgnNode::iterator it   = aNode.Begin();
  dmRgnNode::iterator last = aNode.End();
  for(;it!=last;++it) {
    ___MoveRgnNode(*it,dx,dy); 
  }
}
//---------------------------------------------------------
void dmRegionEditShape::DoMove(int _index , int dx, int dy )
{
  if(_index < 0 && RigidMode())
  {
    ___MoveRgnNode(mTree.GetRoot(), dx, dy );
  }  
}
//---------------------------------------------------------
void dmRegionEditShape::DoMoveIndex(int _index, int dx, int dy) 
{
  // Do nothing here
}
//-----------------------------------------------------------------------------
void dmRegionEditShape::DoFrame( dmGraphics& gr, int _index, int drawFlags ) const
{
  ___DisplayRgnNode( mTree.GetRoot(), gr, drawFlags );
}
//----------------------------------------------
// dmRegionEdit
//----------------------------------------------
#define dmRegionEdit_SCHEMA_VERSION 1
dmImplementClassInfo( dmRegionEdit, dmRegionEditProto,dmRegionEdit_SCHEMA_VERSION);
//----------------------------------------------
dmRegionEdit::dmRegionEdit( dmTool* _parent ) 
: dmRegionEditProto(_parent)
{
  SetCarrets( &mShape );
  LockCarrets( true );
}
//----------------------------------------------
dmRegionEdit::dmRegionEdit( const dmRegion& rgn, dmTool* _parent )
: dmRegionEditProto(rgn,_parent)
{
  SetCarrets( &mShape );
  LockCarrets( true );
}
//----------------------------------------------
dmRegionEdit::dmRegionEdit( const dmRegionEdit& _toCopy, dmTool* _parent )
: dmRegionEditProto(_toCopy,_parent)
{
  SetCarrets( &mShape );
  LockCarrets( true );
}
//----------------------------------------------
dmRegionEdit::~dmRegionEdit()
{}
//----------------------------------------------
void dmRegionEdit::OnDisplay( dmGraphics& gr ) const
{
  const dmRegion& rgn = Region();
  if(!rgn.IsEmptyRoi()) 
    mShape.DoFrame(gr,-1, dmTk::eDrawDefault );
}
//----------------------------------------------
void dmRegionEdit::Recalculate()
{
 dmRegionEditProto::Recalculate();
 mShape.mTree.Build(Region());
}
//----------------------------------------------
dmTool* dmRegionEdit::Clone( dmTool* _parent ) const
{
  return new dmRegionEdit(*this,_parent);
}
//----------------------------------------------
