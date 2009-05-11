
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
#include "dmRegionEditProto.h"

//----------------------------------------------
#define dmRegionEditProto_SCHEMA_VERSION 1
dmImplementClassInfo( dmRegionEditProto, dmAreaEdit,dmRegionEditProto_SCHEMA_VERSION);
//----------------------------------------------
void dmRegionEditProto::ClearTmpRoi()
{
  if(mDeleteTmp) 
    delete mTmpRegion;

  mDeleteTmp = false;
  mTmpRegion = NULL;
}
//----------------------------------------------
void dmRegionEditProto::OnUpdateRegion( dmRegion& _rgn )
{
  if(mMergeMode == rgnKILL) 
  {
    dmASSERT(mTmpRegion==NULL);
    _rgn.KillRoi();
  }

  if(mTmpRegion) { 
    switch(mMergeMode)
    {
      case rgnCOPY : _rgn = *mTmpRegion      ; break;
      case rgnADD  : _rgn.AddRoi(*mTmpRegion); break;
      case rgnSUB  : _rgn.SubRoi(*mTmpRegion); break;
      case rgnAND  : _rgn.AndRoi(*mTmpRegion); break;
      case rgnXOR  : _rgn.XorRoi(*mTmpRegion); break;
    }
    ClearTmpRoi();
  }
  mMergeMode = rgnCOPY; // restore copy mode
}
//----------------------------------------------
dmRegionEditProto::dmRegionEditProto( dmTool* _parent  ) 
: dmAreaEdit(_parent)
 ,mTmpRegion(NULL)
 ,mDeleteTmp(false)
 ,mMergeMode(rgnCOPY)
{
}
//----------------------------------------------
dmRegionEditProto::dmRegionEditProto( const dmRegion& _roi, dmTool* _parent )
: dmAreaEdit(_parent)
 ,mTmpRegion(NULL)
 ,mDeleteTmp(true)
 ,mMergeMode(rgnCOPY)
{
  mTmpRegion = new dmRegion(_roi);
}
//----------------------------------------------
dmRegionEditProto::dmRegionEditProto( const dmRegionEditProto& _toCopy, dmTool* _parent )
: dmAreaEdit(_toCopy,_parent)
 ,mTmpRegion(NULL)
 ,mDeleteTmp(false)
 ,mMergeMode(rgnCOPY)
{
  if(_toCopy.mTmpRegion) {
    mDeleteTmp = true;
    mTmpRegion = new dmRegion(*_toCopy.mTmpRegion);
  }
}
//----------------------------------------------
dmRegionEditProto::~dmRegionEditProto()
{
  if(mDeleteTmp) { delete mTmpRegion; }
}
//----------------------------------------------
void dmRegionEditProto::MergeRoi( const dmRegion& _roi, dmRegionEditProto::MergeMode _mrgmode )
{
  if(!IsInitialized()) { 
    UpdateRegion(); // update region if needed
    mTmpRegion  = &_roi;
    mMergeMode  = _mrgmode;
    UpdateRegion();
  } else {
    ClearTmpRoi();
    mTmpRegion = &_roi;
    mMergeMode  = _mrgmode;
    UpdateObject();
  } 
}
//----------------------------------------------
void dmRegionEditProto::SetRoi( const dmRegion& _roi )
{
  MergeRoi(_roi,dmRegionEditProto::rgnCOPY);
}
//----------------------------------------------
void dmRegionEditProto::KillRoi()
{
  ClearTmpRoi();
  mMergeMode  = rgnKILL;
  if(!IsInitialized()) UpdateRegion();
  else UpdateObject();
}
//-------------------------------------------------------------
dmTool* dmRegionEditProto::Clone( dmTool* _parent ) const
{
  return new dmRegionEditProto(*this,_parent);
}
//--------------------------------------------------------------
