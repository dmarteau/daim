#ifndef dmToolTextPrefs_h
#define dmToolTextPrefs_h

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

//--------------------------------------------------------
// File         : dmToolTextPrefs.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class __dmTools dmToolTextPrefs : public dmToolPrefs
{
 public:
  int                _Alignement;
  int                _MarginSize;
  bool               _AutoAdjust;
  dmTextAttributs    _TextAttrs;
 
 public:
  dmToolTextPrefs();
  virtual ~dmToolTextPrefs();

  virtual void SetDefaultConfiguration();
  virtual bool GetCopy( const dmToolPrefs* toCopy );

  dmDeclareClassInfo( dmToolTextPrefs );

  virtual bool SetPrefs( dmTool* );
  virtual bool GetPrefs( dmTool* );

  dmPREFS_ISA( _TXT("rectangle") ) 
};
//--------------------------------------------------------
#endif // dmToolTextPrefs_h
