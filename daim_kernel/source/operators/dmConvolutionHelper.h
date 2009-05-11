
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

//--------------------------------------------------------
// File         : dmConvolutionHelper.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#ifndef dmConvolutionHelper_h
#define dmConvolutionHelper_h

//-------------------------------------------------
// For implementors of operators using convolution
//-------------------------------------------------

// We have to take care of non-integer operations

typedef daim::filter::kernel<dmKernelDescription::value_type>  kernel_type;

template<class T>
void convolve_mode_(int _mode,
         daim::image<T>& in,daim::image<T>& out,daim::image<T>& tmp,
         const dmRegion& rgn,const dmPoint& src,
         const std::vector<kernel_type>& _family,
         bool _unsigned,int iter,
         daim::integer_true  )
{
  switch(_mode)
  {
    // XXXDavid : The following assume that integer values are unsigned !!
    //            (optimisation purpose)
    case dmTk::Math::AbsPixels :
    case dmTk::Math::AddPixels :
      daim::convolution(daim::add_pixel<T>(),in,out,tmp,rgn,src,_family,_unsigned,iter);
      break;
    case dmTk::Math::SubPixels :
      daim::convolution(daim::sub_pixel<T>(),in,out,tmp,rgn,src,_family,_unsigned,iter);
      break;
    case dmTk::Math::OrPixels  :
      daim::convolution(daim::or_pixel<T>(),in,out,tmp,rgn,src,_family,_unsigned,iter);
      break;
    case dmTk::Math::XorPixels :
      daim::convolution(daim::xor_pixel<T>(),in,out,tmp,rgn,src,_family,_unsigned,iter);
      break;
    case dmTk::Math::AndPixels :
      daim::convolution(daim::and_pixel<T>(),in,out,tmp,rgn,src,_family,_unsigned,iter);
      break;
    case dmTk::Math::MinPixels :
      daim::convolution(daim::min_pixel<T>(),in,out,tmp,rgn,src,_family,_unsigned,iter);
      break;
    case dmTk::Math::MaxPixels :
      daim::convolution(daim::max_pixel<T>(),in,out,tmp,rgn,src,_family,_unsigned,iter);
      break;
    case dmTk::Math::NSubPixels :
      daim::convolution(daim::nsub_pixel<T>(),in,out,tmp,rgn,src,_family,_unsigned,iter);
      break;
   }
}

template<class T>
void convolve_mode_(int _mode,
         daim::image<T>& in,daim::image<T>& out,daim::image<T>& tmp,
         const dmRegion& rgn,const dmPoint& src,
         const std::vector<kernel_type>& _family,
         bool _unsigned,int iter,
         daim::integer_false )
{
   switch(_mode)
   {
     case dmTk::Math::AddPixels :
       daim::convolution(daim::add_pixel<T>(),in,out,tmp,rgn,src,_family,_unsigned,iter);
       break;
     case dmTk::Math::AbsPixels :
      daim::convolution(daim::add_pixel<T>(),in,out,tmp,rgn,src,_family,true,iter);
      break;
     case dmTk::Math::SubPixels :
       daim::convolution(daim::add_pixel<T>(),in,out,tmp,rgn,src,_family,_unsigned,iter);
       break;
     case dmTk::Math::MinPixels :
       daim::convolution(daim::min_pixel<T>(),in,out,tmp,rgn,src,_family,_unsigned,iter);
       break;
     case dmTk::Math::MaxPixels :
       daim::convolution(daim::max_pixel<T>(),in,out,tmp,rgn,src,_family,_unsigned,iter);
       break;
     case dmTk::Math::NSubPixels :
       daim::convolution(daim::nsub_pixel<T>(),in,out,tmp,rgn,src,_family,_unsigned,iter);
       break;
     default:
       daim::convolution(rgn,in,tmp,_family,_unsigned,iter);
    }
}
//--------------------------------------------------------------------

#endif // dmConvolutionHelper_h
