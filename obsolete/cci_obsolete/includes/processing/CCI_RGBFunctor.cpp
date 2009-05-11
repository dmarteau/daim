
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


#define dmUseCCI
#define dmUseUtilitiesExtra
#define dmUseKernelBuffer
#define dmUseKernelImage
#include "daim_kernel.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Parameters.h"
#include "daim_modules/CCI_ImageList.h"

#include "daim_modules/processing/CCI_Colorspace.h"
#include "daim_modules/processing/CCI_RGBFunctor.h"

//------------------------------------------------------------------------
dmColorSpaceFunctorHelper::~dmColorSpaceFunctorHelper() {}
//------------------------------------------------------------------------
bool dmColorSpaceFunctorHelper::Apply( dmIParameters& _IParams,
                                       dmImageBuffer* buffer, 
                                       dmImage*  img,
                                       dmRegion* rgn,
                                       bool merge )
{
  if(!dmIsPixelFormatScalar(img->PixelFormat())) 
  {
    dmIColorSpace _ColorSpace;
    if(_ColorSpace.QueryInterface(_IParams.GetColorSpace()) && 
       (merge && _ColorSpace.CanMerge())) 
    {
      dmRect   _Rect     = img->Rect();
      dmRegion _SavedRoi = *rgn;
      rgn->SetRectRoi(_Rect);

      dm_uint channel = _IParams.GetChannel();
      if(_ColorSpace.Split(_IParams))
      {
        *rgn = _SavedRoi; // Restore region

        // Apply functor to all channels

        if(channel == dmIParameters::flAllChannels) 
        {
          for(int i=_ColorSpace.NumChannels();--i>=0;) {
            img = _ColorSpace.GetBuffer(i);
            if(img) {
              dmBufferParameters _Params(*buffer,*img,*rgn);
              if(!ApplyFunctor(_Params)) {
                ReturnedError = E_BAD_IMAGE;
                break;
              }
            }
          }
        }
        else
        {
          img = _ColorSpace.GetBuffer(channel-1);
          if(img) {
            dmBufferParameters _Params(*buffer,*img,*rgn);
            if(!ApplyFunctor(_Params))
                ReturnedError = E_BAD_IMAGE;
          }
        }
        rgn->SetRectRoi(_Rect);
        if(ReturnedError==E_NO_ERROR && merge)
          _ColorSpace.Merge(_IParams);

      } else
        ReturnedError =  E_SPLIT_ERROR;              

      *rgn = _SavedRoi;     // Restore region
 
    } else
      ReturnedError = E_BAD_COLORSPACE;
  }
  else 
  {
    dmBufferParameters _Params(*buffer,*img,*rgn);
    if(!ApplyFunctor(_Params))
       ReturnedError = E_BAD_IMAGE;
  }
  
  return ReturnedError == E_NO_ERROR;
}
//------------------------------------------------------------------------
const char_t* dmColorSpaceFunctorHelper::GetErrorText( dm_param err ) 
{
  char_t* txt = NULL;
  switch(err) {
    case dmFunctorErrors::E_BAD_IMAGE : txt = _TXT("Invalid image type for this operation!"); break;
    case dmFunctorErrors::E_NO_IMAGE  : txt = _TXT("Cannot get image !")                    ; break;
    case dmFunctorErrors::E_BAD_PARAM : txt = _TXT("Invalid parameters !")                  ; break;

    case dmFunctorErrors::E_BAD_CHANNEL   : txt = _TXT("Invalid map index !")               ; break;
    case dmFunctorErrors::E_BAD_COLORSPACE: txt = _TXT("Invalid colorspace !")              ; break;
    case dmFunctorErrors::E_SPLIT_ERROR   : txt = _TXT("Cannot split image !")              ; break;
  };
  return txt;  
}
//------------------------------------------------------------------------
 