
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

#define dmUserIncludes
#include "daim_kernel.h"
#include "dmKernelsSet.h"
//---------------------------------------------------
void dmKernelsSet::SetName( const dmString::E* pName )
{
  if(pName) { _name = pName; }
  else _name.Clear();
}
//---------------------------------------------------
void dmKernelsSet::SetTitle( const dmString::E* pTitle )
{
  if(pTitle) { _title = pTitle; }
  else _title.Clear();
}
//---------------------------------------------------
void dmKernelsSet::SetDescription( const dmString::E* pDescription )
{
  if(pDescription) { _description = pDescription; }
  else _description.Clear();
}
//-----------------------------------------------
int dmKernelsSet::Add( dmKernelsSet::Element* pElement ) 
{
#ifdef _DEBUG
  int _parent = pElement->parent;
  if(Empty()) dmASSERT(_parent==-1); 
  else {
    int iprev  = _list.size()-1;
    if(_parent!=iprev) {
      while( (iprev=_list[iprev]->parent)!=_parent ) {
         // if not inserted as child of last entry
         // must be child of ancestors
         dmASSERT( _parent <= iprev );
      }
    }
  }
#endif
  _list.push_back(pElement);
  return _list.size()-1; // Return insertion index
}
//---------------------------------------------------
void dmKernelsSet::Clear()
{
  for(iterator it = begin();it!=end();++it) {
    delete *it;
  }
  _list.clear();
}
//---------------------------------------------------
dmKernelsSet::Element* dmKernelsSet::FindFamily( const dmString::E* name )
{
  for(iterator it = begin();it!=end();++it) 
  {
    if((*it)->name==name) return *it;
  }
  return NULL;
}
//---------------------------------------------------
