
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

//-----------------------------------------------------------------------
// Implementation of one type of image to another
//
// Conversions to smaller format (i.e UINT16 to UINT8 for example )
// are handled in rescaling source pixels to the full range of values
// allowed for the destination type
//
// Conversion to larger format are handled by copying values directly
// to the destination
//
// Conversions between scalar and compound pixels are handled using
// scalar <-> compound definitions defined in the corresponding pixel traits
//
// A cascaded call to "dmImplementOperation" generate all cross conversions
// from the templates definitions below
//-----------------------------------------------------------------------

#define  dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "images/dmImageOperations.h"
#include "templates/processing/dmArithmetics.h"

using namespace daim;
//-----------------------------------------------------------------------
template<EPixelFormat _From>
struct dmConversionTable
{
  typedef dmPixelFormat<_From> SrcPixelFormat;
  typedef typename SrcPixelFormat::value_type src_value_type;
  //--------------------------------------
  // convert from scalar to scalar
  //--------------------------------------
  template<EPixelFormat _To>
  static bool Convert(
     const dmIImage<_From>& from_image , dmIImage<_To>& to_image,
     const dmRegion& _rgn, const dmPoint& _p,
     pixel_scalar_tag _tag1,
     pixel_scalar_tag _tag2)
     {
       typedef dmPixelFormat<_To>   DstPixelFormat;
       // conversion need to rescale pixel
       if(SrcPixelFormat::ord() > DstPixelFormat::ord() )
       {
          daim::scale_convert_scalar_to_scalar(
              daim::minmax(_rgn,from_image.Gen()),
              DstPixelFormat::traits_type::bounds(),
              from_image.Gen(),to_image.Gen(), _rgn,_p
          );
         return true;
       } else {
         // conversion dont need to rescale pixel, perform copy
         typedef typename DstPixelFormat::traits_type traits_type;
         typedef typename traits_type::template static_convert<src_value_type> static_convert_type;

         static_convert_type static_convert;
         daim::transform(_rgn,_p,from_image.Gen(),to_image.Gen(),static_convert);
         //daim::copy(_rgn,_p,from_image.Gen(),to_image.Gen());
         return true;
       }
     }

   //--------------------------------------
   // convert from scalar to compound
   //--------------------------------------
   template<EPixelFormat _To>
   static bool Convert(
     const dmIImage<_From>& from_image , dmIImage<_To>& to_image,
     const dmRegion& _rgn, const dmPoint& _p,
     pixel_scalar_tag   _tag1,
     pixel_compound_tag _tag2)
     {
       typedef dmPixelFormat<_To> DstPixelFormat;
       // conversion need to rescale pixel
       if(SrcPixelFormat::ord() > DstPixelFormat::ord())
       {
         typename DstPixelFormat::traits_type::to_scalar _to_scalar;
         gap<src_value_type> _newscale(
             _to_scalar(DstPixelFormat::traits_type::min()),
             _to_scalar(DstPixelFormat::traits_type::max())
           );

         // FIXME Visual C++ workaround
         // I had to assert the following or the previous code return
         // 0 for min and max (bug ?) !!!
         dmASSERT( _newscale.upper > _newscale.lower );

         gap<src_value_type> _minmax =  daim::minmax(_rgn,from_image.Gen());
         daim::scale_convert_scalar_to_compound(
              _minmax,_newscale,from_image.Gen(),to_image.Gen(),_rgn,_p
         );
         return true;
       } else {
         typedef typename DstPixelFormat::traits_type traits_type;
         typedef typename traits_type::template from_scalar<src_value_type> from_scalar_type;

         from_scalar_type from_scalar;
         // conversion dont need to rescale pixel, perform copy
         daim::transform(_rgn,_p,from_image.Gen(),to_image.Gen(),from_scalar);
         return true;
       }
     }

   //--------------------------------------
   // Specialize for copying from and to same type
   // this may prevents extra code generation
   //--------------------------------------
   static bool Convert(
     const dmIImage<_From>& from_image , dmIImage<_From>& to_image,
     const dmRegion& _rgn, const dmPoint& _p,
     pixel_scalar_tag _tag1,
     pixel_scalar_tag _tag2)
   {
     daim::copy(_rgn,_p,from_image.Gen(),to_image.Gen());
     return true;
   }

   static bool Convert(
     const dmIImage<_From>& from_image , dmIImage<_From>& to_image,
     const dmRegion& _rgn, const dmPoint& _p,
     pixel_compound_tag _tag1,
     pixel_compound_tag _tag2)
   {
     daim::copy(_rgn,_p,from_image.Gen(),to_image.Gen());
     return true;
   }

// VERSION : There is no compound type other than RGBPixels
#ifdef dmUseCompoundConversion
     //--------------------------------------
     // convert from compound to scalar
     //--------------------------------------
     template<EPixelFormat _To>
     static bool Convert(
       const dmIImage<_From>& from_image , dmIImage<_To>& to_image,
       const dmRegion& _rgn, const dmPoint& _p,
       pixel_compound_tag _tag1,
       pixel_scalar_tag   _tag2)
     {
       typedef dmPixelFormat<_To>   DstPixelFormat;

       SrcPixelFormat::traits_type::to_scalar _to_scalar;
       // conversion need to rescale pixel
       if(SrcPixelFormat::scalar_tag > DstPixelFormat::scalar_tag)
       {
          gap<DstPixelFormat::value_type> _minmax   = DstPixelFormat::traits_type::bounds();
          daim::for_each(_rgn,*from_image,bind_func(_to_scalar,_minmax.inverted()));

          daim::scale_convert_compound_to_scalar(
              _minmax,DstPixelFormat::traits_type::bounds(),
              *from_image,*to_image,_rgn,_p
          );
         return true;
       } else {
         // conversion dont need to rescale pixel, perform copy
         daim::transform(_rgn,_p,*from_image,*to_image,_to_scalar);
         return true;
       }
     }

     //--------------------------------------
     // compound to compound is not implemented
     //--------------------------------------
     static bool Convert(
       const dmImage& _src , dmImage& _dst,
       const dmRegion& _rgn, const dmPoint& _p,
       pixel_compound_tag _tag1,
       pixel_compound_tag _tag2)
     {
       dmTHROW_MSG_EXCEPTION(EImageError,"Cannot perform <compound> to <compound> pixel type !");
     }
#endif // dmUseCompoundConversion
};
//-----------------------------------------------------------------
// Use specialization for rgb
//-----------------------------------------------------------------
#ifndef dmNoRGBImplementation
template<> struct dmConversionTable<dmPixelFormat24bppRGB>
{
     typedef dmPixelFormat<dmPixelFormat24bppRGB> SrcPixelFormat;

     template<EPixelFormat _To>
      static bool Convert(
       const dmIImage<dmPixelFormat24bppRGB>& from_image , dmIImage<_To>& to_image,
       const dmRegion& _rgn, const dmPoint& _p,
       pixel_compound_tag _tag1,
       pixel_scalar_tag   _tag2)
     {
       typedef dmPixelFormat<_To>  DstPixelFormat;

       //daim::RoiOperation(daim::_RGBtoIntensity(),from_image.Gen(),to_image.Gen(),_rgn,_p);
       SrcPixelFormat::traits_type::to_scalar _to_scalar;
       daim::transform(_rgn,_p,from_image.Gen(),to_image.Gen(),_to_scalar);
       return true;
     }

     static bool Convert(
       const dmIImage<dmPixelFormat24bppRGB>& from_image ,
             dmIImage<dmPixelFormat24bppRGB>& to_image,
       const dmRegion& _rgn, const dmPoint& _p,
       pixel_compound_tag _tag1,
       pixel_compound_tag _tag2)
     {
       daim::copy(_rgn,_p,from_image.Gen(),to_image.Gen());
       return true;
     }
};
#endif // dmNoRGBImplementation
//-----------------------------------------------------------------
template<EPixelFormat _To>
struct __SCALE_CONVERT_TEMPLATE
{
  dmIImage<_To>&  _to_image;
  const dmRegion& _region;
  const dmPoint&  _point;

  __SCALE_CONVERT_TEMPLATE( dmIImage<_To>& anImage, const dmRegion& aRegion,
                            const dmPoint& aPoint )
  :  _to_image(anImage)
    ,_region(aRegion)
    ,_point(aPoint) {}

  template<EPixelFormat _From>
  void operator()( const dmIImage<_From>& _from_image )
  {
     typename dmPixelFormat<_From>::pixel_category from_category;
     typename dmPixelFormat<_To>::pixel_category   to_category;

     dmConversionTable<_From>::Convert(
       _from_image,_to_image,
       _region    ,_point,
       from_category,
       to_category
     );
  }
};
//-----------------------------------------------------------------
struct __SCALE_CONVERT
{
  const dmImage&  _from_image;
  const dmRegion& _region;
  const dmPoint&  _point;

   __SCALE_CONVERT( const dmImage& anImage, const dmRegion& aRegion,
                    const dmPoint& aPoint )
  :  _from_image(anImage)
    ,_region(aRegion)
    ,_point(aPoint) {}

  template<EPixelFormat _To>
   void operator()( dmIImage<_To>& _to_image )
   {
     __SCALE_CONVERT_TEMPLATE<_To> thisCONVERT(_to_image,_region,_point);
     if(!dmImplementOperation(thisCONVERT,_from_image))
     dmTHROW( EConversionError() );
   }
};
//-----------------------------------------------------------------
// Last but not least, this will generate a the code for the cross table
// conversions.
//-----------------------------------------------------------------
void dmImage::GetCopy( const dmImage& _from_image, const dmRegion& aRegion, const dmPoint& aPoint )
{
  __SCALE_CONVERT thisCONVERT(_from_image,aRegion,aPoint);
  if(!dmImplementOperation(thisCONVERT,*this))
   dmTHROW( EConversionError() );
}
//---------------------------------------------------------------------
