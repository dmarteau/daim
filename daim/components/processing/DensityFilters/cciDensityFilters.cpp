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
// File         : cciDensityFilters.cpp
// Date         : 24 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciCOMPtr.h"
#include "cciInterfaceRequestorUtils.h"
#include "cciIFilterContext.h"
#include "cciIHistograms.h"
#include "cciIDensityFilters.h"
#include "cciIRegionContainer.h"
#include "cciIImageContainer.h"

#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "common/dmUserLib.h"

/* Header file */
class cciDensityFilters : public cciIDensityFilters
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IDENSITYFILTERS

  cciDensityFilters();

private:
  ~cciDensityFilters();

protected:
};

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciDensityFilters, cciIDensityFilters)

cciDensityFilters::cciDensityFilters()
{
  /* member initializers and constructor code */
}

cciDensityFilters::~cciDensityFilters()
{
  /* destructor code */
}

//==================================
// cciIDensityFilters
//==================================

/* void extend (in cciImage image, in cciRegion roi, in dm_real rmin, in dm_real rmax, in cciISupports context); */
CCI_IMETHODIMP cciDensityFilters::Extend(cciImage image, cciRegion roi, dm_real rmin, dm_real rmax,
                                         cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  dmRegion rgn = roi ? *CCI_NATIVE(roi) : CCI_NATIVE(image)->Rect();

  dm_real minRange,maxRange;
  filterCtxt->GetMinRange(&minRange);
  filterCtxt->GetMaxRange(&maxRange);

  dmExtendMap _Filter(static_cast<dm_real>(rmin),
                      static_cast<dm_real>(rmax),
                      minRange,maxRange);

  return dmApplyFilter(_Filter,*filterCtxt->NativeBuffer(),*CCI_NATIVE(image),rgn,true)?
         CCI_OK : CCI_ERROR_FAILURE;
}

/* void transform (in cciImage image, in cciRegion roi, [array, size_is (count)] in dm_uint32 data, in dm_uint32 count, in cciISupports context); */
CCI_IMETHODIMP cciDensityFilters::Transform(cciImage image, cciRegion roi, dm_uint32 *data,
                                            dm_uint32 count, cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(data);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  if(count < dmLUT8_MAX_COLORS)
    return CCI_ERROR_INVALID_ARG;

  dmColorIndexTable ctable;
  unsigned short imap;
  dm_uint*        values = data;
  for(int i=0;i<dmLUT8_MAX_COLORS;++i) {
    imap = static_cast<unsigned short>(values[i]);
    if(imap>dmLUT8_MAX_COLOR_INDEX) imap = dmLUT8_MAX_COLOR_INDEX;
    ctable[i]  = imap;
  }

  dm_real minRange,maxRange;
  filterCtxt->GetMinRange(&minRange);
  filterCtxt->GetMaxRange(&maxRange);

  dmRegion rgn = roi ? *CCI_NATIVE(roi) : CCI_NATIVE(image)->Rect();

  dmTransformMap _Filter(ctable,minRange,maxRange);

  return dmApplyFilter(_Filter,*filterCtxt->NativeBuffer(),*CCI_NATIVE(image),rgn,true)?
         CCI_OK : CCI_ERROR_FAILURE;
}


//---------------------------------------------
// Define a specific functor for equalization
//---------------------------------------------
class dmEqualize : public dmDensityMap,
                   public dmTransformMap
{
  public:
   dmEqualize( dmHistogram&    anHistogram,
               dmColorMapArray aMap,
               dm_real&        minRange,
               dm_real&        maxRange,
               bool            autoScan )
   :dmDensityMap(anHistogram,minRange,maxRange,autoScan)
   ,dmTransformMap(aMap,minRange,maxRange) {}

   bool Apply( dmBufferParameters& _Params )
   {
     if(dmDensityMap::Apply(_Params)) {
       this->_Histogram.Equalize( this->_Map );
       return dmTransformMap::Apply(_Params);
     }
     return false;
   }
};

/* void equalize (in cciImage image, in cciRegion roi, in cciISupports context); */
CCI_IMETHODIMP cciDensityFilters::Equalize(cciImage image, cciRegion roi, cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  dmHistogram   Histogram;
  dmColorIndex  Cmap[dmHistogram::hsize];

  dm_real minRange,maxRange;
  filterCtxt->GetMinRange(&minRange);
  filterCtxt->GetMaxRange(&maxRange);

  dmRegion rgn = roi ? *CCI_NATIVE(roi) : CCI_NATIVE(image)->Rect();

  bool autoscan = maxRange <= minRange;
  dmEqualize _Filter(Histogram,Cmap,minRange,maxRange,autoscan);

  return dmApplyFilter(_Filter,*filterCtxt->NativeBuffer(),*CCI_NATIVE(image),rgn,true)?
         CCI_OK : CCI_ERROR_FAILURE;
}


/* void gammaCorrection (in cciImage image, in cciRegion roi, in dm_real gamma, in cciISupports context); */
CCI_IMETHODIMP cciDensityFilters::GammaCorrection(cciImage image, cciRegion roi, dm_real gamma,
                                                  cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  dm_real minRange,maxRange;
  filterCtxt->GetMinRange(&minRange);
  filterCtxt->GetMaxRange(&maxRange);

  dmRegion rgn = roi ? *CCI_NATIVE(roi) : CCI_NATIVE(image)->Rect();

  dmGammaCorrection _Filter(minRange,maxRange,gamma);

  dmBufferParameters _params(*filterCtxt->NativeBuffer(),*CCI_NATIVE(image),rgn);
  if(_Filter.Apply(_params))
     return CCI_OK;

  return CCI_ERROR_FAILURE;
}

/* void brightnessContrast (in cciImage image, in cciRegion roi, in dm_real brightness, in dm_real contrast, in boolean useContextBuffer, in cciISupports context); */
CCI_IMETHODIMP cciDensityFilters::BrightnessContrast(cciImage image, cciRegion roi,
                                                     dm_real brightness, dm_real contrast,
                                                     dm_bool useContextBuffer,
                                                     cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  dm_real minRange = 0,maxRange = 0;
  filterCtxt->GetMaxRange(&minRange);
  filterCtxt->GetMaxRange(&maxRange);

  dmRegion rgn = roi ? *CCI_NATIVE(roi) : CCI_NATIVE(image)->Rect();

  dmEnhanceContrast _Filter(minRange,maxRange,brightness,contrast,useContextBuffer);

  dmBufferParameters _params(*filterCtxt->NativeBuffer(),*CCI_NATIVE(image),rgn);
  if(_Filter.Apply(_params))
     return CCI_OK;

  return CCI_ERROR_FAILURE;
}

//---------------------------------------------------------------------
// Define the following functor so that we can use the
// colorspace separation functor
//---------------------------------------------
class dmScaleFunctor
{
  dm_real Min;
  dm_real Max;

  public:

  dmScaleFunctor( dm_real _Min, dm_real _Max )
  : Min(_Min)
  , Max(_Max) {}

  bool Apply( dmBufferParameters& _Params ) {
     return dmScaleImage( _Params, this->Min, this->Max );
  }
};

/* void scale (in cciImage image, in cciRegion roi, in dm_real minval, in dm_real maxval, in cciISupports context); */
CCI_IMETHODIMP cciDensityFilters::Scale(cciImage image, cciRegion roi, dm_real minval, dm_real maxval,
                                        cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  dmRegion rgn = roi ? *CCI_NATIVE(roi) : CCI_NATIVE(image)->Rect();

  dmScaleFunctor _Filter(minval,maxval);

  return dmApplyFilter(_Filter,*filterCtxt->NativeBuffer(),*CCI_NATIVE(image),rgn,true)?
         CCI_OK : CCI_ERROR_FAILURE;
}


//---------------------------------------------
// Define a specific functor for histogram
// specification
//---------------------------------------------
class dmSpecifyMap : public dmDensityMap,
                     public dmTransformMap
{
  public:
   dmColorMapArray _Input;
   dmSpecifyMap( dmHistogram&    anHistogram,
                 dmColorMapArray aMap,
                 dmColorMapArray anInput,
                 dm_real&        minRange,
                 dm_real&        maxRange,
                 bool            autoScan )
   :dmDensityMap(anHistogram,minRange,maxRange,autoScan)
   ,dmTransformMap(aMap,minRange,maxRange)
   ,_Input(anInput)
   {}

   bool Apply( dmBufferParameters& _Params )
   {
     // Get histogram from image
     if(dmDensityMap::Apply(_Params))
     {
       this->_Histogram.Specify( this->_Map, _Input );
       return dmTransformMap::Apply(_Params);
     }
     return false;
   }
};

/* void specify (in cciImage image, in cciRegion roi, in cciIHistograms histograms, in dm_uint32 index, in cciISupports context); */
CCI_IMETHODIMP cciDensityFilters::Specify(cciImage image, cciRegion roi, cciIHistograms *histograms, dm_uint32 index,
                                          cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(histograms);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  // Get histogram at specified Index
  dmHistogram* _Histogram = histograms->GetNativeHistogram(index);
  if(_Histogram==NULL) {
     dmLOG_ERROR("Invalid histogram index !");
     return CCI_ERROR_INVALID_ARG;
  }

  dmHistogram   Histogram;
  dmColorIndex  Cmap[dmHistogram::hsize];
  dmColorIndex  Inpt[dmHistogram::hsize];

  // Compute equalized input transformation
  _Histogram->Equalize(Inpt);

  dm_real minRange,maxRange;
  filterCtxt->GetMaxRange(&minRange);
  filterCtxt->GetMaxRange(&maxRange);

  bool autoscan = maxRange <= minRange;
  dm_real rmin  = minRange;
  dm_real rmax  = maxRange;

  dmRegion rgn = roi ? *CCI_NATIVE(roi) : CCI_NATIVE(image)->Rect();

  dmSpecifyMap _Filter(Histogram,Cmap,Inpt,rmin,rmax,autoscan);

  return dmApplyFilter(_Filter,*filterCtxt->NativeBuffer(),*CCI_NATIVE(image),rgn,true)?
         CCI_OK : CCI_ERROR_FAILURE;
}

//=====================================
// Module
//=====================================

#include "cciIGenericFactory.h"

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciDensityFilters)

static const cciModuleComponentInfo components[] = {
    {  CCI_DENSITYFILTERS_CLASSNAME,
       CCI_DENSITYFILTERS_CID,
       CCI_DENSITYFILTERS_CONTRACTID,
       cciDensityFiltersConstructor
    },
};

CCI_IMPL_GETMODULE(DensityFiltersModule, components)


