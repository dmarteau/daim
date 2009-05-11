
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
// Create the laplacien pyramid for the image list in parameters
// Assume that the image is in buffer 0 of the _Params object with
// the correct format
//--------------------------------------------------------------------
static ZINLINE void ZFocus_LaplacianPyramid( CCI_INSTANCE_PTR(ZFocus) _This,
                                             dmIParameters&  _Params )
{
  _This->iPyramid.Generate(_Params,_This->Levels); // Generate the pyramid
  _This->LapList.Resize(_This->Levels);
  _This->LapList.ReleaseBuffer(_This->Levels-1);

  // Reconstruct Image k -> k-1
  for(dm_uint i=_This->Levels-1;i>0;--i) {
     _This->iPyramid.Reconstruct(_Params,i,i-1,_This->LapList);
  } 

  // Do the difference between current nodes and new computed nodes
  // Image at max levels will be simply copied copied (because 
  // of the previous ReleaseBuffer(_This->Levels-1)  )
  // WARNING: SubPixels will do the inverse of it should do
  // assuming that _LapList is the lhs operand !!!

  _This->iMath.DoImageListMath(_Params,dmTk::Math::SubPixels,_This->LapList);

  // At this point we have the laplacien pyramid in the list
}
//--------------------------------------------------------------------
// Reconstruct all nodes image from a laplacien pyramid
// We use the TmpList to expand nodes because :
// - Images nodes should have been already allocated
// - We want to leave ResList untouched in order to continue processing
//   by adding new images.
//
// Result will be set buffer 0 of TmpList
//--------------------------------------------------------------------
static ZINLINE void ZFocus_ReconstructPyramid(  CCI_INSTANCE_PTR(ZFocus) _This,
                                                dmIParameters&  _Params )
{
  // Prerequisite 
  dmDEBUG_ASSERT_VALID_INTERFACE(_This->ResList);
  dmDEBUG_ASSERT_VALID_INTERFACE(_This->TmpList);

  dmDEBUG_ASSERT( _Params.GetImageList() == static_cast<cci_Object*>(_This->TmpList));
  dmDEBUG_ASSERT( _This->TmpList.Size() == _This->ResList.Size() );
  dmDEBUG_ASSERT( _This->TmpList.Size() == _This->Levels );

  zfocus::image_type *src=NULL,*dst=NULL;

  for(dm_uint i=_This->Levels-1;i>0;--i) 
  {
     // Get the previous node source image in Result list
     src = zfocus::cast_image(_This->ResList.GetBuffer(i-1));
     dst = zfocus::cast_image(_This->TmpList.GetBuffer(i-1));
     // dst->Clear();

     // Set the destination image from TmpList as parameter
     _Params.SetImage(dst);
     
     // Expand node in destination image
     // Source image is the image stored at i in the TmpList 
     _This->iPyramid.Reconstruct(_Params,i,i-1,NULL);
         
     // Add two images
     daim::combine(src->Rect(),dmPoint(0,0),src->Gen(),dst->Gen(),
                   std::plus<zfocus::value_type>());
  } 

  if(dst!=NULL) {
    // Normalize the final image
    daim::transform(dst->Gen(),
        std::bind2nd(std::divides<zfocus::value_type>(),
         zfocus::value_type(_This->Levels)));
  }

  // Now the image at 0 in TmpList contains the reconstructed image
}
//--------------------------------------------------------------------
