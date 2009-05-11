
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

#include "templates/processing/dmThreshold.h"

using namespace daim;

/* $ChangeLog  v2.5.1, 12 oct. 2006 10:51:23 , David
 *
 * Changed hysteresis to take account for new hysteresis algorithm
 */ 

struct __dm_impl_hysteresis_thr
{
  dmBufferParameters& Params;
  dmRegion&           Result;
  dm_real             LowThr;
  dm_real             HighThr; 
  dm_uint             Connect;

  __dm_impl_hysteresis_thr( 
    dmBufferParameters& _Params, 
    dmRegion&           _Result,
    dm_real             _LowThr,
    dm_real             _HighThr,
    dm_uint             _Connect    
  ) :Params(_Params)
    ,Result(_Result)
    ,LowThr(_LowThr)
    ,HighThr(_HighThr)
    ,Connect(_Connect)
    {}

  template<class T>
  void _Apply( image<T>& im, image<T>& dest )
  {
    // Dest is the image created from buffer
    
    typedef typename image<T>::value_type  value_type;
    typedef typename image<T>::traits_type traits_type;

    typedef typename traits_type::integer_type integer_type;

    dmImageBuffer&  buffer  = Params.thisBuffer;
    dmRegion&       dst_rgn = Result;       

    buffer.Clear();
    dmRegion rgn   = buffer.BufferRgn();
    dmPoint  pDest = rgn.Rectangle().TopLeft(); 
    dmPoint  pSrc  = buffer.BufferSrc() - pDest; 

    rgn.Translate( pSrc.x , pSrc.y );

    value_type m;

    if(LowThr <= HighThr) 
    {   
      m = pixel_traits<value_type>::max();

      gap<value_type> thr(
          _get_range_value(LowThr ,traits_type(),integer_type()),
          _get_range_value(HighThr,traits_type(),integer_type()));
  
      hysteresis_threshold(rgn,pDest,im,dest,thr.max(),thr.min(),Connect);
    } 
    else 
    { 
      m = pixel_traits<value_type>::min();

      gap<value_type> thr(
          _get_range_value(HighThr,traits_type(),integer_type()),
          _get_range_value(LowThr ,traits_type(),integer_type()));
  
      hysteresis_threshold(rgn,pDest,im,dest,thr.min(),thr.max(),Connect);    
    }    

    create_rgnroi(dest,
       std::bind2nd(std::equal_to<value_type>(),m),
       dst_rgn,buffer.BufferRgn());

    dst_rgn.Translate( pSrc );  
  }

  template<EPixelFormat _PixelFormat> 
  void operator()( dmIImage<_PixelFormat>& _Image ) 
  {
     dmIImage<_PixelFormat>* dest;
     dest = dmIImage<_PixelFormat>::Cast(Params.thisBuffer.Buffer());
     dmASSERT( dest != NULL );
     _Apply(_Image.Gen(),dest->Gen() );
  } 

  template<class Op> 
  void __do_transform(
    image<dm_rgb24>& _from,
    image<dm_uint8>& _to,
    const Op& op) {
    transform(Params.thisRegion,dmPoint(0,0),_from,_to,op);
  }

  void operator()( dmIImage<dmPixelFormat24bppRGB>& _Image ) 
  {
    typedef dmIImage<dmPixelFormat24bppRGB>::traits_type traits_type;

    cont_image<dm_uint8> _tmp(_Image.Width(),_Image.Height());
    _tmp.fill(0);

    __do_transform(_Image.Gen(),_tmp,traits_type::to_scalar()  );

    dmIImage<dmPixelFormat8bppIndexed>* dest;
    dest = dmIImage<dmPixelFormat8bppIndexed>::Cast(Params.thisBuffer.Buffer());
    dmASSERT( dest != NULL );
     _Apply( _tmp, dest->Gen() ); 
  }
};
//--------------------------------------------------------------------
bool dmHysteresisThreshold::Apply( dmBufferParameters& _Params )
{
  dmImageDescriptor* pDescriptor = _Params.thisImage.TypeDescriptor();
    
  if(!dmIsPixelFormatScalar(pDescriptor->PixelFormat())) 
    pDescriptor = dmGetDescriptor(dmPixelFormat8bppIndexed);
     
  _Params.CreateBuffer(*pDescriptor,dmMaskDescription(3),false);

  __dm_impl_hysteresis_thr _filter(_Params,
                           this->_Result,
                           this->_LowThreshold,
                           this->_HighThreshold,
                           this->_Connect);

  return dmImplementOperation(_filter,_Params.thisImage);
}
//---------------------------------------------------------------------
struct __dm_impl_region_thr
{
  dmBufferParameters& Params;
  dmRegion&           Result;
  dm_real             LowThr;
  dm_real             HighThr; 
  
  __dm_impl_region_thr(
    dmBufferParameters& _Params, 
    dmRegion&           _Result,
    dm_real             _LowThr,
    dm_real             _HighThr 
  ) :Params(_Params)
    ,Result(_Result)
    ,LowThr(_LowThr)
    ,HighThr(_HighThr) {}

  template<EPixelFormat _PixelFormat> 
  void operator()( dmIImage<_PixelFormat>& _Image ) 
  {
    typedef typename dmIImage<_PixelFormat>::value_type  value_type;
    typedef typename dmIImage<_PixelFormat>::traits_type traits_type;

    typedef typename traits_type::integer_type integer_type;

    gap<value_type> thr(
        _get_range_value(LowThr ,traits_type(),integer_type()),
        _get_range_value(HighThr,traits_type(),integer_type())
    );
    create_rgnroi(_Image.Gen(),between<value_type>(thr.min(),thr.max()),
                  Result,Params.thisRegion);
  }

  void operator()( dmIImage<dmPixelFormat24bppRGB>& _Image ) 
  {
    typedef pixel_traits<dm_rgb24>   rgb_traits;
    typedef pixel_traits<dm_uint8>   traits_type;
    typedef traits_type::value_type  value_type;

    gap<value_type> thr(
        _get_range_value(LowThr ,traits_type(),traits_type::integer_type()),
        _get_range_value(HighThr,traits_type(),traits_type::integer_type())
    );

    between<value_type> pred(thr.min(),thr.max());

    const dmRegion& src_rgn = Params.thisRegion;
    create_rgnroi(_Image.Gen(),bind_func(rgb_traits::to_scalar(),pred),Result,src_rgn);
  }
};
//--------------------------------------------------------------------
bool dmThreshold::Apply( dmBufferParameters& _Params )
{
  __dm_impl_region_thr _filter(_Params,
                       this->_Result,
                       this->_LowThreshold,
                       this->_HighThreshold);

  return dmImplementOperation(_filter,_Params.thisImage);   
}
//--------------------------------------------------------------------
