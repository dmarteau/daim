
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


#include "templates/processing/dmImageStats.h"


using namespace daim;

//--------------------------------------------------------------------------
// Image stat(s)
//--------------------------------------------------------------------------

namespace {

struct imagestat_impl
{
  const dmRegion& _Rgn;
  dm_uint         _Order;
  dm_real         _Central;
  dm_real&        _Value;

  imagestat_impl( const dmRegion& Rgn, 
                  dm_uint  Order,
                  dm_real  Central, 
                  dm_real& Value ) 
  : _Rgn(Rgn)
   ,_Order(Order)
   ,_Central(Central)
   ,_Value(Value) 
   {} 

  template<EPixelFormat _PixelFormat>
  void operator()( const dmIImage<_PixelFormat>& _Img ) 
  {
    if(_Order<=1) {
      dm_real N = _Rgn.Area();
      _Value += daim::accumulate(_Rgn,_Img.Gen(),_Value) - N*_Central;
    } else {
      _Value += daim::imagestat(_Rgn,_Img.Gen(),_Order,_Central);
    }
  }
};

}; // namespace
//----------------------------------------------------------
bool dmImageStat( const dmImage& _Src, const dmRegion& _Rgn, 
                  dm_uint  _Order,
                  dm_real  _Central,
                  dm_real& _Value )
{
  dmRegion Rgn(_Rgn);
    
  Rgn.ClipToRect(_Src.Rect());
  if(!Rgn.IsEmptyRoi())
  {
    if(_Order==0) {
       _Value = Rgn.Area(); 
       return true;
    } else {
      imagestat_impl _filter(Rgn,_Order,_Central,_Value);
      return dmImplementScalarOperation(_filter,_Src);
    }
  }
  return false;
}
//----------------------------------------------------------
// Comput an array of moments
//----------------------------------------------------------

namespace {

struct imagestats_impl
{
  const dmRegion& _Rgn;
  dm_uint         _Order;
  dm_real         _Central;
  dm_real*        _Values;

  imagestats_impl( const dmRegion& Rgn, 
                   dm_uint  Order,
                   dm_real  Central, 
                   dm_real* Values ) 
  : _Rgn(Rgn)
   ,_Order(Order)
   ,_Central(Central)
   ,_Values(Values) 
   {} 

  template<EPixelFormat _PixelFormat>
  void operator()( const dmIImage<_PixelFormat>& _Img ) 
  {
    daim::imagestats(_Rgn,_Img.Gen(),_Values,_Order,_Central);
  }
};

}; // namespace
//----------------------------------------------------------
bool dmImageStats( const dmImage& _Src, const dmRegion& _Rgn, 
                   dm_uint  _Order,
                   dm_real  _Central,
                   dm_real* _Values )
{
  if(_Order>=1)
  {
    dmRegion Rgn(_Rgn);
    Rgn.ClipToRect(_Src.Rect());
    if(!Rgn.IsEmptyRoi())
    {
      imagestats_impl _filter(Rgn,_Order,_Central,_Values);
      return dmImplementScalarOperation(_filter,_Src);
    }
  }
  return false;
}
//--------------------------------------------------------------------------
