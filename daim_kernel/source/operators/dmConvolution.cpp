
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

#include "templates/processing/dmConvolution.h"
#include "dmConvolutionHelper.h"

//----------------------------------------------------------------
typedef std::vector<kernel_type>  kernel_list_t;

struct __dm_impl_convolution
{
   dmBufferParameters& Params;
   dmKernelFamily&     Family;
   int                 Iter;

   __dm_impl_convolution( dmBufferParameters& _Params, dmKernelFamily& _Family, 
                          int _Iter )
   :Params(_Params)
   ,Family(_Family)
   ,Iter(_Iter) {}

  template<EPixelFormat _PixelFormat> 
  void operator()( dmIImage<_PixelFormat>& _Image ) 
  {
     typedef typename dmIImage<_PixelFormat>::pixel_type T;
     typedef typename daim::pixel_traits<T>::integer_type integer_type;

     dmImageBuffer& _buffer = Params.thisBuffer;
     _buffer.SetBufferLimit(_Image.Rect(),dmPERIODIC_BOUNDARY);

     daim::image<T>&  in  = dmIImage<_PixelFormat>::Cast(_buffer.Buffer())->Gen();
     daim::image<T>&  out = _Image.Gen();

     // Create kernel list 
     kernel_list_t _kernels(Family.Size());
     for(size_t i=0;i<_kernels.size();++i)
       _kernels[i].fromDescription(Family[i]);

     // Multipass operation
     if(Iter>1 || Family.Size()>1)
     {
       daim::cont_image<T>  tmp(in);

       const dmRegion& rgn  = _buffer.BufferRgn();
       const dmPoint&  src  = _buffer.BufferSrc();

       int _mode = Family.GetMode();
       if(_mode == dmTk::None) {
         // Chain convolution 
         // Separable kernels can be applied that way
         daim::convolution(rgn,in,tmp,_kernels,false,Iter);
         daim::swap(rgn,src,in,out);
       } else {

         dmLink<dmImage> _copy_of_buffer;
         if(Iter>1)
           _copy_of_buffer = _buffer.Buffer()->CreateCopy(rgn.Rectangle());

           convolve_mode_(_mode,in,out,tmp,rgn,src,_kernels,false,Iter,
                          integer_type());

          // restore back the buffer
          if(!_copy_of_buffer.IsNull())
             _buffer.Buffer()->GetCopy(*_copy_of_buffer,rgn);
        }
     } 
     else
       daim::convolve(_kernels[0],false,_buffer.BufferRgn(),_buffer.BufferSrc(),
                      in,out);
  }
};


//--------------------------------------------------------------------
bool dmConvolution::Apply( dmBufferParameters& _Params )
{
  if(!_Family.Empty())
  {
     dmMaskDescription _mask;
     _Family.GetMaskDescription(_mask);
     _Params.CreateBuffer(_mask);
     
     __dm_impl_convolution _filter(_Params,this->_Family,daim::max(1,this->_Iter));
     return  dmImplementScalarOperation(_filter,_Params.thisImage);
  }
  return false;
} 
//--------------------------------------------------------------------
