#ifndef dmRGBColor_h
#define dmRGBColor_h

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
// File         : dmRGBcolor.h
// Date         : 7/2004
// Author       : David Marteau (daim)
//--------------------------------------------------------

/*
 * Revision 1.1 2006/08/10 daim
 * Added DM_GetAlphaValue macro for 32 bits ARGB encoding
 */

//--------------------------------------------------------------------------
#define dmLUT8_MAX_COLORS 256
#define dmLUT8_MAX_COLOR_INDEX (dmLUT8_MAX_COLORS - 1)
#define dmLUT8_EXTRA_COLORS 6
#define dmLUT8_MAX_RGB_COLORS (dmLUT8_MAX_COLORS+dmLUT8_EXTRA_COLORS)

#define dmLUT8_EXTRA_COLOR1 (dmLUT8_MAX_COLOR_INDEX+1)
#define dmLUT8_EXTRA_COLOR2 (dmLUT8_MAX_COLOR_INDEX+2)
#define dmLUT8_EXTRA_COLOR3 (dmLUT8_MAX_COLOR_INDEX+3)
#define dmLUT8_EXTRA_COLOR4 (dmLUT8_MAX_COLOR_INDEX+4)
#define dmLUT8_EXTRA_COLOR5 (dmLUT8_MAX_COLOR_INDEX+5)
#define dmLUT8_EXTRA_COLOR6 (dmLUT8_MAX_COLOR_INDEX+6)

#define dmLUT8_FIRST_EXTRA_COLOR  dmLUT8_MAX_COLORS
#define dmLUT8_LAST_EXTRA_COLOR  (dmLUT8_MAX_COLOR_INDEX+dmLUT8_EXTRA_COLORS)

namespace dmTk {
  typedef dm_uint32 EColorType;
}

// RGB standard encoding

#define DM_ARGB(a,r,g,b)               \
 ((dmTk::EColorType)(                  \
  ((dm_uint8)(b))|                     \
  ((dm_uint16)((dm_uint8)(g))<<8) |    \
  ((dm_uint16)((dm_uint8)(r))<<16)|    \
  ((dm_uint32)((dm_uint8)(a))<<24)     \
 ))

#define DM_RGB(r,g,b) DM_ARGB(0xff,r,g,b)

#define DM_GETAVALUE( c ) ((dm_byte)((0xFF000000L & c)>>24))
#define DM_GETRVALUE( c ) ((dm_byte)((0x00FF0000L & c)>>16))
#define DM_GETGVALUE( c ) ((dm_byte)((0x0000FF00L & c)>>8))
#define DM_GETBVALUE( c ) ((dm_byte)((0x000000FFL & c)))

//--------------------------------------------
// RGB Color Encoding definition
//--------------------------------------------

// This is mapped to a RGB value
typedef struct { dm_byte red; dm_byte green; dm_byte blue; }                dmRGBColor;

// This is NOT mapped to packed ARGB value
typedef struct { dm_byte alpha; dm_byte red; dm_byte green; dm_byte blue; } dmARGBColor;

// LUT Specification for LUT8 bitmap type
typedef dm_uint16       dmColorIndex;
typedef dmColorIndex*   dmColorMapArray;
typedef dmARGBColor*    dmRGBColorArray;

typedef dmColorIndex dmColorIndexTable[dmLUT8_MAX_COLORS];
typedef dmARGBColor  dmRGBColorTable[dmLUT8_MAX_COLORS];

typedef struct {
  dmARGBColor   ARGB[dmLUT8_MAX_RGB_COLORS]; // 24 bits palette specification
  dmColorIndex  CMap[dmLUT8_MAX_COLORS];     // 8  bits color mapping
} dmLUTSpecs;

#define ARGB_TO_RGB( argb ) (*((dmRGBColor*)&((argb).red)))

inline dmARGBColor MAP_RGB( dm_byte r, dm_byte g, dm_byte b )  {
  dmARGBColor rgbc = { 0xff,r,g,b };
  return rgbc;
}

inline dm_uint32 DM_PACK_ARGB( const dmARGBColor& c ) {
  return DM_ARGB(c.alpha,c.red,c.green,c.blue);
}

//---------------------------------------------------------------------------
#endif // dmRGBColor_h


