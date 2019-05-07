
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

#define dmUseServicesMngr
#define dmUseAreaEdit
#include "daim_tools.h"
#include "dmToolClasses.h"
#include "dmToolPrefs.h"
#include "dmToolPrefsVisitor.h"

#include <list>
//--------------------------------------------------------------------
// Use to maintain a list of prefs managers, referenced by _id
//--------------------------------------------------------------------
dmToolPrefsVisitor& dmToolPrefsVisitor::GetPrefs( const dmString::E* _id )
{
   typedef  dmLink<dmToolPrefsVisitor> link_type;

   link_type _prefs;

   static std::list<link_type> _list;
   std::list<link_type>::iterator it   = _list.begin();
   std::list<link_type>::iterator last = _list.end();
   for(;it!=last;++it) {
     if( (*it)->Id()==_id ) {
       _prefs = *it;
       break;
     }
   }
   
   // Not found : create a new one

   if(_prefs.IsNull()) {
      _prefs = dmAutoLink<dmToolPrefsVisitor>(new  dmToolPrefsVisitor(_id));
      _list.push_back(_prefs);
   }

   return *_prefs;
}
//--------------------------------------------------------------------
dmToolPrefsVisitor::dmToolPrefsVisitor( const dmString& id ) 
: _id( id)
{}
//--------------------------------------------------------------------
dmToolPrefsVisitor::~dmToolPrefsVisitor() 
{}
//--------------------------------------------------------------------
dmServices* dmToolPrefsVisitor::GetServices( dmClassInfo* pInfo )
{
  if(pInfo) 
    return _mManager.GetServices(pInfo);
    
  return NULL;
}
//--------------------------------------------------------------------
// Return the preference for this tool
//--------------------------------------------------------------------
dmLink<dmToolPrefs> dmToolPrefsVisitor::GetPrefs( dmTool* pTool )
{
  if(pTool) {
    CoercePrefs(pTool->ObjectInfo()->Name());
    if(_prefs && _prefs->GetPrefs(pTool))
      return _prefs;
  }
  return dmReturnLink( (dmToolPrefs*)NULL );
}
//--------------------------------------------------------------------
dmLink<dmToolPrefs> dmToolPrefsVisitor::CoercePrefs( const dmString& _class ) 
{
  dmClassInfo* pInfo  = dmGetToolPrefs( _class.CStr() );
  if(pInfo) {
    _prefs = dmAutoLink<dmToolPrefs>(dmToolPrefs::Cast(pInfo->Create()));
    dmASSERT( !_prefs.IsNull() );
  } else _prefs.Release();
  return _prefs;
}

   dmServices* GetServices( dmClassInfo* );

//--------------------------------------------------------------------
void dmToolPrefsVisitor::Visit( dmTool& aTool )
{   
  dmToolPrefs* pPrefs = _prefs; 

  if(pPrefs==NULL) {
    dmClassInfo* pInfo = dmGetToolPrefs( aTool.ObjectInfo()->Name() );
    if(pInfo)
       pPrefs = dmToolPrefs::Cast( _mManager.GetServices(pInfo) );
  }

  if(pPrefs) {
    O_LOCKUPDATE(&aTool)
    pPrefs->SetPrefs(&aTool);
  }
  aTool.UpdateObject();
}
//--------------------------------------------------------------------
// Set the current prefs as default
//--------------------------------------------------------------------
void dmToolPrefsVisitor::SetDefaults()
{
  if(!_prefs.IsNull()) {
    dmToolPrefs* pPrefs = dmToolPrefs::Cast( _mManager.GetServices(_prefs->ObjectInfo()) );
    pPrefs->GetCopy(_prefs);
  }
}
//--------------------------------------------------------------------
