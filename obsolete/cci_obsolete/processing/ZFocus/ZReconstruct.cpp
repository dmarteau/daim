
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
// Reconstruct all nodes image from a laplacien pyramid
//
// The reconstructed image will be stored in the buffer 0 of 
// the TmpList
//--------------------------------------------------------------------
static ZINLINE void ZFocus_Reconstruct(  CCI_INSTANCE_PTR(ZFocus) _This,
                                         dmIParameters&  _Params, dm_uint channel )
{
  if(_This->Count > 0)
  {

   // ZFocus_ReconstructPyramid use the TmpList as intermediates
   // results for reconstructing the image,
   // indeed we copy the accumulated results into the corresponding
   // buffer in TmpList.
  
    _This->TmpList.StoreBuffer(_This->Levels-1,
                               _This->ResList.GetBuffer(_This->Levels-1));

    // Get the result for the accumulated last node
    // and normalize the content of this buffer

    zfocus::image_type* acc = zfocus::cast_image(_This->TmpList.GetBuffer(_This->Levels-1));
    dmDEBUG_ASSERT( acc != NULL );

    daim::transform(acc->Gen(),
         std::bind2nd(std::divides<zfocus::value_type>(),
                      zfocus::value_type(_This->Count)));

    // Reconstruct the image
    // The reconstructed image is in the buffer 0 of TmpList
    ZFocus_ReconstructPyramid(_This,_Params); 

    // Restore channels
    // In case we have only one channel 
    // The channel image is the same as the buffer 0 in TmpList
    // \see Zfocus_SplitChannels
    if(_This->ColorSpace != CCIV_ZFocus_ColorSpaceGrey) 
    {
      dmDEBUG_ASSERT( channel < _This->Channels.size() );

      // Get the reconstructed image
      zfocus::image_type*   src = zfocus::cast_image(_This->TmpList.GetBuffer(0));
      zfocus::channel_type* dst = dmIImage<CHANNEL_FMT>::Cast(_This->Channels[channel]);
       
      // Convert it to 8 bits
      daim::scale_convert_scalar_to_scalar(
             daim::minmax(src->Rect(),src->Gen()),
             _This->Ranges[channel],
             src->Gen(),dst->Gen(),
             src->Rect(),dst->Rect().TopLeft());

      //_This->Channels[channel]->GetCopy(*_This->TmpList.GetBuffer(0));
    }
  }
}
//--------------------------------------------------------------------
