
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

//==============================================
// ZFocus_GetImageFromParams
// Get image from parameters
//==============================================
static dmImage* ZFocus_GetImageFromParams( dmIParameters& _IParams, dm_uint index )
{
  if(index!=_NOINDEX_) {
    dmIImageList _IImages;
    if(_IImages.QueryInterface(_IParams.GetImageList()))
     return _IImages.GetBuffer(index);
  } else
    return _IParams.GetImage();

  return NULL;
}
//==============================================
// ZFocus_AllocImage
//
// Initialize an image for processing with the 
// correct size
//==============================================
static ZINLINE dmImage* ZFocus_AllocImage( CCI_INSTANCE_PTR(ZFocus) _This )
{
  if(!_This->RoiRgn.IsEmptyRoi()) {
    const dmRect& roi_rect = _This->RoiRgn.Rectangle();
    _This->TmpList.CreateBuffer(0,ZFOCUS_FMT,roi_rect.Width(),roi_rect.Height());
    return _This->TmpList.GetBuffer(0);
  }
  return NULL;
}
//==============================================
// Zfocus_SplitChannels
//
// Split images into separate channels
//==============================================
static bool  Zfocus_SplitChannels( CCI_INSTANCE_PTR(ZFocus) _This,
                                   dmImage* _Image )
{
  bool result = false;

  if(_Image!=NULL) 
  {
    // Initialize the ROI
    if(_This->RoiRgn.IsEmptyRoi())
       _This->RoiRgn.SetRectRoi(_Image->Rect());

    if(_This->NumChannels>1 && dmIsPixelFormatScalar(_Image->PixelFormat())) {
      // Ok Composite colorspace has been specified but the image
      // passed was scalar. If this is the first image, change the colorspaces 
      // accordingly:
      if(_This->Count==0) {
        _This->ColorSpace  = CCIV_ZFocus_ColorSpaceGrey;
        _This->NumChannels = 1;
      }
    }

    dm_uint nChannels = _This->NumChannels;

    // Allocate channels if not Grey colorSpace
    if(_This->ColorSpace != CCIV_ZFocus_ColorSpaceGrey) 
    {
      const dmRect&  roi_rect = _This->RoiRgn.Rectangle();
      for(dm_int i=_This->Channels.size();--i>=0;) {
         dmImage* _channel = _This->Channels[i];
         if(_channel==NULL || 
            _channel->Width() !=roi_rect.Width()  ||
            _channel->Height()!=roi_rect.Height())
         
            _This->Channels[i] = dmCreateImage<CHANNEL_FMT>(
                                           roi_rect.Width(),
                                           roi_rect.Height());
      }
    } 

    switch(_This->ColorSpace) 
    {
      case CCIV_ZFocus_ColorSpaceRGB: { // Split image in RGB colorspace; 
        dmDEBUG_ASSERT(_This->Channels.size() == 3); 
        dmIImage<RGB_FMT>* src = dmIImage<RGB_FMT>::Cast(_Image);
          
        if(src!=NULL)
          result =  dmSplitRGB(*src,*_This->Channels[0],
                                    *_This->Channels[1],
                                    *_This->Channels[2],
                                    _This->RoiRgn.Rectangle(), 
                                    dmPoint(0,0));
        else {
          // not an rgb, map scalar components in all channels
          for(dm_uint i=0;i<nChannels;++i)
             _This->Channels[i]->GetCopy(*_Image,_This->RoiRgn.Rectangle(),dmPoint(0,0));
        }
      }
      break;


      default : 
        // Use scalar
        _This->Channels[0] = dmNewLink<dmImage>(_Image);
        result =  true;
    }

    // Compute the dynamic of each channels so that we can restore it
    // when reconstructing the image 
    if(result && _This->ColorSpace != CCIV_ZFocus_ColorSpaceGrey) {
       zfocus::channel_type* img;
       for(dm_uint i=0;i<nChannels;++i) {
         img   = dmIImage<CHANNEL_FMT>::Cast(_This->Channels[i]);
         _This->Ranges[i] = daim::for_each(img->Rect(),img->Gen(),_This->Ranges[i]);
       }
    }
  }

  return result;
}
//==============================================
// Zfocus_MergeChannels
//
// Merge separate channels into one image
//==============================================
static bool  Zfocus_MergeChannels( CCI_INSTANCE_PTR(ZFocus) _This,
                                   dmImage* _Image )
{
  if(_Image!=NULL) 
  {
    dmRect  roi_rect  = _This->RoiRgn.Rectangle();
    dmPoint point     = roi_rect.TopLeft();

    roi_rect.Translate(-point.x,-point.y);

    switch(_This->ColorSpace) 
    {
      case CCIV_ZFocus_ColorSpaceRGB: { // Merge image in RGB colorspace;         
        dmDEBUG_ASSERT(_This->Channels.size() == 3); 
        dmIImage<RGB_FMT>* rgb_result = dmIImage<RGB_FMT>::Cast(_Image);

        if(rgb_result!=NULL)
          return dmMergeRGB(*rgb_result,*_This->Channels[0],
                                        *_This->Channels[1],
                                        *_This->Channels[2],
                                        roi_rect, point);
      }
      break;

      default : {

         // \see ZFocus_Reconstruct
         // \see Zfocus_SplitChannels

         dmImage* result = _This->TmpList.GetBuffer(0);
         if(result) {
           _Image->GetCopy(*result,roi_rect,point);
           return true;
         }
      }
    }
  }  
  return false;
}
//==============================================
