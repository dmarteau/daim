
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

/* $ChangeLog  v2.5, 25 ao�t 2006 12:52:08 , David
 *
 * La fonction blend : prend en compte desormais une
 */

#define dmUseKernelImageTemplates
#define dmUseKernelBuffer
#include "daim_kernel.h"

#include "common/dmUserLib.h"
#include "templates/processing/dmArithmetics.h"
#include "templates/rgb/dmRGBArithmetics.h"

//---------------------------------------------------------------------

namespace {

struct blend_impl
{
   float           Percent;
   const dmImage&  Src;
   const dmRegion& Rgn;
   const dmPoint&  Pt;

   blend_impl(dm_real _Percent,
              const dmImage&  _Src,
              const dmRegion& _Rgn,
              const dmPoint&  _Pt)

   :Percent(static_cast<float>(_Percent))
   ,Src(_Src)
   ,Rgn(_Rgn)
   ,Pt(_Pt)
   {}

  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _Dst )
  {
    dmIImage<_PixelFormat>*  lhs = dmIImage<_PixelFormat>::Cast(&Src);

    typedef typename dmIImage<_PixelFormat>::traits_type traits_type;
    typedef typename traits_type::integer_type           integer_type;

    daim::blend_images(Rgn,Pt,lhs->Gen(),_Dst.Gen(),Percent);
  }
};

}; // namespace
//---------------------------------------------------------------------
bool dmBlendImage( dm_real _Percent,
                   const dmImage& _Src, dmImage& _Dest,
                   const dmRegion& _Rgn, const dmPoint& _Pt )
{
  if(_Src.PixelFormat() == _Dest.PixelFormat()) {
   blend_impl _filter(_Percent,_Src,_Rgn,_Pt);
   return dmImplementOperation(_filter,_Dest);
  }
  return false;
}
//---------------------------------------------------------------------
bool dmBlendImage( dm_real _Percent, const dmImage& _Src, dmImage& _Dst  )
{
  return dmBlendImage(_Percent,_Src,_Dst,_Src.Rect(),dmPoint(0,0));
}
//---------------------------------------------------------------------
// The lhs image for the operation is contained in buffer
// while the rhs image is the image parameter
//---------------------------------------------------------------------
bool dmBlendImage( dmBufferParameters& _Params, dm_real _Percent )
{
  if(!_Params.thisBuffer.IsEmpty())
  {
     if(_Params.thisImage.PixelFormat() ==
        _Params.thisBuffer.Buffer()->PixelFormat())
     {
       const dmRegion& _Rgn  =  _Params.thisBuffer.BufferRgn();
       const dmPoint&  _Pt   =  _Params.thisBuffer.BufferSrc();
       const dmImage&  _Src  = *_Params.thisBuffer.Buffer();

       blend_impl _filter(_Percent,_Src,_Rgn,_Pt);
       return dmImplementOperation(_filter,_Params.thisImage);
    }
  }
  return false;
}
//---------------------------------------------------------------------
