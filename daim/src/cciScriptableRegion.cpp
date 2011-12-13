/* :::BEGIN LICENSE BLOCK:::
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
 *  :::END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : cciScriptableRegion.cpp
// Date         : 12 janv. 2009
// Author       : David Marteau
//--------------------------------------------------------

#include "cciScriptableRegion.h"


/* Implementation file */
CCI_IMPL_ISUPPORTS2(cciScriptableRegion, cciIRegion,cciIRegionContainer)

cciScriptableRegion::cciScriptableRegion( const dmRegion& srcRgn )
: mRegion(srcRgn)
{

}

cciScriptableRegion::cciScriptableRegion()
{
}

cciScriptableRegion::~cciScriptableRegion()
{
}

/* [noscript,notxpcom] dmRegionPtr nativeRegion (); */
CCI_IMETHODIMP_(dmRegion *) cciScriptableRegion::GetNative()
{
    return &mRegion;
}

/* [noscript,notxpcom] cci_result copyNative (in dmRegionPtr srcRgn, in dm_int32 mode); */
CCI_IMETHODIMP_(cci_result) cciScriptableRegion::CopyNative(dmRegion *srcRgn, dm_int32 mode)
{
  CCI_ENSURE_ARG_POINTER(srcRgn);

  switch(mode)
  {
    case 0 : mRegion = *srcRgn; break;

    case dmTk::Math::AddPixels  :
    case dmTk::Math::OrPixels   : mRegion.AddRoi(*srcRgn); break;
    case dmTk::Math::SubPixels  : mRegion.SubRoi(*srcRgn); break;
    case dmTk::Math::XorPixels  : mRegion.XorRoi(*srcRgn); break;
    case dmTk::Math::AndPixels  : mRegion.AndRoi(*srcRgn); break;
    case dmTk::Math::NSubPixels : {
      dmRegion _tmp = *srcRgn;
      _tmp.SubRoi(mRegion);
      mRegion = _tmp;
    } break;

    default:
      return CCI_ERROR_INVALID_ARG;
  }

  return CCI_OK;
}

/* [noscript,notxpcom] void getBoundingBox (in dmRectRef aRect); */
CCI_IMETHODIMP_(void) cciScriptableRegion::GetBoundingBox(dm_rect & aRect)
{
  aRect = mRegion.Rectangle();
}

/* [noscript,notxpcom] boolean intersect (in dmRectRef aRect); */
CCI_IMETHODIMP_(bool) cciScriptableRegion::Intersect(dm_rect & aRect)
{
  return mRegion.Intersect(aRect) ? true : false;
}

/* [noscript,notxpcom] void clipToRect (in dmRectRef aRect); */
CCI_IMETHODIMP_(void) cciScriptableRegion::ClipToRect(dm_rect & aRect)
{
  mRegion.ClipToRect(aRect);
}

/* void setRectRgn (in long x, in long y, in unsigned long width, in unsigned long height); */
CCI_IMETHODIMP cciScriptableRegion::SetRectRgn(dm_int32 x, dm_int32 y, dm_uint32 width, dm_uint32 height)
{
  mRegion.SetRectRoi(dmRect(x,y,width,height));
  return CCI_OK;
}

/*  void setRegion( in cciIRegion aSrc ); */
CCI_IMETHODIMP cciScriptableRegion::SetRegion(cciIRegion *aSrc)
{
  CCI_ENSURE_ARG_POINTER(aSrc);

  mRegion = *aSrc->GetNative();
  return CCI_OK;
}

/* void copy (in cciIRegion aSrc, in dm_int32 mode); */
CCI_IMETHODIMP cciScriptableRegion::Copy(cciIRegion *aSrc, dm_int32 mode)
{
  CCI_ENSURE_ARG_POINTER(aSrc);
  
  return CopyNative(aSrc->GetNative(),mode);
}

/* boolean isEmpty (); */
CCI_IMETHODIMP cciScriptableRegion::IsEmpty(bool *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(_retval);
  *_retval = (mRegion.IsEmptyRoi()?true:false);

  return CCI_OK;
}

/* boolean isRectangle (); */
CCI_IMETHODIMP cciScriptableRegion::IsRectangle(bool *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(_retval);
  *_retval = (mRegion.IsRectRoi()?true:false);

  return CCI_OK;
}

/* boolean isValid (); */
CCI_IMETHODIMP cciScriptableRegion::IsValid(bool *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(_retval);
  *_retval = (mRegion.IsValidRoi()?true:false);

  return CCI_OK;
}

/* boolean pointInRegion (in dm_int32 x, in dm_int32 y); */
CCI_IMETHODIMP cciScriptableRegion::PointInRegion(dm_int32 x, dm_int32 y, bool *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(_retval);

  *_retval = (mRegion.PointInRoi(x,y)?true:false);

  return CCI_OK;
}

/* void clear (); */
CCI_IMETHODIMP cciScriptableRegion::Clear()
{
  mRegion.KillRoi();
  return CCI_OK;
}

/* void translate (in dm_int32 dx, in dm_int32 dy); */
CCI_IMETHODIMP cciScriptableRegion::Translate(dm_int32 dx, dm_int32 dy)
{
  mRegion.Translate(dx,dy);
  return CCI_OK;
}

/* cciIRegion clone (); */
CCI_IMETHODIMP cciScriptableRegion::Clone(cciIRegion * *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(_retval);

  *_retval = new cciScriptableRegion(mRegion);

  if(!*_retval)
     return CCI_ERROR_OUT_OF_MEMORY;

  CCI_ADDREF(*_retval);
  return CCI_OK;
}

/* readonly attribute dm_uint32 area; */
CCI_IMETHODIMP cciScriptableRegion::GetArea(dm_uint32 *aArea)
{
  CCI_ENSURE_ARG_POINTER(aArea);

  *aArea = mRegion.Area();
  return CCI_OK;
}

struct rgn_walker_op
{
  cciRegionWalkerFun walker;
  void *closure;

  rgn_walker_op( cciRegionWalkerFun& aWalker, void* aClosure )
  : walker(aWalker)
  , closure(aClosure)
  {}

  void operator() (long y, long x1, long x2) {
    walker(y,x1,x2,closure);
  }
};

/* [noscript] void walkRegion (in cciRegionWalkerFun walker, in voidPtr closure); */
CCI_IMETHODIMP cciScriptableRegion::WalkRegion(cciRegionWalkerFun walker, void *closure)
{
  rgn_walker_op op(walker,closure);

  dmDigitalRegion(mRegion,op);

  return CCI_OK;
}

