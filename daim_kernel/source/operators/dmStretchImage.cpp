
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
#include "daim_kernel.h"

#include "common/dmUserLib.h"
#include "templates/processing/dmLinearStretch.h"

//---------------------------------------------------------------------

namespace {

struct nn_stretch_impl
{
   const dmImage&  Src;
   const dmRect&   Rect;

   nn_stretch_impl(const dmImage&  _Src,
                   const dmRect&   _Rect )
   :Src(_Src)
   ,Rect(_Rect)
   {}

   template<EPixelFormat _PixelFormat>
   void operator()( dmIImage<_PixelFormat>& _Dest )
   {
     dmIImage<_PixelFormat>*  src = dmIImage<_PixelFormat>::Cast(&Src);

     typedef typename dmIImage<_PixelFormat>::traits_type traits_type;
     typedef typename traits_type::integer_type           integer_type;

     daim::nn_stretch(src->Gen(),Rect,_Dest.Gen());
   }
};
//---------------------------------------------------------------------
struct bilinear_stretch_impl
{
   const dmImage&  Src;
   const dmRect&   Rect;

   bilinear_stretch_impl(const dmImage&  _Src,
                         const dmRect&   _Rect )
   :Src(_Src)
   ,Rect(_Rect)
   {}

   template<EPixelFormat _PixelFormat>
   void operator()( dmIImage<_PixelFormat>& _Dest )
   {
     dmIImage<_PixelFormat>*  src = dmIImage<_PixelFormat>::Cast(&Src);

     typedef typename dmIImage<_PixelFormat>::traits_type traits_type;
     typedef typename traits_type::integer_type           integer_type;

     daim::bilinear_stretch(src->Gen(),Rect,_Dest.Gen());
   }
};

}; // namespace
//---------------------------------------------------------------------
bool dmLinearStretch( dmInterpolationType mode,
                      const  dmImage& _Src, const  dmRect& _Rect,
                      dmImage& _Dest )
{
  if(_Src.PixelFormat() == _Dest.PixelFormat())
  {
    switch(mode)
    {
      case dmDecimation : {
        nn_stretch_impl _filter(_Src,_Rect);
        return dmImplementOperation(_filter,_Dest);
      } break;

      case dmBilinear : {
        bilinear_stretch_impl _filter(_Src,_Rect);
        return dmImplementOperation(_filter,_Dest);
      } break;

      default:
        return false;
    }
  }
  return false;
}
//---------------------------------------------------------------------
bool dmLinearStretch(  dmInterpolationType mode, const dmImage& _Src, dmImage& _Dst )
{
  return dmLinearStretch(mode,_Src,_Src.Rect(),_Dst);
}
//---------------------------------------------------------------------
// The lhs image for the operation is contained in buffer
// while the rhs image is the image parameter
//---------------------------------------------------------------------
bool dmLinearStretch( dmBufferParameters& _Params, dmInterpolationType mode )
{
  if(!_Params.thisBuffer.IsEmpty())
  {
     if(_Params.thisImage.PixelFormat() ==
        _Params.thisBuffer.Buffer()->PixelFormat())
     {
       const dmRegion& _Rgn  =  _Params.thisBuffer.BufferRgn();
       const dmImage&  _Src  = *_Params.thisBuffer.Buffer();

       switch(mode)
       {
         case dmDecimation : {
           nn_stretch_impl _filter(_Src,_Rgn.Rectangle());
           return dmImplementOperation(_filter,_Params.thisImage);
         } break;

         case dmBilinear : {
           bilinear_stretch_impl _filter(_Src,_Rgn.Rectangle());
           return dmImplementOperation(_filter,_Params.thisImage);
         } break;

         default:
           return false;
       }
    }
  }
  return false;
}
//---------------------------------------------------------------------
