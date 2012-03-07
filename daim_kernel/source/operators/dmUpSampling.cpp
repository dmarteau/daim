
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
#include "templates/processing/dmUpSampling.h"
#include "templates/processing/dmConvolution.h"

#include "dmConvolutionHelper.h"
//----------------------------------------------------------------
typedef std::vector<kernel_type>  kernel_list_t;

//--------------------------------------------------------------------
// Up Sampling
//--------------------------------------------------------------------

namespace {

struct upSampling_impl
{
   dmBufferParameters& Params;
   dmKernelFamily&     Family;
   const dmRect&       SrcRect;
   const dmRect&       DstRect;
   const dmPoint&      Step;

   upSampling_impl( dmBufferParameters& _Params, dmKernelFamily& _Family, 
              const dmRect& _SrcRect,
              const dmRect& _DstRect,
              const dmPoint& _Step )
   :Params(_Params)
   ,Family(_Family)
   ,SrcRect(_SrcRect)
   ,DstRect(_DstRect)
   ,Step(_Step) {}

  template<EPixelFormat _PixelFormat> 
  void operator()( dmIImage<_PixelFormat>& _Image ) 
  {
     typedef typename dmIImage<_PixelFormat>::pixel_type T;
     typedef typename daim::pixel_traits<T>::integer_type integer_type;

     dmImageBuffer& _Buffer = Params.thisBuffer;
     const dmRect& buffRect = _Buffer.BufferRgn().Rectangle();

     daim::image<T>&  in  = _Image.Gen();
     daim::image<T>&  out = dmIImage<_PixelFormat>::Cast(&Params.thisImage)->Gen();
     daim::image<T>&  buf = dmIImage<_PixelFormat>::Cast(_Buffer.Buffer())->Gen();

     // Perform up sampling into buffer
     daim::upSampling(SrcRect,buffRect.TopLeft(),in,buf,Step);

     // Set the boundaries
     // These will not change 
     //_Buffer.SetBufferLimit(Params.thisImage.Rect(),dmZERO_BOUNDARY);
     _Buffer.SetBufferLimit(DstRect,dmZERO_BOUNDARY);

     // Apply convolution
      
     if(Family.Size()>1) 
     {
       dmRegion buffRgn;
       buffRgn.SetRectRoi(buffRect);

       int _mode = Family.GetMode();
       if(_mode == dmTk::None)
       {
         // Chain convolution 
         // Separable kernels can be applied that way
         kernel_type k;
         dmKernelFamily::iterator it = Family.Begin();
         for(size_t i=Family.Size();--i>0;) 
         {
           k.fromDescription(*it++);
           k._norm /= 4; // Preserve mean value;
           daim::convolve(k,false,buffRgn,_Buffer.BufferSrc(),buf,out);
           daim::swap(buffRgn,_Buffer.BufferSrc(),buf,out);
         }
         // The last pass
         k.fromDescription(*it);
         daim::convolve(k,false,buffRgn,_Buffer.BufferSrc(),buf,out);

       }
       else 
       {
         // Create kernel list 
         kernel_list_t _kernels(Family.Size());
         for(size_t i=1;i<_kernels.size()-1;++i) {
           _kernels[i].fromDescription(Family[i]);
           _kernels[i]._norm /= 4; // Preserve mean value
         }

         // Create temporary image from buffer
         daim::cont_image<T>  tmp(buf);

         convolve_mode_(_mode,buf,out,tmp,
                        buffRgn,_Buffer.BufferSrc(),_kernels,false,1,
                        integer_type());
       }
     } else {
       kernel_type k;
       k.fromDescription(Family[0]);
       k._norm /= 4; // Preserve mean value
       daim::convolve(k,false,buffRect,_Buffer.BufferSrc(),buf,out);
     }
  }
};

}; // namespace
//--------------------------------------------------------------------
// perform an up sampling of the image \a _Image into buffer
// then apply convolution from buffer to the image parameter.
//--------------------------------------------------------------------
bool dmUpSampling::Apply( dmBufferParameters& _Params )
{
  if(!this->_Family.Empty())
  {
     // Images must have the same pixel formats
     if(this->_Image.PixelFormat()!=_Params.thisImage.PixelFormat())
       return false;

     if(this->_Step.x < 1 || this->_Step.y < 1 ) {
       dmTRACE( "ERROR : dmUpSampling :  invalid step factors !\n" );
       return false;
     }

     dmRect _SrcRect(this->_Rect);

     if(_SrcRect.Clip(this->_Image.Rect())) 
     {
       // Set the size of the destination image
       dmRect _DstRect(0,0,(_SrcRect.Width() -1) *_Step.x + 1,
                           (_SrcRect.Height()-1) *_Step.y + 1);
       
       // Make sure that the image can hold the result
       if(_Params.thisImage.Width()  < _DstRect.Width() ||
          _Params.thisImage.Height() < _DstRect.Height())
            return false;

       dmMaskDescription _Mask;
       this->_Family.GetMaskDescription(_Mask);

       // Create a buffer of the correct size
       _Params.thisBuffer.CreateBuffer(*this->_Image.TypeDescriptor(),_DstRect,_Mask);   

       upSampling_impl _filter(_Params,this->_Family,_SrcRect,_DstRect,this->_Step);
       return  dmImplementScalarOperation(_filter,this->_Image);

     }
  }
  return false;
} 
//--------------------------------------------------------------------
