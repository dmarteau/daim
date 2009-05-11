
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

#include "daim_tools.h"
#include <algorithm>

//------------------------------------------------------
// ToolGroup
//------------------------------------------------------
#define dmToolGroup_SCHEMA_VERSION 1
dmImplementClassInfo( dmToolGroup, dmTool, dmToolGroup_SCHEMA_VERSION );
//------------------------------------------------------
dmToolGroup::dmToolGroup( const dmRect& r, dmTool* _parent  ) 
: dmTool( r, _parent )
{}
//------------------------------------------------------
dmToolGroup::dmToolGroup( dmTool* _parent ) 
: dmTool( _parent )  
{}
//------------------------------------------------------
dmToolGroup::dmToolGroup( const dmToolGroup& _group , dmTool* _parent  )
: dmTool(_group,_parent)
{
  dmTool* tool = NULL;
  for(const_iterator it=_group.Begin(),last=_group.End();it!=last;++it ) 
  { 
    tool = (*it)->Clone(this);
    tool->Obtain();
    mToolList.Push_Back( tool ); 
  }
}
//------------------------------------------------------
dmToolGroup::~dmToolGroup() 
{	  
  ClearAndDestroy();
}
//------------------------------------------------------
void dmToolGroup::OnMove(int dx,int dy,bool _update)
{
  BeginRefresh();
  dmTool::OnMove(dx,dy,_update); 
  for(iterator it=Begin(),last=End();it!=last;++it) 
    (*it)->MoveBy(dx,dy,_update);
  EndRefresh();
}
//-------------------------------------------------------  
dmToolGroup::iterator dmToolGroup::Find( const dmPoint& p ) 
{
  iterator it   = Begin();
  iterator last = End();
  for(;it!=last;++it) { 
    if((*it)->GetObject(p,false)) 
     return it; 
  }
  return End();
} 
//-------------------------------------------------------  
dmTool* dmToolGroup::GetObject( const dmPoint& p, bool bHitTest ) 
{
  dmTool* tool = NULL;
  reverse_iterator it   = rBegin();
  reverse_iterator last = rEnd();
  for(;it!=last && !tool;++it) 
    tool = (*it)->GetObject(p,bHitTest);

  return tool; 
} 
//-------------------------------------------------------
int dmToolGroup::Append( const dmToolGroup& _from )
{
  dmTool* tool = NULL;
  int _Cnt = 0;
  const_iterator it   = _from.Begin();
  const_iterator last = _from.End();
  for(; it != last ; ++it ) {
    tool = (*it)->Clone(this);
    tool->Obtain();
    mToolList.Push_Back( tool ); 
    ++_Cnt;
  }  
  return _Cnt;
}
//-------------------------------------------------------
int dmToolGroup::Copy( const dmToolGroup& _from )
{
  ClearAndDestroy();
  return Append(_from ); 
}
//------------------------------------------------------- 
void dmToolGroup::OnDisplay( dmGraphics& _gr ) const
{	
  for(const_iterator it=Begin(),last=End();it!=last;++it ) {
    if( (*it)->IsVisible() )
        (*it)->DisplayObject(_gr);
  }	
}
//------------------------------------------------------
void dmToolGroup::OnRedrawCarrets( dmGraphics& _gr ) const
{
  for(const_iterator it=Begin(),last=End();it!=last;++it ) {
    if( (*it)->IsVisible() ) 
     (*it)->OnRedrawCarrets(_gr);
  }
}
//------------------------------------------------------
void dmToolGroup::Recalculate()
{
  dmTool::Recalculate(); // Call before updateting object
  for(iterator it=Begin(),last=End();it!=last;++it) 
    (*it)->RecalcObject();
}
//------------------------------------------------------
void dmToolGroup::Push_Front( dmTool* tool) 
{ 
  if(tool) {
    if(mToolList.Find( tool )==End()) {
      tool->Obtain();
      mToolList.Push_Front(tool);
    }
  }
}
//------------------------------------------------------
void dmToolGroup::Push_Back ( dmTool* tool) 
{ 
  if(tool) { 
    if(mToolList.Find( tool )==End()) {
      tool->Obtain();
      mToolList.Push_Back(tool);
    }
  } 
}
//------------------------------------------------------
void dmToolGroup::Insert( iterator it, dmTool* tool) 
{
  if(tool) { 
    if(mToolList.Find( tool )==End()) {
      tool->Obtain();
      mToolList.Insert(it,tool);
    }
  }
}
//------------------------------------------------------
void dmToolGroup::Remove( dmTool* tool )
{
  iterator it = mToolList.Find( tool );
  if(it!=End()) 
    Remove( it );
}
//------------------------------------------------------
dmTool* dmToolGroup::Remove( iterator it )
{
  dmTool* tool = *it;
  tool->Release();
  mToolList.Erase(it);
  return tool;
}
//------------------------------------------------------
void dmToolGroup::ClearAndDestroy() 
{
  for(iterator it=Begin(),last=End();it!=last;++it ) { 
    if( (*it)->Release()==0 ) { 
      delete *it; 
    } else 
      if((*it)->Parent()==this) 
        (*it)->Detach(false);
  }
  mToolList.Clear();
}
//------------------------------------------------------
void dmToolGroup::Clear() 
{ 
  O_INVALIDATE(this)
  ClearAndDestroy(); 
}
//------------------------------------------------------
dmTool* dmToolGroup::Clone( dmTool* _parent ) const
{
  return new dmToolGroup( *this, _parent );
}
//-----------------------------------------------------
void dmToolGroup::Accept( dmVisitor& aVisitor )
{
  dmToolVisitor* _Visitor = dmToolVisitor::Cast(&aVisitor);
  if(_Visitor) {
    _Visitor->Visit(*this);
    for(const_iterator it=Begin(),last=End();it!=last;++it) { 
          (*it)->Accept(*_Visitor); 
    }
  }
}
//-----------------------------------------------------
void dmToolGroup::Accept( dmConstVisitor& aVisitor ) const
{
  dmToolConstVisitor* _Visitor = dmToolConstVisitor::Cast(&aVisitor);
  if(_Visitor) {
    _Visitor->Visit(*this);
    for(const_iterator it=Begin(),last=End();it!=last;++it) { 
      (*it)->Accept(*_Visitor); 
    }
  }
}
//-----------------------------------------------------

