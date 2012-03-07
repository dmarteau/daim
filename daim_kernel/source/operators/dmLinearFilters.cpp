
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
#include "templates/processing/dmLinearFilters.h"

#include "templates/processing/_dmFastMedian.h"

// $ChangeLog 18/08/2006 David
// Used optimized filter for median filter

using namespace daim;
//--------------------------------------------------------------------
// M-Filters
//--------------------------------------------------------------------

namespace {

struct Mfilters_impl
{
  dmBufferParameters&        Params;
  const dmMaskDescription&   Mask;
  dmFilterType               Ftype;

  Mfilters_impl( dmBufferParameters& _Params, const dmMaskDescription& _Mask,
                 dmFilterType _Ftype )
  :Params(_Params)
  ,Mask(_Mask)
  ,Ftype(_Ftype) {}

  // Generic operation on scalar
  template<EPixelFormat _PixelFormat>
  void Apply( dmIImage<_PixelFormat>& _img, const integer_true& )
  {
    typedef typename dmIImage<_PixelFormat>::value_type  T;
    typedef int  U;

    dmImageBuffer& _buffer = Params.thisBuffer;

    filter::kernel<dmKernelDescription::value_type> _kernel;
    _kernel.fromMaskDescription(Mask);

    image<T>& out = _img.Gen();
    image<T>& in  = dmIImage<_PixelFormat>::Cast(_buffer.Buffer())->Gen();

    switch(Ftype) {

      case dmFilterMean  :
        RoiOperation(
           filter::getMeanFunctor(_kernel,filter::accumulator_base<T,T,float>()),
           in,out,_buffer.BufferRgn(),_buffer.BufferSrc());
        break;

      case dmFilterMedian:
        applyGenericFilter(in,out,_kernel,filter::median<T,T,U>(_kernel.size()),
                           _buffer.BufferRgn(),_buffer.BufferSrc());
        break;

      case dmFilterMax   :
         applyGenericFilter (in,out,_kernel,filter::max<T,T,U>()   ,
          _buffer.BufferRgn(),_buffer.BufferSrc());
         break;

      case dmFilterMin   :
         applyGenericFilter (in,out,_kernel,filter::min<T,T,U>()   ,
          _buffer.BufferRgn(),_buffer.BufferSrc());
         break;

      case dmFilterMiddle:
         applyGenericFilter (in,out,_kernel,filter::middle<T,T,U>()   ,
          _buffer.BufferRgn(),_buffer.BufferSrc());
         break;

      default: break;
    }
  }

  // Generic operation on scalar
  template<EPixelFormat _PixelFormat>
  void Apply( dmIImage<_PixelFormat>& _img, const integer_false& )
  {
    typedef typename dmIImage<_PixelFormat>::value_type T;
    typedef float  U;

    dmImageBuffer& _buffer = Params.thisBuffer;

    filter::kernel<dmKernelDescription::value_type> _kernel;
    _kernel.fromMaskDescription(Mask);

    image<T>& out = _img.Gen();
    image<T>& in  = dmIImage<_PixelFormat>::Cast(_buffer.Buffer())->Gen();

    switch(Ftype) {

      case dmFilterMean  :
        RoiOperation(
           filter::getMeanFunctor(_kernel,filter::accumulator_base<T,T,float>()),
            in,out,_buffer.BufferRgn(),_buffer.BufferSrc());
        break;

      case dmFilterMedian:
        applyGenericFilter(in,out,_kernel,filter::median<T,T,U>(_kernel.size()),
                           _buffer.BufferRgn(),_buffer.BufferSrc());
        break;

      case dmFilterMax   :
         applyGenericFilter(in,out,_kernel,filter::max<T,T,U>()   ,
          _buffer.BufferRgn(),_buffer.BufferSrc());
         break;

      case dmFilterMin   :
         applyGenericFilter(in,out,_kernel,filter::min<T,T,U>()   ,
          _buffer.BufferRgn(),_buffer.BufferSrc());
         break;

      case dmFilterMiddle:
         applyGenericFilter(in,out,_kernel,filter::middle<T,T,U>()   ,
          _buffer.BufferRgn(),_buffer.BufferSrc());
         break;

      default: break;
    }
  }

  // Generic operation on scalar
  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _img )
  {
    typedef typename dmIImage<_PixelFormat>::traits_type traits_type;
    typedef typename traits_type::integer_type integer_type;
    Apply(_img,integer_type());
  }


  // Specialize for 16 bits
  // Use histogram filter for fast implementation
  void operator()( dmIImage<dmPixelFormat16bppGrayScale>& _img )
  {
     if(Ftype == dmFilterMedian)
     {
       typedef dmIImage<dmPixelFormat16bppGrayScale>::value_type T;

       dmImageBuffer& _buffer = Params.thisBuffer;

       filter::kernel<dmKernelDescription::value_type> _kernel;
       _kernel.fromMaskDescription(Mask);

       image<T>& out = _img.Gen();
       image<T>& in  = dmIImage<dmPixelFormat16bppGrayScale>::Cast(_buffer.Buffer())->Gen();

       // Allocate array for histogram

       dmVector<int> storage(0xffff);

       RoiOperation(
          filter::getMedianFunctor16bits(_kernel,pixel_traits<T>(),storage.Begin()),
          in,out,_buffer.BufferRgn(),_buffer.BufferSrc());

     }
     else
     {
       typedef dmIImage<dmPixelFormat16bppGrayScale>::traits_type traits_type;
       typedef traits_type::integer_type integer_type;
       Apply(_img,integer_type());
     }
  }


  // Specialize for 8 bits
  // Use histogram filter for fast implementation
  void operator()( dmIImage<dmPixelFormat8bppIndexed>& _img )
  {
    typedef dmIImage<dmPixelFormat8bppIndexed>::value_type T;

    dmImageBuffer& _buffer = Params.thisBuffer;

    filter::kernel<dmKernelDescription::value_type> _kernel;
    _kernel.fromMaskDescription(Mask);

    image<T>& out = _img.Gen();
    image<T>& in  = dmIImage<dmPixelFormat8bppIndexed>::Cast(_buffer.Buffer())->Gen();

    switch(Ftype)
    {
      case dmFilterMean:
        RoiOperation(
           filter::getMeanFunctor(_kernel,filter::accumulator_base<T,T,int>()),
           in,out,_buffer.BufferRgn(),_buffer.BufferSrc());
        break;

      case dmFilterMedian: {
        // Allocate array for histogram on the stack
        int storage[0xff];

        RoiOperation(
           filter::getMedianFunctor8bits(_kernel,pixel_traits<T>(),storage),
           in,out,_buffer.BufferRgn(),_buffer.BufferSrc());
      } break;

      case dmFilterMax   :
         applyHistogramFilter(in,out,_kernel,filter::histogram_max<dm_uint8>(),
          _buffer.BufferRgn(),_buffer.BufferSrc());
         break;

      case dmFilterMin   :
         applyHistogramFilter(in,out,_kernel,filter::histogram_min<dm_uint8>(),
          _buffer.BufferRgn(),_buffer.BufferSrc());
         break;

      case dmFilterMiddle:
         applyHistogramFilter(in,out,_kernel,filter::histogram_middle<dm_uint8>(),
          _buffer.BufferRgn(),_buffer.BufferSrc());
         break;

        default: break;
      }
  }

};

}; // namespace
//--------------------------------------------------------------------
bool dmMFilter::Apply( dmBufferParameters& _Params )
{
   _Params.CreateBuffer(this->_Mask);
   _Params.thisBuffer.SetBufferLimit(_Params.thisImage.Rect(),dmPERIODIC_BOUNDARY );

   Mfilters_impl _filter(_Params,this->_Mask,this->_Type);
   return dmImplementScalarOperation(_filter,_Params.thisImage);
}

//--------------------------------------------------------------------
// R-Filters
//--------------------------------------------------------------------

namespace {

struct Rfilters_impl
{
  dmBufferParameters&        Params;
  const dmMaskDescription&   Mask;
  dm_uint                    Rank;

  Rfilters_impl( dmBufferParameters& _Params, const dmMaskDescription& _Mask,
                 dm_uint _Rank )
  :Params(_Params)
  ,Mask(_Mask)
  ,Rank(_Rank) {}

  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _img )
  {
    typedef typename dmIImage<_PixelFormat>::value_type  T;
    typedef T U;

    filter::kernel<dmKernelDescription::value_type> _kernel;
    _kernel.fromMaskDescription(Mask);

    image<T>& out = _img.Gen();
    image<T>& in  = dmIImage<_PixelFormat>::Cast(Params.thisBuffer.Buffer())->Gen();

    applyGenericFilter(in,out,_kernel,filter::R_filter<T,T,U>(Rank,_kernel.size()),
                       Params.thisBuffer.BufferRgn(),
                       Params.thisBuffer.BufferSrc());

  }
};

}; // namespace
//--------------------------------------------------------------------
bool dmRFilter::Apply( dmBufferParameters& _Params )
{
   if(this->_Rank>this->_Mask.Size()) {
     dmTRACE( "ERROR : dmRFilters : rank is larger than mask size !\n" );
     return false;
   }

   _Params.CreateBuffer(this->_Mask);
   _Params.thisBuffer.SetBufferLimit( _Params.thisImage.Rect(),dmPERIODIC_BOUNDARY);

   Rfilters_impl  _filter(_Params,this->_Mask,this->_Rank);
   return dmImplementScalarOperation(_filter,_Params.thisImage);

   return true;
}
//--------------------------------------------------------------------
// L-Filters
//--------------------------------------------------------------------

namespace {

struct Lfilters_impl
{
  dmBufferParameters&        Params;
  const dmMaskDescription&   Mask;
  dm_real*                   Coeffs;

  Lfilters_impl(dmBufferParameters& _Params, const dmMaskDescription& _Mask,
                dm_real* _Coeffs )
  :Params(_Params)
  ,Mask(_Mask)
  ,Coeffs(_Coeffs) {}

  // Generic operation on scalar
  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _img )
  {
    typedef typename dmIImage<_PixelFormat>::value_type  T;
    typedef float U;

    filter::kernel<dmKernelDescription::value_type> _kernel;
    _kernel.fromMaskDescription(Mask);

    std::vector<float> _vcoeffs(Mask.Size());
    std::copy(Coeffs,Coeffs+Mask.Size(),_vcoeffs.begin());

    image<T>& out = _img.Gen();
    image<T>& in  = dmIImage<_PixelFormat>::Cast(Params.thisBuffer.Buffer())->Gen();

    applyGenericFilter(in,out,_kernel,filter::L_filter<T,T,U>(_vcoeffs,_kernel.size()),
                       Params.thisBuffer.BufferRgn(),
                       Params.thisBuffer.BufferSrc());

  }
};

}; //namespace
//--------------------------------------------------------------------
bool dmLFilter::Apply( dmBufferParameters& _Params )
{
   _Params.CreateBuffer(this->_Mask);
   _Params.thisBuffer.SetBufferLimit(_Params.thisImage.Rect(),dmPERIODIC_BOUNDARY);

   Lfilters_impl _filter(_Params,this->_Mask,this->_Coeffs);
   return dmImplementScalarOperation(_filter,_Params.thisImage);
}
//--------------------------------------------------------------------
