
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

#include "templates/processing/dmMorphology.h"
#include "templates/processing/dmArithmetics.h"

//--------------------------------------------------------------------

namespace {

struct morphology_impl
{
  dmBufferParameters& Params;
  dmKernelFamily&     Family;
  int Operation;
  int Iter;

  morphology_impl( dmBufferParameters& _Params, dmKernelFamily& _Family,
                   int _Operation,
                   int _Iter )
  :Params(_Params)
  ,Family(_Family)
  ,Operation(_Operation)
  ,Iter(_Iter) {}

  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _Image )
  {
     typedef typename dmIImage<_PixelFormat>::pixel_type T;

     dmImageBuffer& _buffer = Params.thisBuffer;

     daim::image<T>&  in  = dmIImage<_PixelFormat>::Cast(_buffer.Buffer())->Gen();
     daim::image<T>&  out = _Image.Gen();

     const dmRegion& rgn  = _buffer.BufferRgn();
     const dmPoint&  src  = _buffer.BufferSrc();

     switch(Operation)
     {
       case dmTk::Math::Erosion      : {
         _buffer.SetBufferLimit(_Image.Rect(),dmOBJECT_BOUNDARY);
         daim::cont_image<T>  tmp(in);
         daim::erosion(in,tmp,rgn,Family,Iter);
         daim::swap(rgn,src,in,out);
       } break;

       case dmTk::Math::Dilation    : {
         _buffer.SetBufferLimit(_Image.Rect(),dmZERO_BOUNDARY);
         daim::cont_image<T>  tmp(in);
         daim::dilation(in,tmp,rgn,Family,Iter);
         daim::swap(rgn,src,in,out);
       } break;

       case dmTk::Math::Thinning   : {
         _buffer.SetBufferLimit(_Image.Rect(),dmOBJECT_BOUNDARY);
         daim::cont_image<T>  tmp(in);
         daim::thinning(in,tmp,rgn,Family,Iter);
         daim::swap(rgn,src,in,out);
       } break;

       case dmTk::Math::Thickening : {
         _buffer.SetBufferLimit(_Image.Rect(),dmZERO_BOUNDARY);
         daim::cont_image<T>  tmp(in);
         daim::thickening(in,tmp,rgn,Family,Iter);
         daim::swap(rgn,src,in,out);
       } break;

       case dmTk::Math::Opening   : {
         _buffer.SetBufferLimit(_Image.Rect(),dmZERO_BOUNDARY);
         daim::cont_image<T>  tmp(in);
         daim::opening(in,tmp,rgn,Family,Iter);
         daim::swap(rgn,src,in,out);
       } break;

       case dmTk::Math::Closing    : {
         _buffer.SetBufferLimit(_Image.Rect(),dmOBJECT_BOUNDARY);
         daim::cont_image<T>  tmp(in);
         daim::closing(in,tmp,rgn,Family,Iter);
         daim::swap(rgn,src,in,out);
       } break;

       case dmTk::Math::HitOrMiss  : {
         _buffer.SetBufferLimit(_Image.Rect(),dmOBJECT_BOUNDARY);
         daim::cont_image<T> tmp(in);
         daim::hitormiss(in,out,tmp,rgn,src,Family);
       } break;

       case dmTk::Math::TopHat    : {
         _buffer.SetBufferLimit(_Image.Rect(),dmZERO_BOUNDARY);
         daim::cont_image<T> tmp(in);
         daim::opening(in,tmp,rgn,Family,Iter);
         daim::swap(rgn,src,in,out);
         daim::sub_images(rgn,src,in,out);
       } break;

       case dmTk::Math::BottomHat : {
         _buffer.SetBufferLimit(_Image.Rect(),dmOBJECT_BOUNDARY);
         daim::cont_image<T> tmp(in);
         daim::closing(in,tmp,rgn,Family,Iter);
         daim::swap(rgn,src,in,out);
         daim::nsub_images(rgn,src,in,out);
       } break;
     }
  }
};

}; // namespace
//--------------------------------------------------------------------
bool dmMorphology::Apply( dmBufferParameters& _Params )
{
  if(!this->_Family.Empty())
  {
     this->_Iter = daim::max(1,this->_Iter);

     dmMaskDescription _Mask;
     this->_Family.GetMaskDescription(_Mask);
     _Params.CreateBuffer(_Mask);

     morphology_impl _filter(_Params,this->_Family,this->_Operation,this->_Iter);
     return  dmImplementScalarOperation(_filter,_Params.thisImage);
  }
  return false;
}
//--------------------------------------------------------------------
