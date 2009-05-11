
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

#define dmUseKernelImageTemplates
#define dmUseKernelBuffer

#define dmUseLibChannels

#include "daim_kernel.h"
#include "images/dmChannels.h"

//----------------------------------------------------------------------
// Split RGB
// Split rgb values in 8 bits destination images
//------------------------------------------------------------------------
bool dmSplitRGB( const dmImage& _RGB,
                 dmImage& _R, dmImage& _G, dmImage& _B,
                 const dmRect&  _Rect, 
                 const dmPoint& _P )
{
  return _dmSplitRGB(_RGB,_R,_G,_B,_Rect,_P);
}

//----------------------------------------------------------------------
// Merge RGB
// Merge rgb 8 bits values in rgb destination images
//------------------------------------------------------------------------
bool dmMergeRGB( dmImage& _RGB,
                 const dmImage& _R, const dmImage& _G, const dmImage& _B,
                 const dmRect&  _Rect, 
                 const dmPoint& _P )
{
  return _dmMergeRGB(_RGB,_R,_G,_B,_Rect,_P);
}
//----------------------------------------------------------------------
// Split HSV
//------------------------------------------------------------------------
bool dmSplitHSV( const dmImage& _RGB,
                 dmImage& _H, dmImage& _S, dmImage& _V,
                 const dmRect&  _Rect, 
                 const dmPoint& _P )
{
  return _dmSplitHSV(_RGB,_H,_S,_V,_Rect,_P);
} 
//----------------------------------------------------------------------
// Merge HSV
// Merge hsv 8 bits values in rgb destination images
//------------------------------------------------------------------------
bool dmMergeHSV( dmImage& _RGB,
                 const dmImage& _H, const dmImage& _S, const dmImage& _V,
                 const dmRect&  _Rect, 
                 const dmPoint& _P )
{
  return _dmMergeHSV(_RGB,_H,_S,_V,_Rect,_P);
}
//----------------------------------------------------------------------
// Split HLS
//------------------------------------------------------------------------
bool dmSplitHLS( const dmImage& _RGB,
                 dmImage& _H, dmImage& _L, dmImage& _S,
                 const dmRect&  _Rect, 
                 const dmPoint& _P )
{
  return _dmSplitHLS(_RGB,_H,_L,_S,_Rect,_P);
} 
//----------------------------------------------------------------------
// Merge HLS
// Merge hls 8 bits values in rgb destination images
//------------------------------------------------------------------------
bool dmMergeHLS( dmImage& _RGB,
                 const dmImage& _H, const dmImage& _L, const dmImage& _S,
                 const dmRect&  _Rect, 
                 const dmPoint& _P )
{
  return _dmMergeHLS(_RGB,_H,_L,_S,_Rect,_P);
} 
 
//----------------------------------------------------------------------
// Split YUV
//------------------------------------------------------------------------
bool dmSplitYUV( const dmImage& _RGB,
                 dmImage& _Y, dmImage& _U, dmImage& _V,
                 const dmRect&  _Rect, 
                 const dmPoint& _P )
{
  return _dmSplitYUV(_RGB,_Y,_U,_V,_Rect,_P);
} 
//----------------------------------------------------------------------
// Merge YUV
//------------------------------------------------------------------------
bool dmMergeYUV( dmImage& _RGB,
                 const dmImage& _Y, const dmImage& _U, const dmImage& _V,
                 const dmRect&  _Rect, 
                 const dmPoint& _P )
{
  return _dmMergeYUV(_RGB,_Y,_U,_V,_Rect,_P);
} 
//----------------------------------------------------------------------
// Split YIQ
//------------------------------------------------------------------------
bool dmSplitYIQ( const dmImage& _RGB,
                 dmImage& _Y, dmImage& _I, dmImage& _Q,
                 const dmRect&  _Rect, 
                 const dmPoint& _P )
{
  return _dmSplitYIQ(_RGB,_Y,_I,_Q,_Rect,_P);
} 
//----------------------------------------------------------------------
// Merge YIQ
//------------------------------------------------------------------------
bool dmMergeYIQ( dmImage& _RGB,
                 const dmImage& _Y, const dmImage& _I, const dmImage& _Q,
                 const dmRect&  _Rect, 
                 const dmPoint& _P )
{
  return _dmMergeYIQ(_RGB,_Y,_I,_Q,_Rect,_P);
} 

//----------------------------------------------------------------------
// Split OCP
//------------------------------------------------------------------------
bool dmSplitOCP( const dmImage& _RGB,
                 dmImage& _RG, dmImage& _BY, dmImage& _WB,
                 const dmRect&  _Rect, 
                 const dmPoint& _P )
{
  return _dmSplitOCP(_RGB,_RG,_BY,_WB,_Rect,_P);
} 
//----------------------------------------------------------------------
// Merge OCP
//------------------------------------------------------------------------
bool dmMergeOCP( dmImage& _RGB,
                 const dmImage& _RG, const dmImage& _BY, const dmImage& _WB,
                 const dmRect&  _Rect, 
                 const dmPoint& _P )
{
  return _dmMergeOCP(_RGB,_RG,_BY,_WB,_Rect,_P);
} 
//----------------------------------------------------------------------
// Split GSC
//------------------------------------------------------------------------
bool dmSplitGSC( const dmImage& _RGB,
                 dmImage& _G, dmImage& _S, dmImage& _C,
                 const dmRect&  _Rect, 
                 const dmPoint& _P )
{
  return _dmSplitGSC(_RGB,_G,_S,_C,_Rect,_P);
}
//----------------------------------------------------------------------
// Split GSL
//------------------------------------------------------------------------
bool dmSplitGSL( const dmImage& _RGB,
                 dmImage& _G, dmImage& _S, dmImage& _L,
                 const dmRect&  _Rect, 
                 const dmPoint& _P )
{
  return _dmSplitGSL(_RGB,_G,_S,_L,_Rect,_P);
}
//------------------------------------------------------------------------
