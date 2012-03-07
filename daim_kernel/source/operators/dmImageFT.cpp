
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

#include "dmMatrixBase.h"
#include "dmMatrixFT.h"

#include "operators/dmImageMatrix.h"
#include "operators/dmImageFT.h"

//------------------------------------------------------------------
// Image to FT
//------------------------------------------------------------------

namespace {

struct image_to_ft_impl
{
  const dmRect& r;
  dmImageFT&    ft;

  image_to_ft_impl( const dmRect& _r, dmImageFT& _ft )
  : r(_r),ft(_ft) {}

  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _img ) 
  {
    ft.image_to_ft(_img.Gen(),r);
  }

  // Specialize for rgb
  void operator()( dmIImage<dmPixelFormat24bppRGB>& _img ) 
  {
    typedef dmIImage<dmPixelFormat24bppRGB>::traits_type  traits_type; 
    daim::transform(r,dmPoint(0,0),_img.Gen(),ft,traits_type::to_scalar());
  }
};
//------------------------------------------------------------------
// FT to Image
//------------------------------------------------------------------
struct ft_to_image_impl
{
  const dmPoint& p;
  dmImageFT&     ft;
  bool           sw;
  dm_real        rmin;
  dm_real        rmax;

  ft_to_image_impl( const dmPoint& _p, dmImageFT& _ft, bool _sw,
                         dm_real _rmin, dm_real _rmax )
  : p(_p),ft(_ft),sw(_sw),rmin(_rmin),rmax(_rmax) {}

  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _img ) 
  {
     typedef typename dmIImage<_PixelFormat>::traits_type  traits_type; 
     typedef typename traits_type::value_type   value_type; 

     daim::gap<value_type> range(
        traits_type::clamp(rmin),
        traits_type::clamp(rmax)
     );

     if(range.diff() <= 0) 
        range = traits_type::bounds();

     ft.ft_to_image(_img.Gen(),p,range,sw);
  }

  // Specialize for float
  void operator()( dmIImage<dmPixelFormat32bppFloat>& _img ) 
  {
     typedef dmIImage<dmPixelFormat32bppFloat>::traits_type  traits_type; 
     typedef traits_type::value_type value_type; 

     daim::gap<value_type> range(
        traits_type::clamp(rmin),
        traits_type::clamp(rmax)
     );

     ft.ft_to_image(_img.Gen(),p,range,sw);
   }
};

}; // namespace
//------------------------------------------------------------------
bool dmCopyImageDataFT( const dmImage& img, dmImageFT& ft, const dmRect& r )
{
  dmRect ftrect = daim::get_base_2_rect(r);
  size_t w = ftrect.Width();
  size_t h = ftrect.Height();
  if(w > 0 && h > 0) 
  {
    ft.initialize(w,h);

    image_to_ft_impl _filter(ftrect,ft);
    return dmImplementOperation(_filter,img);
  }
  return false; 
}
//------------------------------------------------------------------
bool dmComputeForwardFT( const dmImage& img, dmImageFT& ft, bool swblk, 
                         const dmRect r )
{
  dmRect ftrect = daim::get_base_2_rect(r);
  size_t w = ftrect.Width();
  size_t h = ftrect.Height();
  if(w > 0 && h > 0) 
  {
    ft.initialize(w,h);

    image_to_ft_impl _filter(ftrect,ft);
    if(dmImplementScalarOperation(_filter,img)) 
    {
      ft.compute_ft(dmFT_COMPUTE_FORWARD,swblk);
      return true;
    }
  }
  return false; 
} 
//------------------------------------------------------------------
bool dmComputeInverseFT( dmImage& img, dmImageFT& ft,
                         dm_real range_min, dm_real range_max,
                         bool swblk, const dmPoint& p ) 
{
  ft_to_image_impl _filter(p,ft,swblk,range_min,range_max);
  return dmImplementScalarOperation(_filter,img);
}
//------------------------------------------------------------------
// Convolution
//------------------------------------------------------------------
bool dmComputeFTConvolution( const dmImage& img, dmImageFT& ft, int way,
                             const dmPoint p )
{
  dmDEBUG_ASSERT( ft.width() > 0 && ft.height() > 0);

  dmImageFT ft2;
  // Compute ft with blocks swapped
  if(dmComputeForwardFT(img,ft2,true,dmRect(p,ft.width(),ft.height())))
  {
    ft.compute_ft(dmFT_COMPUTE_FORWARD,false);
    ft.ft_convolution(ft2,way);
    return true;
  }
  return false;
}
//------------------------------------------------------------------
// Correlation
//------------------------------------------------------------------
bool dmComputeFTCorrelation( const dmImage& img, dmImageFT& ft, const dmPoint& p )
{
  dmDEBUG_ASSERT( ft.width() > 0 && ft.height() > 0);

  dmImageFT ft2;
  if(dmComputeForwardFT(img,ft2,false,dmRect(p,ft.width(),ft.height())))
  {
    ft.compute_ft(dmFT_COMPUTE_FORWARD,false);
    ft.ft_correlation(ft2);
    return true;
  }
  return false;
}
//------------------------------------------------------------------
// Power spectrum
//------------------------------------------------------------------
bool dmComputeFTPowerSpectrum( dmImage& img, dmImageFT& ft )
{
  if(img.Rect() == ft.rect()) 
  {
    dmRectMatrix _Spectrum;

    ft.compute_ft(dmFT_COMPUTE_FORWARD,false);
    dmFT_PowerSpectrum(ft,_Spectrum );
    dm_matrix_t* pF = _Spectrum.BlkStart();
    dm_matrix_t* pL = _Spectrum.BlkEnd();
    for(;pF!=pL;++pF) *pF = static_cast<dm_matrix_t>( log10(1.0 + *pF) );
    return dmMatrixToImage(img,dmPoint(0,0),0,0,_Spectrum);
  }
  return false;
}
//------------------------------------------------------------------
// Phase spectrum
//------------------------------------------------------------------
bool dmComputeFTPhaseSpectrum( dmImage& img, dmImageFT& ft )
{
  if(img.Rect() == ft.rect()) 
  {
    dmRectMatrix _Spectrum;

    ft.compute_ft(dmFT_COMPUTE_FORWARD,false);
    dmFT_PhaseSpectrum(ft,_Spectrum );
    return dmMatrixToImage(img,dmPoint(0,0),0,0,_Spectrum);
  }
  return false;
}
//------------------------------------------------------------------
