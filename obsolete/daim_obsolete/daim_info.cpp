
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

// daim_info.cpp : implementation file
//
#define dmUseServicesMngr
#define dmUseKernelImageTemplates
#include "daim_info.h"
#include "daim_kernel/operators/dmOperators.h"
#include "tools/dmColorMaps.h"
#include "common/dmUserLib.h"
#include "templates/processing/dmHistogram.h"
#include "templates/processing/dmDensityMap.h"

#include "documents/dmDocTypes.h"

#include "daim_unitsprefs.h"

#include "../../include/daim/daim_decls.h"

//--------------------------------------------------------------------------
daim_info* daim_info::_pInfoActive = NULL;
//--------------------------------------------------------------------------
#define THIS_DOC(me) if(mDocument) mDocument->me

//----------------------------------------------------------------------------
daim_info::daim_info()
: mbFromDisk(false)
 ,mbModified(false)
 ,mMinRange(0)
 ,mMaxRange(0)
 ,mDocAttrs(0)
 ,mLastDocError(ERR_DOC_NOERROR)
{
  mDocument = NULL; 
}
//----------------------------------------------------------------------------
daim_info::~daim_info()
{
  if(_pInfoActive==this) _pInfoActive=NULL;
  if(mDocument)
    delete mDocument;
}
//--------------------------------------------------------------------------
daim_info* daim_info::GetInfo()
{
  return _pInfoActive;
}
//--------------------------------------------------------------------------
bool daim_info::IsActiveInfo() const
{
  return _pInfoActive == this;
}
//--------------------------------------------------------------------------
daim_info* daim_info::SetActiveInfo( bool bactive )
{
  daim_info* pOldActive = _pInfoActive;
  if(bactive) 
  {
    if(_pInfoActive && _pInfoActive!=this) 
       _pInfoActive->SetActiveInfo(false);

    _pInfoActive = this;
  } 
  else 
    if(_pInfoActive==this) 
      _pInfoActive=NULL;

  THIS_DOC(OnSetActiveInfo)(_BOOL(bactive));
  return pOldActive;
}
//----------------------------------------------------------------------------
bool daim_info::IsValidView() const 
{ 
  return !IsEmptyView(); 
}
//----------------------------------------------------------------------------
void daim_info::SetImageMask( dmRegion& _mask, const dmRegion& _roi , int _op )
{
  switch(_op) {
    case dmTk::Math::AddPixels : 
    case dmTk::Math::OrPixels  : _mask.AddRoi(_roi); break;
    case dmTk::Math::SubPixels : _mask.SubRoi(_roi); break;
    case dmTk::Math::XorPixels : _mask.XorRoi(_roi); break;
    case dmTk::Math::AndPixels : _mask.AndRoi(_roi); break;
    default : _mask = _roi;
  }
  UpdateMask(true);
  Refresh();
}
//----------------------------------------------------------------------------
void daim_info::SetExtMask( const dmRegion& _roi )
{
  mExtMask = _roi;
  UpdateMask(false);
}
//----------------------------------------------------------------------------
void daim_info::SetDensityRange( dm_double rmin, dm_double rmax, bool _update )
{
  mMaxRange = rmax;
  mMinRange = rmin;
  if(_update) UpdateMask(true); 
}
//----------------------------------------------------------------------------
void daim_info::GetDensityRange( dm_double& rmin, dm_double& rmax ) const
{
  rmax = mMaxRange;
  rmin = mMinRange;
}
//---------------------------------------------------------------------------
void daim_info::UpdateMask( bool bUpdate )
{
  THIS_DOC(UpdateMask)(bUpdate);
}
//----------------------------------------------------------------------------
void daim_info::KillMask()
{
  // COMMENT:
  // Commented because for visual effects some UI tools relies
  // on updating intensively the mask,(like 'thresholding' tool)
  // If any other tool rely on this for refreshing data, this can
  // be disastrous in term of performances

  // Think that there is "mask" builders" and "mask listeners"
  // A mask builder is responsible for triggering notifications about
  // mask change (think about a user with a 'thresholding slider': notification
  // should be triggered when mousebutton is released)  
 
  //THIS_DOC(OnSetMask)(dm_false);
  if( !mMask.IsEmptyRoi() ) {
    mMask.KillRoi();
    mExtMask.KillRoi();
    UpdateMask(true);
    Refresh();
  }
  //THIS_DOC(OnSetMask)(dm_true);
}
//----------------------------------------------------------------------------
bool daim_info::IsGrayScaleImage() const
{
  return (IsValidView() && dmIsPixelFormatScalar(mImage->PixelFormat()));
}
//----------------------------------------------------------------------------
bool daim_info::IsRGBImage() const
{
  return (IsValidView() && mImage->PixelFormat()==dmPixelFormat24bppRGB);
}
//----------------------------------------------------------------------------
bool daim_info::RestoreFromBuffer()
{
  if(IsValidView()) {
    if(!mImageBuffer.IsEmpty()) 
    {
      mImageBuffer.BufferToImage( *mImage );
      mImageBuffer.ReleaseBuffer();
      UpdateMask(true);
      Refresh();      
   }
   return true; 
  } 
  return false;
} 
//----------------------------------------------------------------------------
bool daim_info::SaveToBuffer()
{
  if(IsValidView()) {
    dmImage* im = mImage;
    if(im) mImageBuffer.CreateBuffer(*im);
    return true; 
  }
  return false;
} 
//----------------------------------------------------------------------------
void daim_info::SetTitle( const dmString& _title )
{
  mTitle = _title;
}
//----------------------------------------------------------------------------
void daim_info::SetImage( dmLink<dmImage>& _image )
{
  if(!_image.IsNull() && !_image->HasUnits()) 
  {
    dmSpatialUnits& cal = dmGetServices<dmUnitsPrefs>()->mUnits;
    _image->SetUnits(cal);
  }

  mImageBuffer.ReleaseBuffer(); // Yes 
  mImage = _image;

  mMask.KillRoi();
  mExtMask.KillRoi();

  UpdateMask(true); 
  UpdateObject();
}
//----------------------------------------------------------------------------
void daim_info::SetMask( const dmRegion& _roi, int _op )
{
  // See KillMask for an explanation of why
  // this is commented
  // THIS_DOC(OnSetMask)(dm_false);

  mExtMask.KillRoi();
  SetImageMask(mMask,_roi,_op);

  // THIS_DOC(OnSetMask)(dm_true);
}
//----------------------------------------------------------------------------
void daim_info::ResetMask()
{
  mExtMask.KillRoi();
  UpdateMask(true);
  Refresh();
}
//----------------------------------------------------------------------------
void daim_info::ClearImage()
{
  mImage.Release();
  mImageBuffer.ReleaseBuffer();
  KillMask();
  UpdateObject();
}
//----------------------------------------------------------------------------
void daim_info::SetModified( bool bModified )
{
  mbModified = bModified;
  THIS_DOC(OnSetModified)(_BOOL(bModified));
}
//--------------------------------------------------------------------------------------
void daim_info::RecalcInfo()
{
  if(!mImage.IsNull())
    mImageRect =  mImage->Rect();
}
//------------------------------------------------------
// Image Conversions
//----------------------------------------------------------------------------
dmLink<dmImage> daim_info::ConvertToRGB()
{
  if(!IsValidView())
    return dmReturnLink( (dmImage*)NULL );

  dmLink<dmImage> img = mImage;

  if(mImage->PixelFormat()!=dmPixelFormat24bppRGB) 
  {
     // First convert to 8 bits
     if(mImage->PixelFormat()!=dmPixelFormat8bppIndexed) 
        img = ConvertTo8Bits();
     
     if(!img.IsNull()) 
        return img->CreateCopy(*dmGetDescriptor(dmPixelFormat24bppRGB));
  }
  return mImage->CreateCopy();
}
//----------------------------------------------------------------------------
dmLink<dmImage> daim_info::ConvertTo8Bits()
{
  if(!IsValidView()) 
    return dmReturnLink( (dmImage*)NULL );

  if(mImage->PixelFormat()!=dmPixelFormat8bppIndexed) 
  {
     if(IsRGBImage())
       return mImage->CreateCopy(*dmGetDescriptor(dmPixelFormat8bppIndexed));
     else {
       dmLink<dmImage> img = dmCreateImage<dmPixelFormat8bppIndexed>(
                                             mImage->Width(),mImage->Height());
       dmScaleTo8Bits(*mImage,*img,mMinRange,mMaxRange);
       return img;
     }
  }

  return  mImage->CreateCopy();
}
//----------------------------------------------------------------------------
dmLink<dmImage> daim_info::ConvertTo( EPixelFormat _Format )
{
  if(IsValidView()) 
  {
    switch(_Format)
    {
      case dmPixelFormat8bppIndexed : return ConvertTo8Bits();
      case dmPixelFormat24bppRGB    : return ConvertToRGB();
      default: {
        dmImageDescriptor* pDesc = dmGetDescriptor(_Format);
        if(pDesc) 
           return mImage->CreateCopy(*pDesc);
      }
    }
  } 
  return dmReturnLink( (dmImage*)NULL);
}
//------------------------------------------------------
// Image transformations
//------------------------------------------------------
dmLink<dmImage> daim_info::FlipImage  ( int way )
{
  if(!mImage.IsNull()) 
  {
    dmRect r = mImage->Rect();
    dmLink<dmImage> img = mImage->TypeDescriptor()->CreateImage(r.Width(),r.Height());
    if(dmFlipCopy(*mImage,*img,r,dmPoint(0,0),way)) 
      return img;
  }
  return dmLink<dmImage>();
}
//------------------------------------------------------
dmLink<dmImage> daim_info::RotateImage( int way )
{
  if(!mImage.IsNull()) {
    dmRect r = mImage->Rect();
    dmLink<dmImage> img = mImage->TypeDescriptor()->CreateImage(r.Height(),r.Width());
    if(dmRotateCopy(*mImage,*img,r,dmPoint(0,0),way)) 
       return img;  
  }
  return dmLink<dmImage>();
}
//----------------------------------------------------------------------------
void daim_info::Refresh()      { THIS_DOC(OnRefresh)();      }
void daim_info::UpdateObject() { THIS_DOC(OnUpdateObject)(); }
//----------------------------------------------------------------------------
void daim_document::OnRefresh() {}
//----------------------------------------------------------------------------
void daim_document::OnUpdateObject() { 
   OnRecalculate(dm_false);
   mInfo->RecalcInfo(); 
   OnRecalculate(dm_true);
} 
//----------------------------------------------------------------------------
