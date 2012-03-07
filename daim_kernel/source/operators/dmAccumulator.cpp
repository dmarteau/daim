
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
#include "operators/dmAccumulator.h"
#include "templates/processing/dmArithmetics.h"

namespace {

//--------------------------------------------------
struct accumulate_impl
{
  dmAccumulator&  This;
  bool            bFinalize;
  const dmRegion& SrcRgn;
  dmPoint         To;

  accumulate_impl(dmAccumulator& _This,const dmRegion& _SrcRgn,
                                            const dmPoint& _To )
  :This(_This)
  ,bFinalize(false) 
  ,SrcRgn(_SrcRgn)
  ,To(_To)
  {}   

  accumulate_impl(dmAccumulator& _This)
  :This(_This)
  ,bFinalize(true) 
  ,SrcRgn(_This.thisRegion)
  {}   

  // Apply 
  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _Src ) 
  {
    typedef typename dmIImage<dmPixelFormat32bppFloat>::value_type  value_type;

    dmIImage<dmPixelFormat32bppFloat>*
      _ThisImage = dmIImage<dmPixelFormat32bppFloat>::Cast(This.thisBuffer);

    if(bFinalize)
    {
      // The region is input region: modify it ccordingly
      To = This.thisRegion.Rectangle().TopLeft();
      This.thisRegion.Translate(-To.x,-To.y);

      typedef typename dmIImage<_PixelFormat>::value_type result_type;
      daim::transform(SrcRgn,To,_ThisImage->Gen(),_Src.Gen(),
                       daim::fn::truncate<value_type,result_type>());

      // restore back the region
      This.thisRegion.Translate(To.x,To.y);
    } 
    else
    {
      To = To - This.thisRegion.Rectangle().TopLeft();

      This.thisCount++; 
      daim::combine(SrcRgn,To,_Src.Gen(),_ThisImage->Gen(),
                                         std::plus<value_type>());
    }
  }
};

}; //namespace
//--------------------------------------------------
dmAccumulator::dmAccumulator()
:thisDest(NULL)
,thisFormat(dmPixelFormatUndefined)
 {}
//--------------------------------------------------
dmAccumulator::~dmAccumulator() 
{}
//--------------------------------------------------
void dmAccumulator::Initialize( EPixelFormat  _PixelFormat , const dmRect&   _Rect )
{
  thisDest     = NULL;
  thisCount    = 0;
  thisFormat   = _PixelFormat;
  thisBuffer   = dmCreateImage<dmPixelFormat32bppFloat>(_Rect.Width(),_Rect.Height());
  thisRegion   = _Rect;

  thisBuffer->Clear();
} 
//--------------------------------------------------
void dmAccumulator::Initialize( dmImage* _Dest , const dmRegion& _Rgn  )
{
  dmASSERT(_Dest != NULL);
  Initialize(_Dest->PixelFormat(),_Rgn.Rectangle());
  thisDest   = _Dest;
  thisRegion = _Rgn;
}
//--------------------------------------------------
void dmAccumulator::Initialize( dmImage* _Dest )
{
  dmASSERT(_Dest != NULL);
  this->Initialize(_Dest,_Dest->Rect());
} 
//--------------------------------------------------
bool dmAccumulator::Add( const dmImage*  _SrcImage, 
                         const dmRegion& _SrcRegion,
                         const dmPoint&  _To )
{
  if(_SrcImage!=NULL && 
     _SrcImage->PixelFormat()==thisFormat)
  {
    accumulate_impl _filter(*this,_SrcRegion,_To);
    return dmImplementScalarOperation(_filter,*_SrcImage);
  }
  return false;
}
//--------------------------------------------------
bool dmAccumulator::Add( const dmImage* _SrcImage )
{
  if(_SrcImage!=NULL && 
     _SrcImage->PixelFormat()==thisFormat)
  {
    dmPoint _To(0,0);
    accumulate_impl _filter(*this,_SrcImage->Rect(),_To);
    return dmImplementScalarOperation(_filter,*_SrcImage);
  }
  return false;
}
//--------------------------------------------------
bool dmAccumulator::Finalize( bool _Normalize )
{
  dmIImage<dmPixelFormat32bppFloat>*
    _ThisImage = dmIImage<dmPixelFormat32bppFloat>::Cast(thisBuffer);

  typedef dmPixelFormat<dmPixelFormat32bppFloat>::value_type value_type;

  if(_ThisImage) {
    if(_Normalize)
      daim::transform(_ThisImage->Gen(),
         std::bind2nd(std::divides<value_type>(),value_type(thisCount)));

    if(thisDest) {
      accumulate_impl _filter(*this);
      return dmImplementScalarOperation(_filter,*thisDest);
    }
    return true;
  } 
  return false;
} 
//--------------------------------------------------
