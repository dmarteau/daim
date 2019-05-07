#ifndef dmChannels_h
#define dmChannels_h

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
// File         : dmChannels.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#include "templates/rgb/dmColorSpaces.h"
#include "templates/rgb/dmSplitRGB.h"

//----------------------------------------------------------------------
// Split RGB image into image triplet components
//------------------------------------------------------------------------
template<EPixelFormat _PixelFormat>
struct dmChannels
{
  template<class S>
  bool Split( S _Splitter,
              const dmImage& _RGB,
              dmImage& _Chan1, dmImage& _Chan2, dmImage& _Chan3,
              const dmRect&  _Rect,
              const dmPoint& _Pt )
  {
    if((_RGB.PixelFormat()!=dmPixelFormat24bppRGB   ) ||
       (_Chan1.PixelFormat()  !=_PixelFormat) ||
       (_Chan2.PixelFormat()  !=_PixelFormat) ||
       (_Chan3.PixelFormat()  !=_PixelFormat)) {
      dmTRACE("ERROR : dmChannels::Split :  invalid images format !\n");
      return false;
    }

    return
      daim::split_rgb(
          _Splitter,
          dmIImage<dmPixelFormat24bppRGB>::Cast(&_RGB)->Gen(),
          dmIImage<_PixelFormat>::Cast(&_Chan1)->Gen(),
          dmIImage<_PixelFormat>::Cast(&_Chan2)->Gen(),
          dmIImage<_PixelFormat>::Cast(&_Chan3)->Gen(),
          _Rect,_Pt
        );
  }

  template<class M>
  bool Merge( M _Merger,
              dmImage& _RGB,
              const dmImage& _Chan1, const dmImage& _Chan2, const dmImage& _Chan3,
              const dmRect&  _Rect,
              const dmPoint& _Pt )
  {
    if((_RGB.PixelFormat()!=dmPixelFormat24bppRGB) ||
       (_Chan1.PixelFormat()  !=_PixelFormat) ||
       (_Chan2.PixelFormat()  !=_PixelFormat) ||
       (_Chan3.PixelFormat()  !=_PixelFormat)) {
      dmTRACE("ERROR : dmChannels::Merge :  invalid images format !\n");
      return false;
    }

    return
      daim::merge_rgb(
          _Merger,
          dmIImage<dmPixelFormat24bppRGB>::Cast(&_RGB)->Gen(),
          dmIImage<_PixelFormat>::Cast(&_Chan1)->Gen(),
          dmIImage<_PixelFormat>::Cast(&_Chan2)->Gen(),
          dmIImage<_PixelFormat>::Cast(&_Chan3)->Gen(),
          _Rect,_Pt
        );
  }
};

//----------------------------------------------------------------------
// Split
//------------------------------------------------------------------------
template<class S>
bool _dmSplit( S _Splitter,
               const dmImage& _RGB,
               dmImage& _Chan1, dmImage& _Chan2, dmImage& _Chan3,
               const dmRect&  _Rect,
               const dmPoint& _Pt )
{
  switch(_Chan1.PixelFormat())
  {
    case dmPixelFormat8bppIndexed : {
      dmChannels<dmPixelFormat8bppIndexed> _Channels;
      return _Channels.Split(_Splitter,_RGB,_Chan1,_Chan2,_Chan3,_Rect,_Pt);
    }
    case dmPixelFormat32bppFloat : {
      dmChannels<dmPixelFormat32bppFloat> _Channels;
      return _Channels.Split(_Splitter,_RGB,_Chan1,_Chan2,_Chan3,_Rect,_Pt);
    }

    default: break;
  }
  return false;
}
//----------------------------------------------------------------------
// Merge
//------------------------------------------------------------------------
template<class M>
bool _dmMerge( M _Merger,
               dmImage& _RGB,
               const dmImage& _Chan1, const dmImage& _Chan2, const dmImage& _Chan3,
               const dmRect&  _Rect,
               const dmPoint& _Pt )
{
  switch(_Chan1.PixelFormat())
  {
    case dmPixelFormat8bppIndexed : {
      dmChannels<dmPixelFormat8bppIndexed> _Channels;
      return _Channels.Merge(_Merger,_RGB,_Chan1,_Chan2,_Chan3,_Rect,_Pt);
    }
    case dmPixelFormat32bppFloat : {
      dmChannels<dmPixelFormat32bppFloat> _Channels;
      return _Channels.Merge(_Merger,_RGB,_Chan1,_Chan2,_Chan3,_Rect,_Pt);
    }

    default: break;
  }
  return false;
}
//------------------------------------------------------------------------
#ifndef dmUseLibChannels

#define dmSplitRGB(_RGB,A,B,C,R,P) _dmSplit(daim::colorspace::RGB::splitter(),_RGB,A,B,C,R,P)
#define dmSplitHSV(_RGB,A,B,C,R,P) _dmSplit(daim::colorspace::HSV::splitter(),_RGB,A,B,C,R,P)
#define dmSplitHLS(_RGB,A,B,C,R,P) _dmSplit(daim::colorspace::HLS::splitter(),_RGB,A,B,C,R,P)
#define dmSplitYUV(_RGB,A,B,C,R,P) _dmSplit(daim::colorspace::YUV::splitter(),_RGB,A,B,C,R,P)
#define dmSplitYIQ(_RGB,A,B,C,R,P) _dmSplit(daim::colorspace::YIQ::splitter(),_RGB,A,B,C,R,P)
#define dmSplitOCP(_RGB,A,B,C,R,P) _dmSplit(daim::colorspace::OCP::splitter(),_RGB,A,B,C,R,P)

#define dmSplitGSC(_RGB,A,B,C,R,P) _dmSplit(daim::colorspace::GSC::splitter(),_RGB,A,B,C,R,P)
#define dmSplitGSL(_RGB,A,B,C,R,P) _dmSplit(daim::colorspace::GSL::splitter(),_RGB,A,B,C,R,P)

#define dmSplitHSI(_RGB,A,B,C,R,P) _dmSplit(daim::colorspace::HSI::splitter(),_RGB,A,B,C,R,P)

#define dmMergeRGB(_RGB,A,B,C,R,P) _dmMerge(daim::colorspace::RGB::merger(),_RGB,A,B,C,R,P)
#define dmMergeHSV(_RGB,A,B,C,R,P) _dmMerge(daim::colorspace::HSV::merger(),_RGB,A,B,C,R,P)
#define dmMergeHLS(_RGB,A,B,C,R,P) _dmMerge(daim::colorspace::HLS::merger(),_RGB,A,B,C,R,P)
#define dmMergeYUV(_RGB,A,B,C,R,P) _dmMerge(daim::colorspace::YUV::merger(),_RGB,A,B,C,R,P)
#define dmMergeYIQ(_RGB,A,B,C,R,P) _dmMerge(daim::colorspace::YIQ::merger(),_RGB,A,B,C,R,P)
#define dmMergeOCP(_RGB,A,B,C,R,P) _dmMerge(daim::colorspace::OCP::merger(),_RGB,A,B,C,R,P)
#define dmMergeHSI(_RGB,A,B,C,R,P) _dmMerge(daim::colorspace::HSI::merger(),_RGB,A,B,C,R,P)

#else // dmUseLibChannels

#define _dmSplitRGB(_RGB,A,B,C,R,P) _dmSplit(daim::colorspace::RGB::splitter(),_RGB,A,B,C,R,P)
#define _dmSplitHSV(_RGB,A,B,C,R,P) _dmSplit(daim::colorspace::HSV::splitter(),_RGB,A,B,C,R,P)
#define _dmSplitHLS(_RGB,A,B,C,R,P) _dmSplit(daim::colorspace::HLS::splitter(),_RGB,A,B,C,R,P)
#define _dmSplitYUV(_RGB,A,B,C,R,P) _dmSplit(daim::colorspace::YUV::splitter(),_RGB,A,B,C,R,P)
#define _dmSplitYIQ(_RGB,A,B,C,R,P) _dmSplit(daim::colorspace::YIQ::splitter(),_RGB,A,B,C,R,P)
#define _dmSplitOCP(_RGB,A,B,C,R,P) _dmSplit(daim::colorspace::OCP::splitter(),_RGB,A,B,C,R,P)

#define _dmSplitGSC(_RGB,A,B,C,R,P) _dmSplit(daim::colorspace::GSC::splitter(),_RGB,A,B,C,R,P)
#define _dmSplitGSL(_RGB,A,B,C,R,P) _dmSplit(daim::colorspace::GSL::splitter(),_RGB,A,B,C,R,P)

#define _dmSplitHSI(_RGB,A,B,C,R,P) _dmSplit(daim::colorspace::HSI::splitter(),_RGB,A,B,C,R,P)

#define _dmMergeRGB(_RGB,A,B,C,R,P) _dmMerge(daim::colorspace::RGB::merger(),_RGB,A,B,C,R,P)
#define _dmMergeHSV(_RGB,A,B,C,R,P) _dmMerge(daim::colorspace::HSV::merger(),_RGB,A,B,C,R,P)
#define _dmMergeHLS(_RGB,A,B,C,R,P) _dmMerge(daim::colorspace::HLS::merger(),_RGB,A,B,C,R,P)
#define _dmMergeYUV(_RGB,A,B,C,R,P) _dmMerge(daim::colorspace::YUV::merger(),_RGB,A,B,C,R,P)
#define _dmMergeYIQ(_RGB,A,B,C,R,P) _dmMerge(daim::colorspace::YIQ::merger(),_RGB,A,B,C,R,P)
#define _dmMergeOCP(_RGB,A,B,C,R,P) _dmMerge(daim::colorspace::OCP::merger(),_RGB,A,B,C,R,P)
#define _dmMergeHSI(_RGB,A,B,C,R,P) _dmMerge(daim::colorspace::HSI::merger(),_RGB,A,B,C,R,P)

#endif // dmUseLibChannels
//------------------------------------------------------------------------

#endif // dmChannels_h
