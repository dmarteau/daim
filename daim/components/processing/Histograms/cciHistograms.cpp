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

//--------------------------------------------------------
// File         : cciHistograms.cpp
// Date         : 24 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciCOMPtr.h"
#include "cciInterfaceRequestorUtils.h"
#include "cciIFilterContext.h"
#include "cciIColorSpace.h"
#include "cciIHistograms.h"
#include "cciImageUtils.h"
#include "cciIRegionContainer.h"
#include "cciIImageContainer.h"

#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "templates/processing/dmColorHistogram.h"

#include "common/dmUserLib.h"

/* Header file */
class cciHistograms final : public cciIHistograms
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IHISTOGRAMS

  cciHistograms();

private:
  ~cciHistograms();

protected:
  dm_uint          mScanFlags;
  dmColorHistogram mHisto;

  cci_result ScanScalarImage(dmImage&, dmRegion&, dm_uint32 index,
                             dm_real& minRange, dm_real& maxRange);
};


#define DFLT_NUM_CHANNELS 3

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciHistograms, cciIHistograms)

cciHistograms::cciHistograms()
: mScanFlags(AUTORANGE)

{
  mHisto.SetChannels(DFLT_NUM_CHANNELS);
}

cciHistograms::~cciHistograms()
{
  /* destructor code */
}

cci_result cciHistograms::ScanScalarImage(dmImage& image, dmRegion& rgn, dm_uint32 index,
                                          dm_real& minRange, dm_real& maxRange)
{
  // Get histogram at specified Index
  dmHistogram* _Histogram = mHisto[index];
  if(_Histogram==NULL) {
    dmLOG_ERROR("Invalid histogram index !");
    return CCI_ERROR_INVALID_ARG;
  }

  dmDensityMap _Filter(*_Histogram,minRange,maxRange,
                      (mScanFlags & AUTORANGE)!=0 || minRange >= maxRange);

  if(!_Filter.Apply(image,rgn))
     return CCI_ERROR_FAILURE;
  else {
    minRange = _Filter._MinRange;
    maxRange = _Filter._MaxRange;
  }

  return CCI_OK;
}

//====================================
// cciIHistograms
//====================================

/* attribute unsigned long scanFlags; */
CCI_IMETHODIMP cciHistograms::GetScanFlags(dm_uint32 *aScanFlags)
{
  *aScanFlags = mScanFlags;
  return CCI_OK;
}
CCI_IMETHODIMP cciHistograms::SetScanFlags(dm_uint32 aScanFlags)
{
  mScanFlags = aScanFlags;
  return CCI_OK;
}

/* attribute unsigned long numChannels; */
CCI_IMETHODIMP cciHistograms::GetNumChannels(dm_uint32 *aNumChannels)
{
  *aNumChannels = mHisto.GetChannels();
  return CCI_OK;
}
CCI_IMETHODIMP cciHistograms::SetNumChannels(dm_uint32 aNumChannels)
{
  mHisto.SetChannels(aNumChannels);
  return CCI_OK;
}

/* void scan (in cciImage image, in cciRegion roi, in unsigned long index, in cciISupports filterContext, in cciIColorSpace colorspace, in dm_uint32 channel); */
CCI_IMETHODIMP cciHistograms::Scan(cciImage image, cciRegion roi, dm_uint32 index, cciIFilterContext *filterContext,
                                   cciIColorSpace *colorspace, dm_uint32 channel)
{
  CCI_ENSURE_ARG_POINTER(image);

  dmLink<dmImage> img = dmNewLink<dmImage>(CCI_NATIVE(image));

  dmRegion rgn = roi ? *CCI_NATIVE(roi) : img->Rect();

  if(!dmIsPixelFormatScalar(img->PixelFormat()))
  {
    CCI_ENSURE_ARG_POINTER(filterContext);
    CCI_ENSURE_ARG_POINTER(colorspace);

    dmImageBuffer* ctxtBuffer = filterContext->NativeBuffer();
    cci_result rv = colorspace->ExtractInBuffer(img,&rgn,channel,ctxtBuffer);
    if(CCI_FAILED(rv))
       return rv;

    img = ctxtBuffer->Buffer();
    rgn = ctxtBuffer->BufferRgn();
  }

  //------------------------------------------
  // Compute histogram on scalar
  //------------------------------------------
  dm_real minRange = 0, maxRange = 0;

  if(filterContext) {
     filterContext->GetMinRange(&minRange);
     filterContext->GetMaxRange(&minRange);
  }

  cci_result rv = ScanScalarImage(*img,rgn,index,minRange,maxRange);

  if(filterContext && (mScanFlags & AUTORANGE)!=0)
     filterContext->SetRange(minRange,maxRange);

  return rv;
}

/* void getHistogram (in unsigned long index, [array, size_is (count)] inout unsigned long data, in unsigned long count); */
CCI_IMETHODIMP cciHistograms::GetHistogram(dm_uint32 index, dm_uint32 **data CCI_INOUTPARAM, dm_uint32 count)
{
  CCI_ENSURE_ARG_POINTER(*data);
  CCI_ENSURE_ARG_MIN(count, daim::histogram_size);

  dmHistogram* _Histogram = mHisto[index];
  if(_Histogram!=NULL)
  {
    dm_uint32* out = *data;
    dm_uint32* in  = _Histogram->Data();
    for(int i=0;i<daim::histogram_size;++i)
        out[i] = in[i];

    return CCI_OK;
  }
  return CCI_ERROR_FAILURE;
}

/* void getDensityBins (in cciIFilterContext filterContext, [array, size_is (count)] inout dm_real data, in unsigned long count); */
CCI_IMETHODIMP cciHistograms::GetDensityBins(cciIFilterContext *filterContext, dm_real **data CCI_INOUTPARAM, dm_uint32 count)
{
  CCI_ENSURE_ARG_POINTER(*data);
  CCI_ENSURE_ARG_MIN(count, daim::histogram_size);

  dm_real minRange = 0,maxRange= 0;

  // Get range from filter context
  if(filterContext) {
    filterContext->GetMinRange(&minRange);
    filterContext->GetMaxRange(&maxRange);
  }

  if(minRange >= maxRange) {
    minRange = 0;
    maxRange = 255;
  }

  dm_real fminrange = minRange;
  dm_real fDelta    = ((maxRange-minRange)/(dmHistogram::hsize-1));
  for(int index=0;index<dmHistogram::hsize;++index)
     (*data)[index] = fminrange + fDelta * index;

  return CCI_OK;
}


/* dm_real getBimodalThreshold (in unsigned long index, in dm_real background, in cciISupports filterContext); */
CCI_IMETHODIMP cciHistograms::GetBimodalThreshold(dm_uint32 index, dm_real background, cciIFilterContext *filterContext,
                                                  dm_real *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(_retval);

  dm_real minRange = 0,maxRange= 0;

  if(filterContext) {
     filterContext->GetMinRange(&minRange);
     filterContext->GetMaxRange(&maxRange);
  }

  dmHistogram* _Histogram = NULL;

  // Get histogram at specified Index
  _Histogram = mHisto[index];
  if(_Histogram==NULL)
     return CCI_ERROR_INVALID_ARG;

  dm_real  hmax = static_cast<dm_real>(dmHistogram::hmax);
  dm_uint8 thr;

  if(maxRange > minRange)
  {
    if(background >= maxRange)
      thr = dmHistogram::hmax;
    else if( background <= minRange )
      thr = 0;
    else
      thr = static_cast<dm_uint8>( hmax*(background-minRange)/(maxRange - minRange) + 0.5 );
  }
  else
    thr = daim::clamp(background,0.,hmax);

  daim::histogram_value val = daim::bimodal_optimal_thrindex(_Histogram->Data(),thr);

  if(maxRange > minRange)
    *_retval = minRange + ((maxRange-minRange)/hmax) * val;
  else
    *_retval = val;

  return CCI_OK;
}

/* [noscript,notxpcom] dmHistogramPtr getNativeHistogram (in unsigned long index); */
CCI_IMETHODIMP_(dmHistogram *) cciHistograms::GetNativeHistogram(dm_uint32 index)
{
  return mHisto[index];
}

//=====================================
// Module
//=====================================

#include "cciIGenericFactory.h"

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciHistograms)

static const cciModuleComponentInfo components[] = {
    { CCI_HISTOGRAMS_CLASSNAME,
      CCI_HISTOGRAMS_CID,
      CCI_HISTOGRAMS_CONTRACTID,
      cciHistogramsConstructor
    },
};

CCI_IMPL_GETMODULE(HistogramsModule, components)

