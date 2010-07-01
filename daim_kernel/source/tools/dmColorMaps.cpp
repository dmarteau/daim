
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

#define dmUseKernelTemplates

#include <math.h>

#include "daim_kernel.h"
#include "tools/dmColorMaps.h"
#include "templates/rgb/dmColorSpaces.h"


namespace dmColorMap {

//----------------------------------------------------
void Spectrum ( dmRGBColorArray rgb )
{
  dm_uint8 hmax=200,H=0,L=0,S=0;

  daim::colorspace::HLS::merger _hls2rgb;
  dmRGBColor _rgb;

  for(int hue=dmLUT8_MAX_COLORS;--hue>=0;++rgb)
  {
   H = (int)( hmax * (hue/(float)dmLUT8_MAX_COLORS) + 0.5);
   L = 128;
   S = 255;
   _hls2rgb(_rgb,H,L,S);
   (*rgb).r   = _rgb.r;
   (*rgb).g = _rgb.g;
   (*rgb).b  = _rgb.b;
  }
}
//----------------------------------------------------
static void __gSpectrumN( dmRGBColorArray rgb, int n )
{
  dm_uint8 hmax=200,H=0,L=0,S=0;

  daim::colorspace::HLS::merger _hls2rgb;
  dmRGBColor _rgb;

  for(int hue=dmLUT8_MAX_COLORS;--hue>=0;++rgb)
  {
   H = (hue/n) * n;
   H = ((int)( hmax * (H/(float)dmLUT8_MAX_COLORS) + 0.5));
   S = 255;
   L = 128;
   _hls2rgb(_rgb,H,L,S);
   (*rgb).r   = _rgb.r;
   (*rgb).g = _rgb.g;
   (*rgb).b  = _rgb.b;
  }
}
//----------------------------------------------------
void Spectrum32( dmRGBColorArray rgb )
{
  __gSpectrumN(rgb,8);
}
//----------------------------------------------------
void Spectrum64( dmRGBColorArray rgb )
{
  __gSpectrumN(rgb,4);
}
//----------------------------------------------------
void GrayScale( dmRGBColorArray rgb)
{
  for(int i=0;i<dmLUT8_MAX_COLORS;++i)
  {
    rgb[i].r = i;
    rgb[i].g = i;
    rgb[i].b = i;
  }
}
//----------------------------------------------------
void LogPalette( dmRGBColorArray rgb )
{
  daim::colorspace::HLS::merger _hls2rgb;
  dmRGBColor _rgb;

  dm_uint8  hmax=200;
  dm_double fscale = hmax/log((dm_real)(hmax+1));
  dm_uint8  H=0,L,S;

  for(int hue=dmLUT8_MAX_COLORS;--hue>=0;++rgb)
  {
    H = static_cast<dm_uint8>( hmax * (hue/(dm_real)dmLUT8_MAX_COLORS) + 0.5 );
    H = static_cast<dm_uint8>( fscale*log((dm_real)(hmax+1-H))+ 0.5 );
    L = 255;
    S = 128;
    _hls2rgb(_rgb,H,L,S);
   (*rgb).r = _rgb.r;
   (*rgb).g = _rgb.g;
   (*rgb).b = _rgb.b;
  }
}
//----------------------------------------------------
void RedPalette( dmRGBColorArray rgb)
{
  for(int i=0;i<dmLUT8_MAX_COLORS;++i)
  {
    rgb[i].r = i;
    rgb[i].g = 0;
    rgb[i].b = 0;
  }
}
//----------------------------------------------------
void GreenPalette( dmRGBColorArray rgb)
{
  for(int i=0;i<dmLUT8_MAX_COLORS;++i)
  {
    rgb[i].r = 0;
    rgb[i].g = i;
    rgb[i].b = 0;
  }
}
//----------------------------------------------------
void BluePalette( dmRGBColorArray rgb)
{
  for(int i=0;i<dmLUT8_MAX_COLORS;++i)
  {
    rgb[i].r = 0;
    rgb[i].g = 0;
    rgb[i].b = i;
  }
}
//----------------------------------------------------
void HuePalette( dmRGBColorArray rgb, dm_uint8 hue )
{
  daim::colorspace::HLS::merger _hls2rgb;
  dmRGBColor _rgb;

  dm_uint8 I,S = 255;

  for(dm_uint i=0;i<dmLUT8_MAX_COLORS;++i)
  {
    I = static_cast<dm_uint8>(i);

    dmARGBColor& c = rgb[i];
    _hls2rgb(_rgb,hue,I,S);
    c.r = _rgb.r;
    c.g = _rgb.g;
    c.b = _rgb.b;
  }
}
//----------------------------------------------------
void RGBColorPalette ( dmRGBColorArray rgb, dm_uint8 r,dm_uint8 g,dm_uint8 b)
{
  daim::colorspace::HLS::channels::get<1,daim::integer_true> _getHue;
  dmRGBColor _rgb;

  _rgb.r = r;
  _rgb.g = g;
  _rgb.b = b;
  HuePalette(rgb,_getHue(_rgb));
}
//----------------------------------------------------
void LevelsPalette( dmRGBColorArray rgb  )
{
  rgb[0].r = rgb[0].g = rgb[0].b = 0;
  rgb[dmLUT8_MAX_COLOR_INDEX].r = 255;
  rgb[dmLUT8_MAX_COLOR_INDEX].g = 255;
  rgb[dmLUT8_MAX_COLOR_INDEX].b = 255;

  for(int i=1;i<dmLUT8_MAX_COLOR_INDEX-2;i+=4)
  {
    rgb[i+1].r = rgb[i].r   = 255;
    rgb[i+1].g = rgb[i+1].b =
    rgb[i].g   = rgb[i].b   = 0;

    rgb[i+2].g = 255;
    rgb[i+2].r = rgb[i+2].b = 0;
    rgb[i+3].g = 255;
    rgb[i+3].r = rgb[i+3].b = 0;
  }
}


//----------------------------------------------------

}; // namespace dmColorMap
