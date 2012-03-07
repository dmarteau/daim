
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
#include "templates/processing/dmArithmetics.h"
#include "templates/rgb/dmRGBArithmetics.h"

//---------------------------------------------------------------------

namespace {

struct addmul_impl
{
  float            a,b;
   const dmRegion& Rgn;

   addmul_impl(dm_real  _a, dm_real _b, const dmRegion& _Rgn )
  : a(static_cast<float>(_a))
   ,b(static_cast<float>(_b))
   ,Rgn(_Rgn) {}

  template<EPixelFormat _PixelFormat> 
  void operator()( dmIImage<_PixelFormat>& _Rhs )
  {
    typedef typename dmIImage<_PixelFormat>::traits_type traits_type;
    typedef typename traits_type::value_type             value_type;

    if(a==0.0) 
      daim::transform(Rgn,_Rhs.Gen(),
            std::bind2nd(daim::fn::add_pixel<value_type>(),b));
    else if(b==0.0)
      daim::mul_image(Rgn,_Rhs.Gen(),a);
    else
      daim::addmul_image(Rgn,_Rhs.Gen(),a,b);
  }

  void operator()( dmIImage<dmPixelFormat24bppRGB>& _Rhs )
  {
    if(a==0.0) a = 1.0;

    if(b==0.0)
      daim::mul_image(Rgn,_Rhs.Gen(),a);
    else
      daim::addmul_image(Rgn,_Rhs.Gen(),a,b);
  }

};

}; // namespace
//---------------------------------------------------------------------
// Perform the x |-> a * x + b transformation
//---------------------------------------------------------------------
bool dmAddMulImage( dmImage& _Image, const dmRegion& _Rgn,
                    dm_real a, dm_real b )
{
  dmRegion Rgn(_Rgn);
  Rgn.ClipToRect(_Image.Rect());
  if(!Rgn.IsEmptyRoi()) {
    addmul_impl _filter(a,b,Rgn); 
    return dmImplementOperation(_filter,_Image);
  }
  return false;    
}
//---------------------------------------------------------------------
bool dmAddMulImage( dmImage& _Image, dm_real a, dm_real b )
{
  addmul_impl _filter(a,b,_Image.Rect());
  return dmImplementOperation(_filter,_Image);
}
//---------------------------------------------------------------------
