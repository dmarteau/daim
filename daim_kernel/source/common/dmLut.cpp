
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

#include "dmKernelBase.h"
#include "dmRGBColor.h"
#include "templates/dmKernelTemplates.h"
#include "common/dmLut.h"

#include <algorithm>

#define IS_EXTRA_COLOR_INDEX( x ) daim::in_range( x ,(dmColorIndex)dmLUT8_FIRST_EXTRA_COLOR,(dmColorIndex)dmLUT8_LAST_EXTRA_COLOR)
//--------------------------------------------------
dmLUT::dmLUT()
{
  for(int i=0;i<NumEntries;++i) {
    CMap[i] = i;
    ARGB[i].r = ARGB[i].g = ARGB[i].b = i;
    ARGB[i].a = 0xff;
  }
  ARGB[dmLUT8_EXTRA_COLOR1] = MAP_RGB(255,0,0);
  ARGB[dmLUT8_EXTRA_COLOR2] = MAP_RGB(0,255,0);
  ARGB[dmLUT8_EXTRA_COLOR3] = MAP_RGB(0,0,255);
  ARGB[dmLUT8_EXTRA_COLOR4] = MAP_RGB(0,255,255);
  ARGB[dmLUT8_EXTRA_COLOR5] = MAP_RGB(0,0,0);
  ARGB[dmLUT8_EXTRA_COLOR6] = MAP_RGB(255,255,255);
}
//--------------------------------------------------
dmLUT::dmLUT( const dmLUT& aLUT )
{
  operator=(aLUT);
}
//--------------------------------------------------
dmLUT::~dmLUT() {}
//--------------------------------------------------
dmLUT& dmLUT::operator=( const dmLUT& aLUT )
{
  if( &aLUT != this ) {
    std::copy(aLUT.CMap,aLUT.CMap+NumEntries,CMap);
    std::copy(aLUT.ARGB,aLUT.ARGB+NumEntries,ARGB);
  }
  return *this;
}
//--------------------------------------------------
void dmLUT::InvertMap()
{
  for(int i=(NumEntries>>1);i>=0;--i) { std::swap(CMap[i],CMap[MaxEntry-i]); }
}
//--------------------------------------------------
void dmLUT::SetMap( dmColorMapArray _map )
{
  std::copy( _map,_map+NumEntries,CMap );
}
//--------------------------------------------------
void dmLUT::SetRGB( dmRGBColorArray _rgb )
{
  std::copy( _rgb,_rgb+NumEntries,ARGB );
}
//--------------------------------------------------
void dmLUT::GetMap( dmColorMapArray _map )
{
  std::copy( CMap,CMap+NumEntries,_map );
}
//--------------------------------------------------
void dmLUT::GetRGB( dmRGBColorArray _rgb )
{
  std::copy( ARGB,ARGB+NumEntries,_rgb );
}
//--------------------------------------------------
void dmLUT::AssignRange( dmLUT::IndexType min_index, dmLUT::IndexType max_index, dmColorIndex rgb_index )
{
  if(rgb_index < dmLUT8_MAX_RGB_COLORS) {
    if( min_index > max_index ) std::swap(min_index,max_index);
    std::fill( CMap+min_index,CMap+max_index+1, rgb_index);
  }
}
//--------------------------------------------------
void dmLUT::RestoreMap()
{
   for(int i=0;i<NumEntries;++i) { CMap[i] = i; }
}
//--------------------------------------------------
void dmLUT::SetExtraColor( dmColorIndex extra, const dmRGBColor& _rgb )
{
  if(IS_EXTRA_COLOR_INDEX(extra)) {
    ARGB[extra] = MAP_RGB(_rgb.r,_rgb.g,_rgb.b);
  }
}
//--------------------------------------------------
void dmLUT::GetExtraColor( dmColorIndex extra, dmRGBColor& _rgb )
{
  if(IS_EXTRA_COLOR_INDEX(extra)) { _rgb = ARGB[extra]; }
}
//--------------------------------------------------
void dmLUT::MakePalette( void (*_buildfunc)(dmRGBColorArray) )
{
  _buildfunc(ARGB);
}
//--------------------------------------------------
