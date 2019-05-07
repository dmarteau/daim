#ifndef daim_unitsprefs_h
#define daim_unitsprefs_h


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

class dmUnitsPrefs : public dmServices
{
  public:
    dmSpatialUnits mUnits;

  public:
    dmUnitsPrefs();
    virtual ~dmUnitsPrefs();

    virtual void SetDefaultConfiguration();

    bool ExportUnits( const dmString::E*  );
    bool ImportUnits( const dmString::E*  );

    static bool ReadSpatialUnits ( dmSpatialUnits&      , const dmString::E* );
    static bool WriteSpatialUnits( const dmSpatialUnits&, const dmString::E* );

    dmDeclareClassInfo( dmUnitsPrefs );
};

#endif // daim_unitsprefs_h 
