
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
#include "templates/processing/dmDownSampling.h"
#include "templates/processing/dmConvolution.h"

//----------------------------------------------------------------

typedef daim::filter::kernel<dmKernelDescription::value_type> kernel_type;
typedef std::vector<kernel_type>  kernel_list_t;

//----------------------------------------------------------------

// We have two take care of non-integer operations

template<class T>
static void downSamplingMode_(int _mode,
         const dmPoint& _step,
         const kernel_list_t& _family,bool _unsigned,
         const dmRect& rect,
         const daim::image<T>& in,daim::image<T>& out,
         daim::integer_true  )
{
  switch(_mode)
  {
    case dmTk::Math::SubPixels :
      daim::conv::downSampling(_family,_unsigned,rect,in,out,_step,daim::fn::sub_pixel<T>());
      break;
    case dmTk::Math::OrPixels  :
      daim::conv::downSampling(_family,_unsigned,rect,in,out,_step,daim::fn::or_pixel<T>());
      break;
    case dmTk::Math::XorPixels :
      daim::conv::downSampling(_family,_unsigned,rect,in,out,_step,daim::fn::xor_pixel<T>());
      break;
    case dmTk::Math::AndPixels :
      daim::conv::downSampling(_family,_unsigned,rect,in,out,_step,daim::fn::and_pixel<T>());
      break;
    case dmTk::Math::MinPixels :
      daim::conv::downSampling(_family,_unsigned,rect,in,out,_step,daim::fn::min_pixel<T>());
      break;
    case dmTk::Math::MaxPixels :
      daim::conv::downSampling(_family,_unsigned,rect,in,out,_step,daim::fn::max_pixel<T>());
      break;
    case dmTk::Math::NSubPixels :
      daim::conv::downSampling(_family,_unsigned,rect,in,out,_step,daim::fn::nsub_pixel<T>());
      break;
    default : // dmTk::Math::AddPixels
     daim::conv::downSampling(_family,_unsigned,rect,in,out,_step,daim::fn::add_pixel<T>());
     break;
   }
}

template<class T>
static void downSamplingMode_(int _mode,
                    const dmPoint& _step,
                    const kernel_list_t& _family,bool _unsigned,
                    const dmRect& rect,
                    const daim::image<T>& in,daim::image<T>& out,
                    daim::integer_false )
{
   switch(_mode)
   {
     case dmTk::Math::SubPixels :
       daim::conv::downSampling(_family,_unsigned,rect,in,out,_step,daim::fn::sub_pixel<T>());
       break;
     case dmTk::Math::MinPixels :
       daim::conv::downSampling(_family,_unsigned,rect,in,out,_step,daim::fn::min_pixel<T>());
       break;
     case dmTk::Math::MaxPixels :
       daim::conv::downSampling(_family,_unsigned,rect,in,out,_step,daim::fn::max_pixel<T>());
       break;
     case dmTk::Math::NSubPixels :
       daim::conv::downSampling(_family,_unsigned,rect,in,out,_step,daim::fn::nsub_pixel<T>());
       break;
     default : // dmTk::Math::AddPixels
       daim::conv::downSampling(_family,_unsigned,rect,in,out,_step,daim::fn::add_pixel<T>());
       break;
     }
}
//--------------------------------------------------------------------
struct __dm_impl_downSampling
{
   dmBufferParameters& Params;
   dmKernelFamily&     Family;
   const dmPoint&      Step;

   __dm_impl_downSampling( dmBufferParameters& _Params, dmKernelFamily& _Family,
                           const dmPoint& _Step )
   :Params(_Params)
   ,Family(_Family)
   ,Step(_Step) {}

  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _Image )
  {
     typedef typename dmIImage<_PixelFormat>::pixel_type T;
     typedef typename daim::pixel_traits<T>::integer_type integer_type;

     dmImageBuffer& _buffer = Params.thisBuffer;

     _buffer.SetBufferLimit(Params.thisImage.Rect(),dmPERIODIC_BOUNDARY);

     daim::image<T>&  in  = dmIImage<_PixelFormat>::Cast(_buffer.Buffer())->Gen();
     daim::image<T>&  out = _Image.Gen();

     const dmRect& rect = _buffer.BufferRgn().Rectangle();

     // Multipass operation
     if(Family.Size()>1)
     {
       if(Family.GetMode()==dmTk::None)
       {
          // TODO : handling borders in a better way

          // We want to apply a separable kernel, but we need a buffer
          // to hold the temporary results
          dmImageBuffer* _Tmp = Params.thisBuffer.GetNewBuffer();
          _Tmp->CreateBuffer(*_buffer.Buffer(),false);

          daim::image<T>&  tmp = dmIImage<_PixelFormat>::Cast(_Tmp->Buffer())->Gen();

         // Chain convolution
         // Separable kernels can be applied that way
         kernel_type k;
         dmKernelFamily::iterator it = Family.Begin();
         for(size_t i=Family.Size();--i>0;)
         {
           k.fromDescription(*it++);
           daim::convolve(k,false,rect,_buffer.BufferSrc(),in,tmp);
           daim::swap(rect,_buffer.BufferSrc(),in,tmp);
         }
         // The last pass
         k.fromDescription(*it);
         daim::conv::_downSampling(k,false,rect,in,out,Step,
                      integer_type());

       }
       else
       {
         // Create kernel list
         kernel_list_t _kernels(Family.Size());
         for(size_t i=0;i<_kernels.size();++i)
           _kernels[i].fromDescription(Family[i]);

         downSamplingMode_(Family.GetMode(),Step,
                           _kernels,false,rect,in,out,
                           integer_type());
       }
     }
     else {
       kernel_type k;
       k.fromDescription(Family[0]);
       daim::conv::_downSampling(k,false,rect,in,out,Step,
                    integer_type());
     }
  }
};

//--------------------------------------------------------------------
bool dmDownSampling::Apply( dmBufferParameters& _Params )
{
  if(!this->_Family.Empty())
  {
     const dmRect& r = _Params.thisRegion.Rectangle();
     if( (this->_Result.Width()  < r.Width() /_Step.x) ||
         (this->_Result.Height() < r.Height()/_Step.y)) {

       dmTRACE( "ERROR : dmDownSampling :  invalid output image size !\n" );
       return false;
     }

     dmMaskDescription _Mask;
     this->_Family.GetMaskDescription(_Mask);
     _Params.CreateBuffer(*this->_Result.TypeDescriptor(),_Mask,true);

      __dm_impl_downSampling _filter(_Params,this->_Family,this->_Step);
     return  dmImplementScalarOperation(_filter,this->_Result);
  }
  return false;
}
//--------------------------------------------------------------------
