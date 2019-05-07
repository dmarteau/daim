
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
#define dmUseKernelImageTemplates
#define dmUseUtilitiesExtra
#define dmUseKernelBuffer
#include "daim_kernel.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Parameters.h"
#include "daim_modules/CCI_ImageList.h"

#include "daim_modules/processing/CCI_Filters.h"
#include "daim_modules/processing/CCI_ZFocus.h"
#include "daim_modules/processing/CCI_KernelFamilies.h"
#include "daim_modules/processing/CCI_ImageShift.h"
#include "daim_modules/processing/CCI_Pyramid.h"
#include "daim_modules/processing/CCI_ImageMath.h"

//------------------------------------------------------------------
#define CCIC_NAME CCIC_ZFocus
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_ZFocus_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"

#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"

#include "common/dmUserLib.h"
#include "operators/dmAccumulator.h"

#include "templates/processing/dmMaskCopy.h"
#include "templates/processing/dmArithmetics.h"
#include "images/dmChannels.h"

#define DEFAULTS_PYRAMID_LEVELS  4

//#define DEFAULTS_PYRAMID_KERNEL  _TXT("Smooth/Gauss5x5-S")
//#define DEFAULTS_PYRAMID_KERNEL  _TXT("Smooth/Gauss5x5")
#define DEFAULTS_PYRAMID_KERNEL  _TXT("Smooth/Gauss(8)")

#define MAP_FMT      dmPixelFormat8bppIndexed
#define RGB_FMT      dmPixelFormat24bppRGB

#define ZFOCUS_FMT  dmPixelFormat32bppFloat
#define CHANNEL_FMT dmPixelFormat8bppIndexed

#define CCIV_ZFocus_ColorSpaceMask 0xFFL 

typedef std::vector< dmIInterface<dmIImageList> > dmChannelsList;

#define _NOINDEX_ dm_noindex

#include "ZFocus.cpp"

//--------------------------------------------------------------------
CCI_DECL_DATA( ZFocus )
{
  dm_uint  Levels;
  dm_uint  Count;
  dm_uint  Options;
  dm_uint  ProcessFlags;
  dm_uint  ColorSpace;
  dm_uint  NumChannels;
  dm_uint  ChannelMap;

  dmString Kernel;

  dmIInterface<dmIImageMath> iMath;
  dmIInterface<dmIPyramid>   iPyramid;

  dmChannelsList ResChannels;
  std::vector< dmLink<dmImage> >       Channels; 
  std::vector<zfocus::channel_range> Ranges; 

  dmIImageList ResList;  // Hold result pyramid

  dmIInterface<dmIImageList> MapList; // Index mapping pyramid
  dmIInterface<dmIImageList> TmpList; // Temporary to hold the Gaussian pyramid
  dmIInterface<dmIImageList> LapList; // Laplacian pyramid
  
  dmRegion RoiRgn;       // The rectangle corresponding to the image size
  dmRect   LastNodeRect; // For storing the last node rectangle
};

#include "ZPyramid.cpp"
#include "ZProcess.cpp"
#include "ZReconstruct.cpp"
#include "ZFuse.cpp"

//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( ZFocus )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( ZFocus)

  CCI_INIT_MEMBER(iMath);
  CCI_INIT_MEMBER(iPyramid);

  CCI_INIT_MEMBER(RoiRgn);
  CCI_INIT_MEMBER(LastNodeRect);

  CCI_INIT_MEMBER(Kernel);

  CCI_INIT_MEMBER(ResList);
  CCI_INIT_MEMBER(MapList);
  CCI_INIT_MEMBER(TmpList);
  CCI_INIT_MEMBER(LapList);

  CCI_INIT_MEMBER(ResChannels);
  CCI_INIT_MEMBER(Channels);
  CCI_INIT_MEMBER(Ranges);

  _This->Kernel.Assign(DEFAULTS_PYRAMID_KERNEL);
  _This->Levels      = DEFAULTS_PYRAMID_LEVELS;
  _This->Count       = 0;
  _This->Options     = 0;
  _This->NumChannels = 0;
  _This->ChannelMap  = 0;

  _This->ColorSpace = CCIV_ZFocus_ColorSpaceGrey;

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( ZFocus )
{
  CCI_INSTANCE( ZFocus )
 
  CCI_DESTROY_MEMBER(Channels);
  CCI_DESTROY_MEMBER(ResChannels);
  CCI_DESTROY_MEMBER(Ranges);

  CCI_DESTROY_MEMBER(RoiRgn)
  CCI_DESTROY_MEMBER(LastNodeRect)

  CCI_DESTROY_MEMBER(TmpList)
  CCI_DESTROY_MEMBER(LapList)
  CCI_DESTROY_MEMBER(ResList)
  CCI_DESTROY_MEMBER(MapList)

  CCI_DESTROY_MEMBER(Kernel);

  CCI_DESTROY_MEMBER(iMath)
  CCI_DESTROY_MEMBER(iPyramid)

  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( ZFocus )
  CCI_ATTR_GET( ZFocus,Kernel     , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->Kernel.CStr())  )
  CCI_ATTR_GET( ZFocus,MapList    , CCIA_RETVAL = dm_ptr_to_long(dmIINTERFACE_PTR(_This->MapList)) )
  CCI_ATTR_GET( ZFocus,Count      , CCIA_RETVAL = _This->Count      )
  CCI_ATTR_GET( ZFocus,Levels     , CCIA_RETVAL = _This->Levels     )
  CCI_ATTR_GET( ZFocus,Options    , CCIA_RETVAL = _This->Options    )
  CCI_ATTR_GET( ZFocus,ColorSpace , CCIA_RETVAL = _This->ColorSpace )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( ZFocus )
  CCI_ATTR_SET( ZFocus,Options , _This->Options = CCIA_DATA )
  CCI_ATTR_GET( ZFocus,Kernel  , _This->Kernel  = CCIA_TRANSLATE_DATA(char_t*))
  CCIA_DISCARD( ZFocus,MapList    )
  CCIA_DISCARD( ZFocus,Count      )
  CCIA_DISCARD( ZFocus,Levels     )
  CCIA_DISCARD( ZFocus,ColorSpace )
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method: Initialize
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ZFocus, Initialize )
{
  CCI_INSTANCE( ZFocus )

  if(CCI_P(levels)==0||CCI_P(levels)==_NOINDEX_)
     _This->Levels = DEFAULTS_PYRAMID_LEVELS;
  else {
    if(CCI_P(levels) > 255)
      CCI_RETURN_INVALID_PARAM()  

    _This->Levels = CCI_P(levels);
  }
 
  _This->Count  = 0;
  _This->iPyramid.SetKernel(_This->Kernel.CStr());
  _This->RoiRgn.KillRoi();
  _This->NumChannels = 1; // Set default to grey

  _This->ColorSpace = CCIV_ZFocus_ColorSpaceMask & CCI_P(flags);

  if(_This->ColorSpace==CCIV_ZFocus_ColorSpaceRGB)
    _This->NumChannels = 3;

   _This->Channels.resize(_This->NumChannels);

  // We cannot do a vector::resize here because
  // of the copy constructor, all instances
  // will share the same cci handle;

  _This->ResChannels.clear();
  _This->Ranges.resize(_This->NumChannels,
      zfocus::channel_type::traits_type::bounds().inverted());

  for(dm_uint i=0;i<_This->NumChannels;++i)
  {
    dmIInterface<dmIImageList> aChannel;
    aChannel.Resize(_This->Levels);
    _This->ResChannels.push_back( aChannel );
  }

  _This->MapList.ReleaseAll();
  _This->MapList.Resize(_This->Levels);
  _This->LastNodeRect.Clear();

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: Process
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ZFocus, Process )
{
  CCI_INSTANCE( ZFocus )
  // Get parameters

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  { 
    dmImage* return_image = ZFocus_GetImageFromParams(_IParams,CCI_P(index));

    dm_uint processed = _This->Count;

    if(_This->NumChannels == 0) {
      CCI_SET_ERROR_MSG(_TXT("Uninitialized object !"));
      CCI_RETURN_FAIL()
    }

    if(_This->RoiRgn.IsEmptyRoi())
      _This->RoiRgn = *_IParams.GetRoi();  // Get the ROI

    _IParams.SetImageList(_This->TmpList); // Set the TmpList

    _This->ProcessFlags = CCI_P(flags);

    if(CCI_P(imagelist)!=NULL) 
    {
      //---------------------------------------------------------
      // Process image list 
      //---------------------------------------------------------
      dmIImageList _IImages;
      if(_IImages.QueryInterface(CCI_P(imagelist))) {
        dm_uint image_count = _IImages.Size();
        for(dm_uint i=0;i<image_count;++i) 
        {
          dmImage* image = _IImages.GetBuffer(i);
          if(Zfocus_SplitChannels(_This,image))
          {
             ++_This->Count;
             for(dm_uint i=0;i<_This->NumChannels;++i) {
               ZFocus_SetChannel(_This,i);
               ZFocus_ProcessImage(_This,_IParams,i,CCI_P(mapindex));
             }
          }
        }
      }
    } else
      if(Zfocus_SplitChannels(_This,return_image)) {
       //---------------------------------------------------------
       // Process single image  
       //---------------------------------------------------------
       ++_This->Count;

       for(dm_uint i=0;i<_This->NumChannels;++i) {
         ZFocus_SetChannel(_This,i);
         ZFocus_ProcessImage(_This,_IParams,i,CCI_P(mapindex)); 
       }
    }    
 
    //---------------------------------------------------------
    // Image reconstruction
    //---------------------------------------------------------
    if(_This->ProcessFlags & CCIV_ZFocus_Finalize) 
    {
       for(dm_uint i=0;i<_This->NumChannels;++i) {
         ZFocus_SetChannel(_This,i);
         ZFocus_Reconstruct(_This,_IParams,i);
       }

       if(_This->ProcessFlags & CCIV_ZFocus_CopyBack) 
       {
         if(return_image!=NULL)
           Zfocus_MergeChannels(_This,return_image);
         else 
           dmLOG(_TXT("WARNING: CCIV_ZFocus_CopyBack specified but no image available !\n"));
       } 
    }

    _IParams.SetImageList(NULL);
    _IParams.SetImage(NULL);

    if(_This->Count > processed)     // Check processing occured
      CCI_RETURN_OK()

  } else 
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ZFocus, Reconstruct )
{
  CCI_INSTANCE( ZFocus )

  // Get parameters

  dmIParameters _IParams;
  if(_This->Count>0 && _IParams.QueryInterface(CCI_P(params)))
  {  
    for(dm_uint i=0;i<_This->NumChannels;++i) {
      ZFocus_SetChannel(_This,i);
      ZFocus_Reconstruct(_This,_IParams,i);
    }

    dmImage* image = ZFocus_GetImageFromParams(_IParams,CCI_P(index));
    if(image!=NULL) 
    {
      if(Zfocus_MergeChannels(_This,image))
        CCI_RETURN_OK()
    }
  } else 
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));

  CCI_RETURN_FAIL()

}
//---------------------------------------------------------------------
// Method: Reset
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( ZFocus, Reset )
{
  CCI_INSTANCE( ZFocus )
 
  _This->Count = 0;

  dmImage* dst;

  dm_uint nChannels = _This->ResChannels.size(); 
  // Clear data in Result List
  for(dm_uint c=0;c<nChannels;++c)
  {
    _This->Ranges[c] = zfocus::channel_type::traits_type::bounds().inverted();
    for(dm_int i=_This->Levels;--i>=0;) {
      dst = _This->ResChannels[c].GetBuffer(i);
      if(dst!=NULL)
        dst->Clear();
    }
  } 
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: GetImage
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ZFocus, GetImage )
{
  CCI_INSTANCE( ZFocus )
 
  if(CCI_P(channel) < _This->NumChannels)
  {
    dmImage* img = _This->Channels[CCI_P(channel)];
    if(img) {
      CCI_RETVAL_P(image) = img;
      CCI_RETURN_OK()
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetResList
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ZFocus, GetResList )
{
  CCI_INSTANCE( ZFocus )
 
  if(CCI_P(channel) < _This->ResChannels.size())
  {
    cci_Object* imagelist = _This->ResChannels[CCI_P(channel)];
    if(imagelist!=NULL) {
      CCI_RETVAL_P(imagelist) = imagelist;
      CCI_RETURN_OK()
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( ZFocus )
  CCI_REGISTER_METHOD_PP( ZFocus, Initialize  )
  CCI_REGISTER_METHOD_PP( ZFocus, Process     )
  CCI_REGISTER_METHOD_PP( ZFocus, Reconstruct )
  CCI_REGISTER_METHOD_NP( ZFocus, Reset )
  CCI_REGISTER_METHOD_PP( ZFocus, GetImage   )
  CCI_REGISTER_METHOD_PP( ZFocus, GetResList )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( ZFocus )
{
  CCI_INVOKE_FACTORY(ZFocus)
}
//---------------------------------------------------------------------
