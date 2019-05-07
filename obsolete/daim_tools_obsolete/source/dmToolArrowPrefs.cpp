
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
#define dmUseExtendedTools
#include "daim_tools.h"
#include "dmToolPrefs.h"
#include "dmToolArrowPrefs.h"
//----------------------------------------------------------
#define dmToolArrowPrefs_SCHEMA_VERSION 1
dmImplementClassInfo( dmToolArrowPrefs, dmToolPrefs,dmToolArrowPrefs_SCHEMA_VERSION );
 //----------------------------------------------------------
dmToolArrowPrefs::dmToolArrowPrefs() 
: _Position(eArrowStart)
{}
//----------------------------------------------------------
dmToolArrowPrefs::~dmToolArrowPrefs() 
{}
//----------------------------------------------------------
#define _DFLT_ARROW_SIZE 10
void dmToolArrowPrefs::SetDefaultConfiguration()
{
  int a = (int)(_DFLT_ARROW_SIZE*1.5+0.5);
  int b = _DFLT_ARROW_SIZE;
  int c = _DFLT_ARROW_SIZE*2;
  _StartArrow.SetParameters(a,b,c);
  _EndArrow.SetParameters(a,b,c);
  _Position = eArrowStart;
}
#undef _DFLT_ARROW_SIZE
//----------------------------------------------------------
bool dmToolArrowPrefs::GetCopy( const dmToolPrefs* toCopy )
{
  dmToolArrowPrefs* pPrefs = dmToolArrowPrefs::Cast(toCopy);
  if(pPrefs) 
  {
    _StartArrow = pPrefs->_StartArrow;
    _EndArrow   = pPrefs->_EndArrow;
    _Position   = pPrefs->_Position;
    return true;
  }
  return false;
}
//----------------------------------------------------------
bool dmToolArrowPrefs::SetPrefs( dmTool* pTool )
{
  dmLineExtEdit* _pLineEx = dmLineExtEdit::Cast(pTool);
  if(_pLineEx) 
  {
   _pLineEx->SetArrowStart((_Position & dmToolArrowPrefs::eArrowStart?&_StartArrow:NULL));
   _pLineEx->SetArrowEnd  ((_Position & dmToolArrowPrefs::eArrowEnd  ?&_EndArrow  :NULL));
    return true;
  }
  return false;
}
//----------------------------------------------------------
bool dmToolArrowPrefs::GetPrefs( dmTool* pTool )
{
  dmLineExtEdit* _pLineEx = dmLineExtEdit::Cast(pTool);
  if(_pLineEx) 
  { 
    _Position = dmToolArrowPrefs::eArrowNone;
    if(dmArrowTerminator::Cast(_pLineEx->GetArrowStart())) { 
      _StartArrow = *((dmArrowTerminator*)_pLineEx->GetArrowStart()); 
      _Position  |= dmToolArrowPrefs::eArrowStart;
    }
    if(dmArrowTerminator::Cast(_pLineEx->GetArrowEnd())) { 
      _EndArrow   = *((dmArrowTerminator*)_pLineEx->GetArrowEnd());
      _Position |= dmToolArrowPrefs::eArrowEnd;
    }    
    return true;
  }
  return false; 
}
//----------------------------------------------------------

