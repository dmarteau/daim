
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

#include "templates/processing/dmGradients.h"
#include "common/dmUserLib.h"

//--------------------------------------------------------------------
struct __dm_impl_nonmaxsuppress
{
   dmBufferParameters& params;
   bool                inner;

   __dm_impl_nonmaxsuppress(dmBufferParameters& _params, bool _inner)
   : params(_params),inner(_inner) {}

  template<EPixelFormat _PixelFormat> 
  void operator()( dmIImage<_PixelFormat>& _Image ) 
  {
     dmImageBuffer& _buffer = params.thisBuffer;

     typedef typename dmIImage<_PixelFormat>::traits_type traits_type;
     typedef typename traits_type::integer_type           integer_type;

     _buffer.SetBufferLimit(_Image.Rect(),dmPERIODIC_BOUNDARY);

     dmIImage<_PixelFormat>* _imgbuf = dmIImage<_PixelFormat>::Cast(_buffer.Buffer());

     dmASSERT( _imgbuf != NULL );

     daim::non_maximal_supression(_buffer.BufferRgn(),_buffer.BufferSrc(),
                                  _imgbuf->Gen(),_Image.Gen(),inner);
  }
};
//--------------------------------------------------------------------
bool dmNonMaxSuppress::Apply( dmBufferParameters& _params )
{
   _params.CreateBuffer(dmMaskDescription(1,1,3,3));
   __dm_impl_nonmaxsuppress _filter(_params,this->_Inner);
   return dmImplementScalarOperation(_filter,_params.thisImage);
} 
//--------------------------------------------------------------------
