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

#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciIImageContainer.h"
#include "cciIRegionContainer.h"
#include "cciColorSpaces.h"
#include "cciIImageList.h"

#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "common/dmUserLib.h"
#include "images/dmChannels.h"


#define RGB_FMT dmPixelFormat24bppRGB

#define CHANNEL_8BITS dmPixelFormat8bppIndexed
#define CHANNEL_FLOAT dmPixelFormat32bppFloat

inline dmImage* ColorSpace_GetChannel( dm_uint32 index, cciIImageList* imglist )
{
  return imglist->GetNativeBuffer(index);
}


//--------------------------------------------------------------------
// ColorSpace_Extract helper
//--------------------------------------------------------------------
template<EPixelFormat _PixelFormat>
cci_result inline ColorSpace_Extract( dm_uint32 channel, const dmIImage<RGB_FMT>* _src,
                                      dmIImage<_PixelFormat>* _dst, const dmRegion& rgn )
{
  typedef typename dmIImage<_PixelFormat>::traits_type::integer_type integer_type ;

  dmDEBUG_ASSERT( _src != NULL );
  dmDEBUG_ASSERT( _dst != NULL );

  switch(channel)
  {
    COLORSPACE_EXTRACT_CHANNELS()
  }

  return CCI_ERROR_FAILURE;
}

CCI_USE_NS

class COLORSPACE_CLASSNAME final : public cciIColorSpace
{
  public:
    CCI_DECL_ISUPPORTS
    CCI_DECL_ICOLORSPACE

    COLORSPACE_CLASSNAME();

  private:
    ~COLORSPACE_CLASSNAME();

  protected:
    EPixelFormat mChannelFmt;
    dmRect       mRect;

    cci_Ptr<cciIImageList> mImageList;

    cci_result InitChannels(dm_uint32 _Channels, bool _Split );
    cci_result EnsureImageList();

    cci_result ExtractChannelToImage(dmImage*  _Dst,
                                     dmRegion& rgn,
                                     dmIImage<RGB_FMT>* src,
                                     dm_uint32 channel );

    cci_result SplitImage( const dmImage * image  );

};

// Implementation

CCI_IMPL_ISUPPORTS1(COLORSPACE_CLASSNAME, cciIColorSpace)

cci_result COLORSPACE_CLASSNAME::InitChannels(dm_uint32 _Channels, bool _Split )
{
  CCI_ENSURE_ARG_POINTER( mImageList );

  dmDEBUG_ASSERT( _Channels > 0 );

  cci_result rv;

  if(_Split)
  {
    mImageList->SetCapacity(_Channels);
    for(dm_uint i=0;i<_Channels;++i)
    {
      rv = mImageList->CreateBuffer(i,mChannelFmt,mRect.Width(),mRect.Height());
      if(CCI_FAILED(rv))
         return rv;
    }
    rv = CCI_OK;
  }
  else
  {
    // Convert formats to channels f
    dm_uint32 size;
    mImageList->GetSize(&size);
    if(size < _Channels)
       rv = CCI_ERROR_FAILURE;
    else
       rv = CCI_OK;
  }

  return rv;
}


cci_result COLORSPACE_CLASSNAME::EnsureImageList()
{
  cci_result rv = CCI_OK;

  if(!mImageList)
      mImageList = do_CreateInstance("@daim.org/image/list;1",&rv);

  return rv;
}

cci_result COLORSPACE_CLASSNAME::ExtractChannelToImage(dmImage*  _Dst,
                                                       dmRegion& rgn,
                                                       dmIImage<RGB_FMT>* src,
                                                       dm_uint32 channel )
{
  switch( mChannelFmt )
  {
    case CHANNEL_8BITS: {
      dmIImage<CHANNEL_8BITS>* dst = dmIImage<CHANNEL_8BITS>::Cast(_Dst);
      return ColorSpace_Extract( channel, src, dst, rgn );
    } break;

    case CHANNEL_FLOAT: {
      dmIImage<CHANNEL_FLOAT>* dst = dmIImage<CHANNEL_FLOAT>::Cast(_Dst);
      return ColorSpace_Extract( channel, src, dst, rgn );
    } break;

    default:
      break;
  }

  return CCI_ERROR_FAILURE;
}


COLORSPACE_CLASSNAME::COLORSPACE_CLASSNAME()
: mChannelFmt(CHANNEL_8BITS)
{
}

COLORSPACE_CLASSNAME::~COLORSPACE_CLASSNAME()
{
  /* destructor code */
}

//==============================
// cciIColorSpace
//==============================

/* [noscript,notxpcom] dmImagePtr getChannel (in dm_uint32 channel); */
CCI_IMETHODIMP_(dmImage *) COLORSPACE_CLASSNAME::GetChannel(dm_uint32 channel)
{
  if(mImageList)
     return mImageList->GetNativeBuffer(channel-1);

  return dm_null;
}


/* attribute cciIImageList imageList; */
CCI_IMETHODIMP COLORSPACE_CLASSNAME::GetImageList(cciIImageList * *aImageList)
{
  CCI_ADDREF(*aImageList = mImageList);
  return CCI_OK;
}
CCI_IMETHODIMP COLORSPACE_CLASSNAME::SetImageList(cciIImageList *aImageList)
{
  mImageList = aImageList;
  return CCI_OK;
}


/* void split (in cciImage image, in cciRegion roi); */
CCI_IMETHODIMP COLORSPACE_CLASSNAME::Split(cciImage image, cciRegion rgn)
{
  CCI_ENSURE_ARG_POINTER(image);

  if(CCI_FAILED(EnsureImageList()))
      return CCI_ERROR_FAILURE;

  mRect = rgn ? CCI_NATIVE(rgn)->Rectangle() : CCI_NATIVE(image)->Rect();

  dmIImage<RGB_FMT>* src = dmIImage<RGB_FMT>::Cast(CCI_NATIVE(image));
  if(src!=NULL && CCI_SUCCEEDED(InitChannels(COLORSPACE_NUMCHANNELS,true)))
  {
    if( COLORSPACE_SPLIT(*src,mRect,dmPoint(0,0)) )
      return CCI_OK;
  }

  return CCI_ERROR_FAILURE;
}

/* void merge (in cciImage image); */
CCI_IMETHODIMP COLORSPACE_CLASSNAME::Merge(cciImage image)
{
#ifndef COLORSPACE_NO_MERGE_SUPPORT

  CCI_ENSURE_ARG_POINTER(image);

  if(!mImageList)
      return CCI_ERROR_FAILURE;

  // Rect is supposed to be the rectangle computed in the split function
  // if empty get the offset from the roi

  dmRect  _Rect = mRect;
  dmPoint _Offset;

  if(_Rect.IsEmpty())
     _Rect = CCI_NATIVE(image)->Rect();

  _Offset = _Rect.TopLeft();
  _Rect.Translate(-_Offset);

  dmIImage<RGB_FMT>* src = dmIImage<RGB_FMT>::Cast(CCI_NATIVE(image));
  if(src!=NULL && CCI_SUCCEEDED(InitChannels(COLORSPACE_NUMCHANNELS,false)))
  {
     if( COLORSPACE_MERGE(*src,_Rect,_Offset) )
         return CCI_OK;
  }

  return CCI_ERROR_FAILURE;

#else // COLORSPACE_NO_MERGE_SUPPORT_DEFINED

  return CCI_ERROR_NOT_IMPLEMENTED;

#endif
}

/* attribute EPixelFormat channelFmt; */
CCI_IMETHODIMP COLORSPACE_CLASSNAME::GetChannelFmt(EPixelFormat *aChannelFmt)
{
  *aChannelFmt = mChannelFmt;
  return CCI_OK;
}
CCI_IMETHODIMP COLORSPACE_CLASSNAME::SetChannelFmt(EPixelFormat aChannelFmt)
{
  mChannelFmt = aChannelFmt;
  return CCI_OK;
}

/* readonly attribute boolean canMerge; */
CCI_IMETHODIMP COLORSPACE_CLASSNAME::GetCanMerge(bool *aCanMerge)
{
  *aCanMerge = COLORSPACE_CAN_MERGE;
  return CCI_OK;
}

/* readonly attribute dm_uint32 numChannels; */
CCI_IMETHODIMP COLORSPACE_CLASSNAME::GetNumChannels(dm_uint32 *aNumChannels)
{
  *aNumChannels = COLORSPACE_NUMCHANNELS;
  return CCI_OK;
}

/* void extract (in cciImage image, in cciRegion roi, in dm_uint32 channel, in dm_uint32 index); */
CCI_IMETHODIMP COLORSPACE_CLASSNAME::Extract(cciImage image, cciRegion roi, dm_uint32 channel, dm_uint32 index)
{
  CCI_ENSURE_ARG_POINTER(image);

  if(channel < 1 || channel > COLORSPACE_NUMCHANNELS)
    return CCI_ERROR_INVALID_ARG;

  dmIImage<RGB_FMT>* src = dmIImage<RGB_FMT>::Cast(CCI_NATIVE(image));
  if(src==NULL)
     return CCI_ERROR_INVALID_ARG;

  dmRegion rgn = CCI_NATIVE_ROI(roi,src->Rect());

  if(!CCI_SUCCEEDED(EnsureImageList()))
      return CCI_ERROR_FAILURE;

  cci_result rv = mImageList->CreateBuffer(index,mChannelFmt,
                                    rgn.Rectangle().Width(),
                                    rgn.Rectangle().Height());
  if(CCI_SUCCEEDED(rv))
  {
    dmImage* _Dst = mImageList->GetNativeBuffer(index);
    if(_Dst)
       return ExtractChannelToImage(_Dst,rgn,src,channel);

    rv = CCI_ERROR_FAILURE;
  }

  return rv;
}

/* [noscript] void extractInBuffer (in dmImagePtr image, in dmRegionPtr roi, in dm_uint32 channel, in dmImageBufferPtr buffer); */
CCI_IMETHODIMP COLORSPACE_CLASSNAME::ExtractInBuffer(dmImage *image, dmRegion *roi, dm_uint32 channel,
                                                     dmImageBuffer *buffer)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(buffer);

  if(channel < 1 || channel > COLORSPACE_NUMCHANNELS)
   return CCI_ERROR_INVALID_ARG;

  dmIImage<RGB_FMT>* src = dmIImage<RGB_FMT>::Cast(image);
  if(src==NULL)
    return CCI_ERROR_INVALID_ARG;

  dmRegion rgn = roi ? *roi : src->Rect();

  dmImageDescriptor* _Desc = dmGetDescriptor(static_cast<EPixelFormat>(mChannelFmt));
  if(_Desc!=NULL)
  {
   buffer->CreateBuffer(*_Desc,rgn);
   return ExtractChannelToImage(buffer->Buffer(),rgn,src,channel);
  }

  return CCI_ERROR_FAILURE;
}

/* void setRect (in dmRectRef rect); */
CCI_IMETHODIMP COLORSPACE_CLASSNAME::SetRect(dm_rect * rect)
{
  if(rect)
    mRect = *rect;
  else
    mRect.Clear();

  return CCI_OK;
}

/* void splitColor (inout dm_real q1, inout dm_real q2, inout dm_real q3, inout dm_real q4); */
CCI_IMETHODIMP COLORSPACE_CLASSNAME::SplitColor(dm_real *q1 CCI_INOUTPARAM, dm_real *q2 CCI_INOUTPARAM, dm_real *q3 CCI_INOUTPARAM, dm_real *q4 CCI_INOUTPARAM)
{
  daim::colorspace::rgb_triple rgb;
  dm_uint8 c1,c2,c3;

  rgb.a = 0xff;
  rgb.r = static_cast<dm_uint8>(*q1);
  rgb.g = static_cast<dm_uint8>(*q2);
  rgb.b = static_cast<dm_uint8>(*q3);

  COLORSPACE_NAMESPACE::splitter _Splitter;
  _Splitter( rgb, c1, c2, c3 );

  *q1 = c1;
  *q2 = c2;
  *q3 = c3;

  return CCI_OK;
}

/* void mergeColor (inout dm_real q1, inout dm_real q2, inout dm_real q3, inout dm_real q4); */
CCI_IMETHODIMP COLORSPACE_CLASSNAME::MergeColor(dm_real *q1 CCI_INOUTPARAM, dm_real *q2 CCI_INOUTPARAM, dm_real *q3 CCI_INOUTPARAM, dm_real *q4 CCI_INOUTPARAM)
{
#ifndef COLORSPACE_NO_MERGE_SUPPORT

  daim::colorspace::rgb_triple rgb;
  dm_uint8 c1,c2,c3;

  c1 = static_cast<dm_uint8>(*q1);
  c2 = static_cast<dm_uint8>(*q2);
  c3 = static_cast<dm_uint8>(*q3);

  COLORSPACE_NAMESPACE::merger _Merger;
  _Merger( rgb, c1, c2, c3 );

  *q1 = rgb.r;
  *q2 = rgb.g;
  *q3 = rgb.b;

  return CCI_OK;

#else // COLORSPACE_NO_MERGE_SUPPORT_DEFINED

   return CCI_ERROR_NOT_IMPLEMENTED;

#endif
}

cci_result COLORSPACE_CLASSNAME::SplitImage( const dmImage * image  )
{
  CCI_ENSURE_ARG_POINTER(image);

  mRect = image->Rect();

  if(!CCI_SUCCEEDED(EnsureImageList()))
     return CCI_ERROR_FAILURE;

  dmIImage<RGB_FMT>* src = dmIImage<RGB_FMT>::Cast(image);
  if(src!=NULL && CCI_SUCCEEDED(InitChannels(COLORSPACE_NUMCHANNELS,true))) {
    if( COLORSPACE_SPLIT(*src,mRect,dmPoint(0,0)) )
        return CCI_OK;
  }

  return CCI_ERROR_FAILURE;
}


//========================================
// Module
//========================================

#include "cciIGenericFactory.h"

#define DECLARE_GENERIC_FACTORY(_InstanceName) \
  CCI_GENERIC_FACTORY_CONSTRUCTOR(_InstanceName)

DECLARE_GENERIC_FACTORY(COLORSPACE_CLASSNAME)

static const cciModuleComponentInfo components[] = {
    { DECLARE_COLORSPACE_CLASSNAME(COLORSPACE_NAME),
      DECLARE_COLORSPACE_CID(COLORSPACE_NAME),
      DECLARE_COLORSPACE_CONTRACTID(COLORSPACE_NAME),
      DECLARE_COLORSPACE_CONSTRUCTOR(COLORSPACE_CLASSNAME)
    },
};

CCI_IMPL_GETMODULE(DECLARE_COLORSPACE_MODULE(COLORSPACE_NAME), components)

