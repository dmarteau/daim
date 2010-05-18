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
#include "cciIResultSet.h"
#include "cciIResultColumn.h"

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
CCI_IMPL_ISUPPORTS2(cciMeasurements,cciIMeasurements,cciIMeasures2)

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
  
  dmRegion* roi = CCI_IF_NATIVE(rgn);
  dmImage*  img = CCI_IF_NATIVE(image);

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

  dmRegion* mskRgn = CCI_IF_NATIVE(mask);
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

/* [noscript] void getLabels (in cciRegion rgn, [shared] out labelArray labels, [retval] out dm_uint32 count); */
CCI_IMETHODIMP cciMeasurements::GetLabels(cciRegion _rgn, const dm_int32 **labels CCI_OUTPARAM, 
                                          dm_uint32 *count CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(labels);
  CCI_ENSURE_ARG_POINTER(count);
  
  IMAGEMAP_ENSURE_BUILT();

  UpdatePartition();
  mLabels.clear();

  const dmRegion* rgn = CCI_IF_NATIVE(_rgn);

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

  *count  =  mLabels.size();
  *labels = &mLabels[0];

  return CCI_OK;
}

/* dm_int32 getLabelFromPoint (in dm_int32 x, in dm_int32 y); */
CCI_IMETHODIMP cciMeasurements::GetLabelFromPoint(dm_int32 x, dm_int32 y, dm_int32 *_retval CCI_OUTPARAM)
{
  IMAGEMAP_ENSURE_BUILT();

  dmPoint p(x,y);
  if(!dmPointInRectangle(p,mMap.rect()))
    return CCI_ERROR_FAILURE;

  UpdatePartition();
  *_retval = mPartition[mMap[p]];

  return CCI_OK;
}

/* void removeLabels ([array, size_is (count)] in dm_int32 labels, in dm_uint32 count); */
CCI_IMETHODIMP cciMeasurements::RemoveLabels(dm_int32 *labels, dm_uint32 count)
{
  IMAGEMAP_ENSURE_BUILT();

  if(count == 0 || !labels)
     return CCI_ERROR_INVALID_ARG;
  
  if(labels != &mLabels[0]) {
    mLabels.resize(count);
    std::copy(labels,labels+count,mLabels.begin());
  }

  daim::labels_array_type::iterator it   = mLabels.begin();
  daim::labels_array_type::iterator last = mLabels.end();

  // Remap partition labels
  for(;it!=last;++it) 
      *it = mPartition[*it];

  RemoveLabels();

  mUpdate &= ~(IMAGEMAP_UPDATE_MAP|IMAGEMAP_UPDATE_CENTROID);

  return CCI_OK;

}

/* void selectLabels (in cciRegion mask, [array, size_is (count)] in dm_int32 labels, in dm_uint32 count); */
CCI_IMETHODIMP cciMeasurements::SelectLabels(cciRegion _mask, dm_int32 *labels, dm_uint32 count)
{
  IMAGEMAP_ENSURE_BUILT();
  
  if(count == 0 || !labels)
     return CCI_ERROR_INVALID_ARG;
  
  dmRegion* mask = CCI_IF_NATIVE(_mask);
  
  CCI_ENSURE_ARG_POINTER(mask);

  if(labels != &mLabels[0]) {
    mLabels.resize(count);
    std::copy(labels,labels+count,mLabels.begin());
  }

  daim::basic_partition _part = mPartition;
  int lbl = daim::merge_labels(_part,mLabels,mLabels[0]);

  daim::create_roi(mMap,_part.bind(std::bind2nd(std::equal_to<dm_int>(),lbl)),tmpRoi);

  *mask = tmpRoi;

  return CCI_OK;
}

/* void setCalibration (in dm_real unitsPerPixel, in dm_real aSpectRatio); */
CCI_IMETHODIMP cciMeasurements::SetCalibration(dm_real unitsPerPixel, dm_real aSpectRatio)
{
  aspect_ratio = aSpectRatio;
  uppxls       = unitsPerPixel;
  
  return CCI_OK;
}

/* [noscript] void getChildRegionLabels (in dm_int32 label, [shared] out labelArray labels, [retval] out dm_uint32 count); */
CCI_IMETHODIMP cciMeasurements::GetChildRegionLabels(dm_int32 label, const dm_int32 **labels CCI_OUTPARAM, 
                                                     dm_uint32 *count CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(labels);
  CCI_ENSURE_ARG_POINTER(count);

  IMAGEMAP_ENSURE_BUILT();
  IMAGEMAP_VERIFY_LABEL(label);

  _RegionInfo& rInfo = _PARTITION_NODE(label);
  if(rInfo.ri_Status==RI_REGION)
  {
    mLabels.clear();

    CollectChildLabels(&rInfo,false);

    *labels = &mLabels[0];
    *count  = mLabels.size();
    
    return CCI_OK;
  }
  
  return CCI_ERROR_FAILURE;
}

/* [noscript] void getIndexTable ([shared] out labelArray table, [retval] out dm_uint32 count); */
CCI_IMETHODIMP cciMeasurements::GetIndexTable(const dm_int32 **table CCI_OUTPARAM, 
                                              dm_uint32 *count CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(table);
  CCI_ENSURE_ARG_POINTER(count);

  IMAGEMAP_ENSURE_BUILT();

  BuildIndexTable();

  *table = &mLabels[0];
  *count =  mLabels.size();

  return CCI_OK;
}

/* void fillHoles (); */
CCI_IMETHODIMP cciMeasurements::FillHoles()
{
  IMAGEMAP_ENSURE_BUILT();

  XNODE root = &mNodeTable[0];

  dmDEBUG_ASSERT( root->ri_Part == 0 );

  dmList::iterator it   = root->ri_ChildList.Begin();
  dmList::iterator last = root->ri_ChildList.End();

  for(XNODE child;it!=last;++it)
  {
    child = static_cast<_RegionLink*>(*it)->rl_Self;
    dmDEBUG_ASSERT( child->ri_Father == 0 );
    MergeChilds(child);
  }

  mNodePartition.update();
  mUpdate &= ~(IMAGEMAP_UPDATE_MAP|IMAGEMAP_UPDATE_CENTROID);

  return CCI_OK;
}

/* void cleanBorders (); */
CCI_IMETHODIMP cciMeasurements::CleanBorders()
{
  IMAGEMAP_ENSURE_BUILT();

  dmRect r = mMap.rect();
  tmpRoi.SetRectRoi(r);

  SHRINK_RECT(r);   // Take care of thin regions

  tmpRoi.SubCoordinates(r);

  UpdatePartition();

  mLabels.clear();

  //------------------------------
  // Get all objects
  //------------------------------
  daim::get_overlapping_regions_labels(tmpRoi,mPartition,mMap,mLabels);

  daim::labels_array_type::iterator it = daim::compact_labels( mLabels );
  mLabels.resize( it - mLabels.begin() );

  RemoveLabels();

  mUpdate &= ~(IMAGEMAP_UPDATE_MAP|IMAGEMAP_UPDATE_CENTROID);

  return CCI_OK;
}

/* [noscript] void getProperties (in dm_int32 label, in dm_uint32 flags, in cciRgnPropertiesRef props); */
CCI_IMETHODIMP cciMeasurements::GetProperties(dm_int32 label, dm_uint32 flags, cciRgnProperties & props)
{
  IMAGEMAP_ENSURE_BUILT();
  IMAGEMAP_VERIFY_LABEL(label);

  props.area   = 0;
  props.dir    = 0;
  props.cx     = 0;
  props.cy     = 0;
  props.l1     = 0;
  props.l2     = 0;
  props.points = dm_null;
  props.count  = 0;
  props.euler  = 0;
  props.depth  = 0;

  _RegionInfo& rInfo = _PARTITION_NODE(label);

  if(rInfo.ri_Status==RI_REGION)
  {
    props.depth = rInfo.ri_Depth;
    props.holes = rInfo.ri_Childs;

    if(flags & CCIV_ImageMap_Area) {
      M_UpdateCount();
      props.area = rInfo.ri_Count;
    }

    if(flags & CCIV_ImageMap_Loc) {
      M_UpdateCentroids();
      props.cx = static_cast<int>(daim::round(rInfo.ri_Centroid_x));
      props.cy = static_cast<int>(daim::round(rInfo.ri_Centroid_y));
    }

    if(flags & CCIV_ImageMap_Shape)  {
      M_UpdateBoundary();
      props.points = rInfo.ri_Boundary.Begin();
      props.count  = rInfo.ri_Boundary.Size();
    }

    if(flags & CCIV_ImageMap_Dir)  {
      M_UpdateDir();
      props.dir = rInfo.ri_Theta;
    }

    if(flags & CCIV_ImageMap_Rect) {
      M_UpdateRectProps();
      props.l1 = rInfo.ri_L1;
      props.l2 = rInfo.ri_L2;
    }

    if(flags & CCIV_ImageMap_Euler) {
      M_UpdateEuler();
      props.euler = rInfo.ri_Euler;
    }

    return CCI_OK;
  }
  
  return CCI_ERROR_FAILURE;
}

/* void reconstruct (in cciRegion mask); */
CCI_IMETHODIMP cciMeasurements::Reconstruct(cciRegion mask)
{
  IMAGEMAP_ENSURE_BUILT();

  dmRegion* rgn = CCI_IF_NATIVE(mask);
  if(!rgn || !rgn->IsEmptyRoi())
     return CCI_ERROR_INVALID_ARG;
  
  UpdatePartition();

  mLabels.clear();
  tmpRoi.KillRoi();

  int index = daim::get_overlapping_regions_labels(*rgn,mPartition,mMap,mLabels);

  // Copy the partition
  daim::basic_partition _part = mPartition;

  int lbl = daim::merge_labels(_part,mLabels,index);
  daim::create_roi(mMap,_part.bind(std::bind2nd(std::equal_to<dm_int>(),lbl)),tmpRoi);

  *rgn = tmpRoi;

  return CCI_OK;
}

/* void cleanRegionBorders (in cciRegion mask); */
CCI_IMETHODIMP cciMeasurements::CleanRegionBorders(cciRegion mask)
{
  IMAGEMAP_ENSURE_BUILT();

  dmRegion* roi = CCI_IF_NATIVE(mask);
  CCI_ENSURE_ARG_POINTER(roi);
  
  if(!roi->IsEmptyRoi())
  {
    dmRect r = mMap.rect();

    dmRegion outline;

    tmpRoi = *roi;
    tmpRoi.GetOutLine(outline);
    tmpRoi.SubRoi(outline);
    tmpRoi.XorCoordinates(r);
/*
    if( _This->tmpRoi.IsEmptyRoi())
    {
      _This->tmpRoi.SetRectRoi(r);

      SHRINK_RECT(r);   // Take care of thin regions
      _This->tmpRoi.SubCoordinates(r);
    }
 */
    UpdatePartition();

    mLabels.clear();

    //------------------------------
    // Get all objects
    //------------------------------
    daim::get_overlapping_regions_labels(tmpRoi,mPartition,mMap,mLabels);

    daim::labels_array_type::iterator it = daim::compact_labels(mLabels);
    mLabels.resize( it - mLabels.begin() );

    RemoveLabels();

    mUpdate &= ~(IMAGEMAP_UPDATE_MAP|IMAGEMAP_UPDATE_CENTROID);
  }
  
  return CCI_OK;
}

cci_result
cciMeasurements::GetImage( dmImage* refImage, dmPoint& offs, dmLink<dmImage>& _retval )
{
  CCI_ENSURE_ARG_POINTER(refImage);
  
  offs = dmPoint(0,0);    

  dmRegion rgn;
  rgn.SetRectRoi(mRoi.Rectangle());
  rgn.Translate(mOffset);
  
  if(refImage->PixelFormat()!=dmPixelFormat32bppFloat) {
    _retval = dmCreateCopy(refImage,rgn.Rectangle(),dmPixelFormat32bppFloat);
  } else {
    offs = rgn.Rectangle().TopLeft();
    _retval = dmNewLink<dmImage>(refImage);
  }
  
  return CCI_OK;
}

/* void doMeasures ([array, size_is (count)] in string properties, in dm_uint32 count, in cciIResultSet results, in cciImage refImage); */
CCI_IMETHODIMP cciMeasurements::DoMeasures(const char * *properties, dm_uint32 count, 
                                           cciIResultSet *results, 
                                           cciImage refImage)
{
  CCI_ENSURE_ARG_POINTER(results);
  
  UpdateRoi();
  BuildIndexTable();

  if(mNumPartitions<=0)                // mNumPartitions is the number of objects
     return CCI_ERROR_NOT_AVAILABLE;   // No objects

  // Use intermediate storage (take care about '0' indexed region)

  mStorage.Resize( mNumPartitions + 1 ); // Numpartition is the number of objects
  dm_real* _results = mStorage.Begin();

  dmLink<dmImage> inputImage;
  floatImage* fimage;
  dmPoint offs(0,0);

#define IF_PROPERTY( _value, f ) \
  if(strcmp(item,_value)==0) { bRes = f ; }
  
  // Collect tags
  for(unsigned int i=0;i<count;++i) 
  {
    bool bRes = false;
    const char* item = properties[i];
    
    if(strncmp("ImgDesc_",item,8)==0 && fimage == dm_null) 
    {
      cci_result rv = GetImage(CCI_IF_NATIVE(refImage),offs,inputImage);
      CCI_ENSURE_SUCCESS(rv,rv);    
      
      fimage = dmIImage<dmPixelFormat32bppFloat>::Cast(inputImage);
    }
    
    IF_PROPERTY("RgnDesc_Labels"      , M_ComputeLabels(_results) ) else
    IF_PROPERTY("RgnDesc_Border"      , M_ComputeBorder(_results) )  else
    IF_PROPERTY("RgnDesc_Boundary"    , M_ComputeBoundary(_results) ) else
    IF_PROPERTY("RgnDesc_Area"        , M_ComputeArea(_results) ) else
    IF_PROPERTY("RgnDesc_Dir"         , M_ComputeDir(_results) ) else
    IF_PROPERTY("RgnDesc_Euler"       , M_ComputeEuler(_results) ) else
    IF_PROPERTY("RgnDesc_CX"          , M_ComputeMomentum(_results,1,0) ) else
    IF_PROPERTY("RgnDesc_CY"          , M_ComputeMomentum(_results,0,1) ) else
    IF_PROPERTY("RgnDesc_L1"          , M_ComputeRectProps(_results,1) ) else
    IF_PROPERTY("RgnDesc_L2"          , M_ComputeRectProps(_results,2) ) else
    IF_PROPERTY("RgnDesc_Depth"       , M_ComputeDepth(_results) ) else
    IF_PROPERTY("RgnDesc_Holes"       , M_ComputeHoles(_results) ) else
    IF_PROPERTY("RgnDesc_PosX"        , M_ComputePosition(_results,1) ) else
    IF_PROPERTY("RgnDesc_PosY"        , M_ComputePosition (_results,2) )  else
    IF_PROPERTY("RgnDesc_ParentLabels", M_ComputeParentLabels(_results) )

    IF_PROPERTY("ImgDesc_MinDensity"  , ComputeMinDensity(*fimage,_results,offs) ) else
    IF_PROPERTY("ImgDesc_MaxDensity"  , ComputeMaxDensity(*fimage,_results,offs) ) else
    IF_PROPERTY("ImgDesc_MeanDensity" , ComputeMeanDensity(*fimage,_results,offs) ) else

    IF_PROPERTY("ImgDesc_TotalDensity?n=1", ComputeDensity(*fimage,1,_results,offs) ) else
    IF_PROPERTY("ImgDesc_TotalDensity?n=2", ComputeDensity(*fimage,2,_results,offs) ) else
    
    if(bRes) {
      // Numpartition is the number of objects
      dm_real* data = results->CreateCol(item,mNumPartitions);
      if(data)
         std::copy(mStorage.Begin()+1,mStorage.End(),data);
      else
        return CCI_ERROR_OUT_OF_MEMORY;
    }
  }
  
#undef IF_PROPERTY
  
  return CCI_OK;
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

/////////////////////////////////
// cciIMeasures2 
/////////////////////////////////

/* void getLabels2 (in cciRegion rgn, in cciIResultColumn col); */
CCI_IMETHODIMP 
cciMeasurements::GetLabels2(cciRegion rgn, cciIResultColumn *col)
{
  CCI_ENSURE_ARG_POINTER(col);
  
  const dm_int32* labels;
  dm_uint32 count = 0;

  cci_result rv = GetLabels(rgn,&labels,&count);
  CCI_ENSURE_SUCCESS(rv,rv);
  
  dm_real* data = col->GetNewData( count );
  if(!data)
     return CCI_ERROR_FAILURE;
  
  std::copy(labels,labels+count,data);
  
  return CCI_OK;
}

/* voidgetChildRegionLabels2 (in dm_int32 label, in cciIResultColumn col); */
CCI_IMETHODIMP 
cciMeasurements::GetChildRegionLabels2(dm_int32 label, cciIResultColumn *col)
{
  CCI_ENSURE_ARG_POINTER(col);
  
  const dm_int32* labels;
  dm_uint32 count = 0;

  cci_result rv = GetChildRegionLabels(label,&labels,&count);
  CCI_ENSURE_SUCCESS(rv,rv);
  
  dm_real* data = col->GetNewData( count );
  if(!data)
     return CCI_ERROR_FAILURE;
  
  std::copy(labels,labels+count,data);
  
  return CCI_OK;
}

/* void getIndexTable2 (in cciIResultColumn col); */
CCI_IMETHODIMP cciMeasurements::GetIndexTable2(cciIResultColumn *col)
{
  CCI_ENSURE_ARG_POINTER(col);
  
  const dm_int32* labels;
  dm_uint32 count = 0;

  cci_result rv = GetIndexTable(&labels,&count);
  CCI_ENSURE_SUCCESS(rv,rv);
  
  dm_real* data = col->GetNewData( count );
  if(!data)
     return CCI_ERROR_FAILURE;
  
  std::copy(labels,labels+count,data);
  
  return CCI_OK;
}

static cci_result CopyLabelArray( cciIResultColumn *col,
                                  daim::labels_array_type& labels, 
                                  daim::basic_partition& part,
                                  dm_int max )
{
  dm_real* data   = dm_null;
  dm_uint32 count = col->GetData(&data);
  
  if(count == 0)
     return CCI_ERROR_INVALID_ARG;

  labels.resize(count);
  
  dm_real* it   = data;
  dm_real* last = data+count;

  daim::labels_array_type::iterator dst = labels.begin();
  
  for(dm_int lbl;it!=last;++it,++dst)
  {
    lbl = static_cast<dm_int>( *it );
    if(lbl >= max) 
       return CCI_ERROR_OUT_OF_RANGE;
    
    *dst = part[lbl];
  }
  
  return CCI_OK;
}

/* void removeLabels2 (in cciIResultColumn col); */
CCI_IMETHODIMP cciMeasurements::RemoveLabels2(cciIResultColumn *col)
{
  CCI_ENSURE_ARG_POINTER(col);
  IMAGEMAP_ENSURE_BUILT();

  cci_result rv = CopyLabelArray(col,mLabels,mPartition,mNodePartition.size());
  CCI_ENSURE_SUCCESS(rv,rv);
 
  RemoveLabels();

  mUpdate &= ~(IMAGEMAP_UPDATE_MAP|IMAGEMAP_UPDATE_CENTROID);

  return CCI_OK;
}

/* void selectLabels2 (in cciRegion mask, in cciIResultColumn col); */
CCI_IMETHODIMP cciMeasurements::SelectLabels2(cciRegion _mask, cciIResultColumn *col)
{
  CCI_ENSURE_ARG_POINTER(col);
  
  IMAGEMAP_ENSURE_BUILT();

  dmRegion* mask = CCI_IF_NATIVE(_mask);
  CCI_ENSURE_ARG_POINTER(mask);
  
  cci_result rv = CopyLabelArray(col,mLabels,mPartition,mNodePartition.size());
  CCI_ENSURE_SUCCESS(rv,rv);
  
  daim::basic_partition _part = mPartition;
  int lbl = daim::merge_labels(_part,mLabels,mLabels[0]);

  daim::create_roi(mMap,_part.bind(std::bind2nd(std::equal_to<dm_int>(),lbl)),tmpRoi);

  *mask = tmpRoi;

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


