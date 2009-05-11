
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

#include <math.h>

#include "common/dmUserLib.h"
#include "templates/processing/dmRotation.h"

//---------------------------------------------------------------------
struct __dm_rotate_impl
{
   const dmImage&  Rhs;
   const dmRect&   Rect;
   dm_real         Angle;

   __dm_rotate_impl(dm_real _Angle,
                    const dmImage&  _Rhs,
                    const dmRect&   _Rect )
   :Rhs(_Rhs)
   ,Rect(_Rect)
   ,Angle(_Angle)
   {}

   template<EPixelFormat _PixelFormat> 
   void operator()( dmIImage<_PixelFormat>& _Lhs )
   {
     dmIImage<_PixelFormat>*  rhs = dmIImage<_PixelFormat>::Cast(&Rhs);

     typedef typename dmIImage<_PixelFormat>::traits_type traits_type;
     typedef typename traits_type::integer_type           integer_type;

     daim::nn_rotate_copy(Angle,rhs->Gen(),Rect,_Lhs.Gen());
   }
};
//---------------------------------------------------------------------
bool dmRotateImage( dm_real _Angle, const  dmImage& _Rhs, const  dmRect& _Rect,
                    dmImage& _Lhs )
{
  if(_Lhs.PixelFormat() == _Rhs.PixelFormat()) 
  {
     __dm_rotate_impl _filter(_Angle,_Rhs,_Rect);
     return dmImplementOperation(_filter,_Lhs);
  }
  return false;    
}
//---------------------------------------------------------------------
bool dmRotateImage( dm_real _Angle, const dmImage& _Rhs, dmImage& _Lhs )
{
  return dmRotateImage(_Angle,_Rhs,_Rhs.Rect(),_Lhs);   
}
//---------------------------------------------------------------------
// The lhs image for the operation is contained in buffer
// while the rhs image is the image parameter
//---------------------------------------------------------------------
bool dmRotateImage( dmBufferParameters& _Params, dm_real _Angle )
{
  if(!_Params.thisBuffer.IsEmpty()) 
  {
     if(_Params.thisImage.PixelFormat() == 
        _Params.thisBuffer.Buffer()->PixelFormat())
     {
       const dmRegion& _Rgn  =  _Params.thisBuffer.BufferRgn();
       const dmImage&  _Rhs  = *_Params.thisBuffer.Buffer();

       __dm_rotate_impl _filter(_Angle,_Rhs,_Rgn.Rectangle());
       return dmImplementOperation(_filter,_Params.thisImage);
    }
  }
  return false;   
}
//---------------------------------------------------------------------
