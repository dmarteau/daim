
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
#include "dmToolImagePrefs.h"
//----------------------------------------------------------
#define dmToolImagePrefs_SCHEMA_VERSION 1
dmImplementClassInfo( dmToolImagePrefs, dmToolPrefs, dmToolImagePrefs_SCHEMA_VERSION )
//----------------------------------------------------------
dmToolImagePrefs::dmToolImagePrefs() 
: _bRealSize(false),
  _bKeepAspectRatio(true),
  _bCenterImage(false),
  _Margins(0,0)
{}
//----------------------------------------------------------
dmToolImagePrefs::~dmToolImagePrefs()
{}
//----------------------------------------------------------
void dmToolImagePrefs::SetDefaultConfiguration()
{
  _bRealSize        = false;
  _bKeepAspectRatio = true;
  _bCenterImage     = false;
  _Margins.x = _Margins.y = 0;
}
//----------------------------------------------------------
bool dmToolImagePrefs::GetCopy( const dmToolPrefs* toCopy )
{
  dmToolImagePrefs* pPrefs = dmToolImagePrefs::Cast(toCopy);
  if(pPrefs) {
    _bRealSize        = pPrefs->_bRealSize;
    _bKeepAspectRatio = pPrefs->_bKeepAspectRatio;
    _bCenterImage     = pPrefs->_bCenterImage;
    _Margins          = pPrefs->_Margins;
    return true;
  }
  return false;
}
//----------------------------------------------------------
bool dmToolImagePrefs::SetPrefs( dmTool* pTool )
{
  dmImageFrame* _pFrame = dmImageFrame::Cast(pTool);
  if(_pFrame) 
  { 
    _pFrame->SetMargins(_Margins);
    _pFrame->KeepAspectRatio(_bKeepAspectRatio,_bCenterImage );
    if(_bRealSize) 
      _pFrame->SetRealSize();
    return true;
  }
  return false;
}
//----------------------------------------------------------
bool dmToolImagePrefs::GetPrefs( dmTool* pTool )
{
  dmImageFrame* _pFrame = dmImageFrame::Cast(pTool);
  if(_pFrame) 
  { 
    _Margins          = _pFrame->GetMargins();
    _bKeepAspectRatio = _pFrame->KeepRatio();
    _bCenterImage     = _pFrame->CenterImage();
    _bRealSize = false;
    return true;
  }
  return false; 
}
//----------------------------------------------------------

