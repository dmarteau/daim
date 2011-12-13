
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
#include "templates/processing/dmBinaryMorphology.h"
//--------------------------------------------------------------------
// Local max code
//--------------------------------------------------------------------
struct __dm_impl_localmax
{
  dmBufferParameters& params;
  dmRegion&           rgn;
  __dm_impl_localmax(dmBufferParameters& _params,dmRegion& _rgn) 
  : params(_params),rgn(_rgn) {}

  template<class ImageType> 
  void operator()( ImageType& _Image ) 
  {
    typedef typename ImageType::pixel_type  T;
    typedef typename ImageType::traits_type traits_type;
    typedef typename ImageType::value_type  value_type;

    dmImageBuffer& _buffer = params.thisBuffer;
    _buffer.Clear();

    rgn  = _buffer.BufferRgn();

    dmIImage<dmPixelFormat8bppIndexed>* map = dmIImage<dmPixelFormat8bppIndexed>::Cast(_buffer.Buffer());
    dmASSERT( map != NULL );
    daim::fill(map->Gen(),0);

    // Place region so that it fit into src;
    dmPoint pDest = rgn.Rectangle().TopLeft();     
    rgn.Translate( _buffer.BufferSrc() - pDest );
    dmRect r = _Image.Rect();
    r.Resize(-1);
    rgn.ClipToRect(r);

    daim::local_maxima8(rgn,pDest,_Image.Gen(),map->Gen());
    
    const dmRect& rbuf = _buffer.BufferRgn().Rectangle();
    daim::create_rgnroi(map->Gen(),std::bind2nd(
       std::equal_to<value_type>(),traits_type::max()),
       rgn,rbuf);
    
    pDest = _buffer.BufferSrc();
    if(pDest.x != 0) pDest.x -= rbuf.TopLeft().x;
    if(pDest.y != 0) pDest.y -= rbuf.TopLeft().y;
    rgn.Translate( pDest );

  }
};
//--------------------------------------------------------------------
bool dmLocalMax( dmBufferParameters& _Params , dmRegion& _Result )
{
  _Params.CreateBuffer(*dmGetDescriptor(dmPixelFormat8bppIndexed),
                         dmMaskDescription(3),false);   

  __dm_impl_localmax _filter(_Params,_Result);
  return dmImplementScalarOperation(_filter,_Params.thisImage);
}
//--------------------------------------------------------------------
