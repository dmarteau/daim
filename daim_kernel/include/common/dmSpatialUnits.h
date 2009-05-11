#ifndef dmSpatialUnits_h
#define dmSpatialUnits_h

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
// File         : dmSpatialUnits.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class dmSpatialUnits : public dmMemoryObject
{
  public:
    dmCString mUnitName;
    dmCString mUnitLabel;

    dm_double mUnitsPerPixel;
    dm_double mAspectRatio;  // y/x pixel aspect ratio;

    dmSpatialUnits( dm_double _ppu = 1.0, dm_double _ar = 1.0  )
    :mUnitsPerPixel( 1.0/_ppu )
    ,mAspectRatio( _ar )
    {}

    __dmKernel dm_double GetLength( const dm_point&,const dm_point&  ) const;

    // Retourne la taille en unité(s) pour un facteur de grossissement g 
    dm_double X_Units( dm_int x = 1 ) const { return mUnitsPerPixel * x; }
    dm_double Y_Units( dm_int y = 1 ) const { return mUnitsPerPixel * mAspectRatio * y; }

    dm_double X_Pixels( dm_real x = 1 ) const { return static_cast<int>(x/mUnitsPerPixel + 0.5); }
    dm_double Y_Pixels( dm_real y = 1 ) const { return static_cast<int>(y/(mUnitsPerPixel * mAspectRatio) + 0.5); }
  
    dm_double GetLength( const dm_line& l ) const { return GetLength(l.start,l.end); }
};

#endif // dmSpatialUnits_h
