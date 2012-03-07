
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
#include "templates/processing/dmDensityMap.h"

using namespace daim;

//------------------------------------------------------------------------
// Scale to 8 bits
//----------------------------------------------------------------------

namespace {

struct scaleto8Bits_impl
{
  dm_real  rmin,rmax;
  dmImage& dest;
  scaleto8Bits_impl(dmImage& _dest,dm_real _rmin, dm_real _rmax)
  : rmin(_rmin),rmax(_rmax),dest(_dest) {}
  //-------------------------------------------------------
  // Generic operation on scalar
  //-------------------------------------------------------
  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& anImage ) 
  {
    typedef typename dmIImage<_PixelFormat>::image_type  image_type;
    typedef typename dmIImage<_PixelFormat>::pixel_type  pixel_type;
    typedef typename dmIImage<_PixelFormat>::traits_type traits_type;
    typedef typename dmIImage<_PixelFormat>::value_type  value_type;

    typedef typename traits_type::integer_type  integer_type;

    image_type& _img = anImage.Gen();
        
    typedef pixel_traits<dm_uint8> traits;

    gap<value_type> _minmax(
        traits_type::clamp(rmin),
        traits_type::clamp(rmax)
    );

    dmIImage<dmPixelFormat8bppIndexed>* _I = dmIImage<dmPixelFormat8bppIndexed>::Cast(&dest);
    dmASSERT( _I!=NULL );

    // construit une region correspondant au pixels compris entre rmin et rmax
    dmRegion rgn;

    if(_minmax.diff()<=0) {
      rgn = _I->Rect();
      _minmax = daim::minmax(rgn,_img);
    } else daim::create_roi(_img,between<value_type>(_minmax.min(),_minmax.max()),rgn);

    if(integer_type()) {
      daim::transform(rgn,dmPoint(0,0),_img,_I->Gen(),scale_fn::scalerange<pixel_type,dm_uint8>(_minmax,traits::bounds()));
    } else {
      daim::transform(rgn,dmPoint(0,0),_img,_I->Gen(),scale_fn::scalerange_noround<pixel_type,dm_uint8>(_minmax,traits::bounds()));
    }

    // set outer region to zero
    if(!rgn.IsEmptyRoi()) {
      rgn.XorCoordinates(_I->Rect());
      _I->Fill(rgn,traits::zero());
    }
  }
};

}; // namespace
//------------------------------------------------------------------------
// if rmin==rmax => the function perform a automatic rescaling
// between the min and the max of the source image
//--------------------------------------------------------------------------
bool dmScaleTo8Bits( const dmImage& _src, dmImage& _dest,dm_real rmin, dm_real rmax)
{
  if(_dest.PixelFormat()==dmPixelFormat8bppIndexed)
  {
    scaleto8Bits_impl _filter(_dest,rmin,rmax);
    if(dmImplementScalarOperation(_filter,_src))
      return true;
  } 
  return false;
}
//------------------------------------------------------------------------
