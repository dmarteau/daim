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
// File         : cciMeasurements.cpp
// Date         : 14 févr. 2009
// Author       : David Marteau
//--------------------------------------------------------

#include "cciMeasures.h"

#define IMAGEMAP_VERIFY_LABEL( lbl ) \
DM_BEGIN_MACRO \
  if(lbl >= mNodePartition.size()) { \
    dmLOG_ERROR("Label is out of range");\
    return CCI_ERROR_FAILURE;\
  }\
DM_END_MACRO

#define IMAGEMAP_ENSURE_BUILT() \
  CCI_ENSURE_TRUE(mBuilt,CCI_ERROR_NOT_INITIALIZED)


/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciMeasurements, cciIMeasurements)

cciMeasurements::cciMeasurements()
:mUpdate(0)
,mNumPartitions(0)
,mDepth(0)
,mBuilt(dm_false)
,use_image_calibration(dm_false)
,aspect_ratio(1.0)
,uppxls(1.0)
{  
}

cciMeasurements::~cciMeasurements()
{
}

//=================================
// cciIMeasures implementation
//=================================

/* void build (in cciImage image, in cciRegion rgn, in dm_uint32 connect, in dm_uint32 flags); */
CCI_IMETHODIMP cciMeasurements::Build(cciImage image, cciRegion rgn, dm_uint32 connect, dm_uint32 flags)
{
  CCI_ENSURE_ARG_POINTER(rgn);
  
  dmRegion* roi = CCI_NATIVE(rgn);
  dmImage*  img = CCI_NATIVE(image);

  if(!roi || roi->IsEmptyRoi())
     return CCI_ERROR_INVALID_ARG;
  
  mRoi           = *roi;
  mUpdate        = 0;
  mNumPartitions = 0;
  mDepth         = 0;
  mBuilt         = dm_false;

  dmRect rect;

  if(img)
  {
    //---------------------------------
    // Get Image calibration if needed
    //---------------------------------
    if(use_image_calibration && img->HasUnits())
    {
      dmConstLink<dmSpatialUnits> units = img->Units();
      aspect_ratio = units->mAspectRatio;
      uppxls       = units->mUnitsPerPixel;
    }

    //---------------------------------
    // Region will be clipped by the image rectangle
    //---------------------------------
    if((flags & CCIV_ImageMap_NoClip)==0) {
      rect = img->Rect();
      mRoi.ClipToRect(rect);
    }
  }

  //---------------------------------
  // Otherwise region is clipped by
  // its own bounding box
  //---------------------------------
  if(rect.IsEmpty())
  {
    rect = mRoi.Rectangle();

    if((flags & CCIV_ImageMap_OffsetRoi)!=0)
    {
      mOffset = rect.TopLeft();
      mRoi.Translate(-rect.Left(),-rect.Top());
      rect = mRoi.Rectangle();
    } else {
      mOffset = dmPoint(0,0);
      rect.Resize(rect.Left(),rect.Top(),0,0);
    }
  }

  //-----------------------------------
  // Build Partition Map
  //-----------------------------------
  mMap.reserve(rect);
  mConnect = (connect==8?daim::connect8:daim::connect4);

  daim::create_region_partition(mNodePartition,
                                mMap,
                                mRoi,
                                mConnect);

  //--------------------------------------
  // Assign empty borders regions to 0
  //--------------------------------------
  tmpRoi.SetRectRoi(rect);
  tmpRoi.XorRoi(mRoi); // Get Complementary regions

  SHRINK_RECT(rect)

  tmpRoi.SubCoordinates(rect);
  mLabels.clear();

  if(!tmpRoi.IsEmptyRoi()) {   // Merge borders regions to 0
    daim::get_overlapping_regions_labels(tmpRoi,mNodePartition,mMap,mLabels);
    daim::merge_labels(mNodePartition,mLabels,0);
  }

  int nPartitions = mNodePartition.get_index_table(mNodeIndexTable);
  if( nPartitions>0 )
  {
    mNodeTable.Clear();
    mNodeTable.Resize( nPartitions );
    if(CreateNodes()) {
      mBuilt = dm_true;
      return CCI_OK;
    }
  }
  
  return CCI_ERROR_FAILURE;
}

//---------------------------------------------------------------------
// Method : CreateRegion
//
// if label==-1  : Background region
// if label== 0  : Objects region
// if label== x  : Get region from its label <x>
//---------------------------------------------------------------------

/* void createRegion (in cciRegion mask, in dm_int32 label, in boolean include_holes); */
CCI_IMETHODIMP 
cciMeasurements::CreateRegion(cciRegion mask, dm_int32 label, dm_bool include_holes)
{
  CCI_ENSURE_ARG_POINTER(mask);

  dmRegion* mskRgn = CCI_NATIVE(mask);
  if(!mskRgn)
     return CCI_ERROR_INVALID_ARG;
  
  if(label==-1)
  {
    // Get background region
    daim::create_roi(mMap,mNodePartition.bind(std::bind2nd(std::equal_to<dm_int>(),0)),
                     tmpRoi);
  }
  else
  {
    // Get object regions
    UpdatePartition();
    if(label==0)
    {
      if(include_holes) {
         UpdatePartRoi();
         tmpRoi = mPartRoi;
      }
      else {
        UpdateRoi();
        tmpRoi = mRoi;
      }
    }
    else
    {
      IMAGEMAP_VERIFY_LABEL(label);

      if(include_holes)
      {
        mLabels.clear();

        XNODE n = &_PARTITION_NODE(label);
        CollectChildLabels(n,true);

        daim::basic_partition _part = mPartition;
        int lbl = daim::merge_labels(_part,mLabels,label);
        daim::create_roi(
          mMap,_part.bind(std::bind2nd(std::equal_to<dm_int>(),lbl)),
          tmpRoi);
      }
      else
      {
        daim::create_roi(
          mMap,
          mPartition.bind(std::bind2nd(std::equal_to<dm_int>(),label)),
          tmpRoi);
      }
    }
  }

  *mskRgn = tmpRoi;
  return CCI_OK;
}

/* void clear (in dm_int32 label); */
CCI_IMETHODIMP cciMeasurements::Clear(dm_int32 label)
{
  if(label > 0)
  {
    IMAGEMAP_ENSURE_BUILT();
    IMAGEMAP_VERIFY_LABEL(label);

    XNODE node = &_PARTITION_NODE(label);
    ClearNode(node);

    mNodePartition.update();
    mUpdate &= ~IMAGEMAP_UPDATE_MAP;
  }
  else
  {
    mRoi.KillRoi();

    mUpdate        = 0;
    mNumPartitions = 0;
    mDepth         = 0;

    mLabels.clear();
    mIndexTable.clear();

    mNodeTable.Clear();
    mNodeList.Clear();

    mPartition.reserve(1);
    mNodePartition.reserve(1);
    
    mBuilt = dm_false;
  }
  return CCI_OK;
}

/* [noscript,notxpcom] dm_uint32 getLabels (in cciRegion rgn, [array] out dm_int32 labels); */
CCI_IMETHODIMP_(dm_uint32) cciMeasurements::GetLabels(cciRegion _rgn, dm_int32 **labels CCI_OUTPARAM)
{
  IMAGEMAP_ENSURE_BUILT()

  UpdatePartition();
  mLabels.clear();

  const dmRegion* rgn = _rgn ? CCI_NATIVE(_rgn) : dm_null;

  if(rgn)
  {
    if(rgn->IsEmptyRoi())
      return CCI_ERROR_INVALID_ARG;

    daim::get_overlapping_regions_labels2(*rgn,
                         mPartition,
                         mMap,
                         mLabels);

    //daim::labels_array_type::iterator it = daim::compact_labels( mLabels );
    //mLabels.resize( it - mLabels.begin() );
  }
  else
  {
    info_list_type::iterator it   = mNodeList.Begin();
    info_list_type::iterator last = mNodeList.End();

    for(;it!= last;++it) {
      if((*it).ri_Status==RI_REGION)
        mLabels.push_back(mPartition[(*it).ri_Part]);
    }
  }

  CCI_RETVAL_P(count)  =  _This->Labels.size();
  CCI_RETVAL_P(labels) = &_This->Labels[0];

  CCI_RETURN_OK()
}

/* dm_int32 getLabelFromPoint (in dm_int32 x, in dm_int32 y); */
CCI_IMETHODIMP cciMeasurements::GetLabelFromPoint(dm_int32 x, dm_int32 y, dm_int32 *_retval CCI_OUTPARAM)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void removeLabels ([array, size_is (count)] in dm_int32 labels, in dm_uint32 count); */
CCI_IMETHODIMP cciMeasurements::RemoveLabels(dm_int32 *labels, dm_uint32 count)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void selectLabels (in cciRegion mask, [array, size_is (count)] in dm_int32 labels, in dm_uint32 count); */
CCI_IMETHODIMP cciMeasurements::SelectLabels(cciRegion mask, dm_int32 *labels, dm_uint32 count)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void setCalibration (in dm_real unitsPerPixel, in dm_real aSpectRatio); */
CCI_IMETHODIMP cciMeasurements::SetCalibration(dm_real unitsPerPixel, dm_real aSpectRatio)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* [noscript,notxpcom] dm_uint32 getChildRegionLabels (in dm_int32 label, [array] out dm_int32 labels); */
CCI_IMETHODIMP_(dm_uint32) cciMeasurements::GetChildRegionLabels(dm_int32 label, dm_int32 **labels CCI_OUTPARAM)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* [noscript,notxpcom] dm_uint32 getIndexTable ([array] out dm_int32 table); */
CCI_IMETHODIMP_(dm_uint32) cciMeasurements::GetIndexTable(dm_int32 **table CCI_OUTPARAM)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void fillHoles (); */
CCI_IMETHODIMP cciMeasurements::FillHoles()
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void cleanBorders (); */
CCI_IMETHODIMP cciMeasurements::CleanBorders()
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void getProperties (in dm_int32 label, in dm_uint32 flags, in cciRgnPropertiesRef props); */
CCI_IMETHODIMP cciMeasurements::GetProperties(dm_int32 label, dm_uint32 flags, cciRgnProperties & props)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void reconstruct (in cciRegion mask); */
CCI_IMETHODIMP cciMeasurements::Reconstruct(cciRegion mask)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void cleanRegionBorders (in cciRegion mask); */
CCI_IMETHODIMP cciMeasurements::CleanRegionBorders(cciRegion mask)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}


/* readonly attribute dm_uint32 connectivity; */
CCI_IMETHODIMP cciMeasurements::GetConnectivity(dm_uint32 *aConnectivity)
{
  *aConnectivity = mConnect;
  return CCI_OK;
}

/* readonly attribute dm_uint32 count; */
CCI_IMETHODIMP cciMeasurements::GetCount(dm_uint32 *aCount)
{
  *aCount = mBuilt ? BuildIndexTable() : 0;
  return CCI_OK;
}

/* readonly attribute dm_uint32 depth; */
CCI_IMETHODIMP cciMeasurements::GetDepth(dm_uint32 *aDepth)
{
  *aDepth = mBuilt ? UpdatePartition() : 0;
  return CCI_OK;
}

///////////////////////////////////////////////////////////////////


//=====================================
// Module
//=====================================

#include "cciIGenericFactory.h"

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciMeasurements)

static const cciModuleComponentInfo components[] = {
    {  CCI_MEASURES_CLASSNAME,
       CCI_MEASURES_CID,
       CCI_MEASURES_CONTRACTID,
       cciMeasurementsConstructor
    },
};

CCI_IMPL_GETMODULE(MeasuresModule, components)


