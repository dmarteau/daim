
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
#include "templates/rgb/dmRGBArithmetics.h"

// XXXDavid : The following assume that integer values are unsigned !!
//            (optimisation purpose)

template<class T>
static bool arithmetics_code( int nWhat, const dmRegion& rgn, const dmPoint& p, 
            const daim::image<T>& in, 
            daim::image<T>& out, 
            const daim::integer_true& ) 
{
  switch( nWhat ) {     
    case dmTk::Math::AbsPixels    :
    case dmTk::Math::AddPixels    : daim::add_images (rgn,p,in,out); return true;
    case dmTk::Math::SubPixels    : daim::sub_images (rgn,p,in,out); return true;
    case dmTk::Math::OrPixels     : daim::or_images  (rgn,p,in,out); return true;
    case dmTk::Math::XorPixels    : daim::xor_images (rgn,p,in,out); return true;
    case dmTk::Math::AndPixels    : daim::and_images (rgn,p,in,out); return true;
    case dmTk::Math::MinPixels    : daim::min_images (rgn,p,in,out); return true;
    case dmTk::Math::MaxPixels    : daim::max_images (rgn,p,in,out); return true;
    case dmTk::Math::NSubPixels   : daim::nsub_images(rgn,p,in,out); return true;
    case dmTk::Math::DiffPixels   : daim::diff_images(rgn,p,in,out); return true;
  }
  return false;
}

template<class T>
static bool arithmetics_code( int nWhat, const dmRegion& rgn, const dmPoint& p, 
            const daim::image<T>& in,
            daim::image<T>& out, 
            const daim::integer_false& ) 
{
  switch( nWhat ) {  
    case dmTk::Math::AddPixels    : daim::add_images (rgn,p,in,out); return true;
    case dmTk::Math::SubPixels    : daim::sub_images (rgn,p,in,out); return true;
    case dmTk::Math::MinPixels    : daim::min_images (rgn,p,in,out); return true;
    case dmTk::Math::MaxPixels    : daim::max_images (rgn,p,in,out); return true;
    case dmTk::Math::NSubPixels   : daim::nsub_images(rgn,p,in,out); return true;
    case dmTk::Math::DiffPixels   : daim::diff_images(rgn,p,in,out); return true;
    case dmTk::Math::AbsPixels    : daim::abs_images (rgn,p,in,out); return true;
  }
  return false;
}
//---------------------------------------------------------------------
struct __dm_arithmetics_impl
{
   dm_int          Operation;
   const dmImage&  Lhs;
   const dmRegion& Rgn;
   const dmPoint&  Pt; 

   __dm_arithmetics_impl(dm_int          _Operation, 
                         const dmImage&  _Lhs, 
                         const dmRegion& _Rgn, 
                         const dmPoint&  _Pt )

   :Operation(_Operation)
   ,Lhs(_Lhs)
   ,Rgn(_Rgn)
   ,Pt(_Pt) {}


  template<EPixelFormat _PixelFormat> 
  void operator()( dmIImage<_PixelFormat>& _Rhs )
  {
    dmIImage<_PixelFormat>*  lhs = dmIImage<_PixelFormat>::Cast(&Lhs);

    typedef typename dmIImage<_PixelFormat>::traits_type traits_type;
    typedef typename traits_type::integer_type           integer_type;

    arithmetics_code(Operation,Rgn,Pt,lhs->Gen(),_Rhs.Gen(),
                         integer_type());
  }

  //Specialize for rgb
  void operator()( dmIImage<dmPixelFormat24bppRGB>& _Rhs ) 
  {
    dmIImage<dmPixelFormat24bppRGB>*  lhs = dmIImage<dmPixelFormat24bppRGB>::Cast(&Lhs);

    typedef dmIImage<dmPixelFormat24bppRGB>::traits_type traits_type;

    arithmetics_code(Operation,Rgn,Pt,lhs->Gen(),_Rhs.Gen(),
                     daim::integer_true());
  }

};
//---------------------------------------------------------------------
bool dmArithmetics( dm_int _Operation, 
                    const dmImage& _Lhs, dmImage& _Rhs, 
                    const dmRegion& _Rgn, const dmPoint& _Pt )
{
 if(_Lhs.PixelFormat() == _Rhs.PixelFormat()) {
   __dm_arithmetics_impl _filter(_Operation,_Lhs,_Rgn,_Pt);
   return dmImplementOperation(_filter,_Rhs);
  }
  return false;    
}
//---------------------------------------------------------------------
bool dmArithmetics( dm_int _Operation, const dmImage& _Lhs, dmImage& _Rhs )
{
  return dmArithmetics(_Operation,_Lhs,_Rhs,_Lhs.Rect(),dmPoint(0,0));   
}
//---------------------------------------------------------------------
// The lhs image for the operation is contained in buffer
// while the rhs image is the image parameter
//---------------------------------------------------------------------
bool dmArithmetics( dmBufferParameters& _Params, dm_int _Operation )
{
  if(!_Params.thisBuffer.IsEmpty()) 
  {
     if(_Params.thisImage.PixelFormat() == 
        _Params.thisBuffer.Buffer()->PixelFormat())
     {
       const dmRegion& _Rgn  =  _Params.thisBuffer.BufferRgn();
       const dmPoint&  _Pt   =  _Params.thisBuffer.BufferSrc();
       const dmImage&  _Lhs  = *_Params.thisBuffer.Buffer();

       __dm_arithmetics_impl _filter(_Operation,_Lhs,_Rgn,_Pt);
       return dmImplementOperation(_filter,_Params.thisImage);   
    }
  }
  return false;   
}
//---------------------------------------------------------------------
// Multiply images
//---------------------------------------------------------------------
struct __dm_multiply_impl
{
   const dmImage&  Rhs;
   const dmRegion& Rgn;
   const dmPoint&  Pt; 

   __dm_multiply_impl(const dmImage&  _Rhs, 
                      const dmRegion& _Rgn, 
                      const dmPoint&  _Pt )
   :Rhs(_Rhs)
   ,Rgn(_Rgn)
   ,Pt(_Pt) {}

  template<EPixelFormat _PixelFormat> 
  void operator()( dmIImage<_PixelFormat>& _Lhs )
  {
    if(Rhs.PixelFormat()!=_PixelFormat) 
    {
       dmLink<dmImage> _Rhs = Rhs.CreateCopy(*dmGetDescriptor(dmPixelFormat32bppFloat));
       dmIImage<dmPixelFormat32bppFloat>*  rhs = dmIImage<dmPixelFormat32bppFloat>::Cast(_Rhs);
       daim::multiply_images(Rgn,Pt,rhs->Gen(),_Lhs.Gen());
    } else {
      dmIImage<_PixelFormat>*  rhs = dmIImage<_PixelFormat>::Cast(&Rhs);
      daim::multiply_images(Rgn,Pt,rhs->Gen(),_Lhs.Gen());
    }
  }
};
//---------------------------------------------------------------------
bool dmMultiplyImage( dmImage& _Lhs, const dmImage& _Rhs, 
                      const dmRegion& _Rgn, const dmPoint& _Pt  )
{
   __dm_multiply_impl _filter(_Lhs,_Rgn,_Pt);
   return dmImplementScalarOperation(_filter,_Rhs);
}
//---------------------------------------------------------------------
// Divide images
//---------------------------------------------------------------------
struct __dm_divide_impl
{
   const dmImage&  Rhs;
   const dmRegion& Rgn;
   const dmPoint&  Pt; 

   __dm_divide_impl(const dmImage&  _Rhs, 
                    const dmRegion& _Rgn, 
                    const dmPoint&  _Pt )
   :Rhs(_Rhs)
   ,Rgn(_Rgn)
   ,Pt(_Pt) {}

  template<EPixelFormat _PixelFormat> 
  void operator()( dmIImage<_PixelFormat>& _Lhs )
  {    
    if(Rhs.PixelFormat()!=_PixelFormat) 
    {       
      dmLink<dmImage> _Rhs = Rhs.CreateCopy(*dmGetDescriptor(dmPixelFormat32bppFloat));
      dmIImage<dmPixelFormat32bppFloat>*  rhs = dmIImage<dmPixelFormat32bppFloat>::Cast(_Rhs);
      daim::divide_images(Rgn,Pt,rhs->Gen(),_Lhs.Gen());
    } else {
      dmIImage<_PixelFormat>*  rhs = dmIImage<_PixelFormat>::Cast(&Rhs);
      daim::divide_images(Rgn,Pt,rhs->Gen(),_Lhs.Gen());
    }
  }
};
//---------------------------------------------------------------------
bool dmDivideImage( dmImage& _Lhs, const dmImage& _Rhs, 
                    const dmRegion& _Rgn, const dmPoint& _Pt  )
{  
   __dm_divide_impl _filter(_Rhs,_Rgn,_Pt);
   return dmImplementScalarOperation(_filter,_Lhs);
}
//---------------------------------------------------------------------
// Truncate images
//---------------------------------------------------------------------
struct __dm_truncate_impl
{
   const dmImage&  Dst;
   const dmRegion& Rgn;
   const dmPoint&  Pt; 

   __dm_truncate_impl(const dmImage&  _Dst, 
                      const dmRegion& _Rgn, 
                      const dmPoint&  _Pt )
   :Dst(_Dst)
   ,Rgn(_Rgn)
   ,Pt(_Pt) {}

  template<EPixelFormat _PixelFormat> 
  void operator()( dmIImage<_PixelFormat>& _Src )
  {    
    if(Dst.PixelFormat()!=_PixelFormat) 
    {       
      switch(Dst.PixelFormat()) {
        case  dmPixelFormat8bppIndexed   : daim::truncate(Rgn,Pt,_Src.Gen(),dmIImage<dmPixelFormat8bppIndexed>::Cast(&Dst)->Gen())   ;break;
        case  dmPixelFormat32bppGrayScale: daim::truncate(Rgn,Pt,_Src.Gen(),dmIImage<dmPixelFormat32bppGrayScale>::Cast(&Dst)->Gen());break;
        case  dmPixelFormat16bppGrayScale: daim::truncate(Rgn,Pt,_Src.Gen(),dmIImage<dmPixelFormat16bppGrayScale>::Cast(&Dst)->Gen());break;
        case  dmPixelFormat32bppFloat    : daim::truncate(Rgn,Pt,_Src.Gen(),dmIImage<dmPixelFormat32bppFloat>::Cast(&Dst)->Gen())    ;break;
        default:
          dmTRACE("Unhandled image format in 'truncate'!\n");
      }
    } else {
      // Simple copy 
      dmIImage<_PixelFormat>*  dst = dmIImage<_PixelFormat>::Cast(&Dst);
      daim::copy(Rgn,Pt,_Src.Gen(),dst->Gen());
    }
  }
};
//---------------------------------------------------------------------
bool dmTruncateImage( const dmImage& _Src, dmImage& _Dst, const dmRegion& _Rgn,
                      const dmPoint& _Point )
{
   __dm_truncate_impl _filter(_Dst,_Rgn,_Point);
   return dmImplementScalarOperation(_filter,_Src);
}
//---------------------------------------------------------------------

