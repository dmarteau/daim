
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

#ifndef NO_DAIM_USER_INTERFACE

#define NO_CCI_DECLS

struct cci_Class;
struct cci_Object;

#include "daim_cmdtarget.h"
//---------------------------------------------------------
bool daim_CmdUI::DoUpdate( dm_uint code, daim_CmdTarget* pTarget)
{
  bEnabled = false;
  nCode    = code;
  if(!pTarget) return false;
  else 
   if(pTarget->OnCommandUpdate(this))
     return true;
   
  Enable(false);
  return bEnabled; // not handled if false
}
//---------------------------------------------------------
void daim_CmdUI::Enable(bool bOn)
{
  bEnabled = bOn;
}
//---------------------------------------------------------
void daim_CmdUI::SetCheck(int nCheck)
{
  checked = nCheck;
}
//---------------------------------------------------------
const COMMAND_MAP*      daim_CmdTarget::GetMessageMap() const { return &daim_CmdTarget::messageMap; }                     \
const COMMAND_MAP       daim_CmdTarget::messageMap        = { NULL, &daim_CmdTarget::_messageEntries[0] };\
const COMMAND_MAP_ENTRY daim_CmdTarget::_messageEntries[] = {0, NULL,NULL };
//-------------------------------------------------------------------
static const COMMAND_MAP_ENTRY* __find_command_entry( const COMMAND_MAP* pMap, dm_uint nId )
{
  const COMMAND_MAP_ENTRY* pEntry = (COMMAND_MAP_ENTRY*)pMap->lpEntries;
  do {
    for( ; pEntry->nCode != 0; ++pEntry ) {
      if(pEntry->nCode == nId) 
        return pEntry;
    }
    pMap   = pMap->pBaseMap;
    pEntry = (pMap?pMap->lpEntries:NULL);
  } while(pEntry != NULL);
  return NULL;
}
//-------------------------------------------------------------------
bool daim_CmdTarget::OnCommandUpdate ( daim_CmdUI* pCmdUI )
{
  const COMMAND_MAP_ENTRY* pEntry = __find_command_entry(GetMessageMap(),pCmdUI->nCode);
  if(pEntry) {
    if(pEntry->pfnUpdate)
      (this->*pEntry->pfnUpdate)(pCmdUI);
    else if(pEntry->pfn)
      pCmdUI->Enable(true);  
    else                     
      pCmdUI->Enable(false);
    return true;
  }
  pCmdUI->Enable(false);
  return false;
}
//-------------------------------------------------------------------
bool daim_CmdTarget::OnCommand( dm_uint nCode )
{
  const COMMAND_MAP_ENTRY* pEntry = __find_command_entry(GetMessageMap(),nCode);
  if(pEntry && pEntry->pfn) 
  {
    if(pEntry->pfnUpdate) {
      daim_CmdUI cmdUI;
      (this->*pEntry->pfnUpdate)(&cmdUI);
      if(!cmdUI.bEnabled) 
        return true;       // handled but disabled
    }
    (this->*pEntry->pfn)();
    return true;
  }
  return false;
}
//-------------------------------------------------------------------
#endif // NO_DAIM_USER_INTERFACE

