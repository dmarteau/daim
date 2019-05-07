
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

//--------------------------------------------------------------------
// Shift correction algorithm
//--------------------------------------------------------------------
/*
static void ZFocus_ShiftCorrection( CCI_INSTANCE_PTR(ZFocus) _This,
                                    dmIParameters& _IParams,
                                    dmRect& roi_rect, 
                                    dm_uint istart,
                                    dm_uint ilast )
{
  dmIImageList _IImages;
  _IImages.QueryInterface(_IParams.GetImageList());

  std::vector<dmPoint> _PointList(ilast); 
  dm_uint index;
  dmImage* image;

  for(index=istart;index<ilast;++index) 
  {
     image = _IImages.GetBuffer(istart);
     if(image) 
     {
       if(roi_rect.IsEmpty())
         roi_rect = image->Rect();
       else
         roi_rect.Clip(image->Rect());
     }
  }

  if(_This->TestArea.IsEmptyRoi()) 
  {
    dm_uint w = TEST_SIZE/2 + 1;
    dmPoint p = roi_rect.Center() - dmPoint(w,w);
    _This->TestArea.SetRectRoi(dmRect(p,TEST_SIZE,TEST_SIZE));
    _This->TestArea.AndCoordinates(roi_rect);
  }

  image = _IImages.GetBuffer(istart);

  _IParams.SetImage(image);
  _IParams.SetRoi(&_This->TestArea);

  dmPoint p;
  dmRect  im_rect_full(image->Rect());
  dmRect  im_rect_part(image->Rect());

  dmIInterface<dmIImageShift> _IShift;
  _IParams.SetImage(_IImages.GetBuffer(istart));


  _PointList[istart] = dmPoint(0,0);

  for(index=istart+1;index<ilast;++index)
  {
    p = _This->TestArea.Rectangle().TopLeft();  
    _IShift.Compute(_IParams,p,index);
    p.x = -p.x;
    p.y = -p.y;

    im_rect_full.Add (dmRect(p,image->Width(),image->Height()));
    im_rect_part.Clip(dmRect(p,image->Width(),image->Height()));
    _PointList[index] = p;
  }
  // Clear the image parameter
  _IParams.SetImage(NULL);

  for(index=istart;index<ilast;++index)  
  {
    image = _IImages.GetBuffer(index);

    // Get the rectangle into the coordinates of the image
    p = im_rect_part.TopLeft() - _PointList[index];

    dmDEBUG_ASSERT( p.x >= 0 );
    dmDEBUG_ASSERT( p.y >= 0 );

    im_rect_full = dmRect(p,im_rect_part.Width(),im_rect_part.Height());

    dmDEBUG_ASSERT( im_rect_full.Width()  <= image->Width()  );
    dmDEBUG_ASSERT( im_rect_full.Height() <= image->Height() );

    // Crop the image and save it
    _IImages.StoreBuffer(index,image,&im_rect_full); 
  }  

  roi_rect = dmRect(0,0,im_rect_part.Width(),im_rect_part.Height());
}
*/
//----------------------------------------------------------------------
