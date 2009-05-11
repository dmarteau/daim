
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

// daim_info_mngr.cpp : implementation file
//
#define dmUseServicesMngr
#define dmUseKernelImageTemplates
#include "daim_info_mngr.h"
#include "common/dmUserLib.h"
#include "templates/processing/dmHistogram.h"
#include "templates/processing/dmDensityMap.h"
#include "tools/dmColorMaps.h"

#include "daim_kernel/images/dmNativeImage.h"
#include "dmStyleManager.h"

#include "daim_layers.h"

#define THIS_INFO() mInfo

//----------------------------------------------------------------------------
daim_info_manager::daim_info_manager(daim_info* _info)
: daim_document(_info)
 ,mTools(NULL)
 ,mbShowMask(true)
 ,mbShowLayers(false)
 ,mbMaskVisibility(true)
 ,mMaskColor(DM_RGB(255,0,0))
 ,mExtMaskColor(DM_RGB(0,255,0))
 ,mbEqualizeLUT(false)
 ,mbCustomLUT(false)
{
  SetDrawStyle(dmGetDrawStyleClass(_TXT("*default")));

  mDisplayableImageInfo.PixelFormat = dmPixelFormatUndefined;
  mDisplayableImageInfo.Scan0       = NULL;
  mDisplayableImageInfo.Stride      = 0;

  mLayers = new daim_layers();

  EnableMultiSelection(false); // Disable multiselection from user interface

  Create_Tools();
  Show(true);
}
//----------------------------------------------------------------------------
daim_info_manager::~daim_info_manager()
{
  Destroy_Tools();
  
  if(mLayers)
     delete mLayers;
}
//----------------------------------------------------------------------------
void daim_info_manager::Refresh()
{
  dmGraphics* gr = GetGraphics();
  if(gr) 
     gr->Refresh();
}
//----------------------------------------------------------------------------
bool daim_info_manager::DrawImage()
{
  if(!mDisplayableImage.IsNull()) 
  {
    dmSurface _s(GetGraphics());
    if(_s)
       _s->Display( mDisplayableImage,dmPoint(0,0) );
                //   THIS_INFO()->GetImageRect(),
                //   THIS_INFO()->GetImage()->Rect() 
                // ); 
    return true;
  }
  return false;
}
//--------------------------------------------------------------------------------------
dmImage* daim_info_manager::GetImage() const
{
  return THIS_INFO()->GetImage();
}
//--------------------------------------------------------------------------------------
void daim_info_manager::Recalculate()
{
  OnRecalculate(dm_false);
  dmDEBUG_ASSERT( mInfo != NULL );
  THIS_INFO()->RecalcInfo(); 

  if(!THIS_INFO()->IsValidView())
     mDisplayableImage.Release();

  mRect = THIS_INFO()->GetImageRect();

  dmToolManager::Recalculate();
  OnRecalculate(dm_true);
}
//--------------------------------------------------------------------------------------
void daim_info_manager::UpdateObject()
{
  dmToolManager::UpdateObject();
}
//----------------------------------------------------------------------------
void daim_info_manager::OnDisplay( dmGraphics& gr ) const
{ 
  dmToolManager::OnDisplay(gr);
}
//----------------------------------------------------------------------------
dmLink<dmNativeImage> daim_info_manager::GetDisplayableImage( dmImage* _src, EPixelFormat _Format )
{
  if(_src!=NULL) 
  {
    dmGraphics* gr = GetGraphics();
    if(gr) {
      dmNativeImageFactory* pFactory = gr->GetImageFactory();
      if(pFactory) 
      {
        dmImageData _imagedata; 
        dmASSERT( _src->GetImageData(_imagedata) ); // Get image data from source

        dmLink<dmNativeImage> _native;
        EPixelFormat          _RequiredFormat;
       
        if(dmIsPixelFormatDisplayable(_imagedata.PixelFormat)) 
        {   
          //-------------------------------------
          // Creation of displayable image
          //-------------------------------------

          if(dmIsPixelFormatDisplayable(_Format))
             _RequiredFormat = _Format;
          else
             _RequiredFormat = _imagedata.PixelFormat;
       
          if(!mDisplayableImage.IsNull() && 
              mDisplayableImageInfo.PixelFormat == _RequiredFormat    &&
              mDisplayableImageInfo.Width       == _imagedata.Width   &&
              mDisplayableImageInfo.Height      == _imagedata.Height) 
          {
              _native = mDisplayableImage;

              if(_RequiredFormat != _imagedata.PixelFormat) 
              {
                // Do the conversion

                dmImageDescriptor* pDesc = dmGetDescriptor(_RequiredFormat);
                dmASSERT(pDesc != NULL); 
            
                _native->LockBits(_RequiredFormat,_imagedata,ELockWrite);

                dmLink<dmImage> _image = pDesc->CreateImage(_imagedata);
                _image->GetCopy(*_src);

                _native->UnlockBits(_imagedata);

              } else {
                // Copy data back into native
                _native->LockBits(_imagedata.PixelFormat,_imagedata,ELockWrite|ELockUserBuffer);
                _native->UnlockBits(_imagedata);
              }
          } 
          else 
          {
            if(!dmIsPixelFormatDisplayable(_Format) || 
                _Format == _imagedata.PixelFormat)
            {
               // -----------------------------------------------
               // If no displayable format specified lets go with 
               // the original image format
               // -----------------------------------------------
               _native = dmAutoLink<dmNativeImage>(pFactory->Create(_imagedata));   
            } 
            else
            {
               // -----------------------------------------------
               // A specific format is required
               // -----------------------------------------------
               
               // Create the native image wih the required format
               _native = dmAutoLink<dmNativeImage>(
                         pFactory->Create(_imagedata.Width,_imagedata.Height,_Format));

               // Do the conversion
               
               dmImageDescriptor* pDesc = dmGetDescriptor(_Format);
               dmASSERT(pDesc != NULL); 
                
                _native->LockBits(_Format,_imagedata,ELockWrite);
                
                dmLink<dmImage> _image = pDesc->CreateImage(_imagedata);
                _image->GetCopy(*_src);

                _native->UnlockBits(_imagedata);
            }
            // Store native image informations
            mDisplayableImageInfo.Width       = _imagedata.Width;
            mDisplayableImageInfo.Height      = _imagedata.Height;
            mDisplayableImageInfo.PixelFormat = _imagedata.PixelFormat;
          }
          //-------------------------------------

          if(_imagedata.PixelFormat==dmPixelFormat8bppIndexed) 
          {
            dmIImage<dmPixelFormat8bppIndexed> _image( _imagedata );
            RestoreLUT(&_image);
            _native->ApplyLUT(mLut);
          }
        } 
        else 
        {
          //---------------------------------------- 
          // Use minrange and max range to
          // scale the pixel values
          //---------------------------------------- 

          if(dmIsPixelFormatDisplayable(_Format))
             _RequiredFormat = _Format;
          else
             _RequiredFormat = dmPixelFormat8bppIndexed;
             
          // Create native image 

          if(!mDisplayableImage.IsNull() && 
              mDisplayableImageInfo.PixelFormat == _RequiredFormat   &&
              mDisplayableImageInfo.Width       == _imagedata.Width  &&
              mDisplayableImageInfo.Height      == _imagedata.Height) 
          {
              _native = mDisplayableImage;
          } 
          else
          {           
              _native = dmAutoLink<dmNativeImage>(
                             pFactory->Create(_imagedata.Width,_imagedata.Height,
                                              _RequiredFormat));

             // Store native image informations
             mDisplayableImageInfo.Width       = _imagedata.Width;
             mDisplayableImageInfo.Height      = _imagedata.Height;
             mDisplayableImageInfo.PixelFormat = _RequiredFormat;
          }
          //-------------------------------------

          if(_RequiredFormat == dmPixelFormat8bppIndexed)
          {
            // Get back handle on image data
            _native->LockBits(dmPixelFormat8bppIndexed,_imagedata,ELockWrite);

            // Scale the pixel values
            dmIImage<dmPixelFormat8bppIndexed> _image( _imagedata );
            dmScaleTo8Bits(*_src,_image,THIS_INFO()->MinRange(),
                                        THIS_INFO()->MaxRange());

            //dmHistogram Histogram;
            //Histogram.GetHistogram(_image.Gen(),_image.Rect());
            //int maxc = Histogram.MaxDensityValue();
            //int minc = Histogram.MinDensityValue();   

            RestoreLUT(&_image);

            _native->UnlockBits(_imagedata);
            _native->ApplyLUT(mLut);
          }
          else
          {
            // -----------------------------------------------
            // A specific format is required
            // -----------------------------------------------

            dmImageDescriptor* pDesc = dmGetDescriptor(_Format);
            dmASSERT(pDesc != NULL); 

            // Get back handle on image data
            _native->LockBits(_RequiredFormat,_imagedata,ELockWrite);
            
            // Create a temporary image
            // XXX Note that the mTmpImage is not cleared afterwards
            //     this can be justified by the fact that 
            //     1 - It does not take too much memory (8 bits)
            //     2 - Expected to deal again with this during 
            //         execution time   
            if(mTmpImage.IsNull() ||
               mTmpImage->Width()  != _imagedata.Width  ||
               mTmpImage->Height() != _imagedata.Height) 
            {
               mTmpImage = dmCreateImage<dmPixelFormat8bppIndexed>(
                                _imagedata.Width,_imagedata.Height);
            }
            
            // We need first converting the data to 8 bits
            dmScaleTo8Bits(*_src,*mTmpImage,THIS_INFO()->MinRange(),
                                            THIS_INFO()->MaxRange());
                                            
            // Do the conversion
            dmLink<dmImage> _image = pDesc->CreateImage(_imagedata);
            _image->GetCopy(*mTmpImage);

            _native->UnlockBits(_imagedata);
          }
          
        }
        return _native;
      }
    }
  }

  return dmReturnLink( (dmNativeImage*)NULL );
}
//---------------------------------------------------------------------------
bool daim_info_manager::RestoreLUT( dmImage* _image )
{
  dmIImage<dmPixelFormat8bppIndexed>* 
     _GreyImage = dmIImage<dmPixelFormat8bppIndexed>::Cast(_image);

  if(_GreyImage) {
     //-------------------------------------
     // Setting Lut display
     //-------------------------------------
     mLut.RestoreMap();
     if(mbEqualizeLUT) 
     {
       dmColorIndex _cmap[dmHistogram::hsize];
       dmHistogram Histogram;
       Histogram.GetHistogram(_GreyImage->Gen(),_GreyImage->Rect());    
       Histogram.Equalize(_cmap);
       mLut.SetMap(_cmap);
     }
     return true;
  }
  return false;
}
//---------------------------------------------------------------------------
daim_layers* daim_info_manager::GetLayers()
{
  return mLayers;
}
//---------------------------------------------------------------------------
void daim_info_manager::ShowLayers( bool bShow )
{
  if(mLayers) {
     bool bUpdate = (mbShowLayers!=bShow);
     mbShowLayers = bShow;
     UpdateMask(bUpdate);
  }
}
//---------------------------------------------------------------------------
void daim_info_manager::KillLayers( bool bUpdate )
{
  if(mLayers) {
     mLayers->ClearAll();
     if(bUpdate) {
        UpdateMask(true);
        Refresh();
     }
  }  
}
//---------------------------------------------------------------------------
void daim_info_manager::UpdateLayers( dmNativeImage* _NativeImage )
{
  if( mLayers && mbShowLayers ) 
  {
    dmImageData _Data;
    if(_NativeImage->LockBits(dmPixelFormat24bppRGB,
                              _Data,ELockWrite|ELockRead)) 
    {
      dmIImage<dmPixelFormat24bppRGB> _img(_Data);
      mLayers->Draw(_img);
      _NativeImage->UnlockBits(_Data);
    }
  }
}
//---------------------------------------------------------------------------
void daim_info_manager::UpdateMask( bool bUpdate )
{
  dmImage* image = THIS_INFO()->GetImage();

  if(bUpdate) { 
     mDisplayableImage = GetDisplayableImage(image,(mbShowLayers && mbMaskVisibility)? 
                                                    dmPixelFormat24bppRGB:
                                                    dmPixelFormatUndefined);                              
  } else if(!THIS_INFO()->IsValidView())
     mDisplayableImage.Release();

  if(!mDisplayableImage.IsNull())
  {
    UpdateLayers(mDisplayableImage);
    
    dmRegion& _Mask    = THIS_INFO()->GetMask();
    dmRegion& _ExtMask = THIS_INFO()->GetExtMask();

    if(!_Mask.IsEmptyRoi() && (mbShowMask && mbMaskVisibility)) 
    {
      dm_uint8 _mRoiIndex = 255;

      _Mask.ClipToRect(THIS_INFO()->GetImageRect());
      _ExtMask.AndRoi(_Mask);
      
      if(dmIsPixelFormatScalar(mDisplayableImageInfo.PixelFormat)) 
      {
        // Display ROI on 8 bits indexed image
        // We use index 255 for displaying the ROI
        // So we need to remap pixels at 255 to 253/254 

        if(!_ExtMask.IsEmptyRoi()) {
          mLut.CMap[254] = 252;
          mLut.CMap[255] = 253;           
        } else {
          mLut.CMap[254] = 253;
          mLut.CMap[255] = 254;
        }

        dmImageData _data;

        if(mDisplayableImage->LockBits(dmPixelFormat8bppIndexed,
                                       _data,ELockWrite|ELockRead)) 
        {
          dmIImage<dmPixelFormat8bppIndexed> _img(_data);

          // Remap pixel values
          daim::transform(_img.Gen(),daim::_apply_map(mLut.CMap));

          // Assign mask color to lut extra color (index > 255)
          mLut.SetExtraColor(dmLUT8_EXTRA_COLOR3,COLORREF_TO_RGB(mMaskColor));

          // Assign lut map index to the mask color
          mLut.AssignRange(_mRoiIndex,_mRoiIndex,dmLUT8_EXTRA_COLOR3 );

          // Assign pixel values to the mask color index 
          daim::fill(_Mask,_img.Gen(),_mRoiIndex);
          
          // Handle extra mask
          if(!_ExtMask.IsEmptyRoi()) {
            dm_uint8 _mExtIndex = 254;
            mLut.SetExtraColor(dmLUT8_EXTRA_COLOR4,COLORREF_TO_RGB(mExtMaskColor));
            mLut.AssignRange(_mExtIndex,_mExtIndex,dmLUT8_EXTRA_COLOR4 ); 
            daim::fill(_ExtMask,_img.Gen(),_mExtIndex);
          }
          
          mDisplayableImage->UnlockBits(_data);
          mDisplayableImage->ApplyLUT(mLut);
        }
      } 
      else if(mDisplayableImageInfo.PixelFormat==dmPixelFormat24bppRGB) 
      {
        // Handle RGB images
         
        dmImageData _data;
        if(mDisplayableImage->LockBits(dmPixelFormat24bppRGB,
                                       _data,ELockWrite|ELockRead)) 
        {
          // Fill pixels with the mask color 
          dmIImage<dmPixelFormat24bppRGB> _img(_data);
          daim::fill(_Mask,_img.Gen(),COLORREF_TO_RGB(mMaskColor));

          // Handle extra mask
          if(!_ExtMask.IsEmptyRoi()) 
            daim::fill(_ExtMask,_img.Gen(),COLORREF_TO_RGB(mExtMaskColor));      

          mDisplayableImage->UnlockBits(_data);
        }
      }
    } else {
      mLut.CMap[254] = 254;
      mLut.CMap[255] = 255;           
      mDisplayableImage->ApplyLUT(mLut);
    }
  }
}
//----------------------------------------------------------------------------
void daim_info_manager::SetMaskColor( EColorType _color, bool bRefresh )
{
  mMaskColor = _color;
  if(bRefresh) {
    UpdateMask(false);
    Refresh();
  }
}
//----------------------------------------------------------------------------
void daim_info_manager::SetExtMaskColor( EColorType _color, bool bRefresh )
{
  mExtMaskColor = _color;
  if(bRefresh) {
    UpdateMask(false);
    Refresh();
  }
}
//----------------------------------------------------------------------------
void daim_info_manager::ShowMask( bool _bShow )
{
  if(mbShowMask != _bShow) {
    mbShowMask = _bShow;
    UpdateMask(true);
    Refresh();
  }
}
//----------------------------------------------------------------------------
void daim_info_manager::SetMaskVisibility( bool bVisibility )
{
  if(mbMaskVisibility != bVisibility) {
    mbMaskVisibility = bVisibility;
    UpdateMask(true);
    Refresh();
  }  
}
//----------------------------------------------------------------------------
void daim_info_manager::SetCustomLUT( dmLUT* pLUT )
{
  if(pLUT) {
    mLut = *pLUT;
    mbCustomLUT = true;
  } else { 
    mbCustomLUT = false;
    mLut.RestoreMap();
    mLut.MakePalette(dmColorMap::GrayScale);
  }
  ApplyLUT();
}
//-----------------------------------------------------
bool daim_info_manager::ApplyLUT()
{
  bool res = false;

  if(!mDisplayableImage.IsNull()) {
    res = mDisplayableImage->ApplyLUT(mLut);
    UpdateMask(false);
    Refresh();
  }

  return res;
}
//----------------------------------------------------------------------------
void daim_info_manager::EqualizeLUT ( bool bEqualize )
{
  if(mbEqualizeLUT != bEqualize) 
  {
    mbEqualizeLUT = bEqualize;
    if(!mbEqualizeLUT)
      mLut.RestoreMap();

    ApplyLUT();
  } 
}
//----------------------------------------------------------------------------
// daim_document members - called by daim_info
//----------------------------------------------------------------------------
void daim_info_manager::OnRefresh()      { Refresh(); }
//----------------------------------------------------------------------------
void daim_info_manager::OnUpdateObject() { UpdateObject(); }
//----------------------------------------------------------------------------
