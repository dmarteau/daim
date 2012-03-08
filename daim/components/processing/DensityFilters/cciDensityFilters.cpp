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
                                         /* optional */ cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);

  dmImage* srcImage = CCI_NATIVE(image);
  
  dmRegion rgn = CCI_NATIVE_ROI(roi,srcImage->Rect());

  dm_real minRange,maxRange;

  if(filterCtxt)
  {
    filterCtxt->GetMinRange(&minRange);
    filterCtxt->GetMaxRange(&maxRange);
  }
  else
  {
    if(srcImage->PixelFormat()==dmPixelFormat8bppIndexed) {
      minRange = 0.0;
      maxRange = 255.0;
    }
    else
    {
      dmLOG_ERROR("cciDensityFilters::Extend : filter context required");
      return CCI_ERROR_ILLEGAL_VALUE;
    }
  }
  
  dmExtendMap _Filter(static_cast<dm_real>(rmin),
                      static_cast<dm_real>(rmax),
                      minRange,maxRange);

  if(!_Filter.Apply(*srcImage,rgn))
     return CCI_ERROR_FAILURE;
  
  return CCI_OK;
}

/* void transform (in cciImage image, in cciRegion roi, [array, size_is (count)] in dm_uint32 data, in dm_uint32 count, in cciISupports context); */
CCI_IMETHODIMP cciDensityFilters::Transform(cciImage image, cciRegion roi, dm_uint32 *data, dm_uint32 count, 
                                            /* optional */ cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(data);

  if(count < dmLUT8_MAX_COLORS)
    return CCI_ERROR_INVALID_ARG;

  dmColorIndexTable ctable;
  unsigned short imap;
  dm_uint32*        values = data;
  
  for(int i=0;i<dmLUT8_MAX_COLORS;++i) 
  {
    imap = static_cast<unsigned short>(values[i]);
    if(imap>dmLUT8_MAX_COLOR_INDEX) imap = dmLUT8_MAX_COLOR_INDEX;
    ctable[i]  = imap;
  }

  dm_real minRange = 0,maxRange = 0;
  
  if(filterCtxt) 
  { 
    filterCtxt->GetMinRange(&minRange);
    filterCtxt->GetMaxRange(&maxRange);
  }
  
  dmRegion rgn = CCI_NATIVE_ROI(roi,CCI_NATIVE(image)->Rect());

  dmTransformMap _Filter(ctable,minRange,maxRange);

  if(!_Filter.Apply(*CCI_NATIVE(image),rgn))
      return CCI_ERROR_FAILURE;
  
  return CCI_OK;
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

   bool Apply( dmImage& _image, const dmRegion& _region )
   {
     if(dmDensityMap::Apply(_image,_region)) {
       this->_Histogram.Equalize( this->_Map );
       return dmTransformMap::Apply(_image,_region);
     }
     return false;
   }
};

/* void equalize (in cciImage image, in cciRegion roi, in cciISupports context); */
CCI_IMETHODIMP cciDensityFilters::Equalize(cciImage image, cciRegion roi, 
                                           /* optional */ cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);

  dmHistogram   Histogram;
  dmColorIndex  Cmap[dmHistogram::hsize];

  dm_real minRange=0,maxRange=0;
  
  if(filterCtxt)
  {
    filterCtxt->GetMinRange(&minRange);
    filterCtxt->GetMaxRange(&maxRange);
  }
  
  dmRegion rgn = CCI_NATIVE_ROI(roi,CCI_NATIVE(image)->Rect());

  bool autoscan = maxRange <= minRange;
  dmEqualize _Filter(Histogram,Cmap,minRange,maxRange,autoscan);

  if(!_Filter.Apply(*CCI_NATIVE(image),rgn))
    return CCI_ERROR_FAILURE;
  
  return CCI_OK;
}


/* void gammaCorrection (in cciImage image, in cciRegion roi, in dm_real gamma, in cciISupports context); */
CCI_IMETHODIMP cciDensityFilters::GammaCorrection(cciImage image, cciRegion roi, dm_real gamma,
                                                  /* optional */ cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);

  dm_real minRange=0,maxRange=0;
  if(filterCtxt)
  { 
    filterCtxt->GetMinRange(&minRange);
    filterCtxt->GetMaxRange(&maxRange);
  }
  
  dmRegion rgn = CCI_NATIVE_ROI(roi,CCI_NATIVE(image)->Rect());

  dmGammaCorrection _Filter(minRange,maxRange,gamma);

  dmBufferParameters _params(*filterCtxt->NativeBuffer(),*CCI_NATIVE(image),rgn);
  if(_Filter.Apply(*CCI_NATIVE(image),rgn))
     return CCI_OK;

  return CCI_ERROR_FAILURE;
}

/* void brightnessContrast (in cciImage image, in cciRegion roi, in dm_real brightness, in dm_real contrast, [optional] in cciISupports context); */
CCI_IMETHODIMP cciDensityFilters::BrightnessContrast(cciImage image, cciRegion roi,
                                                     dm_real brightness, dm_real contrast,
                                                     /* optional */ cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);

  dmRegion rgn = CCI_NATIVE_ROI(roi,CCI_NATIVE(image)->Rect());
  
  dm_real minRange = 0,maxRange = 0;

  if(filterCtxt)
  {
    filterCtxt->GetMinRange(&minRange);
    filterCtxt->GetMaxRange(&maxRange);    
    
    dmEnhanceContrast _Filter(minRange,maxRange,brightness,contrast);
    dmBufferParameters _params(*filterCtxt->NativeBuffer(),*CCI_NATIVE(image),rgn);
    if(_Filter.Apply(_params))
       return CCI_OK;
  }
  else
  {
    dmEnhanceContrast _Filter(0,0,brightness,contrast);
    if(_Filter.Apply(*CCI_NATIVE(image),rgn))
       return CCI_OK;
  }

  return CCI_ERROR_FAILURE;
}

/* void scale (in cciImage image, in cciRegion roi, in dm_real minval, in dm_real maxval); */
CCI_IMETHODIMP cciDensityFilters::Scale(cciImage image, cciRegion roi, dm_real minval, dm_real maxval)
{
  CCI_ENSURE_ARG_POINTER(image);

  dmRegion rgn = CCI_NATIVE_ROI(roi,CCI_NATIVE(image)->Rect());
  
  return dmScaleImage(*CCI_NATIVE(image),rgn,minval,maxval)
       ? CCI_OK 
       : CCI_ERROR_FAILURE;
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

   bool Apply( dmImage& _image, const dmRegion& _region )
   {
     // Get histogram from image
     if(dmDensityMap::Apply(_image,_region))
     {
       this->_Histogram.Specify( this->_Map, _Input );
       return dmTransformMap::Apply(_image,_region);
     }
     return false;
   }
};

/* void specify (in cciImage image, in cciRegion roi, in cciIHistograms histograms, in dm_uint32 index, in cciISupports context); */
CCI_IMETHODIMP cciDensityFilters::Specify(cciImage image, cciRegion roi, cciIHistograms *histograms, dm_uint32 index,
                                          /* optional */ cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(histograms);

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

  dm_real minRange=0,maxRange=0;
  
  if(filterCtxt)
  {
    filterCtxt->GetMaxRange(&minRange);
    filterCtxt->GetMaxRange(&maxRange);
  }
  
  bool autoscan = maxRange <= minRange;

  dmRegion rgn = CCI_NATIVE_ROI(roi,CCI_NATIVE(image)->Rect());

  dmSpecifyMap _Filter(Histogram,Cmap,Inpt,minRange,maxRange,autoscan);

  if(!_Filter.Apply(*CCI_NATIVE(image),rgn))
    return CCI_ERROR_FAILURE;
      
  return CCI_OK;
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


