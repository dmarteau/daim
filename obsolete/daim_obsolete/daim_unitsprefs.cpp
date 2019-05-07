
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

#define dmUseUtilitiesExtra
#define dmUseServicesMngr
#define dmUseKernelImage
#include "daim_kernel.h"
#include "daim_unitsprefs.h"

#define DAIM_MEAN_AND_LEAN
#include "daim/daim_modules.h"

#define DEF_SECTION   _TXT("Units.def")
////////////////////////////////////////////////////////////////////////////
#define dmUnitsPrefs_SCHEMA_VERSION 1
dmImplementClassInfo( dmUnitsPrefs, dmServices, dmUnitsPrefs_SCHEMA_VERSION)
//--------------------------------------------------------------------------
dmUnitsPrefs::dmUnitsPrefs() {}
//--------------------------------------------------------------------------
dmUnitsPrefs::~dmUnitsPrefs() {}
//---------------------------------------------------------------------------
void dmUnitsPrefs::SetDefaultConfiguration() 
{ 
  mUnits.mUnitName      = _TXT("Pixels");
  mUnits.mUnitLabel     = _TXT("pixels");
  mUnits.mAspectRatio   = 1.0;
  mUnits.mUnitsPerPixel = 1.0; 

  dmIInterface<dmIProperties> _IProps;
  dmDEBUG_ASSERT_VALID_INTERFACE(_IProps);

  if(_IProps.Open(_TXT("config://"))) 
  {
    dmString _default = _IProps.GetData(_TXT("Local"),_TXT("Units.default"),NULL);
    if(!_default.Empty())
        ImportUnits(_default.CStr());
  }
}
//---------------------------------------------------------------------------- 
bool dmUnitsPrefs::ExportUnits( const dmString::E* _name )
{
  return WriteSpatialUnits(mUnits,_name);  
}
//----------------------------------------------
bool dmUnitsPrefs::ImportUnits( const dmString::E* _name )
{
  return ReadSpatialUnits(mUnits,_name);
}
//----------------------------------------------
// Static functions
//----------------------------------------------
#define TOKEN_SEP _TXT(":")

bool dmUnitsPrefs::ReadSpatialUnits( dmSpatialUnits& _Data, const char_t* _name )
{
  if(EMPTY_STRING(_name))
      return false;

  bool result = false;

  dmIInterface<dmIProperties> _IProps;
  dmDEBUG_ASSERT_VALID_INTERFACE(_IProps);

  if(_IProps.Open(_TXT("config://"))) 
  {
    dmString _units = _IProps.GetData(DEF_SECTION,_name);
    if(!_units.Empty()) {
      // Parse as name:label:upp:ar
      dmString res;
      size_t pos = 0;
      pos = _units.Parse( res,pos, TOKEN_SEP ); _Data.mUnitName  = res;
      pos = _units.Parse( res,pos, TOKEN_SEP ); _Data.mUnitLabel = res;
      pos = _units.Parse( res,pos, TOKEN_SEP ); _Data.mUnitsPerPixel  = res.AsDouble();             
      pos = _units.Parse( res,pos, TOKEN_SEP ); _Data.mAspectRatio    = res.AsDouble();             
      result = true;             
    } else dmLOG(_TXT("WARNING : not entry '%s' for calibration data\n"),
               _name);
  }
  return result;
}
//----------------------------------------------
bool dmUnitsPrefs::WriteSpatialUnits( const dmSpatialUnits& _Data, const char_t* _name )
{
  if(EMPTY_STRING(_name))
      return false;

  bool result = false;

  dmIInterface<dmIProperties> _IProps;
  dmDEBUG_ASSERT_VALID_INTERFACE(_IProps);

  if(_IProps.Open(_TXT("config://"))) 
  {
    dmString _units;
    _units.Format(dmString::npos,_TXT("%s:%s:%lf:%lf"),
       _Data.mUnitName.CStr(),
       _Data.mUnitLabel.CStr(),
       _Data.mUnitsPerPixel,
       _Data.mAspectRatio
     );
   
     result = _IProps.SetData(DEF_SECTION,_name,_units.CStr());
  }
  return result;
}
//----------------------------------------------

