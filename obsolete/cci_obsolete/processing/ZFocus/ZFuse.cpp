
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
// ZFocus_AccumulateLastNode
//
// Accumulate values from the the last node in the Laplacian 
// Result pyramid
//==============================================
static ZINLINE void ZFocus_AccumulateLastNode( CCI_INSTANCE_PTR(ZFocus) _This )
{  
  zfocus::image_type* src = zfocus::cast_image(_This->LapList.GetBuffer(_This->Levels-1));
  zfocus::image_type* dst = zfocus::cast_image(_This->ResList.GetBuffer(_This->Levels-1));
  if(src!=NULL) {
    if(dst==NULL) {
      _This->LastNodeRect = src->Rect();
      _This->ResList.StoreBuffer(_This->Levels-1,src,
                                &_This->LastNodeRect,ZFOCUS_FMT);
    } else {
      daim::combine(src->Rect(),dst->Rect().TopLeft(),src->Gen(),dst->Gen(),
                    std::plus<zfocus::value_type>());

    }
  }
}
//==============================================
// ZFocus_ProcessImage
//
// Process an image
//==============================================
static void ZFocus_ProcessImage( CCI_INSTANCE_PTR(ZFocus) _This,
                                 dmIParameters&  _Params,
                                 dm_uint         _Channel,
                                 dm_uint         _MapIndex
                               )

{
  dmImage* _Image = _This->Channels[_Channel];
  dmDEBUG_ASSERT( _Image != NULL );

  dmRect roi_rect = _This->RoiRgn.Rectangle();

  // Compute the laplacian pyramid

  // Store the image in buffer 0 
  ASSERT_RESULT(_This->TmpList.StoreBuffer(0,_Image,&roi_rect,ZFOCUS_FMT));

  // Create the laplacian pyramid
  // Place the result in the \a Laplist member of _This
  ZFocus_LaplacianPyramid(_This,_Params);

  //====================================================
  // TODO: here can be set a filter on the laplacien nodes
  //====================================================

  bool _ProcessMap = (_This->ProcessFlags & CCIV_ZFocus_ProcessMap)!=0;
  bool _CreateMap  = (_This->ProcessFlags & CCIV_ZFocus_UpdateMap) !=0 && 
                     (_This->ChannelMap == _Channel);

  if(_MapIndex == _NOINDEX_) 
    _MapIndex = _This->Count;

  // Cumulate result with the previous computation
  for(dm_int i=_This->Levels-1;--i>=0;) 
  {
    zfocus::image_type* src = zfocus::cast_image(_This->LapList.GetBuffer(i));
    zfocus::image_type* dst = zfocus::cast_image(_This->ResList.GetBuffer(i));

    if(src!=NULL) {
      if(dst!=NULL) 
      {
        if(_ProcessMap) {
           zfocus::map_type* map = dmIImage<MAP_FMT>::Cast(_This->MapList.GetBuffer(i));
           dmDEBUG_ASSERT( map != NULL )

           daim::copy_if_mask(src->Rect(),dmPoint(0,0),
                              map->Gen(),src->Gen(),dst->Gen(),
                              std::bind2nd(std::equal_to<dm_uint8>(),dm_uint8(_MapIndex)));

           continue;
        }

        if(_CreateMap) 
        {
          // Update the map by selecting the current index for the pixel
          // if the absolute value is greater
          zfocus::map_type*  map = dmIImage<MAP_FMT>::Cast(_This->MapList.GetBuffer(i));
          daim::copy_mask_if(src->Rect(),dmPoint(0,0),
                             map->Gen(),src->Gen(),dst->Gen(),
                             dm_uint8(_MapIndex),zfocus::greater_abs());
        }
        else 
        {
          daim::copy_if(src->Rect(),dmPoint(0,0),src->Gen(),dst->Gen(),
                        zfocus::greater_abs());
        }
      }
      else 
      {
        _This->ResList.StoreBuffer(i,src);

        if(_CreateMap) 
        {
          _This->MapList.CreateBuffer(i,MAP_FMT,src->Width(),src->Height());
      
          // Initialize the map
          zfocus::map_type* map = dmIImage<MAP_FMT>::Cast(_This->MapList.GetBuffer(i));
          map->Gen().fill(dm_uint8(_MapIndex));
        }
      }
    }
  }

  // Accumulate the last image in Laplacian pyramid
  ZFocus_AccumulateLastNode(_This);
}
//==============================================
