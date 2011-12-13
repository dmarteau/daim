
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
#include "daim_kernel.h"
#include "common/dmUserLib.h"

using namespace daim;

//--------------------------------------------------------------------------
bool dmConvertToRGB ( const dmImage& _src, dmImage& _dest, 
                      dm_real rmin, dm_real rmax )
{
  if(_dest.PixelFormat()!=dmPixelFormat24bppRGB) return 0;

  if(_src.PixelFormat()==dmPixelFormat8bppIndexed||
     _src.PixelFormat()==dmPixelFormat24bppRGB)
  {
    _dest.GetCopy(_src);
    return true;
  } else {
    // first convert to 8 bits;
    dmLink<dmImage> img = dmCreateImage<dmPixelFormat8bppIndexed>(_dest.Width(),_dest.Height());
    if(dmScaleTo8Bits(_src,*img,rmin,rmax))
    {
      _dest.GetCopy(*img);
      return true;
    }
  }
  return false;
}  
//--------------------------------------------------------------------------
bool dmConvertTo8bits( const dmImage& _src, dmImage& _dest,
                       dm_real rmin, dm_real rmax  )
{
  if(_dest.PixelFormat()!=dmPixelFormat8bppIndexed) 
    return 0;

  if(_src.PixelFormat()==dmPixelFormat8bppIndexed||
     _src.PixelFormat()==dmPixelFormat24bppRGB)
  {
     _dest.GetCopy(_src);
     return 1;
  } 
  return dmScaleTo8Bits(_src,_dest,rmin,rmax);
}
//--------------------------------------------------------------------------
bool dmCopyRGBData( const dmImage& src, dmImage& dest , dm_uint channel,
                    const dmRegion& rgn, const dmPoint& p )
{
  if(src.PixelFormat()==dmPixelFormat24bppRGB && 
    dest.PixelFormat()==dmPixelFormat8bppIndexed)  
  {
    const dmIImage<dmPixelFormat24bppRGB>*    pSrc  = dmIImage<dmPixelFormat24bppRGB>   ::Cast(&src);
          dmIImage<dmPixelFormat8bppIndexed>* pDest = dmIImage<dmPixelFormat8bppIndexed>::Cast(&dest);

    switch(channel) {
      case 1: transform(rgn,p,pSrc->Gen(),pDest->Gen(),daim::pixel_traits<dm_rgb24>::get_red())  ; return true;
      case 2: transform(rgn,p,pSrc->Gen(),pDest->Gen(),daim::pixel_traits<dm_rgb24>::get_green()); return true;
      case 3: transform(rgn,p,pSrc->Gen(),pDest->Gen(),daim::pixel_traits<dm_rgb24>::get_blue()) ; return true;
    }
  } 
  
  if( src.PixelFormat() ==dmPixelFormat8bppIndexed && 
      dest.PixelFormat()==dmPixelFormat24bppRGB) 
  {
    const dmIImage<dmPixelFormat8bppIndexed>* pSrc  = dmIImage<dmPixelFormat8bppIndexed> ::Cast(&src);
          dmIImage<dmPixelFormat24bppRGB>*    pDest = dmIImage<dmPixelFormat24bppRGB>    ::Cast(&dest);

    switch(channel) {
      case 1: combine(rgn,p,pSrc->Gen(),pDest->Gen(),daim::pixel_traits<dm_rgb24>::set_red())  ; return true;
      case 2: combine(rgn,p,pSrc->Gen(),pDest->Gen(),daim::pixel_traits<dm_rgb24>::set_green()); return true;
      case 3: combine(rgn,p,pSrc->Gen(),pDest->Gen(),daim::pixel_traits<dm_rgb24>::set_blue()) ; return true;
    }      
  }

  return false;
}
//--------------------------------------------------------------------------
bool dmCopyRGBData( const dmImage& src, dmImage& dest , dm_uint channel )
{
 return dmCopyRGBData(src,dest,channel,src.Rect(),dmPoint(0,0));
}  
//--------------------------------------------------------------------------
bool dmCopyRGBData( dmImage& rgb, dmImage& red , dmImage& green, dmImage& blue , bool toRGB )
{
  if(toRGB) {
    if( 
      dmCopyRGBData(red  ,rgb,1) &&
      dmCopyRGBData(green,rgb,2) &&
      dmCopyRGBData(blue ,rgb,3)
    ) return true;
  } else {
    if( 
      dmCopyRGBData(rgb,red  ,1) &&
      dmCopyRGBData(rgb,green,2) &&
      dmCopyRGBData(rgb,blue ,3)
    ) return true; 
  }
  return false;
}  
//--------------------------------------------------------------------------
