#ifndef dmLut_h
#define dmLut_h

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
// File         : dmLut.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class __dmKernel dmLUT : public dmLUTSpecs
{
  public : enum { MaxEntry=dmLUT8_MAX_COLOR_INDEX, NumEntries=dmLUT8_MAX_COLORS };
  public:
    typedef dm_uint8 IndexType;

    dmLUT();
    dmLUT( const dmLUT& );
    virtual ~dmLUT();

    dmLUT& operator=( const dmLUT& );

    // Acces to color through mapping
    const dmRGBColor& operator[](IndexType i) const { return ARGB_TO_RGB(ARGB[CMap[i]]); }
          dmRGBColor& operator[](IndexType i)       { return ARGB_TO_RGB(ARGB[CMap[i]]); }

    void SetExtraColor( dmColorIndex, const dmRGBColor& );
    void GetExtraColor( dmColorIndex,       dmRGBColor& );

    void AssignRange( IndexType _min, IndexType _max, dmColorIndex );
    void RestoreMap();
    void InvertMap();

    void SetMap( dmColorMapArray );
    void SetRGB( dmRGBColorArray );

    void GetMap( dmColorMapArray );
    void GetRGB( dmRGBColorArray );

    void MakePalette( void (*_buildfunc)(dmRGBColorArray) );
};
//--------------------------------------------------
#endif
