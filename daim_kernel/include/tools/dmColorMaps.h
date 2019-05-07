#ifndef dmColorMaps_h
#define dmColorMaps_h

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
// File         : dmColorMaps.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

namespace dmColorMap {

void GrayScale     ( dmRGBColorArray );
void Spectrum      ( dmRGBColorArray );
void Spectrum32    ( dmRGBColorArray );
void Spectrum64    ( dmRGBColorArray );
void LogPalette    ( dmRGBColorArray );
void LevelsPalette ( dmRGBColorArray );
void RedPalette    ( dmRGBColorArray );
void GreenPalette  ( dmRGBColorArray );
void BluePalette   ( dmRGBColorArray );
void HuePalette    ( dmRGBColorArray, dm_uint8 hue );
void RGBPalette    ( dmRGBColorArray, dm_uint8 r,dm_uint8 g,dm_uint8 b);

};

#endif // dmColorMaps_h

