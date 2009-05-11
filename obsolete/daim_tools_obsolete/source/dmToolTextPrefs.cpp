
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
#include "dmToolPrefs.h"
#include "dmToolTextPrefs.h"
#include "dmText.h"
//----------------------------------------------------------
#define dmToolTextPrefs_SCHEMA_VERSION 1
dmImplementClassInfo( dmToolTextPrefs, dmToolPrefs, dmToolTextPrefs_SCHEMA_VERSION )
//----------------------------------------------------------
dmToolTextPrefs::dmToolTextPrefs() 
: _Alignement(eAlignStyle_center|eAlignStyle_vcenter)
, _MarginSize(0)
, _AutoAdjust(false)
{}
//----------------------------------------------------------
dmToolTextPrefs::~dmToolTextPrefs()
{}
//----------------------------------------------------------
void dmToolTextPrefs::SetDefaultConfiguration()
{
  _TextAttrs.SetColor(DM_RGB(0,0,0));
  _MarginSize = 0;
}
//----------------------------------------------------------
bool dmToolTextPrefs::GetCopy( const dmToolPrefs* toCopy )
{
  dmToolTextPrefs* pPrefs = dmToolTextPrefs::Cast(toCopy);
  if(pPrefs) 
  {
    _TextAttrs  = pPrefs->_TextAttrs;
    _Alignement = pPrefs->_Alignement;
    _MarginSize = pPrefs->_MarginSize;
    _AutoAdjust = pPrefs->_AutoAdjust;
    return true;
  }
  return false;
}
//----------------------------------------------------------
bool dmToolTextPrefs::SetPrefs( dmTool* pTool )
{
  dmText* pText = dmText::Cast(pTool);
  if(pText) 
  { 
    pText->SetAlignement( _Alignement );
    pText->SetMarginSize( _MarginSize );
    if(_AutoAdjust) 
      pText->FitBorderToText(); 
    return true;
  }
  return false;
}
//----------------------------------------------------------
bool dmToolTextPrefs::GetPrefs( dmTool* pTool )
{
  dmText* pText = dmText::Cast(pTool);
  if(pText) 
  { 
    _Alignement = pText->Alignement();
    _MarginSize = pText->MarginSize();
    _AutoAdjust = false;
    return true;
  }
  return false; 
}
//----------------------------------------------------------
